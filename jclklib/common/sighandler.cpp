/*
 * SPDX-License-Identifier: LGPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. All rights reserved.
 */

/** @file sighandler.cpp
 * @brief Signal handling utilities
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation. All rights reserved.
 * @license LGPL-3.0-or-later
 *
 */

#include <iostream>
#include <signal.h>

#include <common/print.hpp>
#include <common/sighandler.hpp>

#define INTR_SIGNAL (SIGUSR1)

using namespace JClkLibCommon;
using namespace std;

#define SIGADDSET(set,sig,ret)            \
    if(sigaddset(set, sig) == -1)         \
        return ret;
	
bool AddStopSignal(sigset_t *sigset)
{
    SIGADDSET(sigset, SIGINT,  false);
    SIGADDSET(sigset, SIGHUP,  false);
    SIGADDSET(sigset, SIGTERM, false);
    SIGADDSET(sigset, SIGQUIT, false);
    SIGADDSET(sigset, SIGALRM, false);
    SIGADDSET(sigset, SIGABRT, false);

    return true;
}

bool AddInterruptSignal(sigset_t *sigset)
{
    SIGADDSET(sigset, SIGUSR1, false);
    SIGADDSET(sigset, SIGUSR2, false);

    return true;
}

bool GenerateBlockSigset(sigset_t *block) {
    sigemptyset(block);

    if (!AddStopSignal(block))
        return false;
    if (!AddInterruptSignal(block))
        return false;

    return true;
}

bool GenerateWaitSigset(sigset_t *wait) {
    sigemptyset(wait);
	
    if (!AddStopSignal(wait))
        return false;

    return true;
}

bool JClkLibCommon::BlockStopSignal()
{
    sigset_t blockSigset;

    if (!GenerateBlockSigset(&blockSigset))
        return false;
    if (pthread_sigmask(SIG_BLOCK,&blockSigset,NULL) != 0)
        return false;

    return true;
}

bool JClkLibCommon::WaitForStopSignal()
{
    sigset_t waitSigset;
    int cause;

    if (!GenerateWaitSigset(&waitSigset))
        return false;
    PrintDebug("Waiting for Interrupt Signal");
    if (sigwait(&waitSigset, &cause) == -1) {
        PrintErrorCode("Waiting for Interrupt Signal");
        return false;
    }
    PrintDebug("Received Interrupt Signal");
	
    return true;
}

void NullSigaction(int sig, siginfo_t *siginfo, void *ctx) {}

bool JClkLibCommon::EnableSyscallInterruptSignal()
{
    sigset_t unblockSigset;
    struct sigaction intrSigaction;

    intrSigaction.sa_sigaction = NullSigaction;
    sigemptyset(&intrSigaction.sa_mask);
    intrSigaction.sa_flags = SA_SIGINFO;
    if (sigaction(INTR_SIGNAL, &intrSigaction, NULL) == -1)
        return false;

    sigemptyset(&unblockSigset);
    SIGADDSET(&unblockSigset, INTR_SIGNAL, false);
    if (pthread_sigmask(SIG_UNBLOCK,&unblockSigset,NULL) != 0)
        return false;

    return true;
}

bool JClkLibCommon::SendSyscallInterruptSignal(thread &t)
{
    int ret;

    if ((ret = pthread_kill(t.native_handle(), INTR_SIGNAL)) != 0) {
        PrintError("pthread_kill()", ret);
        return false;
    }

    return true;
}
