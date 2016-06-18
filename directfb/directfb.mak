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
# libdirect object files
LIB_DIRECT_OBJECTS = \
	$(SOURCE)/DirectFB/lib/direct/clock.o\
	$(SOURCE)/DirectFB/lib/direct/conf.o\
	$(SOURCE)/DirectFB/lib/direct/debug.o\
	$(SOURCE)/DirectFB/lib/direct/direct.o\
	$(SOURCE)/DirectFB/lib/direct/direct_result.o\
	$(SOURCE)/DirectFB/lib/direct/fastlz.o\
	$(SOURCE)/DirectFB/lib/direct/fifo.o\
	$(SOURCE)/DirectFB/lib/direct/flz.o\
	$(SOURCE)/DirectFB/lib/direct/hash.o\
	$(SOURCE)/DirectFB/lib/direct/init.o\
	$(SOURCE)/DirectFB/lib/direct/interface.o\
	$(SOURCE)/DirectFB/lib/direct/list.o\
	$(SOURCE)/DirectFB/lib/direct/log.o\
	$(SOURCE)/DirectFB/lib/direct/log_domain.o\
	$(SOURCE)/DirectFB/lib/direct/map.o\
	$(SOURCE)/DirectFB/lib/direct/mem.o\
	$(SOURCE)/DirectFB/lib/direct/memcpy.o\
	$(SOURCE)/DirectFB/lib/direct/messages.o\
	$(SOURCE)/DirectFB/lib/direct/modules.o\
	$(SOURCE)/DirectFB/lib/direct/print.o\
	$(SOURCE)/DirectFB/lib/direct/processor.o\
	$(SOURCE)/DirectFB/lib/direct/result.o\
	$(SOURCE)/DirectFB/lib/direct/serial.o\
	$(SOURCE)/DirectFB/lib/direct/signals.o\
	$(SOURCE)/DirectFB/lib/direct/stream.o\
	$(SOURCE)/DirectFB/lib/direct/system.o\
	$(SOURCE)/DirectFB/lib/direct/trace.o\
	$(SOURCE)/DirectFB/lib/direct/tree.o\
	$(SOURCE)/DirectFB/lib/direct/thread.o\
	$(SOURCE)/DirectFB/lib/direct/utf8.o\
	$(SOURCE)/DirectFB/lib/direct/util.o\
	$(SOURCE)/DirectFB/lib/direct/uuid.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/clock.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/deprecated.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/filesystem.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/log.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/mem.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/mutex.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/signals.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/system.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/thread.o\
	$(SOURCE)/DirectFB/lib/direct/os/linux/glibc/util.o\


# libfusion object files
LIB_FUSION_OBJECTS = \
	$(SOURCE)/DirectFB/lib/fusion/arena.o\
	$(SOURCE)/DirectFB/lib/fusion/call.o\
	$(SOURCE)/DirectFB/lib/fusion/conf.o\
	$(SOURCE)/DirectFB/lib/fusion/fusion.o\
	$(SOURCE)/DirectFB/lib/fusion/hash.o\
	$(SOURCE)/DirectFB/lib/fusion/init.o\
	$(SOURCE)/DirectFB/lib/fusion/lock.o\
	$(SOURCE)/DirectFB/lib/fusion/object.o\
	$(SOURCE)/DirectFB/lib/fusion/property.o\
	$(SOURCE)/DirectFB/lib/fusion/reactor.o\
	$(SOURCE)/DirectFB/lib/fusion/ref.o\
	$(SOURCE)/DirectFB/lib/fusion/shmalloc.o\
	$(SOURCE)/DirectFB/lib/fusion/vector.o         

