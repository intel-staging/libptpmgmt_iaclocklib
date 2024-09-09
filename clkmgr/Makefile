# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation.
#
# Makefile for clkmgr
#
# @author Christopher Hall <christopher.s.hall@intel.com>
# @copyright © 2024 Intel Corporation.
# @license BSD-3-Clause
#
###############################################################################

CWD := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

.PHONY: clkmgr
clkmgr:
	$(MAKE) -C $(CWD)/common default
	$(MAKE) -C $(CWD)/proxy default
	$(MAKE) -C $(CWD)/client default

.PHONY: clkmgr_clean
clkmgr_clean:
	$(MAKE) -C $(CWD)/client clean
	$(MAKE) -C $(CWD)/proxy clean
	$(MAKE) -C $(CWD)/common clean
