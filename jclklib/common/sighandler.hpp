/** @file sighandler.hpp
 * @brief Signal handling utilities
 *
 * @author Christopher Hall <christopher.s.hall@intel.com>
 * @copyright © 2024 Intel Corporation. All rights reserved.
 * @license LGPL-3.0-or-later
 *
 */

#ifndef SIGHANDLER_HPP
#define SIGHANDLER_HPP

#include <thread>

namespace JClkLibCommon
{
	bool BlockStopSignal();
	bool WaitForStopSignal();
	bool EnableSyscallInterruptSignal();
	bool SendSyscallInterruptSignal(std::thread &t);
}
#endif/*SIGHANDLER_HPP*/

