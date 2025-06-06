/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. */

/** @file
 * @brief C wrapper for clock subscription
 *
 * @author Goh Wei Sheng <wei.sheng.goh@@intel.com>
 * @copyright © 2025 Intel Corporation.
 *
 */

#ifndef CLKMGR_SUBSCRIPTION_C_H
#define CLKMGR_SUBSCRIPTION_C_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handler for clock subscription (C API)
 */
typedef struct Clkmgr_Subscription Clkmgr_Subscription;

/**
 * @brief Opaque handler for clock sync subscription (C API)
 */
typedef struct Clkmgr_SyncSubscription Clkmgr_SyncSubscription;

/**
 * @brief Construct a PTP clock subscription object
 * @return Pointer to a new Clkmgr_Subscription object
 */
Clkmgr_Subscription *clkmgr_ptp_subscription(void);

/**
 * @brief Construct a system clock subscription object
 * @return Pointer to a new Clkmgr_Subscription object
 */
Clkmgr_Subscription *clkmgr_sys_subscription(void);

/**
 * @brief Construct a clock sync subscription object
 * @return Pointer to a new Clkmgr_SyncSubscription object
 */
Clkmgr_SyncSubscription *clkmgr_clock_sync_subscription(void);

/**
 * @brief Destroy a Clkmgr_Subscription object
 * @param[in] sub Pointer to the Clkmgr_Subscription object to destroy
 */
void clkmgr_subscription_destroy(Clkmgr_Subscription *sub);

/**
 * @brief Destroy a Clkmgr_SyncSubscription object
 * @param[in] sub Pointer to the Clkmgr_SyncSubscription object to destroy
 */
void clkmgr_clock_sync_subscription_destroy(Clkmgr_SyncSubscription *sub);

/**
 * Set the event mask
 * @param[in] sub Pointer of the Clkmgr_Subscription
 * @param[in] mask The new event mask to set
 * @return True if the event mask is set successfully, false otherwise
 * @note The event mask is a bitmask where each bit represents an event,
 * as defined by enum EventIndex
 */
bool clkmgr_set_event_mask(Clkmgr_Subscription *sub, uint32_t mask);

/**
 * Get the value of the event mask
 * @param[in] sub Pointer of the Clkmgr_SyncSubscription
 * @return The value of the event mask
 */
uint32_t clkmgr_get_event_mask(const Clkmgr_SyncSubscription *sub);

/**
 * Set the threshold of clock offset
 * @param[in] sub Pointer of the Clkmgr_Subscription
 * @param[in] threshold Threshold of clock offset
 * @return True if the offset threshold is set successfully, false otherwise
 * @note The threshold sets a symmetric range of clock offset
 */
bool clkmgr_set_clock_offset_threshold(Clkmgr_Subscription *sub,
    uint32_t threshold);

/**
 * Get the threshold of PTP clock offset
 * @param[in] sub Pointer of the Clkmgr_SyncSubscription
 * @return Threshold of PTP clock offset
 */
uint32_t clkmgr_get_ptp_clock_offset_threshold(const Clkmgr_SyncSubscription
    *sub);

/**
 * Get the threshold of SYS clock offset
 * @param[in] sub Pointer of the Clkmgr_SyncSubscription
 * @return Threshold of SYS clock offset
 */
uint32_t clkmgr_get_sys_clock_offset_threshold(const Clkmgr_SyncSubscription
    *sub);

/**
 * Set the composite event mask.
 * @param[in] sub Pointer of the Clkmgr_Subscription
 * @param[in] mask The new composite event mask to set.
 * @return True if the composite event mask is set successfully, false
 * otherwise.
 * @note The compositie event mask is a bitmask where each bit represents an
 * event, as defined by enum EventIndex and COMPOSITE_EVENT_ALL.
 */
bool clkmgr_set_composite_event_mask(Clkmgr_Subscription *sub, uint32_t mask);

/**
 * Get the value of the composite event mask.
 * @param[in] sub Pointer of the Clkmgr_SyncSubscription
 * @return The composite event mask.
 */
uint32_t clkmgr_get_composite_event_mask(const Clkmgr_SyncSubscription *sub);

/**
 * Enable the subscription of the PTP clock.
 * @param[in] sub Pointer of the Clkmgr_SyncSubscription
 */
void clkmgr_enable_ptp(Clkmgr_SyncSubscription *sub);

/**
 * Set the PTP clock subscription with a new PTPClockSubscription object
 * @param[in] sub Pointer of the Clkmgr_SyncSubscription
 * @param[in] ptp The new PTPClockSubscription object to update
 */
void clkmgr_set_ptp_subscription(Clkmgr_SyncSubscription *sub,
    const Clkmgr_Subscription *ptp);

/**
 * Enable the subscription of the system clock.
 * @param[in] sub Pointer of the Clkmgr_SyncSubscription
 */
void clkmgr_enable_sys(Clkmgr_SyncSubscription *sub);

/**
 * Set the system clock subscription with a new SysClockSubscription object
 * @param[in] sub Pointer of the Clkmgr_SyncSubscription
 * @param[in] sys The new SysClockSubscription object to update
 */
void clkmgr_set_sys_subscription(Clkmgr_SyncSubscription *sub,
    const Clkmgr_Subscription *sys);

#ifdef __cplusplus
}
#endif

#endif /* CLKMGR_SUBSCRIPTION_C_H */
