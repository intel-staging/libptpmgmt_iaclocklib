/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief class for clock events handler.
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#ifndef CLOCK_EVENT_HANDLER_HPP
#define CLOCK_EVENT_HANDLER_HPP

#include "pub/clkmgr/event.h"

__CLKMGR_NAMESPACE_BEGIN

/**
 * This friend class provides setter functions for ClockEventBase.
 *
 * This class is designed to modify the protected attributes of ClockEventBase.
 * It contains static methods to set various attributes of ClockEventBase.
 */
class ClockEventHandler
{
  public:
    /**
     * Sets the notification timestamp of a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param timestamp The new notification timestamp.
     */
    static void setNotificationTimestamp(ClockEventBase &event,
        uint64_t timestamp) {
        event.notificationTimestamp = timestamp;
    }

    /**
     * Sets the clock offset of a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param offset The new clock offset.
     */
    static void setClockOffset(ClockEventBase &event, int64_t offset) {
        event.clockOffset = offset;
    }

    /**
     * Sets the grandmaster identity of a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param identity The new grandmaster identity.
     */
    static void setGmIdentity(ClockEventBase &event, uint64_t identity) {
        event.sourceClockUUID = identity;
    }

    /**
     * Sets whether the offset is in range for a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param in_range True if the offset is in range, false otherwise.
     */
    static void setOffsetInRange(ClockEventBase &event, bool in_range) {
        event.offsetInRange = in_range;
    }

    /**
     * Sets whether the grandmaster has changed for a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param changed True if the grandmaster has changed, false otherwise.
     */
    static void setGmChanged(ClockEventBase &event, bool changed) {
        event.sourceChanged = changed;
    }

    /**
     * Sets the synchronization interval of a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param interval The new synchronization interval.
     */
    static void setSyncInterval(ClockEventBase &event, int64_t interval) {
        event.syncInterval = interval;
    }

    /**
     * Sets whether the event is composite for a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param composite True if the event is composite, false otherwise.
     */
    static void setCompositeEvent(ClockEventBase &event, bool composite) {
        event.compositeEvent = composite;
    }

    /**
     * Sets the count of offset in range events for a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param count The new count of offset in range events.
     */
    static void setOffsetInRangeEventCount(ClockEventBase &event, uint32_t count) {
        event.offsetInRangeCount = count;
    }

    /**
     * Sets the count of source clock UUID changed events for a
     * ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param count The new count of source clock UUID changed events.
     */
    static void setGmChangedEventCount(ClockEventBase &event, uint32_t count) {
        event.sourceChangedCount = count;
    }

    /**
     * Sets the count of composite events for a ClockEventBase object.
     * @param event The ClockEventBase object to modify.
     * @param count The new count of composite events.
     */
    static void setCompositeEventCount(ClockEventBase &event, uint32_t count) {
        event.compositeEventCount = count;
    }
};

/**
 * This friend class provides setter functions for PTPClockEvent.
 *
 * This class is designed to modify the private attributes of PTPClockEvent.
 * It contains static methods to set various attributes of PTPClockEvent.
 */
class PTPClockEventHandler  : public ClockEventHandler
{
  public:
    /**
     * Sets whether the clock is synced to the primary clock.
     * @param event The ClockEventBase object to modify.
     * @param synced True if the clock is synced to the primary clock, false
     * otherwise.
     */
    static void setSyncedToPrimaryClock(PTPClockEvent &event, bool synced) {
        event.syncedToPrimaryClock = synced;
    }

    /**
     * Sets whether the clock is IEEE 802.1AS capable.
     * @param event The ClockEventBase object to modify.
     * @param capable True if the clock is IEEE 802.1AS capable, false otherwise.
     */
    static void setAsCapable(PTPClockEvent &event, bool capable) {
        event.asCapable = capable;
    }

    /**
     * Sets the count of events where the clock is synced to the primary
     * clock.
     * @param event The ClockEventBase object to modify.
     * @param count The new count of synced to primary clock events.
     */
    static void setSyncedToGmEventCount(PTPClockEvent &event, uint32_t count) {
        event.syncedToPrimaryClockCount = count;
    }

    /**
     * Sets the count of events where the clock is IEEE 802.1AS capable.
     * @param event The ClockEventBase object to modify.
     * @param count The new count of IEEE 802.1AS capable events.
     */
    static void setAsCapableEventCount(PTPClockEvent &event, uint32_t count) {
        event.asCapableCount = count;
    }
};

/**
 * This friend class provides setter functions for SysClockEvent.
 *
 * This class is designed to modify the private attributes of SysClockEvent.
 * It contains static methods to set various attributes of SysClockEvent.
 */
class SysClockEventHandler  : public ClockEventHandler
{
  public:
    /**
     * Constructor to initialize both base and derived class members.
     *
     * Initializes the Chrony-specific attributes to default values.
     */
    SysClockEventHandler() {}

  private:
    /**
     * Add Chrony-specific attributes and methods here if needed in the future.
     */
};

__CLKMGR_NAMESPACE_END

#endif /* CLOCK_EVENT_HANDLER_HPP */
