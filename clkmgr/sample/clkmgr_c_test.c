/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Test clkmgr client c wrapper code
 *
 * @author Christopher Hall <christopher.s.hall@@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 * @note This is a sample code, not a product! You should use it as a reference.
 *  You can compile it with: gcc -Wall clkmgr_c_test.c -o clkmgr_c_test -lclkmgr
 *  or use the Makefile with: make
 *
 */

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <clockmanager.h>
#include "pub/clkmgr/clockmanager_c.h"

double getMonotonicTime() {
    struct timespec time_spec;

    if (clock_gettime(CLOCK_MONOTONIC, &time_spec) == -1) {
        printf("clock_gettime failed");
        return -1;
    }

    double seconds = time_spec.tv_sec;
    double nanoseconds = time_spec.tv_nsec / 1e9;

    return seconds + nanoseconds;
}

bool isPositiveValue(const char *optarg, uint32_t *target, const char *errorMessage) {
    char *endptr = NULL;
    long value = strtol(optarg, &endptr, 10);

    if (endptr == optarg || *endptr != '\0') {
        fprintf(stderr, "Invalid argument: %s\n", optarg);
        return false;
    }
    if (value < 0) {
        fprintf(stderr, "%s\n", errorMessage);
        return false;
    }
    if (value > UINT32_MAX) {
        fprintf(stderr, "Out of range: %s\n", optarg);
        return false;
    }
    *target = (uint32_t)value;
    return true;
}

