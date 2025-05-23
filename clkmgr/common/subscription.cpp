/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief PTP event subscription.
 *
 * @author Christopher Hall <christopher.s.hall@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "pub/clkmgr/subscription.h"

__CLKMGR_NAMESPACE_USE;

ClockSubscriptionBase::ClockSubscriptionBase() noexcept : eventMask(0) {}

void ClockSubscriptionBase::defineClockOffsetThreshold(int32_t threshold)
{
    clockOffsetThreshold = threshold;
}

int32_t ClockSubscriptionBase::getClockOffsetThreshold() const
{
    return clockOffsetThreshold;
}

void ClockSubscriptionBase::setEventMask(uint32_t newEventMask)
{
    eventMask = newEventMask;
}

uint32_t ClockSubscriptionBase::getEventMask() const
{
    return eventMask;
}

void PTPClockSubscription::set_composite_event_mask(uint32_t
    composite_event_mask)
{
    m_composite_event_mask = composite_event_mask;
}

uint32_t PTPClockSubscription::get_composite_event_mask() const
{
    return m_composite_event_mask;
}

bool ClockSubscriptionBase::clockOffsetInRange(int32_t value) const
{
    return value >= -static_cast<int32_t>(clockOffsetThreshold) &&
        value <= static_cast<int32_t>(clockOffsetThreshold);
}

PTPClockSubscription::PTPClockSubscription() noexcept
    : m_composite_event_mask(0) {}

SysClockSubscription::SysClockSubscription() noexcept
    : ClockSubscriptionBase() {}

ClockSyncSubscription::ClockSyncSubscription()
{
    ptpSubscribed = false;
    sysSubscribed = false;
}

bool ClockSyncSubscription::subscribedPTP() const
{
    return ptpSubscribed;
}

void ClockSyncSubscription::setPTPSubscription(bool subscribed)
{
    ptpSubscribed = subscribed;
}

const PTPClockSubscription &ClockSyncSubscription::getPtpSubscription() const
{
    return ptpSubscription;
}

void ClockSyncSubscription::updatePtpSubscription(
    const PTPClockSubscription &newPtpSub)
{
    ptpSubscription = newPtpSub;
}

bool ClockSyncSubscription::subscribedSys() const
{
    return sysSubscribed;
}

void ClockSyncSubscription::setSysSubscription(bool subscribed)
{
    sysSubscribed = subscribed;
}

const SysClockSubscription &ClockSyncSubscription::getSysSubscription() const
{
    return sysSubscription;
}

void ClockSyncSubscription::updateSysSubscription(
    const SysClockSubscription &newSysSub)
{
    sysSubscription = newSysSub;
}
