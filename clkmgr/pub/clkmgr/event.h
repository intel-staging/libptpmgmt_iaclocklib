/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief class and structures for clock events
 *
 * This file defines the model for managing clock synchronization events.
 * The primary class, ClockSyncBases represents a complete event model
 * that encapsulates events for different types of clocks. Each clock event
 * is derived from the ClockEventBase class, providing a unified interface
 * for handling clock-specific attributes and operations.
 *
 * Two types of clock events is supported:
 * - PTPClockEvent represents events specific to Precision Time Protocol
 *   (PTP) clocks.
 * - SysClockEvent represents events specific to system clocks, such as
 *   those managed by Chrony.
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#ifndef EVENT_H
#define EVENT_H

#include "pub/clkmgr/utility.h"
#include "pub/clkmgr/types.h"
#include <array>

__CLKMGR_NAMESPACE_BEGIN

class ClockEventHandler;

/**
 * This class provides a set of common attributes and methods for clock
 * synchronization, including notification timestamp, clock offset, grandmaster
 * identity, offset range, grandmaster change status, synchronization interval,
 * and composite event status.
 */
class ClockEventBase
{
  public:
    /**
     * Gets the notification timestamp.
     *
     * @return The notification timestamp as a uint64_t.
     */
    uint64_t getNotificationTimestamp() const;

    /**
     * Gets the clock offset.
     *
     * @return The clock offset as an int64_t.
     */
    int64_t getClockOffset() const;

    /**
     * Gets the grandmaster identity.
     *
     * @return The grandmaster identity as a std::array of 8 uint8_t elements.
     */
    uint64_t getGmIdentity() const;

    /**
     * Checks if the offset is in range.
     *
     * @return True if the offset is in range, false otherwise.
     */
    bool isOffsetInRange() const;

    /**
     * Checks if the grandmaster has changed.
     *
     * @return True if the grandmaster has changed, false otherwise.
     */
    bool isGmChanged() const;

    /**
     * Gets the synchronization interval.
     *
     * @return The synchronization interval in microsecond.
     */
    int64_t getSyncInterval() const;

    /**
     * Checks if the event is composite.
     *
     * @return True if the event is composite, false otherwise.
     */
    bool isCompositeEvent() const;

    /**
     * Gets the count of offset in range events.
     *
     * @return The count of offset in range events as a uint32_t.
     */
    uint32_t getOffsetInRangeEventCount() const;
    /**
     * Gets the count of source clock UUID changed events.
     *
     * @return The count of source clock UUID changed events as a uint32_t.
     */
    uint32_t getGmChangedEventCount() const;

    /**
     * Gets the count of composite events.
     *
     * @return The count of composite events as a uint32_t.
     */
    uint32_t getCompositeEventCount() const;

    /**
     * Virtual destructor for proper cleanup in derived classes.
     */
    virtual ~ClockEventBase() {}

  protected:
    ClockEventBase();                 /**< Initialize base class members */
    int64_t clockOffset;              /**< Clock offset */
    bool offsetInRange;               /**< Offset in range */
    uint64_t sourceClockUUID;         /**< Source clock UUID */
    bool sourceChanged;               /**< Source clock UUID changed */
    int64_t syncInterval;             /**< Sync interval in microsecond */
    uint64_t notificationTimestamp;   /**< Notification timestamp */
    bool compositeEvent;              /**< Composite event */
    uint32_t offsetInRangeCount;      /**< Offset in range state count */
    uint32_t sourceChangedCount;      /**< Source clock UUID changed state count */
    uint32_t compositeEventCount;     /**< Composite event state count */
    friend class ClockEventHandler;   /**< Friend class to modify ClockEventBase */
};

/**
 * This class extends the ClockEventBase class to include attributes specific to
 * Precision Time Protocol (PTP) synchronization, such as whether the clock is
 * synced to the primary clock and whether it is an IEEE 802.1AS capable.
 */
class PTPClockEvent : public ClockEventBase
{
  public:
    PTPClockEvent();

    /**
     * Checks if the clock is synced to the primary clock.
     *
     * @return True if the clock is synced to the primary clock, false otherwise.
     */
    bool isSyncedToPrimaryClock() const;

    /**
     * Checks if the clock is an IEEE 802.1AS capable.
     *
     * @return True if the clock is an IEEE 802.1AS capable, false otherwise.
     */
    bool isAsCapable() const;
    /**
     * Gets the count of events where the clock is synced to the primary
     * clock.
     *
     * @return The count of synced to primary clock events as a uint32_t.
     */
    uint32_t getSyncedToGmEventCount() const;

    /**
     * Gets the count of events where the clock is an IEEE 802.1AS capable.
     *
     * @return The count of IEEE 802.1AS capable events as a uint32_t.
     */
    uint32_t getAsCapableEventCount() const;

  private:
    bool syncedToPrimaryClock;          /**< Synced to primary clock */
    bool asCapable;                     /**< IEEE 802.1AS capable */
    uint32_t syncedToPrimaryClockCount; /**< Synced event count */
    uint32_t asCapableCount;            /**< AS capable event count */

    friend class ClockEventHandler;  /**< Friend class */
};

/**
 * This class extends the ClockEventBase class to represent synchronization
 * attributes specific to Chrony, a versatile implementation of the Network
 * Time Protocol (NTP). Currently, it does not add any additional attributes
 * or methods beyond those inherited from ClockEventBase.
 */
class SysClockEvent : public ClockEventBase
{
  public:
    SysClockEvent() {}

  private:
    friend class ClockEventHandler;  /**< Friend class */
};

/**
 * This class provides functionality to manage and update the state of PTP
 * and system clocks.
 * It includes methods to check availability, retrieve clock objects, and
 * update clock states.
 */
class ClockSyncBases
{
  public:
    /**
     * Constructs a ClockSyncBases object with specified PTP and
     * system clocks.
     * @param ptpClock Reference to a PTPClockEvent object.
     * @param sysClock Reference to a SysClockEvent object.
     *
     * Initializes the clock synchronization attributes and sets
     * availability to true.
     */
    ClockSyncBases(PTPClockEvent &ptpClock, SysClockEvent &sysClock);

    /**
     * Checks if the PTP clock is available.
     *
     * @return True if the PTP clock is available, false otherwise.
     */
    bool havePTP() const;
    /**
     * Retrieves the PTP clock object.
     *
     * @return Reference to the PTPClockEvent object.
     */
    PTPClockEvent &getPtp() const;

    /**
     * Checks if the system clock is available.
     *
     * @return True if the system clock is available, false otherwise.
     */
    bool haveSys() const;

    /**
     * Retrieves the system clock object.
     * @return Reference to the SysClockEvent object.
     */
    SysClockEvent &getSysClock() const;

    /**
     * Updates the PTP clock with a new PTPClockEvent object.
     * @param newPTPClock The new PTPClockEvent object to update.
     */
    void updatePTPClock(const PTPClockEvent &newPTPClock);

    /**
     * Updates the system clock with a new SysClockEvent object.
     * @param newSysClock The new SysClockEvent object to update.
     */
    void updateSysClock(const SysClockEvent &newSysClock);

  private:
    PTPClockEvent &ptpClockSync;  /**< Reference to the PTP clock object */
    SysClockEvent &sysClockSync;  /**< Reference to the system clock object */
    bool ptpAvailable;            /**< Availability status of the PTP clock */
    bool sysAvailable;            /**< Availability status of the system clock */

    friend class ClockSyncBaseHandler;  /**< Friend class */
};

__CLKMGR_NAMESPACE_END

#endif /* EVENT_H */
