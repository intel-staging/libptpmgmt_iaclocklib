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

void TimeBaseStates::setTimeBaseState(int timeBaseIndex,
    const ptp_event &newEvent)
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
    // Get the current state of the timebase
    PTPClockEvent ptp4lEventState = state.get_ptp4lEventState();;
    SysClockEvent chronyEventState = state.get_chronyEventState();
    // Update eventGMOffset
    if((eventSub & eventGMOffset) &&
        (newEvent.master_offset != ptp4lEventState.getClockOffset())) {
        PTPClockEventHandler::setClockOffset(ptp4lEventState,
            newEvent.master_offset);
        if(sub.in_range(thresholdGMOffset, ptp4lEventState.getClockOffset())) {
            if(!(ptp4lEventState.isOffsetInRange())) {
                PTPClockEventHandler::setOffsetInRange(ptp4lEventState, true);
                PTPClockEventHandler::setOffsetInRangeEventCount(
                    ptp4lEventState, 1);
                state.set_event_changed(true);
            }
        } else {
            if((ptp4lEventState.isOffsetInRange())) {
                PTPClockEventHandler::setOffsetInRange(ptp4lEventState, false);
                PTPClockEventHandler::setOffsetInRangeEventCount(
                    ptp4lEventState, 1);
                state.set_event_changed(true);
            }
        }
    }
    // Update eventSyncedToGM
    if((eventSub & eventSyncedToGM) &&
        (newEvent.synced_to_primary_clock !=
            ptp4lEventState.isSyncedToPrimaryClock())) {
        PTPClockEventHandler::setSyncedToPrimaryClock(ptp4lEventState,
            newEvent.synced_to_primary_clock);
        PTPClockEventHandler::setSyncedToGmEventCount(ptp4lEventState, 1);
        state.set_event_changed(true);
    }
    // Update eventGMChanged
    uint64_t sourceClockUUID = ptp4lEventState.getGmIdentity();
    uint8_t sourceClockUUIDBytes[8];
    for(int i = 0; i < 8; ++i) {
        sourceClockUUIDBytes[i] =
            static_cast<uint8_t>(sourceClockUUID >>(8 * (7 - i)));
    }
    if((eventSub & eventGMChanged) &&
        (memcmp(sourceClockUUIDBytes, newEvent.gm_identity,
                sizeof(newEvent.gm_identity)) != 0)) {
        uint64_t identity = 0;
        for(int i = 0; i < 8; ++i) {
            identity |=
                static_cast<uint64_t>(newEvent.gm_identity[i]) << (8 * (7 - i));
        }
        PTPClockEventHandler::setGmIdentity(ptp4lEventState, identity);
        PTPClockEventHandler::setGmChanged(ptp4lEventState, true);
        PTPClockEventHandler::setSyncedToGmEventCount(ptp4lEventState, 1);
        state.set_event_changed(true);
    }
    // Update eventASCapable
    if((eventSub & eventASCapable) &&
        (newEvent.as_capable != ptp4lEventState.isAsCapable())) {
        PTPClockEventHandler::setAsCapable(ptp4lEventState, newEvent.as_capable);
        PTPClockEventHandler::setAsCapableEventCount(ptp4lEventState, 1);
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
        PTPClockEventHandler::setCompositeEvent(ptp4lEventState,
            composite_event);
        PTPClockEventHandler::setCompositeEventCount(ptp4lEventState, 1);
        state.set_event_changed(true);
    }
    // Update notification timestamp
    uint64_t notification_timestamp = last_notification_time.tv_sec;
    notification_timestamp *= NSEC_PER_SEC;
    notification_timestamp += last_notification_time.tv_nsec;
    PTPClockEventHandler::setNotificationTimestamp(ptp4lEventState,
        notification_timestamp);
    // Update GM logSyncInterval
    PTPClockEventHandler::setSyncInterval(ptp4lEventState,
        newEvent.ptp4l_sync_interval);
    // Update Chrony clock offset
    if(newEvent.chrony_offset != chronyEventState.getClockOffset()) {
        SysClockEventHandler::setClockOffset(chronyEventState,
            newEvent.chrony_offset);
        if(sub.in_range(thresholdChronyOffset,
                chronyEventState.getClockOffset())) {
            if(!(chronyEventState.isOffsetInRange())) {
                SysClockEventHandler::setOffsetInRange(chronyEventState, true);
                SysClockEventHandler::setOffsetInRangeEventCount(
                    chronyEventState, 1);
                state.set_event_changed(true);
            }
        } else {
            if((chronyEventState.isOffsetInRange())) {
                SysClockEventHandler::setOffsetInRange(chronyEventState, false);
                SysClockEventHandler::setOffsetInRangeEventCount(
                    chronyEventState, 1);
                state.set_event_changed(true);
            }
        }
    }
    SysClockEventHandler::setGmIdentity(chronyEventState,
        newEvent.chrony_reference_id);
    SysClockEventHandler::setSyncInterval(chronyEventState,
        newEvent.polling_interval);
    state.set_chronyEventState(chronyEventState);
    state.set_ptpEventState(ptp4lEventState);
}
