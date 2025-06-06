/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Client utilities to setup and cleanup the library.
 *
 * @author Song Yoong Siang <yoong.siang.song@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "pub/clkmgr/types_c.h"
#include "pub/clockmanager.h"
#include "pub/clkmgr/clockmanager_c.h"
#include "pub/clkmgr/timebase_configs_c.h"
#include "client/clock_event_handler.hpp"

#include <cstring>

__CLKMGR_NAMESPACE_USE;

bool clkmgr_connect()
{
    return ClockManager::connect();
}
bool clkmgr_disconnect()
{
    return ClockManager::disconnect();
}

size_t clkmgr_get_timebase_cfgs_size()
{
    return TimeBaseConfigurations::size();
}

bool clkmgr_subscribe_by_name(const Clkmgr_SyncSubscription *sub,
    const char *time_base_name, ClockSyncData_C *data)
{
    if(!data)
        return false;
    size_t time_base_index = 0;
    if(TimeBaseConfigurations::BaseNameToBaseIndex(time_base_name, time_base_index))
        return clkmgr_subscribe(sub, time_base_index, data);
    return false;
}

bool clkmgr_subscribe(const Clkmgr_SyncSubscription *sub,
    size_t time_base_index, ClockSyncData_C *data)
{
    if(!sub || time_base_index == 0 || !data)
        return false;
    // Create a C++ subscription object and set its parameters from the C struct
    PTPClockSubscription ptp4lSub;
    SysClockSubscription chronySub;
    ClockSyncSubscription newSub;
    ptp4lSub.setEventMask(clkmgr_get_event_mask(sub));
    ptp4lSub.setCompositeEventMask(clkmgr_get_composite_event_mask(sub));
    ptp4lSub.setClockOffsetThreshold(clkmgr_get_ptp_clock_offset_threshold(sub));
    chronySub.setClockOffsetThreshold(clkmgr_get_sys_clock_offset_threshold(sub));
    newSub.enablePtpSubscription();
    newSub.enableSysSubscription();
    newSub.setPtpSubscription(ptp4lSub);
    newSub.setSysSubscription(chronySub);
    return ClockManager::subscribe(newSub, time_base_index, *data->obj);
}

int clkmgr_status_wait_by_name(int timeout, const char *time_base_name,
    ClockSyncData_C *data)
{
    if(!data)
        return -1;
    size_t time_base_index = 0;
    if(TimeBaseConfigurations::BaseNameToBaseIndex(time_base_name, time_base_index))
        return clkmgr_status_wait(timeout, time_base_index, data);
    return -1;
}

int clkmgr_status_wait(int timeout, size_t time_base_index,
    ClockSyncData_C *data)
{
    if(time_base_index == 0 || !data)
        return -1;
    return ClockManager::statusWait(timeout, time_base_index, *data->obj);
}

bool clkmgr_gettime(timespec *ts)
{
    return ts != nullptr && clock_gettime(CLOCK_REALTIME, ts) == 0;
}
