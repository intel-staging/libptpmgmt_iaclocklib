/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief ClientState class unit tests
 *
 * @author Goh Wei Sheng <wei.sheng.goh@@intel.com>
 * @copyright © 2025 Intel Corporation.
 *
 */

#include "pub/clockmanager.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include <format>
#include "proxy/connect_ptp4l.hpp"

__CLKMGR_NAMESPACE_USE;

using namespace clkmgr;

/* Remarks: Below code just a very beginning draft */

/* TODO:
 * - Study unit test
 * - Rule out the scope and coverage
 * - gMock
 */

// Test the singleton instance
// static ClockManager &FetchSingle()
TEST(ClockManagerTest, FetchSingle)
{
    ClockManager &cm1 = ClockManager::FetchSingle();
    ClockManager &cm2 = ClockManager::FetchSingle();
    EXPECT_EQ(&cm1, &cm2); // Singleton should return the same instance
}

// Test initialization
// static bool init()
TEST(ClockManagerTest, Init)
{
    EXPECT_TRUE(ClockManager::init());
}

// Test connection establishment
// static bool connect()
TEST(ClockManagerTest, Connect)
{
    // Need to have transmitter queue before connect
    EXPECT_TRUE(ClockManager::connect());
}

// Remarks: Should assign itself and check
// Test fetching time base configurations
// static const TimeBaseConfigurations &get_timebase_cfgs()
TEST(ClockManagerTest, GetTimeBaseConfigs)
{
    int i = 0;

    const TimeBaseConfigurations &cfg = ClockManager::get_timebase_cfgs();
    // The following assumption is base on the sample configuration in proxy_cfg.json
    // Do we need to test all or just one set is enough?
    EXPECT_EQ(cfg.size(), 2); // There are 2 set of time base in proxy_cfg.json
    for(const auto &cfg : cfg) {
        EXPECT_EQ(cfg.index(), (i+1)); // Assuming the first index is 0
        if(cfg.index() == 1) {
            EXPECT_EQ(cfg.name(), "Global Clock");
            EXPECT_EQ(cfg.ptp().domainNumber(), i);
        } else if(cfg.index() == 2) {
            EXPECT_EQ(cfg.name(), "Working Clock");
            EXPECT_EQ(cfg.ptp().domainNumber(), 20);
        }
        EXPECT_EQ(cfg.ptp().ifName(), "eth" + std::to_string(i));
        EXPECT_EQ(cfg.ptp().transportSpecific(), 1);
        // Current no ptp4l and chrony uds address
        i++;
    }
}

// Test subscribing to events by name
// static bool subscribe_by_name(const ClkMgrSubscription &newSub,
//     const std::string &timeBaseName, Event_state &currentState)
TEST(ClockManagerTest, SubscribeByName)
{
    Event_state eventState = {};
    ClkMgrSubscription subscription = {}; // Initialize with appropriate values

    subscription.set_event_mask(eventGMOffset | eventSyncedToGM | eventASCapable);
    subscription.define_threshold(thresholdGMOffset, 100000, -100000);
    subscription.define_threshold(thresholdChronyOffset, 100000, -100000);
    subscription.set_composite_event_mask(eventGMOffset | eventSyncedToGM | eventASCapable);
    EXPECT_TRUE(ClockManager::subscribe_by_name(subscription, "Global Clock", eventState));
    EXPECT_TRUE(ClockManager::subscribe_by_name(subscription, "Working Clock", eventState));
    // Current cannot subscribe back to the same name suspect multi thread
    // Negative Test
    EXPECT_FALSE(ClockManager::subscribe_by_name(subscription, "Invalid Clock", eventState));
    // Might need to check eventState as well

}

// Test subscribing to events by index
// TEST(ClockManagerTest, SubscribeByIndex)
// {
//     Event_state eventState = {};
//     ClkMgrSubscription sub = {}; // Initialize with appropriate values

//     sub.set_event_mask(eventGMOffset | eventSyncedToGM | eventASCapable);
//     sub.define_threshold(thresholdGMOffset, 100000, -100000);
//     sub.define_threshold(thresholdChronyOffset, 100000, -100000);
//     sub.set_composite_event_mask(eventGMOffset | eventSyncedToGM | eventASCapable);
//     EXPECT_TRUE(ClockManager::subscribe(sub, 1, eventState));
//     // Negative Test
//     EXPECT_FALSE(ClockManager::subscribe(sub, -1, eventState));
// }

// Test waiting for status changes by name
//static int status_wait_by_name(int timeout, const std::string &timeBaseName,
//    Event_state &currentState, Event_count &currentCount);
TEST(ClockManagerTest, StatusWaitByName)
{
    Event_state eventState = {};
    Event_count eventCount;
    int result = ClockManager::status_wait_by_name(1, "Global Clock", eventState, eventCount);
    EXPECT_GE(result, 0); // Result should be -1, 0, or 1
    // Negative Test
    result = ClockManager::status_wait_by_name(1, "Invalid Clock", eventState, eventCount);
    EXPECT_EQ(result, -1); // Result should be -1, 0, or 1
    // Might need to check eventState as well
}

// Test waiting for status changes by index
// static int status_wait(int timeout, size_t timeBaseIndex,
//     Event_state &currentState, Event_count &currentCount);
TEST(ClockManagerTest, StatusWaitByIndex)
{
    Event_state eventState = {};
    Event_count eventCount;
    int result = ClockManager::status_wait(1, 1, eventState, eventCount);
    EXPECT_GE(result, 0);
    // Negative Test
    // result = ClockManager::status_wait(1, -1, eventState, eventCount);
    // EXPECT_EQ(result, -1); // Probably have bug
    // Might need to check eventState as well
}

// Test retrieving the current time
// static bool gettime(timespec &ts)
TEST(ClockManagerTest, GetTime)
{
    timespec ts;
    EXPECT_TRUE(ClockManager::gettime(ts));
    EXPECT_GE(ts.tv_sec, 0);
    EXPECT_GE(ts.tv_nsec, 0);
}

// Test disconnection
// static bool disconnect()
TEST(ClockManagerTest, Disconnect)
{
    EXPECT_TRUE(ClockManager::disconnect());
}
