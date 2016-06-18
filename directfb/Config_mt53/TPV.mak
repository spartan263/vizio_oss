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
#
# Set the Root Path
#
export CHILING_ROOT        ?= $(word 1, $(subst /chiling/,/chiling /, $(shell pwd)))
export THIRDPARTY_ROOT     ?= $(CHILING_ROOT)/../third_party
export THIRDPARTY_LIB_ROOT ?= $(THIRDPARTY_ROOT)/library/gnuarm-$(TOOL_CHAIN)
export THIRDPARTY_SRC_ROOT ?= $(THIRDPARTY_ROOT)/source

#
# OpenSrc Link Version
ZLIB_VER     ?= 1.2.3
FT_VER       ?= 2.4.3
JPEG_VER     ?= 6b
PNG_VER      ?= 1.2.43

TOOL_CHAIN   ?= 4.4.2

ROOTFS_DYNAMIC_GEN ?= ture

DFBRC_FROM_DFB ?= true
SAWMANRC_FROM_DFB ?= true

install::
	@(if test -z "$(DESTDIR)"; then echo "**** DESTDIR not set! ****"; exit 1; fi)
	install -d $(DESTDIR)$(datadir)
	install -d $(DESTDIR)$(datadir)/df_neo
	install -d $(DESTDIR)$(datadir)/shopfloor
	install -d $(DESTDIR)$(datadir)/quip
	install -d $(DESTDIR)$(sysconfdir)
	install -d $(DESTDIR)$(libdir)	
	install -c -m 644 $(MEDIATEK)/Tools/$(COMPANY)/data/*.jpg $(DESTDIR)$(datadir)/
	install -c -m 644 $(MEDIATEK)/Tools/$(COMPANY)/data/*.png $(DESTDIR)$(datadir)/
	install -c -m 644 $(MEDIATEK)/Tools/$(COMPANY)/data/*.ttf $(DESTDIR)$(datadir)/
ifeq "$(DFBRC_FROM_DFB)" "true"
	install -c -m 644 Config_mt53/directfbrc* $(DESTDIR)$(sysconfdir)
endif
ifeq "$(SAWMANRC_FROM_DFB)" "true"
	install -c -m 644 Config_mt53/sawmanrc $(DESTDIR)$(sysconfdir)
endif
	install -c -m 644 $(MEDIATEK)/Config/fusiondalerc* $(DESTDIR)$(sysconfdir)

ifeq "$(ROOTFS_DYNAMIC_GEN)" "false"
ifeq "$(FT_VER)" "2.3.7"
	install -c -m 755 $(THIRDPARTY_LIB_ROOT)/freetype/$(FT_VER)/pre-install/lib/libfreetype.so.6.3.18 $(DESTDIR)$(libdir)/
endif
ifeq "$(FT_VER)" "2.4.3"
	install -c -m 755 $(THIRDPARTY_LIB_ROOT)/freetype/$(FT_VER)/pre-install/lib/libfreetype.so.6.6.1 $(DESTDIR)$(libdir)/
endif

ifeq "$(PNG_VER)" "1.2.30"
	install -c -m 755 $(THIRDPARTY_LIB_ROOT)/png/$(PNG_VER)/pre-install/lib/libpng12.so.0.30.0              $(DESTDIR)$(libdir)/
	install -c -m 755 $(THIRDPARTY_LIB_ROOT)/png/$(PNG_VER)/pre-install/lib/libpng.so.3.30.0                $(DESTDIR)$(libdir)/
endif

ifeq "$(PNG_VER)" "1.2.43"
	install -c -m 755 $(THIRDPARTY_LIB_ROOT)/png/$(PNG_VER)/pre-install/lib/libpng12.so.0.$(PNG_VER)              $(DESTDIR)$(libdir)/
	install -c -m 755 $(THIRDPARTY_LIB_ROOT)/png/$(PNG_VER)/pre-install/lib/libpng.so.3.$(PNG_VER)                $(DESTDIR)$(libdir)/
endif

	install -c -m 755 $(THIRDPARTY_LIB_ROOT)/jpeg/$(JPEG_VER)/pre-install/lib/libjpeg.so.62.0.0             $(DESTDIR)$(libdir)/
	install -c -m 755 $(THIRDPARTY_LIB_ROOT)/zlib/$(ZLIB_VER)/pre-install/lib/libz.so.$(ZLIB_VER)       $(DESTDIR)$(libdir)/
	
ifeq "$(PNG_VER)" "1.2.30"
	ln -sf libpng12.so.0.30.0       $(DESTDIR)$(libdir)/libpng12.so.0
	ln -sf libpng12.so.0.30.0       $(DESTDIR)$(libdir)/libpng12.so
	ln -sf libpng.so.3.30.0         $(DESTDIR)$(libdir)/libpng.so.3
	ln -sf libpng.so.3.30.0         $(DESTDIR)$(libdir)/libpng.so
endif

ifeq "$(PNG_VER)" "1.2.43"
	ln -sf libpng12.so.0.$(PNG_VER)       $(DESTDIR)$(libdir)/libpng12.so.0
	ln -sf libpng12.so.0.$(PNG_VER)       $(DESTDIR)$(libdir)/libpng12.so
	ln -sf libpng.so.3.$(PNG_VER)         $(DESTDIR)$(libdir)/libpng.so.3
	ln -sf libpng.so.3.$(PNG_VER)         $(DESTDIR)$(libdir)/libpng.so
endif
	                                                    
	ln -sf libjpeg.so.62.0.0        $(DESTDIR)$(libdir)/libjpeg.so.62
	ln -sf libjpeg.so.62.0.0        $(DESTDIR)$(libdir)/libjpeg.so
	
	ln -sf libz.so.$(ZLIB_VER)  $(DESTDIR)$(libdir)/libz.so.1
	ln -sf libz.so.$(ZLIB_VER)  $(DESTDIR)$(libdir)/libz.so
	
ifeq "$(FT_VER)" "2.3.7"
	ln -sf libfreetype.so.6.3.18    $(DESTDIR)$(libdir)/libfreetype.so
	ln -sf libfreetype.so.6.3.18    $(DESTDIR)$(libdir)/libfreetype.so.6
endif
ifeq "$(FT_VER)" "2.4.3"
	ln -sf libfreetype.so.6.6.1    $(DESTDIR)$(libdir)/libfreetype.so
	ln -sf libfreetype.so.6.6.1    $(DESTDIR)$(libdir)/libfreetype.so.6
endif

endif	

