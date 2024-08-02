/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file connect_msg.hpp
 * @brief Client connect message class. Implements client specific functionality.
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 */

#ifndef CLIENT_CONNECT_MSG_HPP
#define CLIENT_CONNECT_MSG_HPP

#include <client/message.hpp>
#include <common/connect_msg.hpp>
#include <rtpi/condition_variable.hpp>
#include <rtpi/mutex.hpp>

namespace JClkLibClient
{
class ClientConnectMessage : virtual public JClkLibCommon::CommonConnectMessage,
    virtual public ClientMessage
{
  private:
    inline static ClientState *currentClientState;
  public:
    ClientConnectMessage() : MESSAGE_CONNECT() {};
    static rtpi::mutex cv_mtx;
    static rtpi::condition_variable cv;

    /**
     * @brief process the reply for connect msg from proxy.
     * @param LxContext client run-time transport listener context
     * @param TxContext client run-time transport transmitter context
     * @return true
     */
    virtual PROCESS_MESSAGE_TYPE(processMessage);
    virtual PARSE_RXBUFFER_TYPE(parseBuffer);

    /**
     * @brief Create the ClientConnectMessage object
     * @param msg msg structure to be fill up
     * @param LxContext client run-time transport listener context
     * @return true
     */
    static MAKE_RXBUFFER_TYPE(buildMessage);

    /**
     * @brief Add client's CONNECT_MSG type and its builder to transport layer.
     * @return true
     */
    static bool initMessage();

    void setClientState(ClientState *newClientState);
};
}

#endif /* CLIENT_CONNECT_MSG_HPP */
