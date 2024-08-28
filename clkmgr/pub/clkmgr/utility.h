/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file utility.h
 * @brief Helper functions and macros
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 */

#ifndef CLKMGR_UTILITY_H
#define CLKMGR_UTILITY_H

#include <array>
#include <cstdint>

/** Using clkmgr namespace */
#define __CLKMGR_NAMESPACE_USE using namespace clkmgr;
/** Define start of clkmgr namespace block */
#define __CLKMGR_NAMESPACE_BEGIN namespace clkmgr {
/** Define end of clkmgr namespace block */
#define __CLKMGR_NAMESPACE_END }

__CLKMGR_NAMESPACE_BEGIN

#define DECLARE_ACCESSOR(varname) \
    const decltype(varname) &getc_##varname() { return varname; } \
    decltype(varname) &get_##varname() { return varname; } \
    void set_##varname (const decltype(varname) &varname) \
    { this->varname = varname; } \
    decltype(varname) c_get_val_##varname () const { return varname; }

#define TRANSPORT_CLIENTID_LENGTH (512)
typedef std::array<std::uint8_t, TRANSPORT_CLIENTID_LENGTH> TransportClientId;

typedef std::uint16_t sessionId_t;
static const sessionId_t InvalidSessionId = (sessionId_t)(-1);

__CLKMGR_NAMESPACE_END

#endif /* CLKMGR_UTILITY_H */
