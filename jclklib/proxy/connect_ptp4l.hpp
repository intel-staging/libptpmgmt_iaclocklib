/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file connect.hpp
 * @brief Proxy connect ptp4l message class.
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 */

#include <common/connect_msg.hpp>
#include <proxy/message.hpp>
#include <atomic>

using namespace std;

namespace JClkLibProxy
{
    class Connect
    {
    private:
    public:
        struct monitor_handle {
            atomic<bool> is_ptp4l_alive = false;
            pthread_t ptp4l_monitor_thread;
        };
        static int connect();
        static int start_monitoring_threads();
        static monitor_handle monitor_hdle;
    };
}