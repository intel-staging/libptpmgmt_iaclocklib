/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file jcltypes.hpp
 * @brief Common message type
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 */

#ifndef COMMON_JCLTYPES_HPP
#define COMMON_JCLTYPES_HPP

#include <cstdint>

namespace JClkLibCommon
{
typedef std::uint16_t sessionId_t;
static const sessionId_t InvalidSessionId = (sessionId_t)(-1);

typedef std::uint8_t msgAck_t;
enum  : msgAck_t {ACK_FAIL = (msgAck_t) -1, ACK_NONE = 0, ACK_SUCCESS = 1, };

typedef std::uint8_t msgId_t;
enum msgId : msgId_t {INVALID_MSG = (msgId_t) -1, NULL_MSG = 1, CONNECT_MSG,
    SUBSCRIBE_MSG, NOTIFY_MESSAGE, DISCONNECT_MSG
};
}

#endif /* COMMON_JCLTYPES_HPP */