LIB_VOODOO_OBJECTS =\
	$(SOURCE)/DirectFB/lib/voodoo/client.o\
	$(SOURCE)/DirectFB/lib/voodoo/conf.o\
	$(SOURCE)/DirectFB/lib/voodoo/init.o\
	$(SOURCE)/DirectFB/lib/voodoo/interface.o\
	$(SOURCE)/DirectFB/lib/voodoo/play.o\
	$(SOURCE)/DirectFB/lib/voodoo/server.o\
	$(SOURCE)/DirectFB/lib/voodoo/connection.o\
	$(SOURCE)/DirectFB/lib/voodoo/connection_link.o\
	$(SOURCE)/DirectFB/lib/voodoo/connection_packet.o\
	$(SOURCE)/DirectFB/lib/voodoo/connection_raw.o\
	$(SOURCE)/DirectFB/lib/voodoo/dispatcher.o\
	$(SOURCE)/DirectFB/lib/voodoo/instance.o\
	$(SOURCE)/DirectFB/lib/voodoo/manager.o\
	$(SOURCE)/DirectFB/lib/voodoo/manager_c.o


LIB_FUSION_OBJECTS_SINGLE = \
	$(SOURCE)/DirectFB/lib/fusion/shm/fake.o

LIB_FUSION_OBJECTS_MULTI = \
	$(SOURCE)/DirectFB/lib/fusion/shm/heap.o\
	$(SOURCE)/DirectFB/lib/fusion/shm/pool.o\
	$(SOURCE)/DirectFB/lib/fusion/shm/shm.o

