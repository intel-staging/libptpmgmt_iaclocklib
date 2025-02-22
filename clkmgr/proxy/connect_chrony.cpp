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
#include "proxy/connect_chrony.hpp"
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

static int fd;
static int report_index = 0;
extern ptp_event clockEvent;

struct ThreadArgs {
    chrony_session *s;
    int report_index;
};

void chrony_notify_client()
{
    PrintDebug("[clkmgr]::notify_client");
    sessionId_t SessionId;
    unique_ptr<ProxyMessage> notifyMsg(new ProxyNotificationMessage());
    ProxyNotificationMessage *pmsg = dynamic_cast<decltype(pmsg)>(notifyMsg.get());
    if(pmsg == nullptr) {
        PrintErrorCode("[clkmgr::notify_client] notifyMsg is nullptr !!");
        return;
    }
    PrintDebug("[clkmgr::notify_client] notifyMsg creation is OK !!");
    /* Send data for multiple sessions */
    for(size_t i = 0; i < Client::GetSessionCount(); i++) {
        SessionId = Client::GetSessionIdAt(i);
        if(SessionId != InvalidSessionId) {
            PrintDebug("Get client session ID: " + to_string(SessionId));
            auto TxContext = Client::GetClientSession(
                    SessionId).get()->get_transmitContext();
            if(!pmsg->transmitMessage(*TxContext))
                Client::RemoveClientSession(SessionId);
        } else
            PrintError("Unable to get Session ID");
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

static int subscribe_to_chronyd(chrony_session *s, int report_index)
{
    chrony_field_content content;
    const char *report_name;
    chrony_err r;
    int i, j;
    report_name = chrony_get_report_name(report_index);
    i = 0;
    r = chrony_request_record(s, report_name, i);
    if(r != CHRONY_OK)
        return r;
    r = process_chronyd_data(s);
    if(r != CHRONY_OK)
        return r;
    for(j = 0; j < chrony_get_record_number_fields(s); j++) {
        content = chrony_get_field_content(s, j);
        if(content == CHRONY_CONTENT_NONE)
            continue;
        const char *field_name = chrony_get_field_name(s, j);
        if(field_name != nullptr && strcmp(field_name, "Last offset") == 0) {
            float second = (chrony_get_field_float(s, j) * 1e9);
            clockEvent.chrony_offset = (int)second;
            #if 0
            PrintDebug("CHRONY master_offset = " +
                to_string(clockEvent.chrony_offset));
            #endif
        }
        if(field_name != nullptr && strcmp(field_name, "Reference ID") == 0)
            clockEvent.chrony_reference_id = chrony_get_field_uinteger(s, j);
        if(field_name != nullptr && strcmp(field_name, "Poll") == 0) {
            int32_t interval = static_cast<int32_t>
                (static_cast<int16_t>(chrony_get_field_integer(s, j)));
            clockEvent.polling_interval = pow(2.0, interval) * 1000000;
            #if 0
            PrintDebug("CHRONY polling_interval = " +
                to_string(clockEvent.polling_interval) + " us");
            #endif
        }
    }
    chrony_notify_client();
    return CHRONY_OK;
}

void *monitor_chronyd(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    chrony_session *s = args->s;
    for(;;) {
        if(chrony_init_session(&s, fd) == CHRONY_OK) {
            for(int i = 0; i < 2; i++)
                subscribe_to_chronyd(s, i);
        }
        // Sleep duration is based on chronyd polling interval
        usleep(clockEvent.polling_interval);
    }
}

void start_monitor_thread(chrony_session *s, int report_index)
{
    pthread_t thread_id;
    ThreadArgs *args = new ThreadArgs{s, report_index};
    if(pthread_create(&thread_id, nullptr, monitor_chronyd, args) != 0) {
        PrintError("Failed to create thread");
        exit(EXIT_FAILURE);
    }
}

void ConnectChrony::connect_chrony(std::string chronyUdsAddress)
{
    /* connect to chronyd unix socket*/
    fd = chrony_open_socket(chronyUdsAddress.c_str());
    chrony_session *s;
    if(chrony_init_session(&s, fd) == CHRONY_OK) {
        start_monitor_thread(s, report_index);
        chrony_deinit_session(s);
    }
}
