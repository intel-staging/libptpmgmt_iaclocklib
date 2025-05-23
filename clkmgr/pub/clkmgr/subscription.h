/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief class, structures and enums used for events subsciption
 *
 * @author Christopher Hall <christopher.s.hall@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#ifndef CLKMGR_SUBSCRIPTION_H
#define CLKMGR_SUBSCRIPTION_H

#include "pub/clkmgr/utility.h"
#include "pub/clkmgr/types.h"
#include <array>

__CLKMGR_NAMESPACE_BEGIN

/**
 * Provides functionality to manage event masks and clock offset thresholds
 * for clock synchronization subscriptions.
 * It includes methods to set and retrieve these values, as well as to check
 * if a given clock offset is within the defined threshold.
 */
class ClockSubscriptionBase
{
  private:
    int32_t clockOffsetThreshold; /**< Clock offset threshold */
    uint32_t eventMask; /**< Event subscription mask */

  public:
    ClockSubscriptionBase() noexcept;

    /**
     * Set the event mask
     * @param[in] newEventMask The new event mask to set
     */
    void setEventMask(uint32_t newEventMask);

    /**
     * Get the value of the event mask
     * @return The value of the event mask
     */
    uint32_t getEventMask() const;

    /**
     * Define the threshold of clock offset
     * @param[in] threshold Threshold of clock offset
     * @note The threshold defines a symmetric range of clock offset
     */
    void defineClockOffsetThreshold(int32_t threshold);

    /**
     * Get the threshold of clock offset
     * @return Threshold of clock offset
     */
    int32_t getClockOffsetThreshold() const;

    /**
     * Check whether a given value is within predefined threshold
     * @param[in] value current clock offset value
     * @return True if the value is within the threshold range centered around
     * zero, and false otherwise
     */
    bool clockOffsetInRange(int32_t value) const;
};

/**
 * Derived from ClockSubscriptionBase and is used to manage
 * subscriptions related to PTP clock synchronization.
 */
class PTPClockSubscription : public ClockSubscriptionBase
{
  private:
    uint32_t m_composite_event_mask; /**< Composite event mask */

  public:
    /**
     * Initialize a PTPClockSubscription object with default settings.
     * It is marked noexcept to indicate that it does not throw exceptions.
     */
    PTPClockSubscription() noexcept;

    /**
     * Set the composite event mask.
     * @param[in] composite_event_mask The new composite event mask to set.
     */
    void set_composite_event_mask(uint32_t composite_event_mask);

    /**
     * Get the value of the composite event mask.
     * @return the composite event mask.
     */
    uint32_t get_composite_event_mask() const;
};

/**
 * Derived from ClockSubscriptionBase and is used to manage
 * subscriptions related to system clock synchronization.
 */
class SysClockSubscription : public ClockSubscriptionBase
{
  public:
    /**
     * Initialize a SysClockSubscription object with default settings.
     * It is marked noexcept to indicate that it does not throw exceptions.
     */
    SysClockSubscription() noexcept;
};

/**
 * Provide accessors to retrieve these subscriptions which encapsulates both
 * PTPClockSubscription and SysClockSubscription objects.
 */
class ClockSyncSubscription
{
  public:
    ClockSyncSubscription();

    /**
     * Check if the PTP clock is subscribed
     * @return True if the PTP clock is subscribed, false otherwise
     */
    bool subscribedPTP() const;

    /**
     * Set the subscription of the system clock.
     * @param[in] subscribed True if the system clock is subscribed,
     * false otherwise.
     */
    void setPTPSubscription(bool subscribed);

    /**
     * Retrieve the PTP clock manager subscription.
     * @return A constant reference to the PTP clock manager subscription.
     */
    const PTPClockSubscription &getPtpSubscription() const;

    /**
     * Update the PTP clock subscription with a new PTPClockSubscription object
     * @param[in] newPtpSub The new PTPClockSubscription object to update
     */
    void updatePtpSubscription(const PTPClockSubscription &newPtpSub);

    /**
     * Check if the system clock is subscribed
     * @return True if the system clock is subscribed, false otherwise
     */
    bool subscribedSys() const;

    /**
     * Set the subscription of the system clock.
     * @param[in] subscribed True if the system clock is subscribed,
     * false otherwise.
     */
    void setSysSubscription(bool subscribed);

    /**
     * Retrieve the system clock manager subscription.
     * @return A constant reference to the system clock manager subscription.
     */
    const SysClockSubscription &getSysSubscription() const;

    /**
     * Update the PTP clock subscription with a new SysClockSubscription object
     * @param[in] newSysSub The new SysClockSubscription object to update
     */
    void updateSysSubscription(const SysClockSubscription &newSysSub);

  private:
    PTPClockSubscription ptpSubscription; /**< PTP clock subscription */
    SysClockSubscription sysSubscription; /**< System clock subscription */
    bool ptpSubscribed;
    bool sysSubscribed;
};

__CLKMGR_NAMESPACE_END

#endif /* CLKMGR_SUBSCRIPTION_H */
