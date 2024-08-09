/*! \file connect.cpp
    \brief Proxy connect message class.

    (C) Copyright Intel Corporation 2023. All rights reserved. Intel Confidential.
    Author: Lai Peter Jun Ann <peter.jun.ann.lai@intel.com>
*/

#include <proxy/connect.hpp>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <poll.h>
#include <libgen.h>
#include <string>
#include <map>
#include <ptpmgmt/init.h>
#include <sys/epoll.h>
#include <common/print.hpp>
#include <proxy/notification_msg.hpp>
#include <proxy/client.hpp>

//TODO: subsription part
#include "thread.hpp"

using namespace std;
using namespace JClkLibProxy;
using namespace ptpmgmt;

const int SUBSCRIBE_DURATION = 180; /* 3 minutes */

static const size_t bufSize = 2000;
static char buf[bufSize];
static Init obj;
static Message &msg = obj.msg();
static Message msgu; // TODO: to be removed
static SockBase *sk;

static std::unique_ptr<SockBase> m_sk;

struct port_info {
    PortIdentity_t portid;
    int64_t master_offset;
    bool local;
};

int epd;
struct epoll_event epd_event;
SUBSCRIBE_EVENTS_NP_t d;
JClkLibCommon::ptp_event pe = { 0, {0, 0, 0, 0, 0, 0, 0, 0}, 0 , 0 , 0};

void notify_client()
{
    PrintDebug("[JClkLibProxy]::notify_client");
    JClkLibCommon::sessionId_t SessionId;
    std::unique_ptr<ProxyMessage> notifyMsg(new ProxyNotificationMessage());

    ProxyNotificationMessage *pmsg = dynamic_cast<decltype(pmsg)>(notifyMsg.get());
    if (pmsg == NULL) {
        PrintErrorCode("[JClkLibProxy::notify_client] notifyMsg is NULL !!\n");
        return;
    }

    PrintDebug("[JClkLibProxy::notify_client] notifyMsg creation is OK !!\n");

    /* ToDo: loop for multiple sessions */
    SessionId = Client::GetSessionIdAt(0);
    if (SessionId != JClkLibCommon::InvalidSessionId) {
        PrintDebug("Get client session ID: " + to_string(SessionId));

        auto TxContext = Client::GetClientSession(SessionId).get()->get_transmitContext();
        pmsg->transmitMessage(*TxContext);
    } else {
        PrintDebug("Unable to get Session ID");
    }
}

void event_handle()
{
    const BaseMngTlv *data = msg.getData();
    int64_t offset;
    ClockIdentity_t gm_uuid;
    uint8_t servo;
    switch(msg.getTlvId()) {
        case TIME_STATUS_NP:
            offset = ((TIME_STATUS_NP_t *)data)->master_offset;
            servo = ((TIME_STATUS_NP_t *)data)->servo_state;
            gm_uuid = ((TIME_STATUS_NP_t *)data)->gmIdentity;
            pe.gmPresent = ((TIME_STATUS_NP_t *)data)->gmPresent;
            pe.master_offset = offset;
            pe.servo_state = servo;
            memcpy(pe.gmIdentity, gm_uuid.v, sizeof(pe.gmIdentity));

            //TODO: Add debug flag checking, only print if the flag is set
            printf("master_offset = %ld, servo_state = %d gmPresent = %d\n", pe.master_offset, pe.servo_state, pe.gmPresent);
            printf("gmIdentity = %02x%02x%02x.%02x%02x.%02x%02x%02x\n\n",
                   pe.gmIdentity[0], pe.gmIdentity[1],pe.gmIdentity[2],
                   pe.gmIdentity[3], pe.gmIdentity[4],
                   pe.gmIdentity[5], pe.gmIdentity[6],pe.gmIdentity[7]);
            break;
        case PORT_DATA_SET_NP:
            pe.asCapable = ((PORT_DATA_SET_NP_t *)data)->asCapable;
            printf("asCapable = %d\n\n", pe.asCapable);
            break;
        default:
            break;
    }

    notify_client();
}

