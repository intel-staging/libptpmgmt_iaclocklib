/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief The Clock Manager APIs to set up client-runtime.
 *
 * @author Christopher Hall <christopher.s.hall@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#ifndef CLOCKMANAGER_H
#define CLOCKMANAGER_H

#ifdef __cplusplus

#include "pub/clkmgr/event.h"
#include "pub/clkmgr/subscription.h"
#include "pub/clkmgr/timebase_configs.h"
#include <memory>

__CLKMGR_NAMESPACE_BEGIN

/**
 * Class to provide APIs to set up and manage the client-runtime.
 * @note the class is singelton
 */
class ClockManager
{
  private:
    ClockManager() = default;

  public:

    /**
     * Fetch single class object
     * @return reference to single class object
     */
    static ClockManager &FetchSingle();

    /**
     * Initialize the Clock Manager library
     * @return true on success
     */
    static bool init();

    /**
     * Establish connection between Client and Proxy
     * @return true on success, false on failure
     */
    static bool connect();

    /**
     * Remove the connection between Client and Proxy
     * @return true on success, false on failure
     */
    static bool disconnect();

    /**
     * Get the time base configurations
     * @return reference to the TimeBaseConfigurations object
     */
    static const TimeBaseConfigurations &get_timebase_cfgs();

    /**
     * This function allows a new subscription to be added for monitoring
     * clock synchronization events associated with a specific time base,
     * identified by its name. It updates the current state based on the
     * subscription details.
     *
     * @param[in] newSub Reference to the new subscription details.
     * @param[in] timeBaseName Name of the time base to be subscribed.
     * @param[out] clockSyncBases Reference to the map containing clock
     * synchronization states and count.
     * @return True on successful subscription, false on failure.
     */
    static bool subscribe_by_name(const ClkMgrSubscription &newSub,
        const std::string &timeBaseName,
        ClockSyncBases &clockSyncBases);

    /**
     * This function allows a new subscription to be added for monitoring
     * clock synchronization events associated with a specific time base.
     * It updates the current state based on the subscription details.
     *
     * @param[in] newSub Reference to the new subscription details.
     * @param[in] timeBaseIndex Index of the time base to be subscribed.
     * @param[out] clockSyncBases Reference to the object containing clock
     * synchronization states and count.
     * @return True on successful subscription, false on failure.
     */
    static bool subscribe(const ClkMgrSubscription &newSub, size_t timeBaseIndex,
        ClockSyncBases &clockSyncBases);

    /**
     * This function monitors the specified time base, identified by its name,
     * for event changes within a given timeout period. It updates the current
     * event state and count based on the observed changes.
     *
     * @param[in] timeout Timeout period in seconds.
     * @li Use 0 to check without waiting.
     * @li Use -1 to wait indefinitely until an event change occurs.
     * @param[in] timeBaseName Name of the time base to be monitored.
     * @param[out] clockSyncBases Reference to the map containing clock
     * synchronization states and count.
     * @return Result indicating the status of event changes.
     * @li 1 if an event change occurs within the timeout period.
     * @li 0 if no event changes occur.
     * @li -1 if the connection to the Clock Manager Proxy is lost.
     */
    static int status_wait_by_name(int timeout, const std::string &timeBaseName,
        ClockSyncBases &clockSyncBases);

    /**
     * This function monitors the specified time base for event changes
     * within a given timeout period. It updates the current event state
     * and count based on the observed changes.
     *
     * @param[in] timeout Timeout period in seconds.
     * @li Use 0 to check without waiting.
     * @li Use -1 to wait indefinitely until an event change occurs.
     * @param[in] timeBaseIndex Index of the time base to be monitored.
     * @param[out] clockSyncBases Reference to the map containing clock
     * synchronization states and count
     * @return Result indicating the status of event changes.
     * @li 1 if an event change occurs within the timeout period.
     * @li 0 if no event changes occur.
     * @li -1 if the connection to the Clock Manager Proxy is lost.
     */
    static int status_wait(int timeout, size_t timeBaseIndex,
        ClockSyncBases &clockSyncBases);

    /**
     * Retrieve the time of the CLOCK_REALTIME
     * @param[out] ts timestamp of the CLOCK_REALTIME
     * @return true on success
     */
    static bool gettime(timespec &ts);
};

__CLKMGR_NAMESPACE_END
#else /* __cplusplus */
#include "pub/clkmgr/clockmanager_c.h"
#endif /* __cplusplus */

#endif /* CLOCKMANAGER_H */