#
# DirectFB object files
DIRECTFB_OBJECTS = \
	$(SOURCE)/DirectFB/src/core/CoreDFB.o\
	$(SOURCE)/DirectFB/src/core/CoreDFB_real.o\
	$(SOURCE)/DirectFB/src/core/CoreGraphicsState.o\
	$(SOURCE)/DirectFB/src/core/CoreGraphicsState_real.o\
	$(SOURCE)/DirectFB/src/core/CoreGraphicsStateClient.o\
	$(SOURCE)/DirectFB/src/core/CoreLayer.o\
	$(SOURCE)/DirectFB/src/core/CoreLayer_real.o\
	$(SOURCE)/DirectFB/src/core/CoreLayerContext.o\
	$(SOURCE)/DirectFB/src/core/CoreLayerContext_real.o\
	$(SOURCE)/DirectFB/src/core/CoreLayerRegion.o\
	$(SOURCE)/DirectFB/src/core/CoreLayerRegion_real.o\
	$(SOURCE)/DirectFB/src/core/CorePalette.o\
	$(SOURCE)/DirectFB/src/core/CorePalette_real.o\
	$(SOURCE)/DirectFB/src/core/CoreSurface.o\
	$(SOURCE)/DirectFB/src/core/CoreSurface_real.o\
	$(SOURCE)/DirectFB/src/core/CoreWindow.o\
	$(SOURCE)/DirectFB/src/core/CoreWindow_real.o\
	$(SOURCE)/DirectFB/src/core/CoreWindowStack.o\
	$(SOURCE)/DirectFB/src/core/CoreWindowStack_real.o\
	$(SOURCE)/DirectFB/src/core/clipboard.o\
	$(SOURCE)/DirectFB/src/core/colorhash.o\
	$(SOURCE)/DirectFB/src/core/core.o\
	$(SOURCE)/DirectFB/src/core/core_parts.o\
	$(SOURCE)/DirectFB/src/core/fonts.o\
	$(SOURCE)/DirectFB/src/core/gfxcard.o\
	$(SOURCE)/DirectFB/src/core/graphics_state.o\
	$(SOURCE)/DirectFB/src/core/input.o\
	$(SOURCE)/DirectFB/src/core/layer_context.o\
	$(SOURCE)/DirectFB/src/core/layer_control.o\
	$(SOURCE)/DirectFB/src/core/layer_region.o\
	$(SOURCE)/DirectFB/src/core/layers.o\
	$(SOURCE)/DirectFB/src/core/local_surface_pool.o\
	$(SOURCE)/DirectFB/src/core/palette.o\
	$(SOURCE)/DirectFB/src/core/prealloc_surface_pool.o\
	$(SOURCE)/DirectFB/src/core/screen.o\
	$(SOURCE)/DirectFB/src/core/screens.o\
	$(SOURCE)/DirectFB/src/core/shared_surface_pool.o\
	$(SOURCE)/DirectFB/src/core/state.o\
	$(SOURCE)/DirectFB/src/core/surface.o\
	$(SOURCE)/DirectFB/src/core/surface_buffer.o\
	$(SOURCE)/DirectFB/src/core/surface_core.o\
	$(SOURCE)/DirectFB/src/core/surface_pool.o\
	$(SOURCE)/DirectFB/src/core/surface_pool_bridge.o\
	$(SOURCE)/DirectFB/src/core/system.o\
	$(SOURCE)/DirectFB/src/core/windows.o\
	$(SOURCE)/DirectFB/src/core/windowstack.o\
	$(SOURCE)/DirectFB/src/core/wm.o\
	$(SOURCE)/DirectFB/src/directfb.o\
	$(SOURCE)/DirectFB/src/directfb_result.o\
	$(SOURCE)/DirectFB/src/idirectfb.o\
	$(SOURCE)/DirectFB/src/init.o\
	$(SOURCE)/DirectFB/src/dummy.o\
	$(SOURCE)/DirectFB/src/display/idirectfbpalette.o\
	$(SOURCE)/DirectFB/src/display/idirectfbsurface.o\
	$(SOURCE)/DirectFB/src/display/idirectfbsurface_layer.o\
	$(SOURCE)/DirectFB/src/display/idirectfbsurface_window.o\
	$(SOURCE)/DirectFB/src/display/idirectfbdisplaylayer.o\
	$(SOURCE)/DirectFB/src/display/idirectfbscreen.o\
	$(SOURCE)/DirectFB/src/gfx/clip.o\
	$(SOURCE)/DirectFB/src/gfx/convert.o\
	$(SOURCE)/DirectFB/src/gfx/util.o\
	$(SOURCE)/DirectFB/src/gfx/generic/generic.o\
	$(SOURCE)/DirectFB/src/gfx/generic/generic_blit.o\
	$(SOURCE)/DirectFB/src/gfx/generic/generic_draw_line.o\
	$(SOURCE)/DirectFB/src/gfx/generic/generic_fill_rectangle.o\
	$(SOURCE)/DirectFB/src/gfx/generic/generic_stretch_blit.o\
	$(SOURCE)/DirectFB/src/gfx/generic/generic_texture_triangles.o\
	$(SOURCE)/DirectFB/src/gfx/generic/generic_util.o\
	$(SOURCE)/DirectFB/src/input/idirectfbinputdevice.o\
	$(SOURCE)/DirectFB/src/input/idirectfbinputbuffer.o\
	$(SOURCE)/DirectFB/src/media/idirectfbfont.o\
	$(SOURCE)/DirectFB/src/media/idirectfbimageprovider.o\
	$(SOURCE)/DirectFB/src/media/idirectfbvideoprovider.o\
	$(SOURCE)/DirectFB/src/media/idirectfbdatabuffer.o\
	$(SOURCE)/DirectFB/src/media/idirectfbdatabuffer_file.o\
	$(SOURCE)/DirectFB/src/media/idirectfbdatabuffer_memory.o\
	$(SOURCE)/DirectFB/src/media/idirectfbdatabuffer_streamed.o\
	$(SOURCE)/DirectFB/src/misc/conf.o\
	$(SOURCE)/DirectFB/src/misc/util.o\
	$(SOURCE)/DirectFB/src/misc/gfx_util.o\
	$(SOURCE)/DirectFB/src/windows/idirectfbwindow.o


# DirectFB header files
DIRECTFB_INCLUDES += \
	-I$(SOURCE)/DirectFB\
	-I$(SOURCE)/DirectFB/include\
	-I$(SOURCE)/DirectFB/lib\
	-I$(SOURCE)/DirectFB/lib/direct\
	-I$(SOURCE)/DirectFB/src\
	-I$(SOURCE)/DirectFB/proxy/dispatcher\
	-I$(SOURCE)/DirectFB/proxy/requestor\
	-I$(SOURCE)/DirectFB/systems

ifdef OBJ_ROOT
DIRECTFB_INCLUDES += -I$(OBJ_ROOT)/Source/DirectFB/include
endif
