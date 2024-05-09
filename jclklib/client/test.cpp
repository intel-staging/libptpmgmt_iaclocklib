/*
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. All rights reserved.
 */

/** @file test.cpp
 * @brief Test client code
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation. All rights reserved.
 * @license LGPL-3.0-or-later
 *
 */

#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <client/client_state.hpp>
#include <client/connect_msg.hpp>
#include <common/jclklib_import.hpp>

#include "init.hpp"

using namespace JClkLibClient;
using namespace JClkLibCommon;

volatile sig_atomic_t signal_flag = 0;

void signal_handler(int sig)
{
    signal_flag = 1;
}

int main(int argc, char *argv[])
{
    int timeout = 10;

    std::uint32_t event2Sub1[1] = {
        ((1<<gmOffsetEvent)|(1<<servoLockedEvent)|(1<<asCapableEvent)|
        (1<<gmPresentEvent)|(1<<gmChangedEvent))
    };

    std::uint32_t composite_event[1] = {
        ((1<<gmOffsetEvent)|(1<<servoLockedEvent)|(1<<asCapableEvent))
    };

    int opt;
    while ((opt = getopt(argc, argv, "s:c:t:")) != -1) {
        switch (opt) {
        case 's':
            event2Sub1[0] = std::stoul(optarg);
            break;
        case 'c':
            composite_event[0] = std::stoul(optarg);
            break;
        case 't':
            timeout = std::stoi(optarg);
            break;
        default:
            std::cerr << "Usage: " << argv[0] << "[-s subscribe_event_mask] "
                "[-c composite_event_mask] "
                "[-t timeout in waiting notification event]\n";
            return EXIT_FAILURE;
        }
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);

    if (jcl_connect() == false) {
        std::cerr << "[jclklib] Failure in connecting to jclklib Proxy !!!\n";
        return EXIT_FAILURE;
    }

    sleep(1);

    JClkLibCommon::jcl_subscription sub = {};
    sub.get_event().writeEvent(event2Sub1, (std::size_t)sizeof(event2Sub1));
    sub.get_value().setValue(gmOffsetValue, 100000, -100000);
    sub.get_composite_event().writeEvent(composite_event,
        (std::size_t)sizeof(composite_event));
    std::cout << "[jclklib] set subscribe event : " +
        sub.c_get_val_event().toString() << "\n";
    std::cout << "[jclklib] set composite event : " +
        sub.c_get_val_composite_event().toString() << "\n";

    JClkLibCommon::jcl_state jcl_state = {};
    if (jcl_subscribe(sub, jcl_state) == false) {
        std::cerr << "[jclklib] Failure in subscribing to jclklib Proxy !!!\n";
        jcl_disconnect();
        return EXIT_FAILURE;
    }

    printf("[jclklib] Obtained data from Subscription Event:\n");
    printf("+-------------------+--------------------+\n");
    printf("| %-17s | %-18s |\n", "Event", "Event Status");
    if (event2Sub1[0]) {
        printf("+-------------------+--------------------+\n");
    }
    if (event2Sub1[0] & (1<<gmOffsetEvent)) {
        printf("| %-17s | %-18d |\n", "offset_in_range",
            jcl_state.offset_in_range);
    }
    if (event2Sub1[0] & (1<<servoLockedEvent)) {
        printf("| %-17s | %-18d |\n", "servo_locked", jcl_state.servo_locked);
    }
    if (event2Sub1[0] & (1<<asCapableEvent)) {
        printf("| %-17s | %-18d |\n", "as_Capable", jcl_state.as_Capable);
    }
    if (event2Sub1[0] & (1<<gmPresentEvent)) {
        printf("| %-17s | %-18d |\n", "gmPresent", jcl_state.gm_present);
    }
    if (event2Sub1[0] & (1<<gmChangedEvent)) {
        printf("| %-17s | %-18d |\n", "gm_Changed", jcl_state.gm_changed);
        printf("+-------------------+--------------------+\n");
        printf("| %-17s | %02x%02x%02x.%02x%02x.%02x%02x%02x |\n", "UUID",
            jcl_state.gmIdentity[0], jcl_state.gmIdentity[1],
            jcl_state.gmIdentity[2], jcl_state.gmIdentity[3],
            jcl_state.gmIdentity[4], jcl_state.gmIdentity[5],
            jcl_state.gmIdentity[6], jcl_state.gmIdentity[7]);
    }
    printf("+-------------------+--------------------+\n");
    if (composite_event[0]) {
        printf("| %-17s | %-18d |\n", "composite_event",
            jcl_state.composite_event);
    }
    if (composite_event[0] & (1<<gmOffsetEvent)) {
        printf("| - %-15s | %-18s |\n", "offset_in_range", " ");
    }
    if (composite_event[0] & (1<<servoLockedEvent)) {
        printf("| - %-15s | %-18s |\n", "servo_locked", " ");
    }
    if (composite_event[0] & (1<<asCapableEvent)) {
        printf("| - %-15s | %-18s |\n", "as_Capable", " ");
    }
    if (composite_event[0]) {
        printf("+-------------------+--------------------+\n\n");
    } else {
        printf("\n");
    }

    sleep(1);

    JClkLibCommon::jcl_state_event_count eventCount = {};
    while (!signal_flag) {
        if (!jcl_status_wait(timeout, jcl_state , eventCount)) {
            printf("[jclklib] No event changes identified in %d seconds.\n\n",
                timeout);
            sleep(1);
            continue;
        }
        printf("[jclklib] Obtained data from Notification Event:\n");
        printf("+-------------------+--------------+-------------+\n");
        printf("| %-17s | %-12s | %-11s |\n", "Event", "Event Status",
            "Event Count");
        if (event2Sub1[0]) {
        printf("+-------------------+--------------+-------------+\n");
        }
        if (event2Sub1[0] & (1<<gmOffsetEvent)) {
            printf("| %-17s | %-12d | %-11ld |\n", "offset_in_range",
                jcl_state.offset_in_range,
                eventCount.offset_in_range_event_count);
        }
        if (event2Sub1[0] & (1<<servoLockedEvent)) {
            printf("| %-17s | %-12d | %-11ld |\n", "servo_locked",
               jcl_state.servo_locked, eventCount.servo_locked_event_count);
        }
        if (event2Sub1[0] & (1<<asCapableEvent)) {
            printf("| %-17s | %-12d | %-11ld |\n", "as_Capable",
                jcl_state.as_Capable, eventCount.asCapable_event_count);
        }
        if (event2Sub1[0] & (1<<gmPresentEvent)) {
            printf("| %-17s | %-12d | %-11ld |\n", "gmPresent",
                jcl_state.gm_present, eventCount.gmPresent_event_count);
        }
        if (event2Sub1[0] & (1<<gmChangedEvent)) {
            printf("| %-17s | %-12d | %-11ld |\n", "gm_Changed",
                jcl_state.gm_changed, eventCount.gm_changed_event_count);
            printf("+-------------------+--------------+-------------+\n");
            printf("| %-17s |     %02x%02x%02x.%02x%02x.%02x%02x%02x     |\n",
                "UUID", jcl_state.gmIdentity[0], jcl_state.gmIdentity[1],
                jcl_state.gmIdentity[2], jcl_state.gmIdentity[3],
                jcl_state.gmIdentity[4], jcl_state.gmIdentity[5],
                jcl_state.gmIdentity[6], jcl_state.gmIdentity[7]);
        }
        printf("+-------------------+--------------+-------------+\n");
        if (composite_event[0]) {
            printf("| %-17s | %-12d | %-11ld |\n", "composite_event",
                   jcl_state.composite_event, eventCount.composite_event_count);
        }
        if (composite_event[0] & (1<<gmOffsetEvent)) {
            printf("| - %-15s | %-12s | %-11s |\n", "offset_in_range", "", "");
        }
        if (composite_event[0] & (1<<servoLockedEvent)) {
            printf("| - %-15s | %-12s | %-11s |\n", "servo_locked", "", "");
        }
        if (composite_event[0] & (1<<asCapableEvent)) {
            printf("| - %-15s | %-12s | %-11s |\n", "as_Capable", "", "");
        }
        if (composite_event[0]) {
            printf("+-------------------+--------------+-------------+\n\n");
        } else {
            printf("\n");
        }
        sleep(1);
    }

	jcl_disconnect();

    return EXIT_SUCCESS;
}
