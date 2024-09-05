/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file clkmgr_test.cpp
 * @brief Test clkmgr client code
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 * @note This is a sample code, not a product! You should use it as a reference.
 *  You can compile it with: g++ -Wall clkmgr_test.cpp -o clkmgr_test -lclkmgr
 *  or use the Makefile with: make
 *
 */

#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <clockmanager.h>

using namespace clkmgr;

volatile sig_atomic_t signal_flag = 0;

void signal_handler(int sig)
{
    signal_flag = 1;
}

double getMonotonicTime() {
    struct timespec timeSpec;

    if (clock_gettime(CLOCK_MONOTONIC, &timeSpec) == -1) {
        perror("clock_gettime failed");
        return -1;
    }

    double seconds = timeSpec.tv_sec;
    double nanoseconds = timeSpec.tv_nsec / 1e9;

    return seconds + nanoseconds;
}

int main(int argc, char *argv[])
{
    clkmgr_event_count eventCount = {};
    ClkMgrSubscription sub = {};
    clkmgr_event_state state = {};
    int lower_master_offset = -100000;
    int upper_master_offset = 100000;
    int ret = EXIT_SUCCESS;
    int idle_time = 1;
    int timeout = 10;
    int retval;
    int opt;

    std::uint32_t event2Sub = {
        (eventGMOffset | eventSyncedToGM | eventASCapable |
        eventGMChanged)
    };

    std::uint32_t composite_event = {
        (eventGMOffset | eventSyncedToGM | eventASCapable)
    };

    while ((opt = getopt(argc, argv, "s:c:u:l:i:t:h")) != -1) {
        switch (opt) {
        case 's':
            event2Sub = std::stoul(optarg, nullptr, 0);
            break;
        case 'c':
            composite_event = std::stoul(optarg, nullptr, 0);
            break;
        case 'u':
            upper_master_offset = std::stoi(optarg);
            break;
        case 'l':
            lower_master_offset = std::stoi(optarg);
            break;
        case 'i':
            idle_time = std::stoi(optarg);
            break;
        case 't':
            timeout = std::stoi(optarg);
            break;
        case 'h':
            std::cout << "Usage of " << argv[0] << " :\n"
                "Options:\n"
                "  -s subscribe_event_mask\n"
                "     Default: 0x" << std::hex << event2Sub << "\n"
                "     Bit 0: eventGMOffset\n"
                "     Bit 1: eventSyncedToGM\n"
                "     Bit 2: eventASCapable\n"
                "     Bit 3: eventGMChanged\n"
                "  -c composite_event_mask\n"
                "     Default: 0x" << composite_event << "\n"
                "     Bit 0: eventGMOffset\n"
                "     Bit 1: eventSyncedToGM\n"
                "     Bit 2: eventASCapable\n"
                "  -u upper master offset (ns)\n"
                "     Default: " << std::dec << upper_master_offset << " ns\n"
                "  -l lower master offset (ns)\n"
                "     Default: " << lower_master_offset << " ns\n"
                "  -i idle time (s)\n"
                "     Default: " << idle_time << " s\n"
                "  -t timeout in waiting notification event (s)\n"
                "     Default: " << timeout << " s\n";
            return EXIT_SUCCESS;
        default:
            std::cerr << "Usage of " << argv[0] << " :\n"
                "Options:\n"
                "  -s subscribe_event_mask\n"
                "     Default: 0x" << std::hex << event2Sub << "\n"
                "     Bit 0: eventGMOffset\n"
                "     Bit 1: eventSyncedToGM\n"
                "     Bit 2: eventASCapable\n"
                "     Bit 3: eventGMChanged\n"
                "  -c composite_event_mask\n"
                "     Default: 0x" << composite_event << "\n"
                "     Bit 0: eventGMOffset\n"
                "     Bit 1: eventSyncedToGM\n"
                "     Bit 2: eventASCapable\n"
                "  -u upper master offset (ns)\n"
                "     Default: " << std::dec << upper_master_offset << " ns\n"
                "  -l lower master offset (ns)\n"
                "     Default: " << lower_master_offset << " ns\n"
                "  -i idle time (s)\n"
                "     Default: " << idle_time << " s\n"
                "  -t timeout in waiting notification event (s)\n"
                "     Default: " << timeout << " s\n";
            return EXIT_FAILURE;
        }
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);

    ClockManager *cmAPI = new ClockManager();

    cmAPI->init();

    ClientState &myState = cmAPI->getClientState();

    if (cmAPI->clkmgr_connect() == false) {
        std::cout << "[clkmgr] Failure in connecting !!!\n";
        ret = EXIT_FAILURE;
        goto do_exit;
    } else {
        std::cout << "[clkmgr] Connected. Session ID : " <<
            myState.get_sessionId() << "\n";
    }

    sleep(1);

    sub.set_event_mask(event2Sub);
    sub.define_threshold(thresholdGMOffset, upper_master_offset,
        lower_master_offset);
    sub.set_composite_event_mask(composite_event);
    std::cout << "[clkmgr] set subscribe event : "
        << std::hex << sub.c_get_val_event_mask() <<  "\n";
    std::cout << "[clkmgr] set composite event : "
        << std::hex << sub.c_get_val_composite_event_mask() <<  "\n";
    std::cout << "Upper Master Offset: " << upper_master_offset << " ns\n";
    std::cout << "Lower Master Offset: " << lower_master_offset << " ns\n\n";

    if (cmAPI->clkmgr_subscribe(sub, state) == false) {
        std::cerr << "[clkmgr] Failure in subscribing to clkmgr Proxy !!!\n";
        cmAPI->clkmgr_disconnect();
        return EXIT_FAILURE;
    }

    printf("[clkmgr][%.3f] Obtained data from Subscription Event:\n",
        getMonotonicTime());
    printf("+---------------------------+--------------------+\n");
    printf("| %-25s | %-18s |\n", "Event", "Event Status");
    if (event2Sub) {
        printf("+---------------------------+--------------------+\n");
    }
    if (event2Sub & eventGMOffset) {
        printf("| %-25s | %-18d |\n", "offset_in_range",
            state.offset_in_range);
    }
    if (event2Sub & eventSyncedToGM) {
        printf("| %-25s | %-18d |\n", "synced_to_primary_clock", state.synced_to_primary_clock);
    }
    if (event2Sub & eventASCapable) {
        printf("| %-25s | %-18d |\n", "as_capable", state.as_capable);
    }
    if (event2Sub & eventGMChanged) {
        printf("| %-25s | %-18d |\n", "gm_Changed", state.gm_changed);
        printf("+---------------------------+--------------------+\n");
        printf("| %-25s | %02x%02x%02x.%02x%02x.%02x%02x%02x |\n", "UUID",
            state.gm_identity[0], state.gm_identity[1],
            state.gm_identity[2], state.gm_identity[3],
            state.gm_identity[4], state.gm_identity[5],
            state.gm_identity[6], state.gm_identity[7]);
        printf("| %-25s | %-15ld ns |\n",
                "clock_offset", state.clock_offset);
        printf("| %-25s | %-16.3f s |\n",
                "notification_timestamp", state.notification_timestamp / 1e9);
    }
    printf("+---------------------------+--------------------+\n");
    if (composite_event) {
        printf("| %-25s | %-18d |\n", "composite_event",
            state.composite_event);
    }
    if (composite_event & eventGMOffset) {
        printf("| - %-23s | %-18s |\n", "offset_in_range", " ");
    }
    if (composite_event & eventSyncedToGM) {
        printf("| - %-19s | %-18s |\n", "synced_to_primary_clock", " ");
    }
    if (composite_event & eventASCapable) {
        printf("| - %-23s | %-18s |\n", "as_capable", " ");
    }
    if (composite_event) {
        printf("+---------------------------+--------------------+\n\n");
    } else {
        printf("\n");
    }

    sleep(1);

    while (!signal_flag) {
        printf("[clkmgr][%.3f] Waiting for Notification Event...\n",
            getMonotonicTime());
        retval = cmAPI->clkmgr_status_wait(timeout, state , eventCount);
        if (!retval) {
            printf("[clkmgr][%.3f] No event status changes identified in %d seconds.\n\n",
                getMonotonicTime(), timeout);
            printf("[clkmgr][%.3f] sleep for %d seconds...\n\n",
                getMonotonicTime(), idle_time);
            sleep(idle_time);
            continue;
        } else if (retval < 0) {
            printf("[clkmgr][%.3f] Terminating: lost connection to clkmgr Proxy\n",
                getMonotonicTime());
            return EXIT_SUCCESS;
        }

        printf("[clkmgr][%.3f] Obtained data from Notification Event:\n",
            getMonotonicTime());
        printf("+---------------------------+--------------+-------------+\n");
        printf("| %-25s | %-12s | %-11s |\n", "Event", "Event Status",
            "Event Count");
        if (event2Sub) {
        printf("+---------------------------+--------------+-------------+\n");
        }
        if (event2Sub & eventGMOffset) {
            printf("| %-25s | %-12d | %-11d |\n", "offset_in_range",
                state.offset_in_range,
                eventCount.offset_in_range_event_count);
        }
        if (event2Sub & eventSyncedToGM) {
            printf("| %-25s | %-12d | %-11d |\n", "synced_to_primary_clock",
               state.synced_to_primary_clock, eventCount.synced_to_gm_event_count);
        }
        if (event2Sub & eventASCapable) {
            printf("| %-25s | %-12d | %-11d |\n", "as_capable",
                state.as_capable, eventCount.as_capable_event_count);
        }
        if (event2Sub & eventGMChanged) {
            printf("| %-25s | %-12d | %-11d |\n", "gm_Changed",
                state.gm_changed, eventCount.gm_changed_event_count);
            printf("+---------------------------+--------------+-------------+\n");
            printf("| %-25s |     %02x%02x%02x.%02x%02x.%02x%02x%02x     |\n",
                "UUID", state.gm_identity[0], state.gm_identity[1],
                state.gm_identity[2], state.gm_identity[3],
                state.gm_identity[4], state.gm_identity[5],
                state.gm_identity[6], state.gm_identity[7]);
            printf("| %-25s |     %-19ld ns |\n",
                "clock_offset", state.clock_offset);
            printf("| %-25s |     %-20.3f s |\n",
                "notification_timestamp", state.notification_timestamp / 1e9);
        }
        printf("+---------------------------+--------------+-------------+\n");
        if (composite_event) {
            printf("| %-25s | %-12d | %-11d |\n", "composite_event",
                   state.composite_event, eventCount.composite_event_count);
        }
        if (composite_event & eventGMOffset) {
            printf("| - %-23s | %-12s | %-11s |\n", "offset_in_range", "", "");
        }
        if (composite_event & eventSyncedToGM) {
            printf("| - %-19s | %-12s | %-11s |\n", "synced_to_primary_clock", "", "");
        }
        if (composite_event & eventASCapable) {
            printf("| - %-23s | %-12s | %-11s |\n", "as_capable", "", "");
        }
        if (composite_event) {
            printf("+---------------------------+--------------+-------------+\n\n");
        } else {
            printf("\n");
        }

        printf("[clkmgr][%.3f] sleep for %d seconds...\n\n",
            getMonotonicTime(), idle_time);
        sleep(idle_time);
    }

do_exit:
    cmAPI->clkmgr_disconnect();

    return ret;
}