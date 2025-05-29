/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief ClockSyncSubscription class unit tests
 *
 * @author Erez Geva <ErezGeva2@@gmail.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "pub/clkmgr/subscription.h"

using namespace clkmgr;

// ClockSubscriptionBase() noexcept
// uint32_t getEventMask() const
// uint32_t getClockOffsetThreshold() const
TEST(ClockSubscriptionBaseTest, DefaultConstructor_InitializesMembersToZero)
{
    ClockSubscriptionBase sub;
    EXPECT_EQ(sub.getEventMask(), 0u);
    EXPECT_EQ(sub.getClockOffsetThreshold(), 0u);
}

// ClockSubscriptionBase() noexcept
// void setEventMask(uint32_t)
// uint32_t getEventMask() const
TEST(ClockSubscriptionBaseTest, SetAndGetEventMask_StoresAndReturnsValue)
{
    ClockSubscriptionBase sub;
    sub.setEventMask(0xf);
    EXPECT_EQ(sub.getEventMask(), 0xfu);
}

// ClockSubscriptionBase() noexcept
// void setClockOffsetThreshold(uint32_t)
// uint32_t getClockOffsetThreshold() const
TEST(ClockSubscriptionBaseTest,
    SetAndGetClockOffsetThreshold_StoresAndReturnsValue)
{
    ClockSubscriptionBase sub;
    sub.setClockOffsetThreshold(123456);
    EXPECT_EQ(sub.getClockOffsetThreshold(), 123456u);
}

// ClockSubscriptionBase() noexcept
// void setClockOffsetThreshold(uint32_t)
// uint32_t getClockOffsetThreshold() const
TEST(ClockSubscriptionBaseTest, SetClockOffsetThreshold_BoundaryValues)
{
    ClockSubscriptionBase sub;
    sub.setClockOffsetThreshold(0);
    EXPECT_EQ(sub.getClockOffsetThreshold(), 0u);
    sub.setClockOffsetThreshold(UINT32_MAX);
    EXPECT_EQ(sub.getClockOffsetThreshold(), UINT32_MAX);
}

// PTPClockSubscription() noexcept
// uint32_t getCompositeEventMask() const
TEST(PTPClockSubscriptionTest,
    DefaultConstructor_InitializesCompositeEventMaskToZero)
{
    PTPClockSubscription ptpSub;
    EXPECT_EQ(ptpSub.getCompositeEventMask(), 0u);
}

// PTPClockSubscription() noexcept
// void setCompositeEventMask(uint32_t)
// uint32_t getCompositeEventMask() const
TEST(PTPClockSubscriptionTest,
    SetAndGetCompositeEventMask_StoresAndReturnsValue)
{
    PTPClockSubscription ptpSub;
    ptpSub.setCompositeEventMask(0x7);
    EXPECT_EQ(ptpSub.getCompositeEventMask(), 0x7);
}

// SysClockSubscription() noexcept
// uint32_t getEventMask() const
// uint32_t getClockOffsetThreshold() const
TEST(SysClockSubscriptionTest, DefaultConstructor_InitializesBaseMembersToZero)
{
    SysClockSubscription sysSub;
    EXPECT_EQ(sysSub.getEventMask(), 0u);
    EXPECT_EQ(sysSub.getClockOffsetThreshold(), 0u);
}

// ClockSyncSubscription()
// bool isPTPSubscriptionEnable() const
// bool isSysSubscriptionEnable() const
TEST(ClockSyncSubscriptionTest, DefaultConstructor_SubscriptionsDisabled)
{
    ClockSyncSubscription syncSub;
    EXPECT_FALSE(syncSub.isPTPSubscriptionEnable());
    EXPECT_FALSE(syncSub.isSysSubscriptionEnable());
}

// ClockSyncSubscription()
// void enablePtpSubscription()
// bool isPTPSubscriptionEnable() const
// void disablePtpSubscription()
TEST(ClockSyncSubscriptionTest, EnableDisablePTPSubscription)
{
    ClockSyncSubscription syncSub;
    syncSub.enablePtpSubscription();
    EXPECT_TRUE(syncSub.isPTPSubscriptionEnable());
    syncSub.disablePtpSubscription();
    EXPECT_FALSE(syncSub.isPTPSubscriptionEnable());
}

// ClockSyncSubscription()
// void enableSysSubscription()
// bool isSysSubscriptionEnable() const
// void disableSysSubscription()
TEST(ClockSyncSubscriptionTest, EnableDisableSysSubscription)
{
    ClockSyncSubscription syncSub;
    syncSub.enableSysSubscription();
    EXPECT_TRUE(syncSub.isSysSubscriptionEnable());
    syncSub.disableSysSubscription();
    EXPECT_FALSE(syncSub.isSysSubscriptionEnable());
}

// ClockSyncSubscription()
// void setPtpSubscription(const PTPClockSubscription&)
// const PTPClockSubscription& getPtpSubscription() const
// PTPClockSubscription::setCompositeEventMask(uint32_t)
// PTPClockSubscription::setEventMask(uint32_t)
// PTPClockSubscription::setClockOffsetThreshold(uint32_t)
// PTPClockSubscription::getCompositeEventMask() const
// PTPClockSubscription::getEventMask() const
// PTPClockSubscription::getClockOffsetThreshold() const
TEST(ClockSyncSubscriptionTest, SetAndGetPtpSubscription)
{
    ClockSyncSubscription syncSub;
    PTPClockSubscription ptpSub;
    ptpSub.setCompositeEventMask(0x7);
    ptpSub.setEventMask(0x8);
    ptpSub.setClockOffsetThreshold(42);
    syncSub.setPtpSubscription(ptpSub);
    const PTPClockSubscription &result = syncSub.getPtpSubscription();
    EXPECT_EQ(result.getCompositeEventMask(), 0x7u);
    EXPECT_EQ(result.getEventMask(), 0x8u);
    EXPECT_EQ(result.getClockOffsetThreshold(), 42u);
}

// ClockSyncSubscription()
// void setSysSubscription(const SysClockSubscription&)
// const SysClockSubscription& getSysSubscription() const
// SysClockSubscription::setEventMask(uint32_t)
// SysClockSubscription::setClockOffsetThreshold(uint32_t)
// SysClockSubscription::getEventMask() const
// SysClockSubscription::getClockOffsetThreshold() const
TEST(ClockSyncSubscriptionTest, SetAndGetSysSubscription)
{
    ClockSyncSubscription syncSub;
    SysClockSubscription sysSub;
    sysSub.setEventMask(0x1);
    sysSub.setClockOffsetThreshold(99);
    syncSub.setSysSubscription(sysSub);
    const SysClockSubscription &result = syncSub.getSysSubscription();
    EXPECT_EQ(result.getEventMask(), 0x1u);
    EXPECT_EQ(result.getClockOffsetThreshold(), 99u);
}

// PTPClockSubscription() noexcept
// void setCompositeEventMask(uint32_t)
// uint32_t getCompositeEventMask() const
TEST(PTPClockSubscriptionTest, SetCompositeEventMask_BoundaryValues)
{
    PTPClockSubscription ptpSub;
    ptpSub.setCompositeEventMask(0);
    EXPECT_EQ(ptpSub.getCompositeEventMask(), 0u);
    ptpSub.setCompositeEventMask(UINT32_MAX);
    EXPECT_EQ(ptpSub.getCompositeEventMask(), UINT32_MAX);
}
