# SPDX-License-Identifier: LGPL-3.0-or-later
# SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. All rights reserved.
#
# Makefile for jclklib client
#
# @author Christopher Hall <christopher.s.hall@intel.com>
# @copyright © 2024 Intel Corporation. All rights reserved.
# @license LGPL-3.0-or-later
#
###############################################################################


JCLKLIB_CLIENT_DIR := $(shell pwd)/../client
JCLKLIB_TOPLEVEL_DIR = $(JCLKLIB_CLIENT_DIR)/..
JCLKLIB_COMMON_DIR = $(JCLKLIB_TOPLEVEL_DIR)/common

default: test

#.PHONY: build-all
#build-all: test
#	echo "Running" $@
#	$(MAKE) -C $(JCLKLIB_CLIENT_DIR) $(REAL_TARGETS)
test: test.o jclklib.so
	g++ -o test test.o -L $(JCLKLIB_CLIENT_DIR) -I $(JCLKLIB_CLIENT_DIR)/.. -l:jclklib.so

#%.o : %.cpp
#	echo "[COMPILE]" $<
#	g++ -c $< -I $(JCLKLIB_TOPLEVEL_DIR) -fPIC -fdiagnostics-color=always
