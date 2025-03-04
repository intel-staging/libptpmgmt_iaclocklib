/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2025 Intel Corporation. */

/** @file
 * @brief Proxy JSON parser
 *
 * @author by Goh Wei Sheng <wei.sheng.goh@intel.com>
 * @copyright © 2025 Intel Corporation.
 *
 */

#ifndef PROXY_CONFIG_PARSER_HPP
#define PROXY_CONFIG_PARSER_HPP

#include <cstdint>
#include <string>
#include <vector>

struct TimeBaseCfg {
    std::string udsAddrChrony;
    std::string udsAddrPtp4l;
    std::string interfaceName;
    int timeBaseIndex;
    uint8_t transportSpecific;
    uint8_t domainNumber;
};

/* Global vector to hold all time base configurations */
extern std::vector<TimeBaseCfg> timeBaseCfgs;

#endif /* PROXY_CONFIG_PARSER_HPP */
