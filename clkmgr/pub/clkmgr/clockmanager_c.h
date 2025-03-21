/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief C wrapper for the Clock Manager APIs to set up client-runtime.
 *
 * @author Song Yoong Siang <yoong.siang.song@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#ifndef CLKMGR_CLOCKMANAGER_C_H
#define CLKMGR_CLOCKMANAGER_C_H

#include "pub/clkmgr/types_c.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * structure to hold the event mask, composite event mask, and thresholds
 * for events that require user-defined threshold (upper and lower limits).
 */
struct clkmgr_c_subscription {
    uint32_t event_mask; /**< Events subscription mask */
    uint32_t composite_event_mask; /**< Composite events mask */
    struct Clkmgr_Threshold threshold[CLKMGR_THRESHOLD_MAX]; /**< Limits */
};

/** Pointer to the client structure */
typedef void *clkmgr_c_client_ptr;

/**
 * Fetch the single client instance
 * @return Pointer to the single client instance
 */
clkmgr_c_client_ptr clkmgr_c_client_fetch();

/**
 * Connect the client
 * @param[in, out] client_ptr Pointer to the client instance
 * @return true on success, false on failure
 */
bool clkmgr_c_connect(clkmgr_c_client_ptr client_ptr);

/**
 * Disconnect the client
 * @param[in, out] client_ptr Pointer to the client instance
 * @return true on success, false on failure
 */
bool clkmgr_c_disconnect(clkmgr_c_client_ptr client_ptr);

/**
 * Get the time base configuration
 * @param[in, out] client_ptr Pointer to the client instance
 * @param[in] time_base_index Index of the time base to be retrieved
 * @param[out] cfg Pointer to the TimeBaseCfg structures
 * @return true on success, false on failure
 */
bool clkmgr_c_get_timebase_cfgs(clkmgr_c_client_ptr client_ptr,
    int time_base_index, struct Clkmgr_TimeBaseCfg *cfg);

/**
 * Get the size of the time base configurations
 * @param[in, out] client_ptr Pointer to the client instance
 * @return The size of the time base configurations
 */
size_t clkmgr_c_get_timebase_cfgs_size(clkmgr_c_client_ptr client_ptr);

/**
 * Subscribe to client events by name for the time base
 * @param[in, out] client_ptr Pointer to the client instance
 * @param[in] sub Subscription structure
 * @param[in] timeBaseName Name of the time base to be subscribed
 * @param[out] current_state Pointer to the current state structure
 * @return true on success, false on failure
 */
bool clkmgr_c_subscribe_by_name(clkmgr_c_client_ptr client_ptr,
    const struct clkmgr_c_subscription sub,
    char timeBaseName[CLKMGR_STRING_SIZE_MAX],
    struct Clkmgr_Event_state *current_state);

/**
 * Subscribe to client events
 * @param[in, out] client_ptr Pointer to the client instance
 * @param[in] sub Subscription structure
 * @param[in] time_base_index Index of the time base to be subscribed
 * @param[out] current_state Pointer to the current state structure
 * @return true on success, false on failure
 */
bool clkmgr_c_subscribe(clkmgr_c_client_ptr client_ptr,
    const struct clkmgr_c_subscription sub, int time_base_index,
    struct Clkmgr_Event_state *current_state);

/**
 * Waits for a specified timeout period for any event changes by name of the
 * time base
 * @param[in, out] client_ptr Pointer to the client instance
 * @param[in] timeout TThe timeout in seconds. If timeout is 0, the function
 * will check event changes once. If timeout is -1, the function will wait
 * until there is event changes occurs
 * @param[in] timeBaseName Name of the time base to be monitored
 * @param[out] current_state Pointer to the current state structure
 * @param[out] current_count Pointer to the event count structure
 * @return true if there is event changes within the timeout period,
 *         and false otherwise
 */
int clkmgr_c_status_wait_by_name(clkmgr_c_client_ptr client_ptr, int timeout,
    char timeBaseName[CLKMGR_STRING_SIZE_MAX],
    struct Clkmgr_Event_state *current_state,
    struct Clkmgr_Event_count *current_count);

/**
 * Waits for a specified timeout period for any event changes
 * @param[in, out] client_ptr Pointer to the client instance
 * @param[in] timeout TThe timeout in seconds. If timeout is 0, the function
 * will check event changes once. If timeout is -1, the function will wait
 * until there is event changes occurs
 * @param[in] time_base_index Index of the time base to be monitored
 * @param[out] current_state Pointer to the current state structure
 * @param[out] current_count Pointer to the event count structure
 * @return true if there is event changes within the timeout period,
 *         and false otherwise
 */
int clkmgr_c_status_wait(clkmgr_c_client_ptr client_ptr, int timeout,
    int time_base_index, struct Clkmgr_Event_state *current_state,
    struct Clkmgr_Event_count *current_count);

/**
 * Retrieve the time of the CLOCK_REALTIME
 * @param[in, out] client_ptr Pointer to the client instance
 * @param[out] ts timestamp of the CLOCK_REALTIME
 * @return 0 on success, -1 on failure
 */
int clkmgr_c_gettime(clkmgr_c_client_ptr client_ptr, struct timespec *ts);

#ifdef __cplusplus
}
#endif

#endif /* CLKMGR_CLOCKMANAGER_C_H */
