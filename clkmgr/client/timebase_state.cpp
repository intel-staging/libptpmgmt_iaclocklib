/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. */

/** @file
 * @brief Set and get the timebase subscribe event state
 *
 * @author Noor Azura Ahmad Tarmizi <noor.azura.ahmad.tarmizi@@intel.com>
 * @copyright © 2025 Intel Corporation.
 *
 */

#include "client/timebase_state.hpp"
#include "client/msgq_tport.hpp"
#include "common/clkmgrtypes.hpp"
#include "common/message.hpp"
#include "common/print.hpp"
#include "common/transport.hpp"

#include <cstring>
#include <string>

__CLKMGR_NAMESPACE_USE;

using namespace std;

ClockEventHandler ptpClockEventHandler(ClockEventHandler::PTPClock);
ClockEventHandler sysClockEventHandler(ClockEventHandler::SysClock);

bool TimeBaseState::get_subscribed() const
{
    return subscribed;
}

void TimeBaseState::set_subscribed(bool subscriptionState)
{
    subscribed = subscriptionState;
}

bool TimeBaseState::is_event_changed() const
{
    return event_changed;
}

void TimeBaseState::set_event_changed(bool state)
{
    event_changed = state;
}

PTPClockEvent &TimeBaseState::get_ptp4lEventState()
{
    return ptp4lEventState;
}

SysClockEvent &TimeBaseState::get_chronyEventState()
{
    return chronyEventState;
}

void TimeBaseState::set_ptpEventState(const PTPClockEvent &ptpState)
{
    ptp4lEventState = ptpState;
}

void TimeBaseState::set_chronyEventState(const SysClockEvent &chronyState)
{
    chronyEventState = chronyState;
}

string TimeBaseState::toString() const
{
    return string("[TimeBaseState::eventState]") +
        " as_capable = " + to_string(eventState.as_capable) +
        " gm_changed = " + to_string(eventState.gm_changed) +
        " offset_in_range = " + to_string(eventState.offset_in_range) +
        " synced_to_primary_clock = " +
        to_string(eventState.synced_to_primary_clock) + "\n";
}

const ClkMgrSubscription &TimeBaseState::get_eventSub()
{
    return eventSub;
}

void TimeBaseState::set_eventSub(const ClkMgrSubscription &eSub)
{
    eventSub.set_ClkMgrSubscription(eSub);
}

void TimeBaseState::set_last_notification_time(const timespec &newTime)
{
    last_notification_time = newTime;
}

timespec TimeBaseState::get_last_notification_time() const
{
    return last_notification_time;
}

bool TimeBaseStates::getTimeBaseState(int timeBaseIndex, TimeBaseState &state)
{
    std::lock_guard<rtpi::mutex> lock(mtx);
    auto it = timeBaseStateMap.find(timeBaseIndex);
    if(it != timeBaseStateMap.end()) {
        state = it->second; // Copy the TimeBaseState object
        //it->second.set_ptpEventStateCount({}); // reset eventStateCount
        //it->second.set_chronyEventStateCount({}); // reset eventStateCount
        it->second.set_event_changed(false); // reset event_changed
        //it->second.get_ptp4lEventState().setGmChanged(false); // reset gm_changed
        return true;
    }
    // If timeBaseIndex is not found, return false
    return false;
}

