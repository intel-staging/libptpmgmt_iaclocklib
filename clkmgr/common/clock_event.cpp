/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Common clock event class
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "pub/clkmgr/event.h"

__CLKMGR_NAMESPACE_USE;

ClockEventBase::ClockEventBase()
{
    clockOffset = 0;
    offsetInRange = false;
    sourceClockUUID = {};
    sourceChanged = false;
    syncInterval = 0;
    notificationTimestamp = 0;
    compositeEvent = false;
    offsetInRangeCount = 0;
    sourceChangedCount = 0;
    compositeEventCount = 0;
}

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
    return sourceClockUUID;
}

bool ClockEventBase::isOffsetInRange() const
{
    return offsetInRange;
}

bool ClockEventBase::isGmChanged() const
{
    return sourceChanged;
}

int64_t ClockEventBase::getSyncInterval() const
{
    return syncInterval;
}

bool ClockEventBase::isCompositeEvent() const
{
    return compositeEvent;
}

uint32_t ClockEventBase::getOffsetInRangeEventCount() const
{
    return offsetInRangeCount;
}

uint32_t ClockEventBase::getGmChangedEventCount() const
{
    return sourceChangedCount;
}

uint32_t ClockEventBase::getCompositeEventCount() const
{
    return compositeEventCount;
}

PTPClockEvent::PTPClockEvent()
{
    syncedToPrimaryClock = false;
    asCapable = false;
    syncedToPrimaryClockCount = 0;
    asCapableCount = 0;
}

bool PTPClockEvent::isSyncedToPrimaryClock() const
{
    return syncedToPrimaryClock;
}

bool PTPClockEvent::isAsCapable() const
{
    return asCapable;
}

uint32_t PTPClockEvent::getSyncedToGmEventCount() const
{
    return syncedToPrimaryClockCount;
}

uint32_t PTPClockEvent::getAsCapableEventCount() const
{
    return asCapableCount;
}

ClockSyncBases::ClockSyncBases(PTPClockEvent &ptpClock, SysClockEvent &sysClock)
    : ptpClockSync(ptpClock), sysClockSync(sysClock), ptpAvailable(true),
      sysAvailable(true) {}

bool ClockSyncBases::havePTP() const
{
    return ptpAvailable;
}

PTPClockEvent &ClockSyncBases::getPtp() const
{
    return ptpClockSync;
}

bool ClockSyncBases::haveSys() const
{
    return sysAvailable;
}

SysClockEvent &ClockSyncBases::getSysClock() const
{
    return sysClockSync;
}

void ClockSyncBases::setPTPAvailability(bool available)
{
    ptpAvailable = available;
}

void ClockSyncBases::setSysAvailability(bool available)
{
    sysAvailable = available;
}

void ClockSyncBases::updatePTPClock(const PTPClockEvent &newPTPClock)
{
    ptpClockSync = newPTPClock;
}

void ClockSyncBases::updateSysClock(const SysClockEvent &newSysClock)
{
    sysClockSync = newSysClock;
}
