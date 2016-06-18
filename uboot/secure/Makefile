###############################################################################
# chiling/uboot/secure/Makefile                                               #
#                                                                             #
# SMP core init                                                               #
#                                                                             #
# Copyright (c) 2010-2012 MediaTek Inc.                                       #
# $Author: jacky.ko $                                                         #
#                                                                             #
# This program is free software; you can redistribute it and/or modify        #
# it under the terms of the GNU General Public License version 2 as           #
# published by the Free Software Foundation.                                  #
#                                                                             #
# This program is distributed in the hope that it will be useful, but WITHOUT #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       #
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for   #
# more details.                                                               #
###############################################################################

# System config file
include ../config.mak

LZHS_TOOL = $(VM_LINUX_ROOT)/project_x/tools/mt5391_pack/lzhsenc
PACKIMG   = $(VM_LINUX_ROOT)/project_x/tools/mt5391_pack/packimg
T_MTK     = $(VM_LINUX_ROOT)/project_x/tools/image-tools/t_mtk

ifeq "$(CUST_CLR_IMG)" "true"

CUST_CLR_ROOT      = $(THIS_ROOT)/secure_rls
CUST_CLR_TOOL_PATH = $(CUST_CLR_ROOT)/tools

ifeq "$(BUILD_CFG)" "debug"
CUST_CLR_PATH = $(CUST_CLR_ROOT)/cli/bin
else
ifeq "$(BUILD_CFG)" "cli"
CUST_CLR_PATH = $(CUST_CLR_ROOT)/cli/bin
else
CUST_CLR_PATH = $(CUST_CLR_ROOT)/rel/bin
endif
endif

endif

CFLAGS= -std=c99 -pedantic -Wall -Wpointer-arith -Wstrict-prototypes -O0 -g
CC= gcc

SIGGEN_KEY := $(shell if [ -e $(THIS_ROOT)/data/public_key -a -e $(THIS_ROOT)/data/private_key ]; then echo "$(THIS_ROOT)/data/public_key $(THIS_ROOT)/data/private_key"; else echo "";fi)

clean distclean:
	rm -f  $(UBOOT_SECURE_PACK_ROOT)/uboot.* $(UBOOT_SECURE_PACK_ROOT)/*.map $(UBOOT_SECURE_PACK_ROOT)/*preloader*

5365_package:
	chmod +x $(PACKIMG)
	chmod +x $(LZHS_TOOL)
	chmod +x $(T_MTK)
	rm -f $(UBOOT_SECURE_PACK_ROOT)/u-boot_pack.bin
	rm -f $(UBOOT_SECURE_PACK_ROOT)/u-boot_pack.lzhs
	cp $(UBOOT_OBJ_ROOT)/u-boot.bin $(UBOOT_SECURE_PACK_ROOT)/
	cp $(UBOOT_PACK_ROOT)/mtkloader.bin $(UBOOT_SECURE_PACK_ROOT)/
	cd $(UBOOT_SECURE_PACK_ROOT) ; $(LZHS_TOOL) u-boot.bin u-boot_pack.lzhs
ifeq "$(CUST_CLR_IMG)" "true"
	cp -f $(UBOOT_SECURE_PACK_ROOT)/u-boot_pack.lzhs $(CUST_CLR_PATH)
endif

	cd $(UBOOT_SECURE_PACK_ROOT)/ ; $(T_MTK) u-boot_pack.lzhs 0 1 0 $(SIGGEN_KEY)

	@rm -f $(UBOOT_SECURE_PACK_ROOT)/MTK_PRIVATE_KEY_0
	@rm -f $(UBOOT_SECURE_PACK_ROOT)/MTK_PUBLIC_KEY_0
ifeq ($(TARGET_IC), $(filter $(TARGET_IC), mt5365 mt5395))
	/bin/sed 's/MODEL/$(MODEL)/g' mt5365_secure_nor.sc > $(UBOOT_SECURE_PACK_ROOT)/temp1.sc;
else
	/bin/sed 's/MODEL/$(MODEL)/g' mt5396_secure_nor.sc > $(UBOOT_SECURE_PACK_ROOT)/temp1.sc;
endif
	/bin/sed 's/BOOT/$(BOOT)/g' $(UBOOT_SECURE_PACK_ROOT)/temp1.sc > $(UBOOT_SECURE_PACK_ROOT)/temp2.sc;
	cd $(UBOOT_SECURE_PACK_ROOT)/ ; $(PACKIMG) -f temp2.sc;
ifeq "$(CUST_CLR_IMG)" "true"
	cp -f $(UBOOT_SECURE_PACK_ROOT)/temp2.sc $(CUST_CLR_TOOL_PATH)
	cp -f $(PACKIMG) $(CUST_CLR_TOOL_PATH)
endif
	mv -f $(UBOOT_SECURE_PACK_ROOT)/$(MODEL)_secure_$(BOOT)boot.bin  $(UBOOT_SECURE_PACK_ROOT)/..

-include $(wildcard $(UBOOT_SECURE_PACK_ROOT)/*.d)
