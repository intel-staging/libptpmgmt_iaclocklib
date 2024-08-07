/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file jclklib_import.hpp
 * @brief C API import.
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 */

#ifndef JCLKLIB_IMPORT_HPP
#define JCLKLIB_IMPORT_HPP

#include <cstdint>

#include <common/util.hpp>

namespace JClkLibCommon
{
typedef enum : std::uint8_t {
    gmOffsetValue,
    valueLast
} valueType;
#define MAX_VALUE_COUNT 12

class jcl_value
{
  private:
    struct value_t {
        std::int32_t upper;
        std::int32_t lower;
        value_t();
        value_t(uint32_t limit);
        bool equal(const value_t &v);
        bool operator== (const value_t &value) { return this->equal(value); }
        bool operator!= (const value_t &value) { return !this->equal(value); }
        void zero() { upper = 0; lower = 0; }
    };
    value_t value[valueLast];
    value_t reserved[MAX_VALUE_COUNT - sizeof(value) / sizeof(value[0])];
  public:
    std::uint8_t *parse(std::uint8_t *buf, std::size_t &length);
    std::uint8_t *write(std::uint8_t *buf, std::size_t &length);
    bool equal(const jcl_value &c);
    bool operator== (const jcl_value &value) { return this->equal(value); }
    bool operator!= (const jcl_value &value) { return !this->equal(value); }
    /* Add a method to set the upper and lower values
        of a specific value_t in the value array */
    void setValue(int index, std::int32_t upper, std::int32_t lower) {
        if(index >= 0 && index < valueLast) {
            value[index].upper = upper;
            value[index].lower = lower;
        }
    }
    std::int32_t getUpper(int index) {
        if(index >= 0 && index < valueLast)
            return value[index].upper;
        return 0;
    }

    std::int32_t getLower(int index) {
        if(index >= 0 && index < valueLast)
            return value[index].lower;
        return 0;
    }
    std::string toString();
};

/* Events clients can subscribe to */
typedef enum : std::uint8_t {
    gmOffsetEvent, syncedToPrimaryClockEvent, asCapableEvent, gmChangedEvent,
    eventLast
} eventType;

#define BITS_PER_BYTE (8)
#define MAX_EVENT_COUNT (128)

class jcl_event
{
  private:
    std::uint32_t event_mask[eventLast / (sizeof(std::uint32_t)*BITS_PER_BYTE) + 1];
    std::uint32_t reserved[MAX_EVENT_COUNT /
                        sizeof(event_mask[0]*BITS_PER_BYTE)
                        - sizeof(event_mask) / sizeof(std::uint32_t)];
  public:
    std::uint8_t *parse(std::uint8_t *buf, std::size_t &length);
    std::uint8_t *write(std::uint8_t *buf, std::size_t &length);
    void zero();
    bool equal(const jcl_event &c);
    bool operator== (const jcl_event &event) { return this->equal(event); }
    bool operator!= (const jcl_event &event) { return !this->equal(event); }
    std::string toString();
    int8_t writeEvent(uint32_t *newEvent, std::size_t length);
    int8_t readEvent(uint32_t *readEvnt, std::size_t length);
    int8_t copyEventMask(jcl_event &newEvent);
    std::uint32_t *getEventMask() { return event_mask; }
};

class jcl_eventcount
{
    std::uint32_t count[eventLast];
    std::uint32_t reserved[MAX_EVENT_COUNT - eventLast];
  public:
    std::uint8_t *parse(std::uint8_t *buf, std::size_t &length);
    std::uint8_t *write(std::uint8_t *buf, std::size_t &length);
    void zero();
    bool equal(const jcl_eventcount &ec);
    bool operator== (const jcl_eventcount &ec) { return this->equal(ec); }
    bool operator!= (const jcl_eventcount &ec) { return !this->equal(ec); }
};

class jcl_subscription
{
  private:
    jcl_event        event;
    jcl_value        value;
    jcl_event        composite_event;
  public:
    DECLARE_ACCESSOR(event);
    DECLARE_ACCESSOR(value);
    DECLARE_ACCESSOR(composite_event);
};

struct ptp_event {
    int64_t master_offset;
    uint8_t gm_identity[8]; /* Grandmaster clock ID */
    bool as_capable; /* 802@.1AS Capable */
    bool synced_to_primary_clock;
    uint8_t ptp4l_id;
};

struct client_ptp_event {
    int64_t master_offset;
    int64_t master_offset_low;
    int64_t master_offset_high;
    uint8_t gm_identity[8]; /* Grandmaster clock ID */
    uint8_t ptp4l_id;
    bool as_capable; /* 802@.1AS Capable */
    bool synced_to_primary_clock;
    bool master_offset_in_range;
    bool composite_event;
    std::atomic<int> offset_in_range_event_count{};
    std::atomic<int> as_capable_event_count{};
    std::atomic<int> synced_to_primary_clock_event_count{};
    std::atomic<int> gm_changed_event_count{};
    std::atomic<int> composite_event_count{};
};
}

#endif /* JCLKLIB_IMPORT_HPP */
