/*! \file notification_msg.cpp
    \brief Proxy notification message implementation. Implements proxy specific notification message function.

    (C) Copyright Intel Corporation 2024. All rights reserved. Intel Confidential.
    Author: Christopher Hall <christopher.s.hall@intel.com>
*/

#include <proxy/notification_msg.hpp>
#include <common/serialize.hpp>
#include <proxy/clock_config.hpp>
#include <common/print.hpp>
#include <common/message.hpp>

using namespace std;
using namespace JClkLibCommon;
using namespace JClkLibProxy;

extern JClkLibCommon::ptp_event pe;

/** @brief Create the ProxyNotificationMessage object
 *
 * @param msg msg structure to be fill up
 * @param LxContext proxy transport listener context
 * @return true
 */
MAKE_RXBUFFER_TYPE(ProxyNotificationMessage::buildMessage)
{
	msg = new ProxyNotificationMessage();
	return true;
}

/** @brief Add proxy's NOTIFY_MESSAGE type and its builder to transport layer.
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

	/* Add ptp data here */
	if (!WRITE_TX(FIELD, pe, TxContext))
		return false;

	return true;
}

PROCESS_MESSAGE_TYPE(ProxyNotificationMessage::processMessage)
{
	return true;
}

/*
TO BE REMOVED
*/
bool ProxyNotificationMessage::generateResponse(uint8_t *msgBuffer, size_t &length,
					   const ClockStatus &status)
{
	return false;
}
