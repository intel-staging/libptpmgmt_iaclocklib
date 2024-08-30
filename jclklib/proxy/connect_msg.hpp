/*
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. All rights reserved.
 */

/** @file connect_msg.hpp
 * @brief Proxy connect message class. Implements proxy specific connect message function.
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation. All rights reserved.
 * @license LGPL-3.0-or-later
 *
 */

#ifndef PROXY_CONNECT_MSG_HPP
#define PROXY_CONNECT_MSG_HPP

#include <common/connect_msg.hpp>
#include <proxy/message.hpp>

namespace JClkLibProxy
{
    class ProxyConnectMessage : virtual public ProxyMessage,
        virtual public JClkLibCommon::CommonConnectMessage
    {
    protected:
        ProxyConnectMessage() : MESSAGE_CONNECT() {};
    public:
        /**
         * @brief process the connect msg from client-runtime
         * @param LxContext proxy transport listener context
         * @param TxContext proxy transport transmitter context
         * @return true
         */
        virtual PROCESS_MESSAGE_TYPE(processMessage);
        virtual BUILD_TXBUFFER_TYPE(makeBuffer) const;

        bool generateResponse(uint8_t *msgBuffer, std::size_t &length,
            const ClockStatus &status)
            { return false; }

        /**
         * @brief Create the ProxyConnectMessage object
         * @param msg msg structure to be fill up
         * @param LxContext proxy transport listener context
         * @return true
         */
        static MAKE_RXBUFFER_TYPE(buildMessage);

        /** @brief Add proxy's CONNECT_MSG type and its builder to transport layer.
         * @return true
         */
        static bool initMessage();
    };
}

#endif /* PROXY_CONNECT_MSG_HPP */
