/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Class to get and set the state of client-runtime
 *
 * @author Noor Azura Ahmad Tarmizi <noor.azura.ahmad.tarmizi@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#ifndef CLKMGR_CLIENT_STATE_H
#define CLKMGR_CLIENT_STATE_H

#include <atomic>
#include <string>

#include "pub/clkmgr/event_state.h"
#include "pub/clkmgr/subscription.h"
#include "pub/clkmgr/utility.h"

__CLKMGR_NAMESPACE_BEGIN

/**
 * @class ClientState
 * @brief Class to keep the current state of client-runtime
 */
class ClientState
{
  private:
    std::atomic_bool connected{false}; /**< Connection status */
    std::atomic_bool subscribed{false}; /**< Subscription status */
    sessionId_t m_sessionId = InvalidSessionId; /**< Session ID */
    TransportClientId clientID = {}; /**< Client ID */
    uint8_t m_ptp4l_id = 0; /**< PTP4L ID */
    clkmgr_event_state eventState = {}; /**< Event state */
    clkmgr_event_count eventStateCount = {}; /**< Event count */
    ClkMgrSubscription eventSub = {}; /**< Event subscription */
    struct timespec last_notification_time; /**< Last notification time */

  public:
    /**
     * @brief Default constructor
     */
    ClientState();

    /**
     * @brief Copy constructor
     * @param[in] newState Reference to the new state
     */
    ClientState(ClientState &newState);

    /**
     * @brief Set the client state
     * @param[in] newState Reference to the new state
     */
    void set_clientState(ClientState &newState);

    /**
     * @brief Get the connection status
     * @return true if connected, false otherwise
     */
    bool get_connected();

    /**
     * @brief Set the connection status
     * @param[in] state Connection status
     */
    void set_connected(bool state);

    /**
     * @brief Get the subscription status
     * @return true if subscribed, false otherwise
     */
    bool get_subscribed();

    /**
     * @brief Set the subscription status
     * @param[in] subscriptionState Subscription status
     */
    void set_subscribed(bool subscriptionState);

    /**
     * @brief Get the client ID
     * @return Client ID
     */
    TransportClientId get_clientID();

    /**
     * @brief Set the client ID
     * @param[in] cID Reference to the client ID
     */
    void set_clientID(TransportClientId &cID);

    /**
     * @brief Get the event state counts
     * @return Reference to the event state counts
     */
    clkmgr_event_count &get_eventStateCount();

    /**
     * @brief Get the event state
     * @return Reference to the event state
     */
    clkmgr_event_state &get_eventState();

    /**
     * @brief Set the event state counts
     * @param[in] eCount Event state counts
     */
    void set_eventStateCount(clkmgr_event_count eCount);

    /**
     * @brief Set the event state
     * @param[in] eState Event state
     */
    void set_eventState(clkmgr_event_state eState);

    /**
     * @brief Set the last notification time
     * @param[in] last_notification_time Last notification time
     */
    void set_last_notification_time(struct timespec last_notification_time);

    /**
     * @brief Get the last notification time
     * @return Last notification time
     */
    struct timespec get_last_notification_time();

    /**
     * @brief Convert the client state to a string
     * @return String representation of the client state
     */
    std::string toString();

    /**
     * @brief Get the event subscription
     * @return Reference to the event subscription
     */
    ClkMgrSubscription &get_eventSub();

    /**
    * @brief Get the constant reference to the session ID.
    * @return session ID.
    */
    sessionId_t get_sessionId() const { return m_sessionId; }

    /**
    * @brief Set the session ID.
    * @param[in] sessionId The new session ID to set.
    */
    void set_sessionId(sessionId_t sessionId) {
        m_sessionId = sessionId;
    }

    /**
    * @brief Set the ptp4l ID.
    * @param[in] ptp4l_id The new ptp4l ID to set.
    */
    void set_ptp4l_id(uint8_t ptp4l_id) {
        m_ptp4l_id = ptp4l_id;
    }

    /**
    * @brief Get the value of the ptp4l ID.
    * @return the ptp4l ID.
    */
    uint8_t get_ptp4l_id() const { return m_ptp4l_id; }
};

__CLKMGR_NAMESPACE_END

#endif /* CLKMGR_CLIENT_STATE_H */