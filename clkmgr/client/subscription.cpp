/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Clock events subscription.
 *
 * @author Christopher Hall <christopher.s.hall@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "pub/clkmgr/subscription.h"
#include "common/print.hpp"

__CLKMGR_NAMESPACE_USE;

#define GET_SET(cls, type, func, var)\
    void cls::set##func(type _v) { var = _v; } \
    type cls::get##func() const { return var; }

#define GET(cls, type, func, var)\
    type cls::get##func() const { return var; }

GET_SET(ClockSubscriptionBase, uint32_t,
    ClockOffsetThreshold, clockOffsetThreshold)
GET(ClockSubscriptionBase, uint32_t, EventMask, eventMask)
GET(PTPClockSubscription, uint32_t,
    CompositeEventMask, m_composite_event_mask)

bool ClockSubscriptionBase::setEventMask(uint32_t newEventMask)
{
    if(newEventMask >= eventLast) {
        PrintDebug("Event mask contains invalid bits.");
        return false;
    }
    eventMask = newEventMask;
    return true;
}

bool PTPClockSubscription::setCompositeEventMask(uint32_t composite_event_mask)
{
    if(composite_event_mask & ~COMPOSITE_EVENT_ALL) {
        PrintDebug("Composite event mask contains invalid bits.");
        return false;
    }
    m_composite_event_mask = composite_event_mask;
    return true;
}

PTPClockSubscription::PTPClockSubscription() noexcept
    : m_composite_event_mask(0)
{
}

SysClockSubscription::SysClockSubscription() noexcept
    : ClockSubscriptionBase()
{
}

ClockSyncSubscription::ClockSyncSubscription()
{
    ptpSubscribed = false;
    sysSubscribed = false;
}

#define OBJ_FUNC(Nm, NM, nm)\
    void ClockSyncSubscription::enable##Nm##Subscription() {\
        nm##Subscribed = true; }\
    void ClockSyncSubscription::disable##Nm##Subscription() {\
        nm##Subscribed = false; }\
    bool ClockSyncSubscription::is##NM##SubscriptionEnable() const {\
        return nm##Subscribed; }\
    void ClockSyncSubscription::set##Nm##Subscription(\
        const NM##ClockSubscription &_o) {\
        nm##Subscription = _o; }\
    const NM##ClockSubscription &ClockSyncSubscription::\
    get##Nm##Subscription() const {\
        return nm##Subscription; }

OBJ_FUNC(Ptp, PTP, ptp)
OBJ_FUNC(Sys, Sys, sys)

extern "C" {

    struct Clkmgr_Subscription {
        // Use base pointer for polymorphism
        ClockSubscriptionBase *base;
        // For PTP and SYS, derived pointer is needed for composite event mask
        PTPClockSubscription *ptp;
        SysClockSubscription *sys;
        bool is_ptp;
    };

    struct Clkmgr_SyncSubscription {
        ClockSyncSubscription *sync;
    };

    Clkmgr_Subscription *clkmgr_ptp_subscription(void)
    {
        auto *sub = new Clkmgr_Subscription;
        sub->ptp = new PTPClockSubscription();
        sub->base = sub->ptp;
        sub->sys = nullptr;
        sub->is_ptp = true;
        return sub;
    }

    Clkmgr_Subscription *clkmgr_sys_subscription(void)
    {
        auto *sub = new Clkmgr_Subscription;
        sub->sys = new SysClockSubscription();
        sub->base = sub->sys;
        sub->ptp = nullptr;
        sub->is_ptp = false;
        return sub;
    }

    Clkmgr_SyncSubscription *clkmgr_clock_sync_subscription(void)
    {
        auto *sub = new Clkmgr_SyncSubscription;
        sub->sync = new ClockSyncSubscription();
        return sub;
    }

    void clkmgr_subscription_destroy(Clkmgr_Subscription *sub)
    {
        if(!sub)
            return;
        if(sub->ptp)
            delete sub->ptp;
        if(sub->sys)
            delete sub->sys;
        delete sub;
    }

    void clkmgr_clock_sync_subscription_destroy(Clkmgr_SyncSubscription *sub)
    {
        if(!sub)
            return;
        if(sub->sync)
            delete sub->sync;
        delete sub;
    }

    bool clkmgr_set_event_mask(Clkmgr_Subscription *sub, uint32_t mask)
    {
        if(sub && sub->base)
            sub->base->setEventMask(mask);
        return false;
    }

    uint32_t clkmgr_get_event_mask(const Clkmgr_SyncSubscription *sub)
    {
        if(sub && sub->sync)
            return sub->sync->getPtpSubscription().getEventMask();
        return 0;
    }

    bool clkmgr_set_composite_event_mask(Clkmgr_Subscription *sub, uint32_t mask)
    {
        if(sub && sub->is_ptp && sub->ptp)
            sub->ptp->setCompositeEventMask(mask);
        return false;
    }

    uint32_t clkmgr_get_composite_event_mask(const Clkmgr_SyncSubscription *sub)
    {
        if(sub && sub->sync)
            return sub->sync->getPtpSubscription().getCompositeEventMask();
        return 0;
    }

    bool clkmgr_set_clock_offset_threshold(Clkmgr_Subscription *sub,
        uint32_t threshold)
    {
        if(sub && sub->base)
            sub->base->setClockOffsetThreshold(threshold);
        return false;
    }

    uint32_t clkmgr_get_ptp_clock_offset_threshold(const Clkmgr_SyncSubscription
        *sub)
    {
        if(sub && sub->sync)
            return sub->sync->getPtpSubscription().getClockOffsetThreshold();
        return 0;
    }

    uint32_t clkmgr_get_sys_clock_offset_threshold(const Clkmgr_SyncSubscription
        *sub)
    {
        if(sub && sub->sync)
            return sub->sync->getSysSubscription().getClockOffsetThreshold();
        return 0;
    }

    void clkmgr_enable_ptp(Clkmgr_SyncSubscription *sub)
    {
        if(sub && sub->sync)
            sub->sync->enablePtpSubscription();
    }

    void clkmgr_enable_sys(Clkmgr_SyncSubscription *sub)
    {
        if(sub && sub->sync)
            sub->sync->enableSysSubscription();
    }

    void clkmgr_set_ptp_subscription(Clkmgr_SyncSubscription *sub,
        const Clkmgr_Subscription *ptp)
    {
        if(sub && sub->sync && ptp && ptp->ptp)
            sub->sync->setPtpSubscription(*ptp->ptp);
    }

    void clkmgr_set_sys_subscription(Clkmgr_SyncSubscription *sub,
        const Clkmgr_Subscription *sys)
    {
        if(sub && sub->sync && sys && sys->sys)
            sub->sync->setSysSubscription(*sys->sys);
    }

} // extern "C"
