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
#include "client/clock_event_handler.hpp"

using namespace clkmgr;

// static ClockManager &fetchSingleInstance()
TEST(ClockManagerTest, singleInstance) {
    ClockManager &cm1 = ClockManager::fetchSingleInstance();
    ClockManager &cm2 = ClockManager::fetchSingleInstance();
    EXPECT_EQ(&cm1, &cm2);
}

// 2. Connection Management
TEST(ClockManagerTest, ConnectAndDisconnectIdempotency) {
    EXPECT_TRUE(ClockManager::connect());
    EXPECT_TRUE(ClockManager::connect()); // Should be idempotent
    EXPECT_TRUE(ClockManager::disconnect());
    EXPECT_TRUE(ClockManager::disconnect()); // Should be idempotent
}

// 3. Timebase Configurations
TEST(ClockManagerTest, GetTimebaseCfgsTriggersConnectIfNeeded) {
    // Should connect if not already connected
    EXPECT_NO_THROW({
        const auto &cfgs = ClockManager::getTimebaseCfgs();
        (void)cfgs;
    });
}

// 4. Subscription by Index
TEST(ClockManagerTest, SubscribeValidAndInvalidIndex) {
    ClockSyncSubscription sub;
    ClockSyncData data;
    // Assuming 1 is a valid index in test environment
    EXPECT_TRUE(ClockManager::subscribe(sub, 1, data));
    // Invalid index (e.g., 9999) should fail gracefully
    EXPECT_FALSE(ClockManager::subscribe(sub, 9999, data));
}

// 5. Subscription by Name
TEST(ClockManagerTest, SubscribeByNameValidAndInvalid) {
    ClockSyncSubscription sub;
    ClockSyncData data;
    // Assuming "me" is a valid name in test environment
    EXPECT_TRUE(ClockManager::subscribeByName(sub, "me", data));
    // Invalid name should fail gracefully
    EXPECT_FALSE(ClockManager::subscribeByName(sub, "invalid_name", data));
}

// 6. Status Wait by Index
TEST(ClockManagerTest, StatusWaitReturnsExpectedValues) {
    ClockSyncData data;
    // Valid index, no event change expected (simulate with 0 timeout)
    int ret = ClockManager::statusWait(0, 1, data);
    EXPECT_GE(ret, 0); // 0 or 1
    // Invalid index should return error
    EXPECT_EQ(ClockManager::statusWait(0, 9999, data), -1);
}

// 7. Status Wait by Name
TEST(ClockManagerTest, StatusWaitByNameReturnsExpectedValues) {
    ClockSyncData data;
    // Valid name, no event change expected (simulate with 0 timeout)
    int ret = ClockManager::statusWaitByName(0, "me", data);
    EXPECT_GE(ret, 0); // 0 or 1
    // Invalid name should return error
    EXPECT_EQ(ClockManager::statusWaitByName(0, "invalid_name", data), -1);
}

// 8. GetTime Success and Failure
TEST(ClockManagerTest, GetTimeReturnsTrueOnSuccess) {
    timespec ts;
    EXPECT_TRUE(ClockManager::getTime(ts));
    // Simulate failure: not possible with standard clock_gettime, but could be tested with a mock.
}

// 9. Edge: Disconnect Without Connect
TEST(ClockManagerTest, DisconnectWithoutConnectIsSafe) {
    EXPECT_TRUE(ClockManager::disconnect());
}

// 10. Edge: Multiple Subscriptions
TEST(ClockManagerTest, MultipleSubscriptions) {
    ClockSyncSubscription sub;
    ClockSyncData data;
    EXPECT_TRUE(ClockManager::subscribe(sub, 1, data));
    // Subscribe again to the same index
    EXPECT_TRUE(ClockManager::subscribe(sub, 1, data));
}

// 11. Edge: Subscribe After Disconnect
TEST(ClockManagerTest, SubscribeAfterDisconnectReconnects) {
    ClockManager::disconnect();
    ClockSyncSubscription sub;
    ClockSyncData data;
    EXPECT_TRUE(ClockManager::subscribe(sub, 1, data));
}

// 12. Edge: StatusWaitAfterDisconnect
TEST(ClockManagerTest, StatusWaitAfterDisconnectFails) {
    ClockManager::disconnect();
    ClockSyncData data;
    EXPECT_EQ(ClockManager::statusWait(0, 1, data), -1);
}

//
// Coverage: Use gcov or llvm-cov with CMake/Makefile integration
// Maintenance: Place tests in utest/, update with API changes, use git branching for test scripts
//
