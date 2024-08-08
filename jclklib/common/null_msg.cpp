/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file null_msg.cpp
 * @brief Non-functional message implementation for debug
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 */

#include <common/null_msg.hpp>

using namespace JClkLibCommon;
using namespace std;

MAKE_RXBUFFER_TYPE(CommonNullMessage::buildMessage)
{
    return true;
}

bool CommonNullMessage::initMessage()
{
    return true;
}
