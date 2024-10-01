/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Common subscribe message implementation.
 * Implements common functions and (de-)serialization
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "common/print.hpp"
#include "common/serialize.hpp"
#include "common/subscribe_msg.hpp"

__CLKMGR_NAMESPACE_USE

using namespace std;

string CommonSubscribeMessage::toString()
{
    string name = ExtractClassName(string(__PRETTY_FUNCTION__),
            string(__FUNCTION__));
    name += "\n";
    name += Message::toString();
    //name += "Client ID: " + string((char *)clientId.data()) + "\n";
    return name;
}

PARSE_RXBUFFER_TYPE(CommonSubscribeMessage::parseBuffer)
{
    PrintDebug("[CommonSubscribeMessage]::parseBuffer ");
    if(!Message::parseBuffer(LxContext))
        return false;
    if(!PARSE_RX(FIELD, get_sessionId(), LxContext))
        return false;
    if(!PARSE_RX(FIELD, subscription, LxContext))
        return false;
    return true;
}

BUILD_TXBUFFER_TYPE(CommonSubscribeMessage::makeBuffer) const
{
    auto ret = Message::makeBuffer(TxContext);
    if(!ret)
        return ret;
    PrintDebug("[CommonSubscribeMessage]::makeBuffer - sessionId : " + \
        to_string(c_get_val_sessionId()));
    if(!WRITE_TX(FIELD, c_get_val_sessionId(), TxContext))
        return false;
    PrintDebug("[CommonSubscribeMessage]::makeBuffer - subscription event : " + \
        std::to_string(subscription.get_event_mask()) + \
        ", composite event : " + \
        std::to_string(subscription.get_composite_event_mask()));
    if(!WRITE_TX(FIELD, subscription, TxContext))
        return false;
    return true;
}

TRANSMIT_MESSAGE_TYPE(CommonSubscribeMessage::transmitMessage)
{
    PrintDebug("[CommonSubscribeMessage]::transmitMessage ");
    if(!presendMessage(&TxContext))
        return false;
    return TxContext.sendBuffer();
}

void setSubscription(ClkMgrSubscription &newsub)
{
    PrintDebug("[CommonSubscribeMessage]::setSubscription ");
}