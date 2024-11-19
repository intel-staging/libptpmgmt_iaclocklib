/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Proxy connect ptp4l message class.
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "common/print.hpp"
#include "proxy/client.hpp"
#include "proxy/connect_ptp4l.hpp"
#include "proxy/notification_msg.hpp"
#include "proxy/thread.hpp"

#include "init.h" /* libptpmgmt */

#include <libgen.h>
#include <map>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string>
#include <sys/epoll.h>
#include <unistd.h>
#include <chrony.h>

__CLKMGR_NAMESPACE_USE

using namespace std;
using namespace ptpmgmt;

static const size_t bufSize = 2000;
static char buf[bufSize];
static Init obj;
static ptpmgmt::Message &msg = obj.msg();
static ptpmgmt::Message msgu;
static SockBase *sk;

static std::unique_ptr<SockBase> m_sk;

SUBSCRIBE_EVENTS_NP_t d;
ptp_event pe = { 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 0};
chrony_session *s;
int fd;
int report_index = 0;
int polling_interval;

void notify_client()
{
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

void event_handle()
{
    const BaseMngTlv *data = msg.getData();
    switch(msg.getTlvId()) {
        case TIME_STATUS_NP: {
            const auto *timeStatus = static_cast<const TIME_STATUS_NP_t *>(data);
            pe.master_offset = timeStatus->master_offset;
            memcpy(pe.gm_identity, timeStatus->gmIdentity.v,
                sizeof(pe.gm_identity));
            /* Uncomment for debug data printing */
            /*
            printf("master_offset = %ld, synced_to_primary_clock = %d\n",
                  pe.master_offset, pe.synced_to_primary_clock);
            printf("gm_identity = %02x%02x%02x.%02x%02x.%02x%02x%02x\n\n",
                   pe.gm_identity[0], pe.gm_identity[1],pe.gm_identity[2],
                   pe.gm_identity[3], pe.gm_identity[4],
                   pe.gm_identity[5], pe.gm_identity[6],pe.gm_identity[7]);
            */
            break;
        }
        case PORT_PROPERTIES_NP: /* Get initial port state when Proxy starts */
        case PORT_DATA_SET: {
            const auto *portDataSet = static_cast<const PORT_DATA_SET_t *>(data);
            portState_e portState = portDataSet->portState;
            pe.synced_to_primary_clock = false;
            if(portState == SLAVE)
                pe.synced_to_primary_clock = true;
            else if(portState == MASTER) {
                /* Set own clock identity as GM identity */
                pe.master_offset = 0;
                memcpy(pe.gm_identity,
                    portDataSet->portIdentity.clockIdentity.v,
                    sizeof(pe.gm_identity));
                break;
            } else if(portState <= PASSIVE) {
                /* Reset master offset and GM identity */
                pe.master_offset = 0;
                memset(pe.gm_identity, 0, sizeof(pe.gm_identity));
            }
            break;
        }
        case CMLDS_INFO_NP: {
            const auto *cmldsInfo = static_cast<const CMLDS_INFO_NP_t *>(data);
            bool asCapable = cmldsInfo->as_capable > 0 ? true : false;
            /* Skip client notification if no event changes */
            if(pe.as_capable == asCapable) {
                PrintDebug("Ignore unchanged as_capable");
                return;
            }
            pe.as_capable = asCapable;
            break;
        }
        default:
            return;
    }
    notify_client();
}

static inline bool msg_send(bool local)
{
    static int seq = 0;
    ptpmgmt::Message *m;
    if(local)
        m = &msgu;
    else
        m = &msg;
    MNG_PARSE_ERROR_e err = m->build(buf, bufSize, seq);
    if(err != MNG_PARSE_ERROR_OK) {
        fprintf(stderr, "build error %s\n", msgu.err2str_c(err));
        return false;
    }
    bool ret;
    ret = sk->send(buf, m->getMsgLen());
    if(!ret) {
        //fprintf(stderr, "send failed\n");
        return false;
    }
    seq++;
    return true;
}

static inline bool msg_set_action(bool local, mng_vals_e id)
{
    bool ret;
    if(local)
        ret = msgu.setAction(GET, id);
    else
        ret = msg.setAction(GET, id);
    if(!ret) {
        fprintf(stderr, "Fail get %s\n", msg.mng2str_c(id));
        return false;
    }
    return msg_send(local);
}

/**
 * @brief Subscribes to a set of PTP (Precision Time Protocol) events.
 *
 * This function configures a subscription to various PTP events
 * by setting the appropriate bitmask in a subscription data
 * structure. It then sends an action to subscribe to these
 * events using a message handling system.
 * The function clears the message data after sending the action
 * to avoid referencing local data that may go out of scope.
 *
 * @param handle A double pointer to a clkmgr_handle structure
 *               representing the handle to be used for the
 *               subscription.
 *               The actual usage of this parameter is not shown in
 *               the provided code snippet, so it may need to be
 *               implemented or removed.
 * @return A boolean value indicating the success of the subscription
 *         action.
 *         Returns true if the subscription action is successfully
 *         sent, false otherwise.
 *
 */
bool event_subscription(struct clkmgr_handle **handle)
{
    memset(d.bitmask, 0, sizeof d.bitmask);
    d.duration = UINT16_MAX;
    d.setEvent(NOTIFY_TIME_SYNC);
    d.setEvent(NOTIFY_PORT_STATE);
    d.setEvent(NOTIFY_CMLDS);
    if(!msg.setAction(SET, SUBSCRIBE_EVENTS_NP, &d)) {
        fprintf(stderr, "Fail set SUBSCRIBE_EVENTS_NP\n");
        return false;
    }
    bool ret = msg_send(false);
    /* Remove referance to local SUBSCRIBE_EVENTS_NP_t */
    msg.clearData();
    return ret;
}

static chrony_err process_chronyd_data(chrony_session *s)
{
    struct pollfd pfd = { .fd = chrony_get_fd(s), .events = POLLIN };
    int n, timeout;
    chrony_err r;
    timeout = 1000;
    while (chrony_needs_response(s)) {
        n = poll(&pfd, 1, timeout);
        if (n < 0) {
            perror("poll");
//          return -1;
        } else if (n == 0) {
            fprintf(stderr, "No valid response received\n");
//          return -1;
        }
        r = chrony_process_response(s);
        if (r != CHRONY_OK)
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
    if (r != CHRONY_OK)
        return r;
    r = process_chronyd_data(s);
    if (r != CHRONY_OK)
        return r;
    for (j = 0; j < chrony_get_record_number_fields(s); j++) {
        content = chrony_get_field_content(s, j);
        if (content == CHRONY_CONTENT_NONE)
            continue;
        const char *field_name = chrony_get_field_name(s, j);
        if (field_name != NULL && strcmp(field_name, "Last offset") == 0) {
            float second = (chrony_get_field_float(s, j) * 1e9);
            pe.chrony_offset = (int)second;
            printf("offset (chrony): %ld nanosecond \n\n", pe.chrony_offset);
        }
        if (field_name != NULL && strcmp(field_name, "Reference ID") == 0) {
            pe.chrony_reference_id = chrony_get_field_uinteger(s, j);
            printf("Reference ID: %lX\n", pe.chrony_reference_id);
        }
        if (field_name != NULL && strcmp(field_name, "Poll") == 0) {
            polling_interval = chrony_get_field_integer(s, j);
            //printf("Poll: %d\n", pe.chrony_poll);
            printf("Pollng interval: %ld s\n",polling_interval);
        }
	}
	return CHRONY_OK;
}


struct ThreadArgs
{
    chrony_session *s;
    int report_index;
};

void* monitor_chronyd(void* arg)
{
    ThreadArgs* args = (ThreadArgs*)arg;
    chrony_session *s = args->s;
    int report_index = args->report_index;
    while (true) {
        if (chrony_init_session(&s, fd) == CHRONY_OK) {
            for (int i = 0; i < 2; i++) {
                subscribe_to_chronyd(s, i);
               // chrony_deinit_session(s);
            }
        }
        usleep(polling_interval); // Sleep duration is based on chronyd polling interval
    }
    return NULL;
}

void start_monitor_thread(chrony_session *s, int report_index)
{
    pthread_t thread_id;
    ThreadArgs* args = new ThreadArgs{s, report_index};
    if (pthread_create(&thread_id, NULL, monitor_chronyd, args) != 0) {
        fprintf(stderr, "Failed to create thread\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Runs the main event loop for handling PTP (Precision Time Protocol)
 *        events.
 *
 * This function enters an infinite loop, where it sends a GET request
 * with the intention of receiving a reply from the local PTP daemon.
 * If the GET request is successful, it waits for incoming messages
 * with a timeout and processes them if received. The loop terminates
 * if a message is successfully handled. After breaking out of the first
 * loop, the function sends a GET request to all destinations and
 * enters a second infinite loop to handle asynchronous events using epoll.
 *
 * @param arg A void pointer to an argument that can be passed to the function.
 *            The actual type and content of this argument should be
 *            defined by the user and cast appropriately within the function.
 * @return This function does not return a value. If a return value is needed,
 *         the function signature and implementation should be modified accordingly.
 *
 */
void *ptp4l_event_loop(void *arg)
{
    const uint64_t timeout_ms = 1000;
    bool lost_connection = false;
    for(;;) {
        if(!msg_set_action(true, PORT_PROPERTIES_NP))
            break;
        ssize_t cnt;
        sk->poll(timeout_ms);
        cnt = sk->rcv(buf, bufSize);
        if(cnt > 0) {
            MNG_PARSE_ERROR_e err = msg.parse(buf, cnt);
            if(err == MNG_PARSE_ERROR_OK) {
                event_handle();
                break;
            }
        }
    }
    msg_set_action(false, PORT_PROPERTIES_NP);
    event_subscription(nullptr);
    while(1) {
        if(sk->poll(timeout_ms) > 0) {
            const auto cnt = sk->rcv(buf, bufSize);
            if(cnt > 0) {
                MNG_PARSE_ERROR_e err = msg.parse(buf, cnt);
                if(err == MNG_PARSE_ERROR_OK)
                    event_handle();
            }
        } else {
            while(1) {
                if(event_subscription(nullptr))
                    break;
                if(!lost_connection) {
                    PrintError("Lost connection to ptp4l.");
                    PrintInfo("Resetting clkmgr's ptp4l data.");
                    pe.master_offset = 0;
                    memset(pe.gm_identity, 0, sizeof(pe.gm_identity));
                    pe.synced_to_primary_clock = false;
                    pe.as_capable = 0;
                    notify_client();
                    lost_connection = true;
                }
                PrintInfo("Attempting to reconnect to ptp4l...");
                sleep(2);
            }
            if(lost_connection) {
                PrintInfo("Successful connected to ptp4l.");
                lost_connection = false;
            }
        }
    }
}

/** @brief Establishes a connection to the local PTP (Precision Time Protocol)
 *         daemon.
 *
 * This method initializes a Unix socket connection to the local PTP daemon.
 * It sets up the message parameters, including the boundary hops and the
 * process ID as part of the clock identity.
 * It also configures the epoll instance to monitor the socket for incoming
 * data or errors.
 *
 * @return An integer indicating the status of the connection attempt.
 *         Returns 0 on success, or -1 if an error occurs during socket
 *         initialization, address setting, or epoll configuration.
 */
int Connect::connect(uint8_t transport_specific)
{
    std::string uds_address;
    SockUnix *sku = new SockUnix;
    if(sku == nullptr)
        return -1;
    m_sk.reset(sku);
    pe.ptp4l_id = 1;
    uds_address = "/var/run/ptp4l";
    if(!sku->setDefSelfAddress() || !sku->init() ||
        !sku->setPeerAddress(uds_address))
        return -1;
    /* Set Transport Specific */
    MsgParams prms = msg.getParams();
    prms.transportSpecific = transport_specific;
    msg.updateParams(prms);
    sk = m_sk.get();

    /* connect to chronyd unix socket*/
    fd = chrony_open_socket("/var/run/chrony/chronyd.sock");
    chrony_session *s;
    if (chrony_init_session(&s, fd) == CHRONY_OK) {
        start_monitor_thread(s, report_index);
		chrony_deinit_session(s);
	}

    handle_connect();
    return 0;
}

void Connect::disconnect()
{
    obj.close();
    sk->close();
}
