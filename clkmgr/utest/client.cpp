/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. */

/** @file
 * @brief Client class unit tests
 *
 * @author Goh Wei Sheng <wei.sheng.goh@@intel.com>
 * @copyright © 2025 Intel Corporation.
 *
 */

#include <gtest/gtest.h>

#include "client/client_state.hpp"
#include "common/print.hpp"
#include "proxy/client.hpp"

using namespace clkmgr;

bool Client::connect_ptp4l()
{
    return true;
}

bool Client::connect_chrony()
{
    return true;
}

// Test ptpEvent
// int64_t master_offset
// int64_t ptp4l_sync_interval
// bool synced_to_primary_clock
// bool as_capable
// uint8_t gm_identity[8]
// ptpEvent(size_t index)
// void portClear()
// void copy()
// static void getPTPEvent(size_t timeBaseIndex, ptp_event &event)
// Index 0
TEST(ClientTest, constructAndCopyPTPEvent)
{
    // Local event structure
    ptpEvent ptp(0);
    // Shared event structure
    ptp_event event = {};
    // Check the initialized values
    EXPECT_EQ(ptp.master_offset, 0);
    EXPECT_EQ(ptp.ptp4l_sync_interval, 0);
    EXPECT_FALSE(ptp.synced_to_primary_clock);
    EXPECT_FALSE(ptp.as_capable);
    uint8_t test_id1[8] = {0};
    EXPECT_EQ(memcmp(ptp.gm_identity, test_id1, 8), 0);
    EXPECT_EQ(event.master_offset, 0);
    EXPECT_EQ(event.ptp4l_sync_interval, 0);
    EXPECT_FALSE(event.synced_to_primary_clock);
    EXPECT_FALSE(event.as_capable);
    EXPECT_EQ(memcmp(event.gm_identity, test_id1, 8), 0);
    // Assign the values
    ptp.master_offset = 123;
    ptp.ptp4l_sync_interval = 456;
    ptp.synced_to_primary_clock = true;
    ptp.as_capable = true;
    uint8_t test_id2[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    memcpy(ptp.gm_identity, test_id2, 8);
    // Copy values from local to shared structures
    ptp.copy();
    // Get the shared event and check the values
    Client::getPTPEvent(0, event);
    EXPECT_EQ(event.master_offset, 123);
    EXPECT_EQ(event.ptp4l_sync_interval, 456);
    EXPECT_TRUE(event.synced_to_primary_clock);
    EXPECT_TRUE(event.as_capable);
    EXPECT_EQ(memcmp(event.gm_identity, test_id2, 8), 0);
    // Clear and ensure the values are reset
    ptp.portClear();
    EXPECT_EQ(ptp.master_offset, 0);
    EXPECT_EQ(ptp.ptp4l_sync_interval, 0);
    EXPECT_FALSE(ptp.synced_to_primary_clock);
}

// Test Chrony event
// int64_t chrony_offset
// uint32_t chrony_reference_id
// int64_t polling_interval
// chronyEvent(size_t index)
// void clear()
// void copy()
// static void getPTPEvent(size_t timeBaseIndex, ptp_event &event)
// Index 1
TEST(ClientTest, constructAndCopyChronyEvent)
{
    // Local chrony event structure
    chronyEvent chrony(1);
    // Shared chrony event structure
    ptp_event event = {};
    // Check the initialized values
    EXPECT_EQ(chrony.chrony_offset, 0);
    EXPECT_EQ(chrony.chrony_reference_id, 0);
    EXPECT_EQ(chrony.polling_interval, 0);
    EXPECT_EQ(event.chrony_offset, 0);
    EXPECT_EQ(event.chrony_reference_id, 0);
    EXPECT_EQ(event.polling_interval, 0);
    // Assign the values
    chrony.chrony_offset = 789;
    chrony.chrony_reference_id = 42;
    chrony.polling_interval = 1000;
    // Copy values from local to shared structures
    chrony.copy();
    // Get the shared event and check the values
    Client::getPTPEvent(1, event);
    EXPECT_EQ(event.chrony_offset, 789);
    EXPECT_EQ(event.chrony_reference_id, 42);
    EXPECT_EQ(event.polling_interval, 1000);
    // Clear and ensure the values are reset
    chrony.clear();
    EXPECT_EQ(chrony.chrony_offset, 0);
    EXPECT_EQ(chrony.chrony_reference_id, 0u);
    EXPECT_EQ(chrony.polling_interval, 0);
}
