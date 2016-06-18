###############################################################################
# Copyright Statement:                                                        #
#                                                                             #
#   This software/firmware and related documentation ("MediaTek Software")    #
# are protected under international and related jurisdictions'copyright laws  #
# as unpublished works. The information contained herein is confidential and  #
# proprietary to MediaTek Inc. Without the prior written permission of        #
# MediaTek Inc., any reproduction, modification, use or disclosure of         #
# MediaTek Software, and information contained herein, in whole or in part,   #
# shall be strictly prohibited.                                               #
# MediaTek Inc. Copyright (C) 2010. All rights reserved.                      #
#                                                                             #
#   BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND      #
# AGREES TO THE FOLLOWING:                                                    #
#                                                                             #
#   1)Any and all intellectual property rights (including without             #
# limitation, patent, copyright, and trade secrets) in and to this            #
# Software/firmware and related documentation ("MediaTek Software") shall     #
# remain the exclusive property of MediaTek Inc. Any and all intellectual     #
# property rights (including without limitation, patent, copyright, and       #
# trade secrets) in and to any modifications and derivatives to MediaTek      #
# Software, whoever made, shall also remain the exclusive property of         #
# MediaTek Inc.  Nothing herein shall be construed as any transfer of any     #
# title to any intellectual property right in MediaTek Software to Receiver.  #
#                                                                             #
#   2)This MediaTek Software Receiver received from MediaTek Inc. and/or its  #
# representatives is provided to Receiver on an "AS IS" basis only.           #
# MediaTek Inc. expressly disclaims all warranties, expressed or implied,     #
# including but not limited to any implied warranties of merchantability,     #
# non-infringement and fitness for a particular purpose and any warranties    #
# arising out of course of performance, course of dealing or usage of trade.  #
# MediaTek Inc. does not provide any warranty whatsoever with respect to the  #
# software of any third party which may be used by, incorporated in, or       #
# supplied with the MediaTek Software, and Receiver agrees to look only to    #
# such third parties for any warranty claim relating thereto.  Receiver       #
# expressly acknowledges that it is Receiver's sole responsibility to obtain  #
# from any third party all proper licenses contained in or delivered with     #
# MediaTek Software.  MediaTek is not responsible for any MediaTek Software   #
# releases made to Receiver's specifications or to conform to a particular    #
# standard or open forum.                                                     #
#                                                                             #
#   3)Receiver further acknowledge that Receiver may, either presently        #
# and/or in the future, instruct MediaTek Inc. to assist it in the            #
# development and the implementation, in accordance with Receiver's designs,  #
# of certain softwares relating to Receiver's product(s) (the "Services").    #
# Except as may be otherwise agreed to in writing, no warranties of any       #
# kind, whether express or implied, are given by MediaTek Inc. with respect   #
# to the Services provided, and the Services are provided on an "AS IS"       #
# basis. Receiver further acknowledges that the Services may contain errors   #
# that testing is important and it is solely responsible for fully testing    #
# the Services and/or derivatives thereof before they are used, sublicensed   #
# or distributed. Should there be any third party action brought against      #
# MediaTek Inc. arising out of or relating to the Services, Receiver agree    #
# to fully indemnify and hold MediaTek Inc. harmless.  If the parties         #
# mutually agree to enter into or continue a business relationship or other   #
# arrangement, the terms and conditions set forth herein shall remain         #
# effective and, unless explicitly stated otherwise, shall prevail in the     #
# event of a conflict in the terms in any agreements entered into between     #
# the parties.                                                                #
#                                                                             #
#   4)Receiver's sole and exclusive remedy and MediaTek Inc.'s entire and     #
# cumulative liability with respect to MediaTek Software released hereunder   #
# will be, at MediaTek Inc.'s sole discretion, to replace or revise the       #
# MediaTek Software at issue.                                                 #
#                                                                             #
#   5)The transaction contemplated hereunder shall be construed in            #
# accordance with the laws of Singapore, excluding its conflict of laws       #
# principles.  Any disputes, controversies or claims arising thereof and      #
# related thereto shall be settled via arbitration in Singapore, under the    #
# then current rules of the International Chamber of Commerce (ICC).  The     #
# arbitration shall be conducted in English. The awards of the arbitration    #
# shall be final and binding upon both parties and shall be entered and       #
# enforceable in any court of competent jurisdiction.                         #
###############################################################################
KERNEL_VERSION    = $(shell uname -r)
KERNEL_MODLIB     = /lib/modules/$(KERNEL_VERSION)
KERNEL_BUILD     = $(KERNEL_MODLIB)/build
KERNEL_SOURCE    = $(KERNEL_MODLIB)/source
KERNEL_MAJORVERSION = $(shell echo $(KERNEL_VERSION) | cut -d . -f 1)
KERNEL_PATCHLEVEL = $(shell echo $(KERNEL_VERSION) | cut -d . -f 2)
#KERNEL_PATCHLEVEL = $(shell grep 'PATCHLEVEL =' $(KERNEL_BUILD)/Makefile | cut -d ' ' -f 3)

