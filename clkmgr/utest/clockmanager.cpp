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
#include "client/client_state.hpp"

using namespace clkmgr;

bool ClientState::init()
{
    printf("SiangDebug: init\n");
    return true;
}

bool ClientState::connect(uint32_t timeOut, timespec *lastConnectTime)
{
    printf("SiangDebug: connect\n");
    return true;
}

DECLARE_STATIC(ClientState::m_clientID);
DECLARE_STATIC(ClientState::m_sessionId, InvalidSessionId);
DECLARE_STATIC(ClientState::m_connected, false);

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
