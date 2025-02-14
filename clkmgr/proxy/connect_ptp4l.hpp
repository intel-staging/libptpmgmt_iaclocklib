/* SPDX-License-Identifier: BSD-3-Clause
   SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. */

/** @file
 * @brief Proxy connect ptp4l message class.
 *
 * @author Lai Peter Jun Ann <peter.jun.ann.lai@intel.com>
 * @copyright © 2024 Intel Corporation.
 *
 */

#include "common/connect_msg.hpp"
#include "proxy/message.hpp"

__CLKMGR_NAMESPACE_BEGIN

class Connect
{
  private:
  public:
    static int connect(uint8_t transport_specific,
      const std::vector<std::string>& uds_addresses,
      const std::vector<int>& domains);
    static void disconnect();
};

__CLKMGR_NAMESPACE_END
