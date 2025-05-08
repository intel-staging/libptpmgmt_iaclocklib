/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Proxy connect chrony message class.
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "common/print.hpp"
#include "common/clock_event_handler.hpp"
#include "proxy/client.hpp"
#include "proxy/config_parser.hpp"
#include "proxy/connect_chrony.hpp"
#include "proxy/connect_ptp4l.hpp"
#include "proxy/notification_msg.hpp"
#include "pub/clockmanager.h"

#include <chrony.h>
#include <poll.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <cmath>

__CLKMGR_NAMESPACE_USE;

using namespace std;

std::map<int, SysClockEvent> chronyEvents;
std::map<int, std::vector<sessionId_t>> subscribedClientsChrony;
ClockEventHandler sysClockEventHandler(ClockEventHandler::SysClock);

struct ThreadArgs {
    int timeBaseIndex;
    std::string udsAddrChrony;
};

int ConnectChrony::remove_chrony_subscriber(sessionId_t sessionId)
{
    for(const auto &entry : subscribedClientsChrony) {
        int timeBaseIndex = entry.first; // Get the timeBaseIndex
        auto &clients = subscribedClientsChrony[timeBaseIndex];
        for(auto it = clients.begin(); it != clients.end(); ++it) {
            if(*it == sessionId) {
                // sessionId found, remove it
                clients.erase(it);
                return 0;
            }
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
        // Send data for multiple sessions
        pmsg->setTimeBaseIndex(timeBaseIndex);
        // Set the Chrony clock as clock name
        pmsg->setClockType(SYSTEM_CLOCK);
        PrintDebug("Get client session ID: " + to_string(sessionId));
        auto TxContext = Client::GetClientSession(
                sessionId).get()->get_transmitContext();
        if(!pmsg->transmitMessage(*TxContext)) {
            ConnectPtp4l::remove_ptp4l_subscriber(sessionId);
            ConnectChrony::remove_chrony_subscriber(sessionId);
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

static int subscribe_to_chronyd(chrony_session *s, int timeBaseIndex)
{
    chrony_err r;
    int record_index = 0;
    int field_index = 0;
    r = chrony_request_record(s, "sources", record_index);
    if(r != CHRONY_OK)
        return r;
    r = process_chronyd_data(s);
    if(r != CHRONY_OK)
        return r;
    field_index = chrony_get_field_index(s, "reference ID");
    SysClockEvent &event = chronyEvents[timeBaseIndex];
    sysClockEventHandler.setGmIdentity(event, chrony_get_field_uinteger(s,
            field_index));
    field_index = chrony_get_field_index(s, "poll");
    int32_t interval = static_cast<int32_t>
        (static_cast<int16_t>(chrony_get_field_integer(s, field_index)));
    sysClockEventHandler.setSyncInterval(event,
        pow(2.0, interval) * 1000000);
    PrintDebug("CHRONY polling_interval = " +
        to_string(chronyEvents[timeBaseIndex].getSyncInterval()) + " us");
    field_index = chrony_get_field_index(s, "original last sample offset");
    float second = (chrony_get_field_float(s, field_index) * 1e9);
    sysClockEventHandler.setClockOffset(event, (int)second);
    PrintDebug("CHRONY master_offset = " +
        to_string(chronyEvents[timeBaseIndex].getClockOffset()));
    chrony_notify_client(timeBaseIndex);
    return CHRONY_OK;
}

void *monitor_chronyd(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    chrony_session *s;
    int timeBaseIndex = args->timeBaseIndex;
    SysClockEvent &event = chronyEvents[timeBaseIndex];
    std::string udsAddrChrony = args->udsAddrChrony;
    // connect to chronyd unix socket using udsAddrChrony
    int fd = chrony_open_socket(udsAddrChrony.c_str());
    if(chrony_init_session(&s, fd) == CHRONY_OK && fd > 0)
        PrintInfo("Connected to Chrony at " + udsAddrChrony);
    for(;;) {
        if(subscribe_to_chronyd(s, timeBaseIndex) != CHRONY_OK) {
            chrony_deinit_session(s);
            sysClockEventHandler.setGmIdentity(event, 0);
            sysClockEventHandler.setSyncInterval(event, 0);
            sysClockEventHandler.setClockOffset(event, 0);
            chrony_notify_client(timeBaseIndex);
            PrintError("Failed to connect to Chrony at " + udsAddrChrony);
            // Reconnection loop
            for(;;) {
                PrintInfo("Attempting to reconnect to Chrony at " +
                    udsAddrChrony);
                fd = chrony_open_socket(udsAddrChrony.c_str());
                if(fd < 0) {
                    sleep(5); // Wait before retrying
                    continue;
                }
                if(chrony_init_session(&s, fd) == CHRONY_OK) {
                    PrintInfo("Reconnected to Chrony at " + udsAddrChrony);
                    break;
                }
            }
        }
        // Sleep duration is based on chronyd polling interval
        usleep(chronyEvents[timeBaseIndex].getSyncInterval());
    }
}

void start_monitor_thread(int timeBaseIndex, const std::string &udsAddrChrony)
{
    pthread_t thread_id;
    ThreadArgs *args = new ThreadArgs{timeBaseIndex, udsAddrChrony};
    if(pthread_create(&thread_id, nullptr, monitor_chronyd, args) != 0) {
        PrintError("Failed to create thread");
        exit(EXIT_FAILURE);
    }
}

int ConnectChrony::subscribe_chrony(int timeBaseIndex, sessionId_t sessionId)
{
    auto it = chronyEvents.find(timeBaseIndex);
    if(it != chronyEvents.end()) {
        // timeBaseIndex exists in the map
        subscribedClientsChrony[timeBaseIndex].push_back(sessionId);
    } else {
        // timeBaseIndex does not exist in the map
        PrintDebug("timeBaseIndex does not exist in the map");
    }
    return 0;
}

void ConnectChrony::connect_chrony()
{
    for(const auto &param : JsonConfigParser::getInstance()) {
        // skip if chrony UDS address is empty
        if(!param.udsAddrChrony.empty())
            start_monitor_thread(param.base.timeBaseIndex, param.udsAddrChrony);
    }
}
