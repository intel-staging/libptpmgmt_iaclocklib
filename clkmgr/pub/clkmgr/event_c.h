/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. */

/** @file
 * @brief C wrapper for clock event
 *
 * @author Goh Wei Sheng <wei.sheng.goh@@intel.com>
 * @copyright © 2025 Intel Corporation.
 *
 */

#ifndef EVENT_C_H
#define EVENT_C_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handler for base clock event (C API)
 */
typedef struct ClockEventBase_C ClockEventBase_C;

/**
 * @brief Opaque handler for PTP clock event (C API)
 */
typedef struct PTPClockEvent_C PTPClockEvent_C;

/**
 * @brief Opaque handler for system clock event (C API)
 */
typedef struct SysClockEvent_C SysClockEvent_C;

/**
 * @brief Opaque handler for clock synchronization data (C API)
 */
typedef struct ClockSyncData_C ClockSyncData_C;

/**
 * Get the clock offset in nanosecond
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return Clock offset in nanosecond
 */
int64_t clkmgr_get_clock_offset(const ClockEventBase_C *evt);

/**
 * Check if the clock offset is in-range
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return True if the clock offset is in-range, false otherwise
 * @note The range of clock offset is defined by user during subscription
 */
bool clkmgr_is_offset_in_range(const ClockEventBase_C *evt);

/**
 * Get the count of clock offset in-range event
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return The count of clock offset in-range event
 * @note This count indicates the number of times the clock offset has
 *  transitioned from in-range to out-of-range or vice versa since the last
 *  call to statusWait() or statusWaitByName()
 */
uint32_t clkmgr_get_offset_in_range_event_count(const ClockEventBase_C *evt);

/**
 * Get the synchronization interval in microsecond
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return Synchronization interval in microsecond
 * @note This interval indicates the time between clock update messages send
 *  to the local service that synchronizing clock. This interval specify the
 *  time the synchronization protocol may need to react for a change in the
 *  grandmaster.
 */
int64_t clkmgr_get_sync_interval(const ClockEventBase_C *evt);

/**
 * Get the grandmaster clock identity
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return Grandmaster clock identity
 * @note The grandmaster is the source clock for the PTP domain (network),
 *  whom all the clocks will synchronize to.
 */
uint64_t clkmgr_get_gm_identity(const ClockEventBase_C *evt);

/**
 * Check if the grandmaster has changed
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return True if the grandmaster has changed, false otherwise
 * @note This boolean variable indicates if the grandmaster has changed
 *  since the last call of statusWait() or statusWaitByName()
 */
bool clkmgr_is_gm_changed(const ClockEventBase_C *evt);

/**
 * Get the count of grandmaster changed event
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return The count of grandmaster changed event
 * @note This count indicates how many times the grandmaster has changed
 *  since the last call to statusWait() or statusWaitByName()
 */
uint32_t clkmgr_get_gm_changed_event_count(const ClockEventBase_C *evt);

/**
 * Get the notification timestamp in nanosecond
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return Notification timestamp in nanosecond
 * @note This timestamp indicates to the most recent time in CLOCK_REALTIME
 *  when the Client received a clock sync data notification from the Proxy
 */
uint64_t clkmgr_get_notification_timestamp(const ClockEventBase_C *evt);

/**
 * Check if the PTP clock is synchronized with a grandmaster
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return True if the clock is synchronized with a grandmaster, false if
 *  otherwise
 */
bool clkmgr_is_synced_with_gm(const PTPClockEvent_C *evt);

/**
 * Get the count of clock synced with grandmaster event
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return The count of clock synced with grandmaster event
 * @note This count indicates the number of times the clock has transitioned
 *  from synced with grandmaster to out-of-sync or vice versa since the last
 *  call to statusWait() or statusWaitByName()
 */
uint32_t clkmgr_get_synced_with_gm_event_count(const PTPClockEvent_C *evt);

/**
 * Check if the clock is an IEEE 802.1AS capable
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return True if the clock is an IEEE 802.1AS capable, false otherwise
 */
bool clkmgr_is_as_capable(const PTPClockEvent_C *evt);

/**
 * Get the count of IEEE 802.1AS capable event
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return The count of IEEE 802.1AS capable event
 * @note This count indicates the number of times the clock has transitioned
 *  from non capable to capable or vice versa since the last call to
 *  statusWait() or statusWaitByName()
 */
uint32_t clkmgr_get_as_capable_event_count(const PTPClockEvent_C *evt);

/**
 * Check if all the conditions in the subscribed composite event are met
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return True if all the conditions are met, false otherwise
 * @note Condition of composite event is defined by the user during
 *  subsciption
 */
bool clkmgr_is_composite_event_met(const PTPClockEvent_C *evt);

/**
 * Get the count of composite event
 * @param[in] evt Pointer of the ClockEventBase_C
 * @return The count of composite event
 * @note This count indicates the number of times the condition of composite
 *  event has transitioned from not met to met or vice versa since the last
 *  call to statusWait() or statusWaitByName()
 */
uint32_t clkmgr_get_composite_event_count(const PTPClockEvent_C *evt);

/**
 * @brief Construct a ClockSyncData_C object
 */
ClockSyncData_C *clkmgr_clock_sync_data_create();

/**
 * Destroy a ClockSyncData_C object
 * @param[in] data Pointer to the ClockSyncData_C object to destroy
 */
void clkmgr_clock_sync_data_destroy(ClockSyncData_C *data);

/**
 * Check if the PTP clock is available
 * @param[in] data Pointer of the ClockSyncData_C
 * @return True if the PTP clock is available, false otherwise
 */
bool clkmgr_have_ptp(const ClockSyncData_C *data);

/**
 * Retrieve the PTP clock object
 * @param[in] data Pointer of the ClockSyncData_C
 * @return Reference to the PTPClockEvent object
 */
PTPClockEvent_C *clkmgr_get_ptp(ClockSyncData_C *data);

/**
 * Check if the system clock is available
 * @param[in] data Pointer of the ClockSyncData_C
 * @return True if the system clock is available, false otherwise
 */
bool clkmgr_have_sys(const ClockSyncData_C *data);

/**
 * Retrieve the system clock object
 * @param[in] data Pointer of the ClockSyncData_C
 * @return Reference to the SysClockEvent object
 */
SysClockEvent_C *clkmgr_get_sys(ClockSyncData_C *data);

#ifdef __cplusplus
}
#endif

#endif /* EVENT_C_H */
