/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief C wrapper for the Clock Manager APIs to set up client-runtime.
 *
 * @author Song Yoong Siang <yoong.siang.song@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "pub/clkmgr/clockmanager_c.h"
#include "pub/clockmanager.h"

#include <cstring>

__CLKMGR_NAMESPACE_USE;

static void clkmgr_fill_event_state(ClockSyncData &data,
    Clkmgr_Event_state *state)
{
    PTPClockEvent &ptp = data.getPtp();
    SysClockEvent &sys = data.getSysClock();
    state->as_capable = ptp.isAsCapable();
    state->offset_in_range = ptp.isOffsetInRange();
    state->synced_to_primary_clock = ptp.isSyncedWithGm();
    state->gm_changed = ptp.isGmChanged();
    state->composite_event = ptp.isCompositeEventMet();
    state->clock_offset = ptp.getClockOffset();
    state->notification_timestamp = ptp.getNotificationTimestamp();
    uint64_t gm_id = ptp.getGmIdentity();
    for(int i = 0; i < 8; ++i)
        state->gm_identity[i] = (uint8_t)(gm_id >> (8 * (7 - i)));
    state->ptp4l_sync_interval = ptp.getSyncInterval();
    state->chrony_clock_offset = sys.getClockOffset();
    state->chrony_reference_id = sys.getGmIdentity();
    state->chrony_offset_in_range = sys.isOffsetInRange();
    state->polling_interval = sys.getSyncInterval();
}

static void clkmgr_fill_event_count(ClockSyncData &data,
    Clkmgr_Event_count *count)
{
    PTPClockEvent &ptp = data.getPtp();
    SysClockEvent &sys = data.getSysClock();
    count->as_capable_event_count = ptp.getAsCapableEventCount();
    count->composite_event_count = ptp.getCompositeEventCount();
    count->gm_changed_event_count = ptp.getGmChangedEventCount();
    count->offset_in_range_event_count = ptp.getOffsetInRangeEventCount();
    count->synced_to_gm_event_count = ptp.getSyncedWithGmEventCount();
    count->chrony_offset_in_range_event_count = sys.getOffsetInRangeEventCount();
}

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
    return ClockManager::getTimebaseCfgs().size();
}

bool clkmgr_subscribe_by_name(const struct clkmgr_c_subscription sub,
    const char *timeBaseName, struct Clkmgr_Event_state *cur_state)
{
    if(!timeBaseName || !cur_state)
        return false;
    size_t timeBaseIndex = 0;
    if(TimeBaseConfigurations::BaseNameToBaseIndex(timeBaseName, timeBaseIndex))
        return clkmgr_subscribe(sub, timeBaseIndex, cur_state);
    return false;
}

bool clkmgr_subscribe(const struct clkmgr_c_subscription sub,
    size_t timeBaseIndex,
    struct Clkmgr_Event_state *cur_state)
{
    if(!cur_state || timeBaseIndex == 0)
        return false;
    ClkMgrSubscription newSub = {};
    ClockSyncData data;
    bool ret;
    newSub.set_event_mask(sub.event_mask);
    newSub.set_composite_event_mask(sub.composite_event_mask);
    newSub.define_threshold(thresholdGMOffset,
        sub.threshold[Clkmgr_thresholdGMOffset].upper_limit,
        sub.threshold[Clkmgr_thresholdGMOffset].lower_limit);
    newSub.define_threshold(thresholdChronyOffset,
        sub.threshold[Clkmgr_thresholdChronyOffset].upper_limit,
        sub.threshold[Clkmgr_thresholdChronyOffset].lower_limit);
    ret = ClockManager::subscribe(newSub, timeBaseIndex, data);
    if(ret)
        clkmgr_fill_event_state(data, cur_state);
    return ret;
}

int clkmgr_status_wait_by_name(int timeout, const char *timeBaseName,
    struct Clkmgr_Event_state *cur_state, struct Clkmgr_Event_count *cur_count)
{
    if(!timeBaseName || !cur_state || !cur_count)
        return -1;
    ClockSyncData data;
    size_t timeBaseIndex = 0;
    if(TimeBaseConfigurations::BaseNameToBaseIndex(std::string(timeBaseName),
            timeBaseIndex))
        return clkmgr_status_wait(timeout, timeBaseIndex, cur_state, cur_count);
    return -1;
}

int clkmgr_status_wait(int timeout, size_t timeBaseIndex,
    struct Clkmgr_Event_state *cur_state, struct Clkmgr_Event_count *cur_count)
{
    if(!cur_state || !cur_count || timeBaseIndex == 0)
        return -1;
    ClockSyncData data;
    int ret = ClockManager::statusWait(timeout, timeBaseIndex, data);
    if(ret < 0)
        return ret;
    clkmgr_fill_event_state(data, cur_state);
    if(ret > 0)
        clkmgr_fill_event_count(data, cur_count);
    return ret;
}

bool clkmgr_gettime(struct timespec *ts)
{
    return ClockManager::getTime(*ts);
}
