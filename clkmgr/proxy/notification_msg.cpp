/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Proxy notification message implementation.
 * Implements proxy specific notification message function.
 *
 * @author Christopher Hall <christopher.s.hall@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "common/message.hpp"
#include "common/print.hpp"
#include "common/serialize.hpp"
#include "proxy/notification_msg.hpp"
#include "pub/clockmanager.h"

__CLKMGR_NAMESPACE_USE;

using namespace std;

extern std::map<int, PTPClockEvent> ptp4lEvents;
#ifdef HAVE_LIBCHRONY
extern std::map<int, SysClockEvent> chronyEvents;
#endif

/**
 * Create the ProxyNotificationMessage object
 * @param msg msg structure to be fill up
 * @param LxContext proxy transport listener context
 * @return true
 */
MAKE_RXBUFFER_TYPE(ProxyNotificationMessage::buildMessage)
{
    msg = new ProxyNotificationMessage();
    return true;
}

/**
 * @brief Add proxy's NOTIFY_MESSAGE type and its builder to transport layer.
 *
 * This function will be called during init to add a map of NOTIFY_MESSAGE
 * type and its corresponding buildMessage function.
 *
 * @return true
 */
bool ProxyNotificationMessage::initMessage()
{
    addMessageType(parseMsgMapElement_t(NOTIFY_MESSAGE, buildMessage));
    return true;
}

BUILD_TXBUFFER_TYPE(ProxyNotificationMessage::makeBuffer) const
{
    PrintDebug("[ProxyNotificationMessage]::makeBuffer");
    if(!Message::makeBuffer(TxContext))
        return false;
    // Add timeBaseIndex into the message
    if(!WRITE_TX(FIELD, timeBaseIndex, TxContext))
        return false;
    // Add clockType into the message
    if(!WRITE_TX(FIELD, clockType, TxContext))
        return false;
    if(clockType == PTP_CLOCK) {
        PTPClockEvent event = ptp4lEvents[timeBaseIndex];
        if(!WRITE_TX(FIELD, event, TxContext))
            return false;
    }
    #ifdef HAVE_LIBCHRONY
    if(clockType == SYSTEM_CLOCK) {
        SysClockEvent event = chronyEvents[timeBaseIndex];
        if(!WRITE_TX(FIELD, event, TxContext))
            return false;
    }
    #endif
    return true;
}

PROCESS_MESSAGE_TYPE(ProxyNotificationMessage::processMessage)
{
    return true;
}
