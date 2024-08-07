/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file client.hpp
 * @brief Common client infomation implementation
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 */

#ifndef PROXY_CLIENT
#define PROXY_CLIENT

#include <map>

#include <common/message.hpp>

namespace JClkLibProxy
{
class Client;
typedef std::shared_ptr<JClkLibProxy::Client> ClientX;

class Client
{
  public:
    typedef std::pair<JClkLibCommon::sessionId_t, ClientX> SessionMapping_t;
  private:
    static JClkLibCommon::sessionId_t nextSession;
    static std::map<SessionMapping_t::first_type, SessionMapping_t::second_type>
    SessionMap;
  public:
    static JClkLibCommon::sessionId_t CreateClientSession();
    static size_t GetSessionCount() { return SessionMap.size(); }
    static void RemoveClientSession(JClkLibCommon::sessionId_t sessionId) {
        SessionMap.erase(sessionId);
    }
    static JClkLibCommon::sessionId_t GetSessionIdAt(size_t index);
    static ClientX GetClientSession(JClkLibCommon::sessionId_t sessionId);
  private:
    std::unique_ptr<JClkLibCommon::TransportTransmitterContext> transmitContext;
  public:
    void set_transmitContext(decltype(transmitContext)::pointer context)
    { this->transmitContext.reset(context); }
    auto get_transmitContext() { return transmitContext.get(); }
};
}

#endif /* PROXY_CLIENT */
