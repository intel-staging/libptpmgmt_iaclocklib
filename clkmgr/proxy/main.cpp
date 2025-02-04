/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Proxy application implementation
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include <cstdio>
#include <getopt.h>

#include "common/print.hpp"
#include "common/sighandler.hpp"
#include "proxy/connect_ptp4l.hpp"
#ifdef HAVE_LIBCHRONY
#include "proxy/connect_chrony.hpp"
#endif
#include "proxy/message.hpp"
#include "proxy/transport.hpp"

__CLKMGR_NAMESPACE_USE;

using namespace std;

int main(int argc, char *argv[])
{
    uint8_t transport_specific = 0;
    int value = 0;
    int opt;
    while((opt = getopt(argc, argv, "t:h")) != -1) {
        switch(opt) {
            case 't':
                value = stoi(optarg);
                if(value < 0 || value > 255) {
                    fprintf(stderr, "Invalid transport specific value: %s\n",
                        optarg);
                    return EXIT_FAILURE;
                }
                transport_specific = static_cast<uint8_t>(value);
                break;
            case 'h':
                printf("Usage of %s:\n"
                    "Options:\n"
                    " -t transport specific\n"
                    "    Default: 0x%x\n",
                    argv[0], transport_specific);
                return EXIT_SUCCESS;
            default:
                fprintf(stderr, "Usage of %s:\n"
                    "Options:\n"
                    " -t transport specific\n"
                    "    Default: 0x%x\n",
                    argv[0],
                    transport_specific);
                return EXIT_FAILURE;
        }
    }
    BlockStopSignal();
    if(!ProxyTransport::init()) {
        PrintError("Transport init failed");
        return EXIT_FAILURE;
    }
    if(!ProxyMessage::init()) {
        PrintError("Message init failed");
        return EXIT_FAILURE;
    }
    #ifdef HAVE_LIBCHRONY
    ConnectChrony::connect_chrony();
    #endif
    Connect::connect(transport_specific);
    WaitForStopSignal();
    PrintDebug("Got stop signal");
    Connect::disconnect();
    if(!ProxyTransport::stop()) {
        PrintError("stop failed");
        return EXIT_FAILURE;
    }
    if(!ProxyTransport::finalize()) {
        PrintError("finalize failed");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
