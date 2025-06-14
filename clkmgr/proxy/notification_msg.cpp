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

#include "proxy/notification_msg.hpp"
#include "proxy/client.hpp"
#include "common/serialize.hpp"
#include "common/print.hpp"

__CLKMGR_NAMESPACE_USE;

using namespace std;

bool ProxyNotificationMessage::makeBufferTail(Buffer &buff) const
{
    PrintDebug("[ProxyNotificationMessage]::makeBufferTail");
    ptp_event event;
    Client::getPTPEvent(timeBaseIndex, event);
    // Add event data into the message
    return WRITE_TX(event, buff);
}
