/*! \file test.cpp
    \brief Test client code

    (C) Copyright Intel Corporation 2023. All rights reserved. Intel Confidential.
    Author: Christopher Hall <christopher.s.hall@intel.com>
*/

#include "init.hpp"

#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <common/jclklib_import.hpp>
#include <client/client_state.hpp>
#include <client/connect_msg.hpp>
#include <signal.h>

using namespace JClkLibClient;
using namespace JClkLibCommon;
using namespace std;

volatile sig_atomic_t signal_flag = 0;

void signal_handler(int sig)
{
    signal_flag = 1;
}

int main()
{
    int ret = EXIT_SUCCESS;
    JClkLibCommon::jcl_subscription sub = {};
    JClkLibCommon::jcl_state currentState = {};

    std::uint32_t event2Sub1[1] = {((1<<gmPresentEvent)|(1<<gmChangedEvent)|(1<<servoLockedEvent)|(1<<gmOffsetEvent))};

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGHUP, signal_handler);

    std::cout << "[CLIENT] Before connect : Session ID : " << state.get_sessionId() << "\n";

    if (connect() == false) {
        std::cout << "[CLIENT] Failure in connecting !!!\n";
        ret = EXIT_FAILURE;
        goto do_exit;
    }
    else {
        std::cout << "[CLIENT] Connected. Session ID : " << state.get_sessionId() << "\n";
    }
    sleep(5);

    sub.get_event().writeEvent(event2Sub1, (std::size_t)sizeof(event2Sub1));
    std::cout << "[CLIENT] set subscribe event : " + sub.c_get_val_event().toString() << "\n";
    subscribe(sub, currentState);
    std::cout << "[CLIENT] " + state.toString();
    while (!signal_flag) {
        /* ToDo: call wait API here */
        sleep(1);
    }

 do_exit:
	disconnect();

    return ret;
}
