# Global setup
ifndef COMPANY
COMPANY = mtk
endif

MEDIATEK = $(TOP)/MediaTek
#lzhang May/12/2009
#DIRECTFBTOP = $(TOP)/../..

#modifed to the ramdisk
ifndef ROOTFS_NAME
ROOTFS_NAME = 5391_free
endif

#
# for variable "CHILING_ROOT", we include this makefile
#
export CHILING_ROOT ?= $(word 1, $(subst /chiling/,/chiling /, $(shell pwd)))


DESTDIR := $(ROOTFS_TARGET)
###lzhang, Mar/04/2009, the last slash "/" could NOT be deleted!

#ifndef DESTDIR
#DESTDIR = $(TOP)/../../opt/mt53
#endif

prefix  = /home
bindir  = /home
datadir = /home/data
libdir  = /lib
sysconfdir = /etc

#
# Toolchain location
ifndef CROSS_COMPILE
CROSS_COMPILE    = arm11_mtk_le-
endif

CC ?= $(CROSS_COMPILE)gcc
LD ?= $(CROSS_COMPILE)ld
NM ?= $(CROSS_COMPILE)nm
STRIP ?= $(CROSS_COMPILE)strip

#
# Configuration
CPPFLAGS += -DHAVE_FUSIONDALE -DLINUX_2_6 -DPAGE_SHIFT=12 -Werror #-DSAWMAN_NO_LAYER_DST_WINDOW 

ifeq ($(GTV),true)
  CPPFLAGS += -DFONT=\"/system/fonts/DroidSans.ttf\"
else
  CPPFLAGS += -DFONT=\"$(datadir)/decker.ttf\"
endif

CPPFLAGS += -D_CPU_LITTLE_ENDIAN_ -DNDEBUG


#
# Compiler flags
ifeq ($(DEBUG),yes)
CFLAGS += -g -ggdb -O1 -fno-omit-frame-pointer 
else ifeq "$(ENABLE_CA9)" "true"
CFLAGS += -g -ggdb -O2 -ffast-math -fomit-frame-pointer -march=armv7-a -mtune=cortex-a9 -mfloat-abi=softfp -mfpu=vfpv3-d16
else ifeq "$(ENABLE_VFP)" "true"
CFLAGS += -g -ggdb -O2 -ffast-math -fomit-frame-pointer -march=armv6z -mtune=arm1176jz-s -mfloat-abi=softfp -mfpu=vfp
else
CFLAGS += -g -ggdb -O2 -ffast-math -fomit-frame-pointer -march=armv6j -mtune=arm1136jf-s
endif

#
# Compiler flags
CFLAGS += -DCC_OSD_USE_FBM
#CFLAGS += -DCC_GFX_IMPROVE
#CFLAGS += -DCC_GFX_LOG
#CFLAGS += -DCC_GFX_STATISTIC
CFLAGS += -DSTRETCH_WA
CFLAGS += -DCC_GFX_FLIP_MIRROR

# -Werror

#
# System module headers
INCLUDES += -I$(MEDIATEK)/DirectFB/systems

#
# FreeType / PNG location

#
# Different modules, drivers etc.
OBJECTS += \
	$(SOURCE)/DirectFB/interfaces/IDirectFBFont/idirectfbfont_ft2.o \
	$(SOURCE)/DirectFB/interfaces/IDirectFBImageProvider/idirectfbimageprovider_dfiff.o	\
	$(SOURCE)/DirectFB/interfaces/IDirectFBImageProvider/idirectfbimageprovider_gif.o	\
	$(SOURCE)/DirectFB/interfaces/IDirectFBImageProvider/idirectfbimageprovider_jpeg.o	\
	$(SOURCE)/DirectFB/interfaces/IDirectFBImageProvider/idirectfbimageprovider_png.o	\
	$(SOURCE)/DirectFB/wm/default/default.o							\
	$(MEDIATEK)/DirectFB/gfxdrivers/mt53/mt53_2d.o						\
	$(MEDIATEK)/DirectFB/gfxdrivers/mt53/mt53_gfxdriver.o					\
	$(MEDIATEK)/DirectFB/systems/mt53/mt53.o						\
	$(MEDIATEK)/DirectFB/systems/mt53/mt53_primary.o					\
	$(MEDIATEK)/DirectFB/systems/mt53/mt53_surface_pool.o					\
	$(MEDIATEK)/DirectFB/systems/mt53/surfacemanager.o					\
	$(MEDIATEK)/DirectFB/systems/mt53/mt53_vdp.o					\
	$(MEDIATEK)/DirectFB/systems/mt53/surfacemanager_vdp.o						

OBJECTS += \
	$(MEDIATEK)/gfx/gfx_cmdque.o	\
	$(MEDIATEK)/gfx/gfx_debug.o	\
	$(MEDIATEK)/gfx/vgfx_matrix.o	\
	$(MEDIATEK)/gfx/vgfx_drvif.o	\
	$(MEDIATEK)/gfx/gfx_drvif.o	\
	$(MEDIATEK)/gfx/gfx_if.o	


ifeq "$(KEY_FROM_DFB)" "y"
CFLAGS += -DKEY_FROM_DFB
ifeq ($(GTV),true)
  OBJECTS += $(MEDIATEK)/DirectFB/inputdrivers/mtipc/mtipc.o
else # !GTV

OBJECTS += $(SOURCE)/DirectFB/inputdrivers/linux_input/linux_input.o				
OBJECTS += $(MEDIATEK)/DirectFB/inputdrivers/mtrc/mtrc.o
OBJECTS += $(MEDIATEK)/DirectFB/inputdrivers/mtnet/mtnet.o

ifeq "$(DFB_USBMOUSE_SUPPORT)" "y"
OBJECTS += $(MEDIATEK)/DirectFB/inputdrivers/usbmouse/usbmouse.o
endif
ifeq "$(DFB_USBKEYBOARD_SUPPORT)" "y"
OBJECTS += $(MEDIATEK)/DirectFB/inputdrivers/usbkeyboard/usbkeyboard.o
endif

endif # GTV
endif


INCLUDES += -I$(MEDIATEK)/inc -I$(MEDIATEK)/x_inc

INCLUDES += -I$(MEDIATEK)/mt53xx -I$(MEDIATEK)/gfx

include $(MEDIATEK)/Tools/$(COMPANY)/tool.mak

include Config_mt53/$(COMPANY).mak
