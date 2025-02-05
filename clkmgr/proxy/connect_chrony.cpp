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

int fd;
int fd1;
int report_index = 0;
int report_index1 = 0;
extern ptp_event pe;

struct ThreadArgs {
    chrony_session *s;
    int report_index;
};

void chrony_notify_client()
{
    printf("chrony notify client\n\n");
    PrintDebug("[clkmgr]::notify_client");
    sessionId_t SessionId;
    std::unique_ptr<ProxyMessage> notifyMsg(new ProxyNotificationMessage());
    ProxyNotificationMessage *pmsg = dynamic_cast<decltype(pmsg)>(notifyMsg.get());
    if(pmsg == nullptr) {
        PrintErrorCode("[clkmgr::notify_client] notifyMsg is nullptr !!\n");
        return;
    }
    PrintDebug("[clkmgr::notify_client] notifyMsg creation is OK !!\n");
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
            PrintError("Unable to get Session ID\n");
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
            perror("poll");
        else if(n == 0)
            fprintf(stderr, "No valid response received\n");
        r = chrony_process_response(s);
        if(r != CHRONY_OK)
            return r;
    }
    return CHRONY_OK;
}
ChronyDataStorage storage;
static int subscribe_to_chronyd(chrony_session *s, int report_index, int index)
{
    chrony_field_content content;
    const char *report_name;
    chrony_err r;
    int i, j;
    i = 0;
    report_name = chrony_get_report_name(report_index);
    //for(i = 0; i <2; i++) { //loop 2 if have 2 refclock
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
            if(field_name != nullptr && strcmp(field_name, "Reference ID") == 0) {
                pe.chrony_reference_id = chrony_get_field_uinteger(s, j);
                printf("CHRONY chrony_reference_id = %d \n",
                    pe.chrony_reference_id);
            }
            if(field_name != nullptr && strcmp(field_name, "Poll") == 0) {
                int32_t interval = static_cast<int32_t>
                    (static_cast<int16_t>(chrony_get_field_integer(s, j)));
                pe.polling_interval = std::pow(2.0, interval) * 1000000;
                //printf("CHRONY polling_interval = %d us\n",
                //      pe.polling_interval);}
            }
            if(field_name != nullptr && strcmp(field_name, "Last sample offset (original)") == 0) {
                float second = (chrony_get_field_float(s, j) * 1e9);
                pe.chrony_offset = (int)second;
                printf("CHRONY SOUCRES master_offset 1 = %ld \n",
                    pe.chrony_offset);
                storage.updateChronyOffset(index, second);
                ChronyData data = storage.getData(index);
                printf("CHRONY SOUCRES master_offset 2 = %ld , index = %d\n",
                    data.chrony_offset, index);
            }
            if(field_name != nullptr && strcmp(field_name, "State") == 0) {
                uint64_t uval = (chrony_get_field_uinteger(s, j) * 1e9);
                const char *str = chrony_get_field_constant_name(s, j, uval);
                if (str)
                    printf("CHRONY SOUCRES State = %s \n",
                        str);
            }
        }
        chrony_notify_client();
    //}
    return CHRONY_OK;
}

void *monitor_chronyd(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    chrony_session *s = args->s;
    int i = 1; //sources
    ChronyData data1 = {100, 1, 10};
    ChronyData data2 = {200, 2, 20};

    storage.addData(data1);
    storage.addData(data2);
    while(true) {
        if(chrony_init_session(&s, fd) == CHRONY_OK)
            subscribe_to_chronyd(s, i, 0);
        if(chrony_init_session(&s, fd1) == CHRONY_OK)
            subscribe_to_chronyd(s, i, 1);
        // Sleep duration is based on chronyd polling interval
        usleep(pe.polling_interval);
    }
}

void start_monitor_thread(chrony_session *s, int report_index)
{
    pthread_t thread_id;
    ThreadArgs *args = new ThreadArgs{s, report_index};
    if(pthread_create(&thread_id, nullptr, monitor_chronyd, args) != 0) {
        fprintf(stderr, "Failed to create thread\n");
        exit(EXIT_FAILURE);
    }
}

void ConnectChrony::connect_chrony(const char *address, const char *address1)
{
    /* connect to chronyd unix socket*/
    fd = chrony_open_socket(address);
    fd1 = chrony_open_socket(address1);
    chrony_session *s;
    if((chrony_init_session(&s, fd) == CHRONY_OK) && chrony_init_session(&s, fd1) == CHRONY_OK) {
        start_monitor_thread(s, report_index1);
        chrony_deinit_session(s);
    }
}
