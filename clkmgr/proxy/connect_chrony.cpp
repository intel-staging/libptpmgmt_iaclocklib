/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Proxy connect chrony message class.
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "common/print.hpp"
#include "proxy/client.hpp"
#include "proxy/config_parser.hpp"
#include "proxy/connect_chrony.hpp"
#include "proxy/connect_ptp4l.hpp"
#include "proxy/notification_msg.hpp"
#include <chrony.h>

#include <poll.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <cmath>

__CLKMGR_NAMESPACE_USE;

using namespace std;

std::map<int, int> chronyIndex;
int report_index = 0;
extern std::map<int, ptp_event> ptp4lEvents;
std::map<int, std::vector<sessionId_t>> subscribedClientsChrony;

struct ThreadArgs {
    chrony_session *s;
    int report_index;
    int fd;
};

int ConnectChrony::remove_chrony_subscriber(int timeBaseIndex,
    sessionId_t sessionId)
{
    auto &clients = subscribedClientsChrony[timeBaseIndex];
    for(auto it = clients.begin(); it != clients.end(); ++it) {
        if(*it == sessionId) {
            // sessionId found, remove it
            clients.erase(it);
            return 0;
        }
    }
    return 0;
}

void chrony_notify_client(int timeBaseIndex)
{
    PrintDebug("[clkmgr]::notify_client");
    for(const auto &sessionId : subscribedClientsChrony[timeBaseIndex]) {
        unique_ptr<ProxyMessage> notifyMsg(new ProxyNotificationMessage());
        ProxyNotificationMessage *pmsg =
            dynamic_cast<decltype(pmsg)>(notifyMsg.get());
        if(pmsg == nullptr) {
            PrintErrorCode("[clkmgr::notify_client] notifyMsg is nullptr !!");
            return;
        }
        PrintDebug("[clkmgr::notify_client] notifyMsg creation is OK !!");
        /* Send data for multiple sessions */
        pmsg->setTimeBaseIndex(timeBaseIndex);
        PrintDebug("Get client session ID: " + to_string(sessionId));
        auto TxContext = Client::GetClientSession(
                sessionId).get()->get_transmitContext();
        if(!pmsg->transmitMessage(*TxContext)) {
            ConnectPtp4l::remove_ptp4l_subscriber(timeBaseIndex, sessionId);
            ConnectChrony::remove_chrony_subscriber(timeBaseIndex, sessionId);
            Client::RemoveClientSession(sessionId);
        }
    }
}

static chrony_err process_chronyd_data(chrony_session *s)
{
    pollfd pfd = { .fd = chrony_get_fd(s), .events = POLLIN };
    int n, timeout;
    chrony_err r;
    timeout = 1000;
    while(chrony_needs_response(s)) {
        n = poll(&pfd, 1, timeout);
        if(n < 0)
            PrintErrorCode("poll");
        else if(n == 0)
            PrintError("No valid response received");
        r = chrony_process_response(s);
        if(r != CHRONY_OK)
            return r;
    }
    return CHRONY_OK;
}

static int subscribe_to_chronyd(chrony_session *s, int report_index,
    int timeBaseIndex)
{
    const char *report_name;
    chrony_err r;
    int record_index = 0;
    int field_index = 0;
    report_name = chrony_get_report_name(report_index);
    r = chrony_request_record(s, report_name, record_index);
    if(r != CHRONY_OK)
        return r;
    r = process_chronyd_data(s);
    if(r != CHRONY_OK)
        return r;
    field_index = chrony_get_field_index(s, "reference ID");
    ptp4lEvents[timeBaseIndex].chrony_reference_id =
        chrony_get_field_uinteger(s, field_index);
    field_index = chrony_get_field_index(s, "poll");
    int32_t interval = static_cast<int32_t>
        (static_cast<int16_t>(chrony_get_field_integer(s, field_index)));
    ptp4lEvents[timeBaseIndex].polling_interval =
        pow(2.0, interval) * 1000000;
    PrintDebug("CHRONY polling_interval = " +
        to_string(ptp4lEvents[timeBaseIndex].polling_interval) + " us");
    field_index = chrony_get_field_index(s, "original last sample offset");
    float second = (chrony_get_field_float(s, field_index) * 1e9);
    ptp4lEvents[timeBaseIndex].chrony_offset = (int)second;
    PrintDebug("CHRONY master_offset = " +
        to_string(ptp4lEvents[timeBaseIndex].chrony_offset));
    chrony_notify_client(timeBaseIndex);
    return CHRONY_OK;
}

void *monitor_chronyd(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    chrony_session *s = args->s;
    int fd = args->fd;
    for(;;) {
        if(chrony_init_session(&s, fd) == CHRONY_OK) {
            /* Subscribe to chronyd source index 1 */
            subscribe_to_chronyd(s, 1, chronyIndex[fd]);
        }
        /* Sleep duration is based on chronyd polling interval */
        usleep(ptp4lEvents[chronyIndex[fd]].polling_interval);
    }
}

void start_monitor_thread(chrony_session *s, int report_index, int fd)
{
    pthread_t thread_id;
    ThreadArgs *args = new ThreadArgs{s, report_index, fd};
    if(pthread_create(&thread_id, nullptr, monitor_chronyd, args) != 0) {
        PrintError("Failed to create thread");
        exit(EXIT_FAILURE);
    }
}

int ConnectChrony::subscribe_chrony(int timeBaseIndex, sessionId_t sessionId)
{
    auto it = ptp4lEvents.find(timeBaseIndex);
    if(it != ptp4lEvents.end()) {
        /* timeBaseIndex exists in the map */
        subscribedClientsChrony[timeBaseIndex].push_back(sessionId);
    } else {
        /* timeBaseIndex does not exist in the map */
        PrintDebug("timeBaseIndex does not exist in the map");
    }
    return 0;
}

void ConnectChrony::connect_chrony()
{
    for(const auto &param : timeBaseCfgs) {
        /* skip if chrony UDS address is empty */
        if(param.udsAddrChrony[0] == '\0')
            continue;
        /* connect to chronyd unix socket using udsAddrChrony */
        int fd = chrony_open_socket(param.udsAddrChrony);
        chronyIndex.insert(std::pair<int, int>(fd, param.timeBaseIndex));
        chrony_session *s;
        if(chrony_init_session(&s, fd) == CHRONY_OK) {
            start_monitor_thread(s, report_index, fd);
            chrony_deinit_session(s);
        }
    }
}
