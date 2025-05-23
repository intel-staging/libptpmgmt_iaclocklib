/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Client notification message class.
 * Implements client specific notification message function.
 *
 * @author Christopher Hall <christopher.s.hall@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "client/notification_msg.hpp"
#include "client/timebase_state.hpp"
#include "common/serialize.hpp"
#include "common/print.hpp"

__CLKMGR_NAMESPACE_USE;

using namespace std;

/**
 * Create the ClientNotificationMessage object
 * @param msg msg structure to be fill up
 * @param LxContext proxy transport listener context
 * @return true
 */
bool ClientNotificationMessage::buildMessage(Message *&msg,
    TransportListenerContext &LxContext)
{
    msg = new ClientNotificationMessage();
    return true;
}

/**
 * @brief Add proxy's NOTIFY_MESSAGE type and its builder to transport layer.
 * This function will be called during init to add a map of NOTIFY_MESSAGE
 * type and its corresponding buildMessage function.
 *
 * @return true
 */
bool ClientNotificationMessage::initMessage()
{
    addMessageType(parseMsgMapElement_t(NOTIFY_MESSAGE, buildMessage));
    return true;
}

bool ClientNotificationMessage::makeBuffer(TransportTransmitterContext
    &TxContext) const
{
    PrintDebug("[ClientNotificationMessage]::makeBuffer");
    return true;
}

bool ClientNotificationMessage::processMessage(TransportListenerContext
    &LxContext, TransportTransmitterContext *&TxContext)
{
    PrintDebug("[ClientNotificationMessage]::processMessage ");
    return true;
}

bool ClientNotificationMessage::parseBuffer(TransportListenerContext &LxContext)
{
    PrintDebug("[ClientNotificationMessage]::parseBuffer ");
    int timeBaseIndex = 0;
    ptp_event data = {};
    if(!Message::parseBuffer(LxContext))
        return false;
    if(!PARSE_RX(FIELD, timeBaseIndex, LxContext))
        return false;
    if(!PARSE_RX(FIELD, data, LxContext))
        return false;
    TimeBaseStates::getInstance().setTimeBaseState(timeBaseIndex, data);
    return true;
}