void TimeBaseStates::setTimeBaseState(int timeBaseIndex, int clockType,
    const ClockEventBase &newEvent)
{
    std::lock_guard<rtpi::mutex> lock(mtx);
    auto &state = timeBaseStateMap[timeBaseIndex];
    // Update the notification timestamp
    timespec last_notification_time = {};
    if(clock_gettime(CLOCK_REALTIME, &last_notification_time) == -1)
        PrintDebug("Failed to update notification time.");
    else
        state.set_last_notification_time(last_notification_time);
    // Get a copy of subscription mask
    ClkMgrSubscription sub = state.get_eventSub();
    uint32_t eventSub = sub.get_event_mask();
    uint32_t composite_eventSub = sub.get_composite_event_mask();
    if(clockType == PTP_CLOCK) {
        const PTPClockEvent *ptpEvent =
            dynamic_cast<const PTPClockEvent *>(&newEvent);
        PTPClockEvent ptp4lEventState = state.get_ptp4lEventState();
        // Update eventGMOffset
        if((eventSub & eventGMOffset) &&
            (ptpEvent->getClockOffset() != ptp4lEventState.getClockOffset())) {
            ptpClockEventHandler.setClockOffset(ptp4lEventState,
                ptpEvent->getClockOffset());
            if(sub.in_range(thresholdGMOffset, ptp4lEventState.getClockOffset())) {
                if(!(ptp4lEventState.isOffsetInRange())) {
                    ptpClockEventHandler.setOffsetInRange(ptp4lEventState, true);
                    ptpClockEventHandler.setOffsetInRangeEventCount(
                        ptp4lEventState, 1);
                    state.set_event_changed(true);
                }
            } else {
                if((ptp4lEventState.isOffsetInRange())) {
                    ptpClockEventHandler.setOffsetInRange(ptp4lEventState, false);
                    ptpClockEventHandler.setOffsetInRangeEventCount(
                        ptp4lEventState, 1);
                    state.set_event_changed(true);
                }
            }
        }
        // Update eventSyncedToGM
        if((eventSub & eventSyncedToGM) && (ptpEvent->isSyncedToPrimaryClock() !=
                ptp4lEventState.isSyncedToPrimaryClock())) {
            ptpClockEventHandler.setSyncedToPrimaryClock(ptp4lEventState,
                ptpEvent->isSyncedToPrimaryClock());
            ptpClockEventHandler.setSyncedToGmEventCount(ptp4lEventState, 1);
            state.set_event_changed(true);
        }
        // Update eventGMChanged
        if((eventSub & eventGMChanged) &&
            (ptpEvent->getGmIdentity() != ptp4lEventState.getGmIdentity())) {
            ptpClockEventHandler.setGmIdentity(ptp4lEventState,
                ptpEvent->getGmIdentity());
            ptpClockEventHandler.setGmChanged(ptp4lEventState, true);
            ptpClockEventHandler.setSyncedToGmEventCount(ptp4lEventState, 1);
            state.set_event_changed(true);
        }
        // Update eventASCapable
        if((eventSub & eventASCapable) &&
            (ptpEvent->isAsCapable() != ptp4lEventState.isAsCapable())) {
            ptpClockEventHandler.setAsCapable(ptp4lEventState,
                ptpEvent->isAsCapable());
            ptpClockEventHandler.setAsCapableEventCount(ptp4lEventState, 1);
            state.set_event_changed(true);
        }
        // Update composite event
        bool composite_event = true;
        if(composite_eventSub & eventGMOffset)
            composite_event &= ptp4lEventState.isOffsetInRange();
        if(composite_eventSub & eventSyncedToGM)
            composite_event &= ptp4lEventState.isSyncedToPrimaryClock();
        if(composite_eventSub & eventASCapable)
            composite_event &= ptp4lEventState.isAsCapable();
        if(composite_eventSub &&
            (composite_event != ptp4lEventState.isCompositeEvent())) {
            ptpClockEventHandler.setCompositeEvent(ptp4lEventState,
                composite_event);
            ptpClockEventHandler.setCompositeEventCount(ptp4lEventState, 1);
            state.set_event_changed(true);
        }
        // Update notification timestamp
        uint64_t notification_timestamp = last_notification_time.tv_sec;
        notification_timestamp *= NSEC_PER_SEC;
        notification_timestamp += last_notification_time.tv_nsec;
        ptpClockEventHandler.setNotificationTimestamp(ptp4lEventState,
            notification_timestamp);
        // Update GM logSyncInterval
        ptpClockEventHandler.setSyncInterval(ptp4lEventState,
            ptpEvent->getSyncInterval());
        //state.set_ptpEventStateCount(ptpEventCount);
        state.set_ptpEventState(ptp4lEventState);
    }
    if(clockType == SYSTEM_CLOCK) {
        const SysClockEvent *chronyEvent =
            dynamic_cast<const SysClockEvent *>(&newEvent);
        // Get the current state of the timebase
        SysClockEvent chronyEventState = state.get_chronyEventState();
        // Update Chrony clock offset
        if(chronyEvent->getClockOffset() != chronyEventState.getClockOffset()) {
            sysClockEventHandler.setClockOffset(chronyEventState,
                chronyEvent->getClockOffset());
            if(sub.in_range(thresholdChronyOffset,
                    chronyEventState.getClockOffset())) {
                if(!(chronyEventState.isOffsetInRange())) {
                    sysClockEventHandler.setOffsetInRange(chronyEventState, true);
                    sysClockEventHandler.setOffsetInRangeEventCount(
                        chronyEventState, 1);
                    state.set_event_changed(true);
                }
            } else {
                if((chronyEventState.isOffsetInRange())) {
                    sysClockEventHandler.setOffsetInRange(chronyEventState, false);
                    sysClockEventHandler.setOffsetInRangeEventCount(
                        chronyEventState, 1);
                    state.set_event_changed(true);
                }
            }
        }
        sysClockEventHandler.setGmIdentity(chronyEventState,
            chronyEvent->getGmIdentity());
        sysClockEventHandler.setSyncInterval(chronyEventState,
            chronyEvent->getSyncInterval());
        state.set_chronyEventState(chronyEventState);
    }
}
