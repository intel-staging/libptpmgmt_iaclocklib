/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. */

/** @file
 * @brief Clock Manager class unit tests
 *
 * @author Song Yoong Siang <yoong.siang.song@@intel.com>
 * @copyright © 2025 Intel Corporation.
 *
 */

#include "pub/clockmanager.h"
#include "client/client_state.hpp"
#include "client/timebase_state.hpp"

using namespace clkmgr;

// Used to define static members in ClientState class
DECLARE_STATIC(ClientState::m_clientID);
DECLARE_STATIC(ClientState::m_sessionId, InvalidSessionId);
DECLARE_STATIC(ClientState::m_connected, false);

// Used in ClockManager::connect()
bool ClientState::init()
{
    return true;
}

// Used in ClockManager::connect() and check_proxy_liveness()
bool ClientState::connect(uint32_t timeOut, timespec *lastConnectTime)
{
    m_connected = true;
    if(lastConnectTime != nullptr)
        clock_gettime(CLOCK_REALTIME, lastConnectTime);
    return true;
}

// Used in _subscribe()
bool TimeBaseStates::subscribe(size_t timeBaseIndex,
    const ClockSyncSubscription &newSub)
{
//    if(newSub.isPTPSubscriptionEnable()) {
//        const PTPClockSubscription &newPtpSub = newSub.getPtpSubscription();
//        if(!setPtpEventSubscription(timeBaseIndex, newPtpSub))
 //           return false;
  //  }
    // ToDo: Check whether system clock is available for subscription
//    if(newSub.isSysSubscriptionEnable()) {
 //       const SysClockSubscription &newSysSub = newSub.getSysSubscription();
//        if(!setSysEventSubscription(timeBaseIndex, newSysSub))
//            return false;
 //   }
 //   setSubscribed(timeBaseIndex, true);
    return true;
}

static ClockEventHandler ptpClockEventHandler(ClockEventHandler::PTPClock);
static ClockEventHandler sysClockEventHandler(ClockEventHandler::SysClock);

bool TimeBaseStates::getTimeBaseState(size_t timeBaseIndex,
    TimeBaseState &state)
{
  //  lock_guard<rtpi::mutex> lock(mtx);
  //  auto it = timeBaseStateMap.find(timeBaseIndex);
  //  if(it != timeBaseStateMap.end()) {
    //    state = it->second; // Copy the TimeBaseState object
        // Get the current state of the timebase
 //       PTPClockEvent ptp4lEventState = it->second.get_ptp4lEventState();
  //      SysClockEvent chronyEventState = it->second.get_chronyEventState();
        // Reset the Event Count
  //      ptpClockEventHandler.setOffsetInRangeEventCount(ptp4lEventState, 0);
   //     ptpClockEventHandler.setSyncedWithGmEventCount(ptp4lEventState, 0);
    //    ptpClockEventHandler.setGmChangedEventCount(ptp4lEventState, 0);
     //   ptpClockEventHandler.setAsCapableEventCount(ptp4lEventState, 0);
 //       ptpClockEventHandler.setCompositeEventCount(ptp4lEventState, 0);
  //      ptpClockEventHandler.setGmChanged(ptp4lEventState, false);
   //     it->second.set_ptpEventState(ptp4lEventState);
    //    sysClockEventHandler.setOffsetInRangeEventCount(chronyEventState, 0);
     //   it->second.set_chronyEventState(chronyEventState);
//        it->second.set_event_changed(false);
 //       return true;
  //  }
    // If timeBaseIndex is not found, return false
    return true;
}

const timespec &TimeBaseState::get_last_notification_time() const
{
    return last_notification_time;
}


bool TimeBaseState::get_subscribed() const
{
    return subscribed;
}

bool TimeBaseState::is_event_changed() const
{
    return event_changed;
}

const PTPClockEvent &TimeBaseState::get_ptp4lEventState() const
{
    return ptp4lEventState;
}

const SysClockEvent &TimeBaseState::get_chronyEventState() const
{
    return chronyEventState;
}

// static ClockManager &fetchSingleInstance()
TEST(ClockManagerTest, singleInstance) {
    ClockManager &cm1 = ClockManager::fetchSingleInstance();
    ClockManager &cm2 = ClockManager::fetchSingleInstance();
    EXPECT_EQ(&cm1, &cm2);
}

// 2. Connection Management
TEST(ClockManagerTest, ConnectAndDisconnectIdempotency) {
    printf("SiangDebug: start\n");
    EXPECT_TRUE(ClockManager::connect());
    printf("SiangDebug: start2\n");
    EXPECT_TRUE(ClockManager::connect()); // Should be idempotent
}
