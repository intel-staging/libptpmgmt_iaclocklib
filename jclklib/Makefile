# SPDX-License-Identifier: LGPL-3.0-or-later
# SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. All rights reserved.
#
# Makefile for jclklib
#
# @author Christopher Hall <christopher.s.hall@intel.com>
# @copyright © 2024 Intel Corporation. All rights reserved.
# @license LGPL-3.0-or-later
#
###############################################################################

CWD := $(shell pwd)

.PHONY: jclklib
jclklib:
	$(MAKE) -C $(CWD)/jclklib/common default
	$(MAKE) -C $(CWD)/jclklib/proxy default
	$(MAKE) -C $(CWD)/jclklib/client default
#	$(MAKE) -C $(CWD)/jclklib/test default

.PHONY: jclklib_test
jclklib_test:
	$(MAKE) -C $(CWD)/jclklib/test default

.PHONY: jclklib_clean
jclklib_clean:
	$(MAKE) -C $(CWD)/jclklib/client clean
	$(MAKE) -C $(CWD)/jclklib/proxy clean
	$(MAKE) -C $(CWD)/jclklib/common clean
#	$(MAKE) -C $(CWD)/jclklib/test clean
