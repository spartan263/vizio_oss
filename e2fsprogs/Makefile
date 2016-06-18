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
###########################################################################
# $RCSfile: Makefile,v $
# $Revision: #1 $
# $Date: 2010/08/16 $
# $Author: jun.wang $
# $SWAuthor: Jun Wang $
#
# Description:
#         Makefile to build a "e2fsprogs".
#
#############################################################################

VM_LINUX := $(word 1, $(subst /vm_linux/,/vm_linux /, $(shell pwd)))
THIRDPARTY := $(word 1, $(subst /third_party/,/third_party /, $(shell pwd)))
THIRDPARTY_SRC := $(THIRDPARTY)/source

include $(THIRDPARTY_SRC)/mak/target.mak

E2FSPROGS_BUILD_ROOT := $(THIRDPARTY_SRC)/e2fsprogs/build
E2FSPROGS_SRC_ROOT := $(THIRDPARTY_SRC)/e2fsprogs/e2fsprogs-1.41.14
E2FSPROGS_RELEASE_ROOT := $(THIRDPARTY_LIB_ROOT)/e2fsprogs

ifeq "$(ENABLE_CA9)" "true"
  ifeq "$(TOOL_CHAIN)" "4.5.1"
    OS_CFLAGS := -march=armv7-a -mtune=cortex-a9 -mfloat-abi=softfp -mfpu=vfpv3-d16 -fPIC
  else
    $(error TOOL_CHAIN=$(TOOL_CHAIN) is not support CA9 support)		
  endif
else ifeq "$(ENABLE_VFP)" "true"
  OS_CFLAGS := -march=armv6z -mtune=arm1176jz-s -mfloat-abi=softfp -mfpu=vfp
else
  OS_CFLAGS := -march=armv6z -mtune=arm1176jz-s
endif

.PHONY: all release clean 

all: $(E2FSPROGS_BUILD_ROOT)/.configured $(E2FSPROGS_BUILD_ROOT)/sbin/mkfs.ext4
	
release:
	@echo "Release e2fsprogs."
	@rm -rf $(E2FSPROGS_RELEASE_ROOT)
	@mkdir -p $(E2FSPROGS_RELEASE_ROOT)/bin
	@mkdir -p $(E2FSPROGS_RELEASE_ROOT)/lib
	@cp -f $(E2FSPROGS_BUILD_ROOT)/sbin/mkfs.ext4 $(E2FSPROGS_RELEASE_ROOT)/bin
	@cp -f $(E2FSPROGS_BUILD_ROOT)/lib/libblkid.so.1.0 $(E2FSPROGS_RELEASE_ROOT)/lib
	@cp -f $(E2FSPROGS_BUILD_ROOT)/lib/libcom_err.so.2.1 $(E2FSPROGS_RELEASE_ROOT)/lib
	@cp -f $(E2FSPROGS_BUILD_ROOT)/lib/libe2p.so.2.3 $(E2FSPROGS_RELEASE_ROOT)/lib
	@cp -f $(E2FSPROGS_BUILD_ROOT)/lib/libext2fs.so.2.4 $(E2FSPROGS_RELEASE_ROOT)/lib
	@cp -f $(E2FSPROGS_BUILD_ROOT)/lib/libuuid.so.1.2 $(E2FSPROGS_RELEASE_ROOT)/lib
	@echo "Done."

clean:
	@echo "Clean e2fsprogs."
	@rm -rf $(E2FSPROGS_BUILD_ROOT)
	@echo "Done."

$(E2FSPROGS_BUILD_ROOT)/.configured:
	@echo "Configure e2fsprogs."
	@mkdir -p $(E2FSPROGS_BUILD_ROOT)
	@(cd $(E2FSPROGS_BUILD_ROOT); \
	CC="$(CROSS_COMPILE)gcc $(OS_CFLAGS)" \
	CXX="$(CROSS_COMPILE)g++" \
	LD="$(CROSS_COMPILE)ld" \
	AR="$(CROSS_COMPILE)ar" \
	AS="$(CROSS_COMPILE)as" \
	RANLIB="$(CROSS_COMPILE)ranlib" \
	$(E2FSPROGS_SRC_ROOT)/configure \
	--build=x86-linux \
	--host=arm-linux \
	--disable-tls \
	--disable-debugfs \
	--disable-imager \
	--disable-resizer \
	--enable-fsck \
	--disable-e2initrd-helper \
	--disable-testio-debug \
	--enable-elf-shlibs \
	--prefix=$(E2FSPROGS_BUILD_ROOT) \
	);
	@touch $(E2FSPROGS_BUILD_ROOT)/.configured
	@echo "Done."

$(E2FSPROGS_BUILD_ROOT)/sbin/mkfs.ext4:
	@echo "Make e2fsprogs."
	@make -C $(E2FSPROGS_BUILD_ROOT)
	@make -C $(E2FSPROGS_BUILD_ROOT) install
	@echo "Done."
