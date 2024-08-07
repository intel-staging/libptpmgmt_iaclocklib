/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
 */

/** @file transport.cpp
 * @brief Client transport base implementation for JClkLib.
 * It is extended for specific transports such as POSIX message queue.
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation.
 * @license BSD-3-Clause
 *
 */

#include <client/msgq_tport.hpp>
#include <client/null_tport.hpp>
#include <client/transport.hpp>
#include <common/print.hpp>
#include <common/sighandler.hpp>
#include <common/util.hpp>

using namespace JClkLibClient;
using namespace JClkLibCommon;
using namespace std;

bool ClientTransport::init()
{
    if(!Transport::init())
        return false;
    PrintDebug("Finished common init");
    return JClkLibCommon::_initTransport<ClientMessageQueue>();
}

bool ClientTransport::stop()
{
    if(!Transport::stop())
        return false;
    /* Do any transport specific stop */
    return JClkLibCommon::_stopTransport<ClientMessageQueue>();
}

bool ClientTransport::finalize()
{
    if(!Transport::finalize())
        return false;
    return JClkLibCommon::_finalizeTransport<ClientMessageQueue>();
}