static inline bool msg_send(bool local)
{
    static int seq = 0;
    Message *m;
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
 * This function configures a subscription to various PTP events by setting the appropriate
 * bitmask in a subscription data structure. It then sends an action to subscribe to these
 * events using a message handling system. The function clears the message data after sending
 * the action to avoid referencing local data that may go out of scope.
 *
 * @param handle A double pointer to a jcl_handle structure representing the handle to be used
 *               for the subscription. The actual usage of this parameter is not shown in the
 *               provided code snippet, so it may need to be implemented or removed.
 * @return A boolean value indicating the success of the subscription action.
 *         Returns true if the subscription action is successfully sent, false otherwise.
 *
 */
bool event_subscription(struct jcl_handle **handle)
{
    memset(d.bitmask, 0, sizeof d.bitmask);
    d.duration = SUBSCRIBE_DURATION;
    d.setEvent(NOTIFY_TIME_SYNC);
    d.setEvent(NOTIFY_PORT_STATE_NP);

    if(!msg.setAction(SET, SUBSCRIBE_EVENTS_NP, &d)) {
        fprintf(stderr, "Fail set SUBSCRIBE_EVENTS_NP\n");
        return false;
    }
    bool ret = msg_send(false);
    msg.clearData(); // Remove referance to local SUBSCRIBE_EVENTS_NP_t
    return ret;
}

/**
 * @brief Runs the main event loop for handling PTP (Precision Time Protocol) events.
 *
 * This function enters an infinite loop, where it sends a GET request with the intention
 * of receiving a reply from the local PTP daemon. If the GET request is successful, it
 * waits for incoming messages with a timeout and processes them if received. The loop
 * terminates if a message is successfully handled. After breaking out of the first loop,
 * the function sends a GET request to all destinations and enters a second infinite loop
 * to handle asynchronous events using epoll.
 *
 * @param arg A void pointer to an argument that can be passed to the function. The actual
 *            type and content of this argument should be defined by the user and cast
 *            appropriately within the function.
 * @return This function does not return a value. If a return value is needed, the function
 *         signature and implementation should be modified accordingly.
 *
 */
void *ptp4l_event_loop( void *arg)
{
    const uint64_t timeout_ms = 1000; // 1 second

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

    // Send to all
    msg_set_action(false, PORT_PROPERTIES_NP);
    event_subscription(NULL);

    while (1) {
        if (epoll_wait( epd, &epd_event, 1, 100) != -1) {
        //if ( sk->poll(timeout_ms)) {
            const auto cnt = sk->rcv(buf, bufSize);
            MNG_PARSE_ERROR_e err = msg.parse(buf, cnt);
            if(err == MNG_PARSE_ERROR_OK)
                event_handle();
        }
    }
}

/** @brief Establishes a connection to the local PTP (Precision Time Protocol) daemon.
 *
 * This method initializes a Unix socket connection to the local PTP daemon. It sets up
 * the message parameters, including the boundary hops and the process ID as part of the
 * clock identity. It also configures the epoll instance to monitor the socket for incoming
 * data or errors.
 *
 * @return An integer indicating the status of the connection attempt.
 *         Returns 0 on success, or -1 if an error occurs during socket initialization,
 *         address setting, or epoll configuration.
 */
int Connect::connect()
{
    // Ensure we recieve reply from local ptp daemon
    MsgParams prms = msg.getParams();
    prms.boundaryHops = 0;

    std::unique_ptr<SockBase> m_sk;
    std::string uds_address;

    SockUnix *sku = new SockUnix;
    if(sku == nullptr) {
        return -1;
    }
    m_sk.reset(sku);

    while (system("ls /var/run/ptp4l*") != 0) {
        //sleep for 2 seconds and keep looping until there is ptp4l available
        sleep(2);
    }
    pe.ptp4l_id = 1;

    //TODO: hard-coded uds_socket
    uds_address = "/var/run/ptp4l";
    if(!sku->setDefSelfAddress() || !sku->init() ||
            !sku->setPeerAddress(uds_address))
        return -1;
    pid_t pid = getpid();
    prms.self_id.clockIdentity.v[6] = (pid >> 24) && 0xff;
    prms.self_id.clockIdentity.v[7] = (pid >> 16) && 0xff;
    prms.self_id.portNumber = pid & 0xffff;

    sk = m_sk.get();

    msgu.updateParams(prms);

    int ret;
    epd = epoll_create1( 0);
    if( epd == -1)
        ret = -errno;

    epd_event.data.fd = sk->fileno();
    epd_event.events  = ( EPOLLIN | EPOLLERR);
    if( epoll_ctl( epd, EPOLL_CTL_ADD, sk->fileno(), &epd_event) == 1)
        ret = -errno;

    //TODO: subscription part
    handle_connect( NULL, epd_event);
    while (1) {
        sleep(1);
    }

    return 0;
}
