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

#include <stdbool.h>
#include <stdint.h>

#include "pub/clkmgr/types_c.h"

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

/**
 * Structure to represent the current state of events.
 */
struct clkmgr_c_event_state {
    uint64_t notification_timestamp; /**< Timestamp for last notification */
    int64_t clock_offset; /**< Clock offset */
    uint8_t gm_identity[8]; /**< Primary clock UUID */
    bool offset_in_range; /**< Clock offset in range */
    bool synced_to_primary_clock; /**< Synced to primary clock */
    bool as_capable; /**< IEEE 802.1AS capable */
    bool gm_changed; /**< Primary clock UUID changed */
    bool composite_event; /**< Composite event */
    bool reserved[27]; /**< Reserved for future */
};

/**
 * Structure to represent the event counts.
 */
struct clkmgr_c_event_count {
    uint32_t offset_in_range_event_count; /**< Clock offset in range */
    uint32_t synced_to_gm_event_count; /**< Synced to primary clock */
    uint32_t as_capable_event_count; /**< IEEE 802.1AS capable */
    uint32_t gm_changed_event_count; /**< Primary clock UUID changed */
    uint32_t composite_event_count; /**< Composite event */
    uint32_t reserved[27]; /**< Reserved for future */
};

/** Pointer to the client structure */
typedef void *clkmgr_c_client_ptr;

/**
 * @brief Create a new client instance
 * @return Pointer to the new client instance
 */
clkmgr_c_client_ptr clkmgr_c_client_create();

/**
 * @brief Destroy a client instance
 * @param[in, out] client_ptr Pointer to the client instance
 */
void clkmgr_c_client_destroy(clkmgr_c_client_ptr client_ptr);

/**
 * @brief Connect the client
 * @param[in, out] client_ptr Pointer to the client instance
 * @return true on success, false on failure
 */
bool clkmgr_c_connect(clkmgr_c_client_ptr client_ptr);

/**
 * @brief Disconnect the client
 * @param[in, out] client_ptr Pointer to the client instance
 * @return true on success, false on failure
 */
bool clkmgr_c_disconnect(clkmgr_c_client_ptr client_ptr);

/**
 * @brief Subscribe to client events
 * @param[in, out] client_ptr Pointer to the client instance
 * @param[in] sub Subscription structure
 * @param[out] current_state Pointer to the current state structure
 * @return true on success, false on failure
 */
bool clkmgr_c_subscribe(clkmgr_c_client_ptr client_ptr,
    struct clkmgr_c_subscription sub,
    struct clkmgr_c_event_state *current_state);

/**
 * @brief Waits for a specified timeout period for any event changes
 * @param[in, out] client_ptr Pointer to the client instance
 * @param[in] timeout TThe timeout in seconds. If timeout is 0, the function
 * will check event changes once. If timeout is -1, the function will wait
 * until there is event changes occurs
 * @param[out] current_state Pointer to the current state structure
 * @param[out] current_count Pointer to the event count structure
 * @return true if there is event changes within the timeout period,
 *         and false otherwise
 */
int clkmgr_c_status_wait(clkmgr_c_client_ptr client_ptr, int timeout,
    struct clkmgr_c_event_state *current_state,
    struct clkmgr_c_event_count *current_count);

#ifdef __cplusplus
}
#endif

#endif /* CLKMGR_CLOCKMANAGER_C_H */
