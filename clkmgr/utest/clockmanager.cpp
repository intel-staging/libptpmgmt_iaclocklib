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
#include "common/timebase.hpp"

using namespace clkmgr;

// Used to define static members in ClientState class
DECLARE_STATIC(ClientState::m_clientID);
DECLARE_STATIC(ClientState::m_sessionId, InvalidSessionId);
DECLARE_STATIC(ClientState::m_connected, false);

// Used in ClockManager::connect() to setup listener and transmitter queues
bool ClientState::init()
{
    m_clientID = "/clkmgr.112233";
    return true;
}

// Used in ClockManager::connect() and check_proxy_liveness() to send a connect
// message to the proxy and wait for a reply
bool utest_connected_with_proxy = true;
bool ClientState::connect(uint32_t timeOut, timespec *lastConnectTime)
{
    // Simulate failure for testing
    if(!utest_connected_with_proxy) {
        m_connected = false;
        return false; // Simulate failure for testing
    }
    // Simulate a successful connection
    m_connected = true;
    if(lastConnectTime != nullptr)
        clock_gettime(CLOCK_REALTIME, lastConnectTime);
    return true;
}

// Used to define functions in TimeBaseState class
void TimeBaseState::set_subscribed(bool subscriptionState)
{
    subscribed = subscriptionState;
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
void TimeBaseState::set_event_changed(bool state)
{
    event_changed = state;
}
void TimeBaseState::set_ptpEventState(const PTPClockEvent &ptpState)
{
    ptp4lEventState = ptpState;
}
void TimeBaseState::set_chronyEventState(const SysClockEvent &chronyState)
{
    chronyEventState = chronyState;
}
void TimeBaseState::set_last_notification_time(const timespec &newTime)
{
    last_notification_time = newTime;
}

// Used in _subscribe() to send a subscribe message to the proxy and wait for
// a confirmation reply
bool utest_subscribed_with_proxy = true;
bool TimeBaseStates::subscribe(size_t timeBaseIndex,
    const ClockSyncSubscription &newSub)
{
    // Check whether connection between Proxy and Client is established or not
    if(!ClientState::get_connected()) {
        return false;
    }
    // Check whether requested timeBaseIndex is valid or not
    if(!TimeBaseConfigurations::isTimeBaseIndexPresent(timeBaseIndex)) {
        return false;
    }
    // Simulate failure for testing
    if(!utest_subscribed_with_proxy){
        setSubscribed(timeBaseIndex, false);
        return false;
    }
    // Simulate a successful subscription
    setSubscribed(timeBaseIndex, true);
    return true;
}

// Used in _subscribe() and _statusWait() to get the current clock sync data
// and reset the event counts
static ClockEventHandler ptpClockEventHandler(ClockEventHandler::PTPClock);
static ClockEventHandler sysClockEventHandler(ClockEventHandler::SysClock);
bool TimeBaseStates::getTimeBaseState(size_t timeBaseIndex,
    TimeBaseState &state)
{
    auto it = timeBaseStateMap.find(timeBaseIndex);
    if(it != timeBaseStateMap.end()) {
        state = it->second; // Copy the TimeBaseState object
        // Get the current state of the timebase
        PTPClockEvent ptp4lEventState = it->second.get_ptp4lEventState();
        SysClockEvent chronyEventState = it->second.get_chronyEventState();
        // Reset the Event Count
        ptpClockEventHandler.setOffsetInRangeEventCount(ptp4lEventState, 0);
        ptpClockEventHandler.setSyncedWithGmEventCount(ptp4lEventState, 0);
        ptpClockEventHandler.setGmChangedEventCount(ptp4lEventState, 0);
        ptpClockEventHandler.setAsCapableEventCount(ptp4lEventState, 0);
        ptpClockEventHandler.setCompositeEventCount(ptp4lEventState, 0);
        ptpClockEventHandler.setGmChanged(ptp4lEventState, false);
        it->second.set_ptpEventState(ptp4lEventState);
        sysClockEventHandler.setOffsetInRangeEventCount(chronyEventState, 0);
        it->second.set_chronyEventState(chronyEventState);
        it->second.set_event_changed(false);
        return true;
    }
    // If timeBaseIndex is not found, return false
    return false;
}

// Create dummy timebase state for testing
void TimeBaseStates::setTimeBaseState(size_t timeBaseIndex,
    const ptp_event &newEvent)
{
    auto &state = timeBaseStateMap[1];
    // Update the notification timestamp
    timespec last_notification_time = {};
    clock_gettime(CLOCK_REALTIME, &last_notification_time);
    state.set_last_notification_time(last_notification_time);
    state.set_event_changed(true);
    // Get the current state of the timebase
    PTPClockEvent ptp4lEventState = state.get_ptp4lEventState();
    SysClockEvent chronyEventState = state.get_chronyEventState();
    // Update eventGMOffset
    ptpClockEventHandler.setClockOffset(ptp4lEventState, 23);
    ptpClockEventHandler.setOffsetInRange(ptp4lEventState, true);
    ptpClockEventHandler.setOffsetInRangeEventCount(ptp4lEventState, 8);
    // Update eventSyncedToGM
    ptpClockEventHandler.setSyncedWithGm(ptp4lEventState, true);
    ptpClockEventHandler.setSyncedWithGmEventCount(ptp4lEventState, 9);
    // Update eventGMChanged
    ptpClockEventHandler.setGmIdentity(ptp4lEventState, 0x1234ABCD);
    ptpClockEventHandler.setGmChanged(ptp4lEventState, true);
    ptpClockEventHandler.setGmChangedEventCount(ptp4lEventState, 10);
    // Update eventASCapable
    ptpClockEventHandler.setAsCapable(ptp4lEventState, true);
    ptpClockEventHandler.setAsCapableEventCount(ptp4lEventState, 11);
    // Update composite event
    ptpClockEventHandler.setCompositeEvent(ptp4lEventState, true);
    ptpClockEventHandler.setCompositeEventCount(ptp4lEventState, 12);
    // Update notification timestamp
    uint64_t notification_timestamp = last_notification_time.tv_sec;
    notification_timestamp *= NSEC_PER_SEC;
    notification_timestamp += last_notification_time.tv_nsec;
    ptpClockEventHandler.setNotificationTimestamp(ptp4lEventState,
        notification_timestamp);
    // Update GM logSyncInterval
    ptpClockEventHandler.setSyncInterval(ptp4lEventState, 125000);
    // Update Chrony clock offset
    sysClockEventHandler.setClockOffset(chronyEventState, 50000);
    sysClockEventHandler.setOffsetInRange(chronyEventState, true);
    sysClockEventHandler.setOffsetInRangeEventCount(chronyEventState, 13);
    sysClockEventHandler.setGmIdentity(chronyEventState, 0x5678EF01);
    sysClockEventHandler.setSyncInterval(chronyEventState, 10000);
    state.set_chronyEventState(chronyEventState);
    state.set_ptpEventState(ptp4lEventState);
}

// Create dummy TimeBaseConfigurations for testing
class clkmgr::ClientConnectMessage
{
  public:
    static void set() {
        const TimeBaseConfigurations &cfg = TimeBaseConfigurations::getInstance();
        cfg.addTimeBaseCfg({
            .timeBaseIndex = 1,
            .timeBaseName = { 'm', 'e', 0 },
            .interfaceName = { 'e', 't', 'h', '0', 0 },
            .transportSpecific = 4,
            .domainNumber = 1,
            .haveSys = true,
            .havePtp = true
        });
        cfg.addTimeBaseCfg({
            .timeBaseIndex = 2,
            .timeBaseName = { 't', 'o', 'o', 0 },
            .interfaceName = { 'e', 't', 'h', '1', 0 },
            .transportSpecific = 5,
            .domainNumber = 5,
            .haveSys = true,
            .havePtp = false
        });
    }
};

class ClockManagerTest : public ::testing::Test
{
  protected:
    void SetUp() override {
        ClientConnectMessage::set();
        ptp_event data = {};
        TimeBaseStates::getInstance().setTimeBaseState(1, data);
    }
};

// static ClockManager &fetchSingleInstance()
TEST_F(ClockManagerTest, singleInstance) {
    ClockManager &cm1 = ClockManager::fetchSingleInstance();
    ClockManager &cm2 = ClockManager::fetchSingleInstance();
    EXPECT_EQ(&cm1, &cm2);
}

// 2. Connection Management
TEST_F(ClockManagerTest, ConnectAndDisconnectIdempotency) {
    printf("SiangDebug: start\n");
    EXPECT_TRUE(ClockManager::connect());
    printf("SiangDebug: start2\n");
    EXPECT_TRUE(ClockManager::connect()); // Should be idempotent
}