int main(int argc, char *argv[])
{
    uint32_t  ptp4lClockOffsetThreshold = 100000;
    uint32_t  chronyClockOffsetThreshold = 100000;
    bool subscribeAll = false;
    bool userInput = false;
    int ret = EXIT_SUCCESS;
    uint32_t idle_time = 1;
    uint32_t timeout = 10;
    char input[8];
    int index[8];
    int index_count = 0;
    struct timespec ts;
    int retval;
    int option;

    uint32_t event_mask = (Clkmgr_eventGMOffset | Clkmgr_eventSyncedToGM |
        Clkmgr_eventASCapable | Clkmgr_eventGMChanged);
    uint32_t composite_event_mask = (Clkmgr_eventGMOffset |
        Clkmgr_eventSyncedToGM | Clkmgr_eventASCapable);

    ClockSyncData_C* sync_data = clkmgr_clock_sync_data_create();
    PTPClockEvent_C* ptpClock = clkmgr_get_ptp(sync_data);
    SysClockEvent_C* sysClock = clkmgr_get_sys(sync_data);

    Clkmgr_Subscription* ptp_sub = clkmgr_ptp_subscription();
    Clkmgr_Subscription* sys_sub = clkmgr_sys_subscription();
    if (!ptp_sub || !sys_sub) {
        printf("Failed to create subscriptions\n");
        return EXIT_FAILURE;
    }

    while ((option = getopt(argc, argv, "aps:c:u:l:i:t:m:n:h")) != -1) {
        switch (option) {
        case 'a':
            subscribeAll = true;
            break;
        case 'p':
            userInput = true;
            break;
        case 's':
            event_mask = strtoul(optarg, NULL, 0);;
            break;
        case 'c':
            composite_event_mask = strtoul(optarg, NULL, 0);
            break;
        case 'l':
            if (!isPositiveValue(optarg, &ptp4lClockOffsetThreshold, "Invalid ptp4l GM Offset threshold!")) {
                return EXIT_FAILURE;
            }
            break;
        case 'i':
            idle_time = strtol(optarg, NULL, 10);
            break;
        case 't':
            timeout = strtol(optarg, NULL, 10);
            break;
        case 'm':
            if (!isPositiveValue(optarg, &chronyClockOffsetThreshold, "Invalid Chrony Offset threshold!")) {
                return EXIT_FAILURE;
            }
            break;
        case 'h':
            printf("Usage of %s :\n"
                   "Options:\n"
                   "  -a subscribe to all time base indices\n"
                   "     Default: timeBaseIndex: 1\n"
                   "  -p enable user to subscribe to specific time base indices\n"
                   "  -s subscribe_event_mask\n"
                   "     Default: 0x%x\n"
                   "     Bit 0: eventGMOffset\n"
                   "     Bit 1: eventSyncedToGM\n"
                   "     Bit 2: eventASCapable\n"
                   "     Bit 3: eventGMChanged\n"
                   "  -c composite_event_mask\n"
                   "     Default: 0x%x\n"
                   "     Bit 0: eventGMOffset\n"
                   "     Bit 1: eventSyncedToGM\n"
                   "     Bit 2: eventASCapable\n"
                   "  -l gm offset threshold (ns)\n"
                   "     Default: %d ns\n"
                   "  -i idle time (s)\n"
                   "     Default: %d s\n"
                   "  -m chrony offset threshold (ns)\n"
                   "     Default: %d ns\n"
                   "  -t timeout in waiting notification event (s)\n"
                   "     Default: %d s\n",
                   argv[0], event_mask,
                   composite_event_mask,
                   ptp4lClockOffsetThreshold, chronyClockOffsetThreshold,
                   idle_time, timeout);
            return EXIT_SUCCESS;
        default:
            printf("Usage of %s :\n"
                   "Options:\n"
                   "  -a subscribe to all time base indices\n"
                   "     Default: timeBaseIndex: 1\n"
                   "  -p enable user to subscribe to specific time base indices\n"
                   "  -s subscribe_event_mask\n"
                   "     Default: 0x%x\n"
                   "     Bit 0: eventGMOffset\n"
                   "     Bit 1: eventSyncedToGM\n"
                   "     Bit 2: eventASCapable\n"
                   "     Bit 3: eventGMChanged\n"
                   "  -c composite_event_mask\n"
                   "     Default: 0x%x\n"
                   "     Bit 0: eventGMOffset\n"
                   "     Bit 1: eventSyncedToGM\n"
                   "     Bit 2: eventASCapable\n"
                   "  -l gm offset threshold (ns)\n"
                   "     Default: %d ns\n"
                   "  -i idle time (s)\n"
                   "     Default: %d s\n"
                   "  -m chrony offset threshold (ns)\n"
                   "     Default: %d ns\n"
                   "  -t timeout in waiting notification event (s)\n"
                   "     Default: %d s\n",
                   argv[0], event_mask,
                   composite_event_mask,
                    ptp4lClockOffsetThreshold, chronyClockOffsetThreshold,
                   idle_time, timeout);
            return EXIT_FAILURE;
        }
    }

    if (clkmgr_connect() == false) {
        printf("[clkmgr] Failure in connecting !!!\n");
        ret = EXIT_FAILURE;
        goto do_exit;
    }

    sleep(1);

    clkmgr_set_event_mask(ptp_sub, event_mask);
    clkmgr_set_composite_event_mask(ptp_sub, composite_event_mask);
    clkmgr_set_clock_offset_threshold(ptp_sub, ptp4lClockOffsetThreshold);
    clkmgr_set_clock_offset_threshold(sys_sub, chronyClockOffsetThreshold);

    size_t index_size = clkmgr_get_timebase_cfgs_size();
    if(index_size == 0) {
        printf("[clkmgr] No available clock found !!!\n");
        ret = EXIT_FAILURE;
        goto do_exit;
    }
    printf("[clkmgr] List of available clock: \n");
    for(size_t i = 1; i <= index_size; i++) {
        if (clkmgr_isTimeBaseIndexPresent(i)) {
            printf("TimeBaseIndex: %zu\n", i);
            printf("timeBaseName: %s\n", clkmgr_timeBaseName(i));
            if(clkmgr_havePtp(i)) {
                printf("interfaceName: %s\n", clkmgr_ifName(i));
                printf("transportSpecific: %d\n", clkmgr_transportSpecific(i));
                printf("domainNumber: %d\n\n", clkmgr_domainNumber(i));
            }
        } else {
            printf("Failed to get time base configuration for index %ld\n", i);
        }
    }

    if (subscribeAll) {
        for (size_t i = 1; i <= index_size; i++) {
            if (clkmgr_isTimeBaseIndexPresent(i)) {
                index[index_count++] = i;
            }
        }
    } else if (userInput) {
        printf("Enter the time base indices to subscribe (comma-separated, default is 1): ");
        fgets(input, sizeof(input), stdin);
        if (strlen(input) > 1) {
            char *token = strtok(input, ", ");
            while (token != NULL) {
                if (isdigit(*token)) {
                    index[index_count++] = atoi(token);
                } else {
                    printf("Invalid time base index!\n");
                    return EXIT_FAILURE;
                }
                token = strtok(NULL, ", ");
            }
        } else {
            printf("Invalid input. Using default time base index 1.\n");
            index[index_count++] = 1;
        }
    } else {
        index[index_count++] = 1;
    }

    for (size_t i = 0; i < index_count; i++) {
        Clkmgr_SyncSubscription* clock_sub = clkmgr_clock_sync_subscription();
        clkmgr_enable_ptp(clock_sub);
        clkmgr_enable_sys(clock_sub);
        clkmgr_set_ptp_subscription(clock_sub, ptp_sub);
        clkmgr_set_sys_subscription(clock_sub, sys_sub);
        /* Subscribe to default time base index 1 */
        printf("[clkmgr] Subscribe to time base index: %d\n", index[i]);
        if (clkmgr_subscribe(clock_sub, index[i], sync_data) == false) {
            printf("[clkmgr] Failure in subscribing to clkmgr Proxy !!!\n");
            clkmgr_disconnect();
            ret = EXIT_FAILURE;
            goto do_exit;
        }

        printf("[clkmgr][%.3f] Obtained data from Subscription Event:\n",
            getMonotonicTime());
        if (!clkmgr_gettime(&ts)) {
            perror("clock_c_gettime failed");
        } else {
            printf("[clkmgr] Current Time of CLOCK_REALTIME: %ld ns\n",
                (ts.tv_sec * 1000000000) + ts.tv_nsec);
        }

        printf("|---------------------------|------------------------|\n");
        printf("| %-25s | %-22s |\n", "Event", "Event Status");
        if (event_mask) {
            printf("|---------------------------|------------------------|\n");
        }
        if (event_mask & Clkmgr_eventGMOffset) {
            printf("| %-25s | %-22d |\n", "offset_in_range",
                clkmgr_is_offset_in_range((const ClockEventBase_C*)ptpClock));
        }
        if (event_mask & Clkmgr_eventSyncedToGM) {
            printf("| %-25s | %-22d |\n", "synced_to_primary_clock",
                clkmgr_is_synced_with_gm(ptpClock));
        }
        if (event_mask & Clkmgr_eventASCapable) {
            printf("| %-25s | %-22d |\n", "as_capable",
                clkmgr_is_as_capable(ptpClock));
        }
        if (event_mask & Clkmgr_eventGMChanged) {
            printf("| %-25s | %-22d |\n", "gm_Changed",
                clkmgr_is_gm_changed((const ClockEventBase_C*)ptpClock));
        }
        printf("|---------------------------|------------------------|\n");
        uint64_t gmClockUUID = clkmgr_get_gm_identity((const ClockEventBase_C*)ptpClock);
        uint8_t gm_identity[8];
        for (int i = 0; i < 8; ++i) {
            gm_identity[i] = (uint8_t)(gmClockUUID >> (8 * (7 - i)));
        }
        printf("| %-25s | %02x%02x%02x.%02x%02x.%02x%02x%02x     |\n",
            "GM UUID", gm_identity[0], gm_identity[1],
            gm_identity[2], gm_identity[3],
            gm_identity[4], gm_identity[5],
            gm_identity[6], gm_identity[7]);
        printf("| %-25s | %-19ld ns |\n",
            "clock_offset", clkmgr_get_clock_offset((const ClockEventBase_C*)ptpClock));
        printf("| %-25s | %-19ld ns |\n",
                "notification_timestamp", clkmgr_get_notification_timestamp((const ClockEventBase_C*)ptpClock));
        printf("| %-25s | %-19ld us |\n",
                "gm_sync_interval", clkmgr_get_sync_interval((const ClockEventBase_C*)ptpClock));
        printf("|---------------------------|------------------------|\n");
        if (composite_event_mask) {
            printf("| %-25s | %-22d |\n", "composite_event",
                clkmgr_is_composite_event_met(ptpClock));
        }
        if (composite_event_mask & Clkmgr_eventGMOffset) {
            printf("| - %-23s | %-22s |\n", "offset_in_range", " ");
        }
        if (composite_event_mask & Clkmgr_eventSyncedToGM) {
            printf("| - %-19s | %-22s |\n", "synced_to_primary_clock", " ");
        }
        if (composite_event_mask & Clkmgr_eventASCapable) {
            printf("| - %-23s | %-22s |\n", "as_capable", " ");
        }
        if (composite_event_mask) {
            printf("|---------------------------|------------------------|\n\n");
        } else {
            printf("\n");
        }
        printf("|---------------------------|------------------------|\n");
        printf("| %-25s | %-22d |\n", "chrony_offset_in_range",
                clkmgr_is_offset_in_range((const ClockEventBase_C*)sysClock));
        printf("|---------------------------|------------------------|\n");
        printf("| %-25s | %-19ld ns |\n",
            "chrony_clock_offset", clkmgr_get_clock_offset((const ClockEventBase_C*)sysClock));
        // Question: Why chrony gm id and ptp gm id are different?
        printf("| %-25s | %-19lX    |\n",
            "chrony_clock_reference_id", clkmgr_get_gm_identity((const ClockEventBase_C*)sysClock));
        printf("| %-25s | %-19ld us |\n",
            "chrony_polling_interval", clkmgr_get_sync_interval((const ClockEventBase_C*)sysClock));
        printf("|---------------------------|------------------------|\n\n");
            clkmgr_clock_sync_subscription_destroy(clock_sub);
    }
    sleep(1);

    while (1) {
        for (size_t i = 0; i < index_count; i++) {
            printf("[clkmgr][%.3f] Waiting Notification from time base index %d ...\n",
                getMonotonicTime(), index[i]);
            retval = clkmgr_status_wait(timeout, index[i], sync_data);
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
            if (!clkmgr_gettime(&ts)) {
                perror("clock_c_gettime failed");
            } else {
                printf("[clkmgr] Current Time of CLOCK_REALTIME: %ld ns\n",
                    (ts.tv_sec * 1000000000) + ts.tv_nsec);
            }
            printf("|---------------------------|--------------|-------------|\n");
            printf("| %-25s | %-12s | %-11s |\n", "Event", "Event Status",
                "Event Count");
            if (event_mask) {
            printf("|---------------------------|--------------|-------------|\n");
            }
            if (event_mask & Clkmgr_eventGMOffset) {
                printf("| %-25s | %-12d | %-11d |\n", "offset_in_range",
                    clkmgr_is_offset_in_range((const ClockEventBase_C*)ptpClock),
                    clkmgr_get_offset_in_range_event_count((const ClockEventBase_C*)ptpClock));
            }
            if (event_mask & Clkmgr_eventSyncedToGM) {
                printf("| %-25s | %-12d | %-11d |\n", "synced_to_primary_clock",
                    clkmgr_is_synced_with_gm(ptpClock),
                    clkmgr_get_synced_with_gm_event_count(ptpClock));
            }
            if (event_mask & Clkmgr_eventASCapable) {
                printf("| %-25s | %-12d | %-11d |\n", "as_capable",
                    clkmgr_is_as_capable(ptpClock),
                    clkmgr_get_as_capable_event_count(ptpClock));
            }
            if (event_mask & Clkmgr_eventGMChanged) {
                printf("| %-25s | %-12d | %-11d |\n", "gm_Changed",
                    clkmgr_is_gm_changed((const ClockEventBase_C*)ptpClock),
                    clkmgr_get_gm_changed_event_count((const ClockEventBase_C*)ptpClock));
            }
            printf("|---------------------------|--------------|-------------|\n");
            uint64_t gmClockUUID = clkmgr_get_gm_identity((const ClockEventBase_C*)ptpClock);
            uint8_t gm_identity[8];
            for (int i = 0; i < 8; ++i) {
                gm_identity[i] = (uint8_t)(gmClockUUID >> (8 * (7 - i)));
            }
            printf("| %-25s |     %02x%02x%02x.%02x%02x.%02x%02x%02x     |\n",
                "GM UUID", gm_identity[0], gm_identity[1],
                gm_identity[2], gm_identity[3],
                gm_identity[4], gm_identity[5],
                gm_identity[6], gm_identity[7]);
            printf("| %-25s |     %-19ld ns |\n",
                "clock_offset",
                clkmgr_get_clock_offset((const ClockEventBase_C*)ptpClock));
            printf("| %-25s |     %-19ld ns |\n",
                "notification_timestamp",
                clkmgr_get_notification_timestamp((const ClockEventBase_C*)ptpClock));
            printf("| %-25s |     %-19ld us |\n",
                "gm_sync_interval", clkmgr_get_sync_interval((const ClockEventBase_C*)ptpClock));
            printf("|---------------------------|--------------|-------------|\n");
            if (composite_event_mask) {
                printf("| %-25s | %-12d | %-11d |\n", "composite_event",
                    clkmgr_is_composite_event_met(ptpClock),
                    clkmgr_get_composite_event_count(ptpClock));
            }
            if (composite_event_mask & Clkmgr_eventGMOffset) {
                printf("| - %-23s | %-12s | %-11s |\n", "offset_in_range", "", "");
            }
            if (composite_event_mask & Clkmgr_eventSyncedToGM) {
                printf("| - %-19s | %-12s | %-11s |\n", "synced_to_primary_clock", "", "");
            }
            if (composite_event_mask & Clkmgr_eventASCapable) {
                printf("| - %-23s | %-12s | %-11s |\n", "as_capable", "", "");
            }
            if (composite_event_mask) {
                printf("|---------------------------|--------------|-------------|\n\n");
            } else {
                printf("\n");
            }
            printf("|---------------------------|----------------------------|\n");
            printf("| %-25s | %-12d | %-11d |\n", "chrony_offset_in_range",
                clkmgr_is_offset_in_range((const ClockEventBase_C*)sysClock),
                clkmgr_get_offset_in_range_event_count((const ClockEventBase_C*)sysClock));
            printf("|---------------------------|----------------------------|\n");
            printf("| %-25s |     %-19ld ns |\n",
                "chrony_clock_offset",
                clkmgr_get_clock_offset((const ClockEventBase_C*)sysClock));
            printf("| %-25s |     %-19lX    |\n",
                "chrony_clock_reference_id",
                clkmgr_get_gm_identity((const ClockEventBase_C*)sysClock));
            printf("| %-25s |     %-19ld us |\n",
                "chrony_polling_interval",
                clkmgr_get_sync_interval((const ClockEventBase_C*)sysClock));
            printf("|---------------------------|----------------------------|\n\n");

            printf("[clkmgr][%.3f] sleep for %d seconds...\n\n",
                getMonotonicTime(), idle_time);
            sleep(idle_time);
        }
    }

do_exit:
    clkmgr_subscription_destroy(ptp_sub);
    clkmgr_subscription_destroy(sys_sub);
    clkmgr_clock_sync_data_destroy(sync_data);
    clkmgr_disconnect();

    return ret;
}
