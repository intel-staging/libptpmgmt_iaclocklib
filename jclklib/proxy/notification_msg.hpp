/*
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. All rights reserved.
 */

/** @file notification_msg.hpp
 * @brief Proxy notification message class. Implements proxy specific notification message function.
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation. All rights reserved.
 * @license LGPL-3.0-or-later
 *
 */

#include <cstdint>

#ifndef PROXY_NOTIFICATION_MSG_HPP
#define PROXY_NOTIFICATION_MSG_HPP

#include <proxy/message.hpp>
#include <common/notification_msg.hpp>
#include <common/jclklib_import.hpp>

namespace JClkLibProxy
{
	class ProxyNotificationMessage : virtual public ProxyMessage,
					 virtual public JClkLibCommon::NotificationMessage
	{
	public:
		ProxyNotificationMessage() : MESSAGE_NOTIFY() {}
		virtual PROCESS_MESSAGE_TYPE(processMessage);
		bool generateResponse(std::uint8_t *msgBuffer, std::size_t &length,
				      const ClockStatus &status);
		virtual BUILD_TXBUFFER_TYPE(makeBuffer) const;

		/**
		 * @brief Create the ProxyNotificationMessage object
		 * @param msg msg structure to be fill up
		 * @param LxContext proxy transport listener context
		 * @return true
		 */
		static MAKE_RXBUFFER_TYPE(buildMessage);

		/**
		 * @brief Add proxy's NOTIFY_MESSAGE type and its builder to transport layer.
		 * @return true
		 */
		static bool initMessage();
	};
}

#endif/*PROXY_NOTIFICATION_MSG_HPP*/
