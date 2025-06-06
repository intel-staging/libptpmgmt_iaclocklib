/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. */

/** @file
 * @brief Client clock event class
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@@intel.com>
 * @copyright © 2025 Intel Corporation.
 *
 */

#include "pub/clkmgr/event.h"
#include "client/clock_event_handler.hpp"
#include "client/timebase_state.hpp"

__CLKMGR_NAMESPACE_USE;

uint64_t ClockEventBase::getNotificationTimestamp() const
{
    return notificationTimestamp;
}

int64_t ClockEventBase::getClockOffset() const
{
    return clockOffset;
}

uint64_t ClockEventBase::getGmIdentity() const
{
    return gmClockUUID;
}

bool ClockEventBase::isOffsetInRange() const
{
    return offsetInRange;
}

bool ClockEventBase::isGmChanged() const
{
    return gmChanged;
}

int64_t ClockEventBase::getSyncInterval() const
{
    return syncInterval;
}

uint32_t ClockEventBase::getOffsetInRangeEventCount() const
{
    return offsetInRangeCount;
}

uint32_t ClockEventBase::getGmChangedEventCount() const
{
    return gmChangedCount;
}

bool PTPClockEvent::isSyncedWithGm() const
{
    return syncedWithGm;
}

bool PTPClockEvent::isAsCapable() const
{
    return asCapable;
}

bool PTPClockEvent::isCompositeEventMet() const
{
    return compositeEvent;
}

uint32_t PTPClockEvent::getSyncedWithGmEventCount() const
{
    return syncedWithGmCount;
}

uint32_t PTPClockEvent::getAsCapableEventCount() const
{
    return asCapableCount;
}

uint32_t PTPClockEvent::getCompositeEventCount() const
{
    return compositeEventCount;
}

bool ClockSyncData::havePTP() const
{
    return ptpAvailable;
}

PTPClockEvent &ClockSyncData::getPtp()
{
    return ptpClockSync;
}

bool ClockSyncData::haveSys() const
{
    return sysAvailable;
}

SysClockEvent &ClockSyncData::getSysClock()
{
    return sysClockSync;
}

bool ClockSyncBaseHandler::updateAll(const TimeBaseState &state)
{
    // TODO: check ptp4l and chrony data is received
    clockSyncData.ptpClockSync = state.get_ptp4lEventState();
    clockSyncData.ptpAvailable = true;
    clockSyncData.sysClockSync = state.get_chronyEventState();
    clockSyncData.sysAvailable = true;
    return true;
}

extern "C" {

    // ClockEventBase
    int64_t clkmgr_get_clock_offset(const ClockEventBase_C *evt)
    {
        return evt->obj->getClockOffset();
    }
    bool clkmgr_is_offset_in_range(const ClockEventBase_C *evt)
    {
        return evt->obj->isOffsetInRange();
    }
    uint32_t clkmgr_get_offset_in_range_event_count(const ClockEventBase_C *evt)
    {
        return evt->obj->getOffsetInRangeEventCount();
    }
    int64_t clkmgr_get_sync_interval(const ClockEventBase_C *evt)
    {
        return evt->obj->getSyncInterval();
    }
    uint64_t clkmgr_get_gm_identity(const ClockEventBase_C *evt)
    {
        return evt->obj->getGmIdentity();
    }
    bool clkmgr_is_gm_changed(const ClockEventBase_C *evt)
    {
        return evt->obj->isGmChanged();
    }
    uint32_t clkmgr_get_gm_changed_event_count(const ClockEventBase_C *evt)
    {
        return evt->obj->getGmChangedEventCount();
    }
    uint64_t clkmgr_get_notification_timestamp(const ClockEventBase_C *evt)
    {
        return evt->obj->getNotificationTimestamp();
    }

    // PTPClockEvent
    bool clkmgr_is_synced_with_gm(const PTPClockEvent_C *evt)
    {
        return evt->obj->isSyncedWithGm();
    }
    uint32_t clkmgr_get_synced_with_gm_event_count(const PTPClockEvent_C *evt)
    {
        return evt->obj->getSyncedWithGmEventCount();
    }
    bool clkmgr_is_as_capable(const PTPClockEvent_C *evt)
    {
        return evt->obj->isAsCapable();
    }
    uint32_t clkmgr_get_as_capable_event_count(const PTPClockEvent_C *evt)
    {
        return evt->obj->getAsCapableEventCount();
    }
    bool clkmgr_is_composite_event_met(const PTPClockEvent_C *evt)
    {
        return evt->obj->isCompositeEventMet();
    }
    uint32_t clkmgr_get_composite_event_count(const PTPClockEvent_C *evt)
    {
        return evt->obj->getCompositeEventCount();
    }

    // ClockSyncData
    ClockSyncData_C *clkmgr_clock_sync_data_create()
    {
        auto *c = new ClockSyncData_C;
        c->obj = new ClockSyncData();
        return c;
    }
    void clkmgr_clock_sync_data_destroy(ClockSyncData_C *data)
    {
        delete data->obj;
        delete data;
    }
    bool clkmgr_have_ptp(const ClockSyncData_C *data)
    {
        return data->obj->havePTP();
    }
    PTPClockEvent_C *clkmgr_get_ptp(ClockSyncData_C *data)
    {
        auto *c = new PTPClockEvent_C;
        c->obj = &data->obj->getPtp();
        return c;
    }
    bool clkmgr_have_sys(const ClockSyncData_C *data)
    {
        return data->obj->haveSys();
    }
    SysClockEvent_C *clkmgr_get_sys(ClockSyncData_C *data)
    {
        auto *c = new SysClockEvent_C;
        c->obj = &data->obj->getSysClock();
        return c;
    }
} // extern "C"