# kernel 3.x, hack to treate it like 2.6
ifeq ($(KERNEL_MAJORVERSION),3)
KERNEL_PATCHLEVEL = 6
endif

SUB = linux/drivers/char/fusion

export CONFIG_FUSION_DEVICE=m


ifeq ($(DEBUG),yes)
  CPPFLAGS += -DFUSION_DEBUG_SKIRMISH_DEADLOCK
endif

ifeq "$(TOOL_CHAIN)" "4.4.1"
CPPFLAGS += -fno-dwarf2-cfi-asm
endif

ifeq ($(shell test -e $(KERNEL_BUILD)/include/linux/autoconf.h && echo yes),yes)
  AUTOCONF_H = -include $(KERNEL_BUILD)/include/linux/autoconf.h
endif

ifeq ($(shell test -e $(KERNEL_BUILD)/include/linux/config.h && echo yes),yes)
  CPPFLAGS += -DHAVE_LINUX_CONFIG_H
endif

.PHONY: all install clean

all:
	rm -f $(SUB)/Makefile
	ln -s Makefile-2.$(KERNEL_PATCHLEVEL) $(SUB)/Makefile
	$(MAKE) -C $(KERNEL_BUILD) \
		EXTRA_CFLAGS="$(CPPFLAGS) -D__KERNEL__ -I`pwd`/linux/include -I$(KERNEL_BUILD)/include -I$(KERNEL_SOURCE)/include $(AUTOCONF_H)" \
		SUBDIRS=`pwd`/$(SUB) modules

install: all
	install -d $(DESTDIR)/usr/include/linux
	install -m 644 linux/include/linux/fusion.h $(DESTDIR)/usr/include/linux

	install -d $(DESTDIR)$(KERNEL_MODLIB)/drivers/char/fusion

ifeq ($(KERNEL_PATCHLEVEL),4)
	install -m 644 $(SUB)/fusion.o $(DESTDIR)$(KERNEL_MODLIB)/drivers/char/fusion
	rm -f $(DESTDIR)$(KERNEL_MODLIB)/fusion.o
else
	install -m 644 $(SUB)/fusion.ko $(DESTDIR)$(KERNEL_MODLIB)/drivers/char/fusion
	rm -f $(DESTDIR)$(KERNEL_MODLIB)/fusion.ko
endif
ifneq ($(strip $(DESTDIR)),)
	/sbin/depmod -ae -b $(DESTDIR) $(KERNEL_VERSION)
else
	/sbin/depmod -ae $(KERNEL_VERSION)
endif



clean:
	find $(SUB) -name *.o -o -name *.ko -o -name .*.o.cmd -o \
		-name fusion.mod.* -o -name .fusion.* | xargs rm -f
	rm -f $(SUB)/Makefile
