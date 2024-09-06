# SPDX-License-Identifier: LGPL-3.0-or-later
# SPDX-FileCopyrightText: Copyright © 2024 Intel Corporation. All rights reserved.
#
# Makefile for clkmgr
#
# @author Christopher Hall <christopher.s.hall@intel.com>
# @copyright © 2024 Intel Corporation. All rights reserved.
# @license LGPL-3.0-or-later
#
###############################################################################

CWD := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(CWD)/../tools/version
LIB_NAME:=libclkmgr
LIB_NAME_SO:=$(LIB_NAME).so
DEF_INSTALL_DIR := /usr/local

ifndef DESTDIR
INSTALL_DIR:=$(DEF_INSTALL_DIR)
else
INSTALL_DIR:=$(DESTDIR)
endif

.PHONY: clkmgr
clkmgr:
	$(MAKE) -C $(CWD)/common default
	$(MAKE) -C $(CWD)/proxy default
	$(MAKE) -C $(CWD)/client default

.PHONY: clean
clean:
	$(MAKE) -C $(CWD)/client clean
	$(MAKE) -C $(CWD)/proxy clean
	$(MAKE) -C $(CWD)/common clean

.PHONY: install
install:
	echo "$(LIB_NAME) will be installed here:"
	echo "$(INSTALL_DIR) lib, include and bin folders"
	mkdir -p $(INSTALL_DIR)/lib
	cp -a $(CWD)/client/$(LIB_NAME_SO)* $(INSTALL_DIR)/lib
	mkdir -p $(INSTALL_DIR)/include
	cp -aR $(CWD)/pub/* $(INSTALL_DIR)/include
	mkdir -p $(INSTALL_DIR)/bin
	cp -a $(CWD)/proxy/clkmgr_proxy $(INSTALL_DIR)/bin
	chmod +x $(INSTALL_DIR)/bin/clkmgr_proxy
