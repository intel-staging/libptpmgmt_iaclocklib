/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief C wrapper for the Clock Manager APIs to set up client-runtime.
 *
 * @author Song Yoong Siang <yoong.siang.song@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#ifndef CLKMGR_CLOCKMANAGER_C_H
#define CLKMGR_CLOCKMANAGER_C_H

#include "pub/clkmgr/types_c.h"
#include "pub/clkmgr/timebase_configs_c.h"
#include "pub/clkmgr/subscription_c.h"
#include "pub/clkmgr/event_c.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Connect the client
 * @return true on success, false on failure
 */
bool clkmgr_connect();

/**
 * Disconnect the client
 * @return true on success, false on failure
 */
bool clkmgr_disconnect();

/**
 * Get the size of the time base configurations
 * @return The size of the time base configurations
 */
size_t clkmgr_get_timebase_cfgs_size();

/**
 * Subscribe to client events by name for the time base
 * @param[in] sub Pointer to the C wrapper of SyncSubscription
 * @param[in] time_base_name Name of the time base to be subscribed
 * @param[out] data Pointer to the C wrapper of ClockSyncData
 * @return true on success, false on failure
 */
bool clkmgr_subscribe_by_name(const Clkmgr_SyncSubscription *sub,
    const char *time_base_name, ClockSyncData_C *data);

/**
 * Subscribe to client events
 * @param[in] sub Pointer to the C wrapper of SyncSubscription
 * @param[in] time_base_index Index of the time base to be subscribed
 * @param[out] data Pointer to the C wrapper of ClockSyncData
 * @return true on success, false on failure
 */
bool clkmgr_subscribe(const Clkmgr_SyncSubscription *sub,
    size_t time_base_index, ClockSyncData_C *data);

/**
 * Waits for a specified timeout period for any event changes by name of the
 * time base
 * @param[in] timeout The timeout in seconds. If timeout is 0, the function
 * will check event changes once. If timeout is -1, the function will wait
 * until there is event changes occurs
 * @param[in] time_base_name Name of the time base to be monitored
 * @param[out] data Pointer to the C wrapper of ClockSyncData
 * @return true if there is event changes within the timeout period,
 *         and false otherwise
 */
int clkmgr_status_wait_by_name(int timeout, const char *time_base_name,
    ClockSyncData_C *data);

/**
 * Waits for a specified timeout period for any event changes
 * @param[in] timeout TThe timeout in seconds. If timeout is 0, the function
 * will check event changes once. If timeout is -1, the function will wait
 * until there is event changes occurs
 * @param[in] time_base_index Index of the time base to be monitored
 * @param[out] data Pointer to the C wrapper of ClockSyncData
 * @return true if there is event changes within the timeout period,
 *         and false otherwise
 */
int clkmgr_status_wait(int timeout, size_t time_base_index,
    ClockSyncData_C *data);

/**
 * Retrieve the time of the CLOCK_REALTIME
 * @param[out] ts timestamp of the CLOCK_REALTIME
 * @return true on success
 */
bool clkmgr_gettime(struct timespec *ts);

#ifdef __cplusplus
}
#endif

#endif /* CLKMGR_CLOCKMANAGER_C_H */
