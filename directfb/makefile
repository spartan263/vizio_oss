#
# Global setup
TOP    = .
SOURCE = $(TOP)/Source
KERNEL = $(TOP)/Kernel

CONFIG ?= mt53

NM ?= nm

export OBJ_ROOT := $(BUILD_TARGET_OBJ_ROOT)
export SHELL=/bin/bash
#
# for variable "CHILING_ROOT", we include this makefile
#

export CHILING_ROOT ?= $(word 1, $(subst /chiling/,/chiling /, $(shell pwd)))

#
# Find the local cfg.mak, if existed, include it
#
CFG_MAK_NAME := cfg.mak

INC_CFG_MAK := $(shell if [ -e $(DFB_CFG_DIR)/$(CFG_MAK_NAME) ];then echo "$(DFB_CFG_DIR)/$(CFG_MAK_NAME)"; else echo ""; fi)

ifneq "$(INC_CFG_MAK)" ""
include $(INC_CFG_MAK)
CFLAGS += $(DEFINES)
endif



#
# Version definition
MAJOR   = 1
MINOR   = 5
TINY    = 3
VERSION = $(MAJOR).$(MINOR).$(TINY)

FD_MAJOR = 8
FD_MINOR = 7
FD_TINY  = 0

SWM_MAJOR   = 1
SWM_MINOR   = 5
SWM_TINY    = 3

# Default target
default: all

#
# Compiler flags
CFLAGS += -fpic -pipe -O2 -DKEY_FROM_DFB

ifeq "$(TOOL_CHAIN)" "4.4.1"
CFLAGS += -fno-dwarf2-cfi-asm
else
ifeq "$(TOOL_CHAIN)" "4.2.1"
CFLAGS += -Wall
endif
endif


ZLIB_VER                  ?= 1.2.3
PNG_VER                   ?= 1.2.43
JPEG_VER                  ?= 6b
FT_VER                    ?= 2.4.3

#
# Linker flags

VFP_SUFFIX =

ifeq "$(TOOL_CHAIN)" "4.5.1"
ifeq "$(ENABLE_VFP)" "true"
VFP_SUFFIX = _vfp
else ifeq "$(ENABLE_CA9)" "true"
VFP_SUFFIX = _vfp_ca9
endif
else
ifeq "$(ENABLE_VFP)" "true"
VFP_SUFFIX = _vfp
endif
endif

LIB_SUB_ROOT  = library/gnuarm-$(TOOL_CHAIN)$(VFP_SUFFIX)
THIRDPARTY_LIB_ROOT = $(CHILING_ROOT)/../oss/$(LIB_SUB_ROOT)


LDFLAGS += -L.
LDFLAGS += -L$(THIRDPARTY_LIB_ROOT)/zlib/$(ZLIB_VER)/pre-install/lib
LDFLAGS += -L$(THIRDPARTY_LIB_ROOT)/freetype/$(FT_VER)/pre-install/lib
LDFLAGS += -L$(THIRDPARTY_LIB_ROOT)/jpeg/$(JPEG_VER)/pre-install/lib
LDFLAGS += -L$(THIRDPARTY_LIB_ROOT)/png/$(PNG_VER)/pre-install/lib

#
# Configuration header files
INCLUDES += -I$(TOP)/Config_$(CONFIG)

#
# Fusion Kernel module headers
INCLUDES += -I$(KERNEL)/linux-fusion/linux/include
INCLUDES += -I$(THIRDPARTY_LIB_ROOT)/zlib/$(ZLIB_VER)/pre-install/include
INCLUDES += -I$(THIRDPARTY_LIB_ROOT)/freetype/$(FT_VER)/pre-install/include 
INCLUDES += -I$(THIRDPARTY_LIB_ROOT)/jpeg/$(JPEG_VER)/pre-install/include
INCLUDES += -I$(THIRDPARTY_LIB_ROOT)/png/$(PNG_VER)/pre-install/include
INCLUDES += -I$(CHILING_ROOT)/app_if/mtal/include
INCLUDES += -I$(CHILING_ROOT)/app_if/mtal/mtal_inc

#
# Preprocessor flags
CPPFLAGS += \
	-DPIC								\
	-DHAVE_CONFIG_H							\
	-DHAVE_STDBOOL_H						\
	-D_GNU_SOURCE							\
	-D_REENTRANT							\
	-DVERSION=\"$(VERSION)\"					\
	-DBUILDTIME="\"$(buildtime)\""\
	-DHF_MW_MODIFY

#
# Debug option

BUILD_CFG ?=

ifeq ($(BUILD_CFG),debug)
  CPPFLAGS += -DENABLE_DEBUG=1
  CFLAGS   += -g3
else
  CPPFLAGS += -DENABLE_DEBUG=0
endif

PARALLEL  ?= 12

#
# Trace option
ifeq ($(TRACE),yes)
  CFLAGS   += -finstrument-functions
  CPPFLAGS += -DENABLE_TRACE=1
else
  CPPFLAGS += -DENABLE_TRACE=0
endif

#
# Include the specific configuration #########################
-include $(TOP)/Config_$(CONFIG)/makefile

CPPFLAGS += \
	-DDATADIR=\"$(datadir)\"	\
	-DMODULEDIR=\"$(moddir)\"	\
	-DSYSCONFDIR=\"$(sysconfdir)\"

buildtime = $(shell /bin/bash -c date -u +%Y-%m-%d %H:%M)

-include directfb.mak
-include sawman.mak

#
# Build list of all objects


OBJECTS +=\
	$(LIB_DIRECT_OBJECTS)\
	$(LIB_FUSION_OBJECTS)\
	$(LIB_FUSION_OBJECTS_MULTI)\
	$(DIRECTFB_OBJECTS)\
	$(PROXY_OBJECTS)\
	$(SAWMAN_OBJECTS)


#
# Build complete include path
INCLUDES += \
	$(DIRECTFB_INCLUDES)						\
	$(SAWMAN_INCLUDES)			



#
# All libraries
LIBS += \
	-lfreetype							\
	-lpng								\
	-ljpeg								\
	-lz								\
	-lm								\
	-lpthread							\
	-ldl


TOOLS += \
#	$(SOURCE)/DirectFB/tools/dfbdump
#	$(SOURCE)/DirectFB/tools/fusion_bench\
#	$(SOURCE)/DirectFB/tests/fusion_reactor\
#	$(SOURCE)/SaWMan/tools/swmdump\
#	$(SOURCE)/DirectFB/tools/dfbinfo\

#
# List of generated header files
GEN_HEADERS = 

OBJECTS     := $(addprefix $(OBJ_ROOT)/,$(OBJECTS))
GEN_HEADERS := $(addprefix $(OBJ_ROOT)/,$(GEN_HEADERS))
TOOLS       := $(addprefix $(OBJ_ROOT)/,$(TOOLS))
TOOLS_OBJS  := $(TOOLS:=.o)


V ?= 0

ifeq "$(V)" "0"
QUIET = @
else
QUIET = 
endif

define rule_cc_o_c
	@/bin/echo -e "  CC\t$@"
	@mkdir -p $(dir $@)
	@/bin/echo -E '$(CC) $(CFLAGS) -c $(CPPFLAGS) $(INCLUDES) -o $@ $<' > $@.cmd
	$(QUIET)$(CC) $(CFLAGS) -c $(CPPFLAGS) $(INCLUDES) -o $@ $<
endef

define rule_cc_o_cpp
	@/bin/echo -e "  CXX\t$@"
	@mkdir -p $(dir $@)
	@/bin/echo -E '$(CXX) $(CFLAGS) -c $(CPPFLAGS) $(INCLUDES) -o $@ $<' > $@.cmd
	$(QUIET)$(CXX) $(CFLAGS) -c $(CPPFLAGS) $(INCLUDES) -o $@ $<
endef

define rule_cc_o_ld
	@/bin/echo -e "  LD\t$@"
	@$(QUIET)$(CC) $(LDFLAGS) -o $@ $< -L$(OBJ_ROOT) -ldirectfb $(LIBS)
endef

#
# Rules
all:  gen_obj
	$(MAKE) libdirectfb.o libdirectfb.so libdirectfb.so.nm libdirectfb.a $(TOOLS) $(TEST) $(TOOLS:%=%.nm) fusion.ko --no-print-directory

gen_obj:$(GEN_HEADERS)
	rm -f $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx/mt53_fb.h
	rm -f $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx/mt53xx_fb.h
	rm -f $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx/gfx_common_if.h
	rm -f $(CHILING_ROOT)/driver/directfb/MediaTek/gfx/gfx_common_if.h
	if [ ! -d $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx ]; then \
		mkdir -p $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx; \
	fi
	ln -s $(CHILING_ROOT)/../project_x/target/mt53xx_com/53xx_com_driver/gfx6896/gfx_common_if.h  $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx/gfx_common_if.h
	ln -s $(CHILING_ROOT)/../project_x/target/mt53xx_com/53xx_com_driver/gfx6896/gfx_common_if.h  $(CHILING_ROOT)/driver/directfb/MediaTek/gfx/gfx_common_if.h
	ln -s $(CHILING_ROOT)/driver/linux/native/mt53xx_fb/mt53_fb.h  $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx/mt53_fb.h
	$(MAKE) $(if $(filter -j,$(MAKEFLAGS)),,-j $(PARALLEL)) parallel_obj --no-print-directory
	
parallel_obj: $(OBJECTS) $(TOOLS_OBJS)

libdirectfb.o:  $(GEN_HEADERS) $(OBJECTS)
	@/bin/echo -e "  LD\t$@"
	@$(LD) $(LDFLAGS) -r -o $(OBJ_ROOT)/$@ $(OBJECTS)

libdirectfb.a:  $(GEN_HEADERS) $(OBJECTS)
	@/bin/echo -e "  AR\t$@"
	@$(AR) -ru $(OBJ_ROOT)/$@ $(OBJECTS)

libdirectfb.so: $(GEN_HEADERS) $(OBJECTS)
	@/bin/echo -e "  LD\t$@"
	@$(CC) -shared -Wl,-soname,libdirectfb.so $(LDFLAGS) $(LIBS) -o $(OBJ_ROOT)/$@ $(OBJECTS)

libdirectfb.so.nm: libdirectfb.so
	@/bin/echo -e "  NM\t$@"
	@$(NM) -n $(OBJ_ROOT)/$< > $(OBJ_ROOT)/$@

$(OBJ_ROOT)/%.o: %.c
	$(rule_cc_o_c)

$(OBJ_ROOT)/%.o: %.cpp
	$(rule_cc_o_cpp)
	
$(OBJ_ROOT)/$(SOURCE)/DirectFB/tests/%: $(OBJ_ROOT)/$(SOURCE)/DirectFB/tests/%.o libdirectfb.so
	$(rule_cc_o_ld)

$(OBJ_ROOT)/$(SOURCE)/DirectFB/tools/%: $(OBJ_ROOT)/$(SOURCE)/DirectFB/tools/%.o libdirectfb.so
	$(rule_cc_o_ld)

$(OBJ_ROOT)/$(SOURCE)/SaWMan/tools/%: $(OBJ_ROOT)/$(SOURCE)/SaWMan/tools/%.o libdirectfb.so
	$(rule_cc_o_ld)

$(OBJ_ROOT)/$(MEDIATEK)/Tools/%: $(OBJ_ROOT)/$(MEDIATEK)/Tools/%.o $(TOP)/libdirectfb.so
	$(rule_cc_o_ld)



%.nm: %
	@/bin/echo -e "  NM\t$@"
	@mkdir -p $(dir $@)
	@$(NM) -n $< > $@

fusion.ko:
#	$(MAKE) -C Kernel $(if $(filter -j,$(MAKEFLAGS)),,-j $(PARALLEL)) --no-print-directory
	$(MAKE) -C Kernel $(if $(filter -j,$(MAKEFLAGS)),,-j $(PARALLEL))

.PHONY: clean install gen_obj

clean::
	$(FCLEAN) $(OBJ_ROOT)
	make -C Kernel clean
	rm -f libdirectfb.*o* $(TOOLS) $(TEST)
	rm -f libdirectfb.a
	rm -f $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx/mt53_fb.h
	rm -f $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx/gfx_common_if.h
	rm -f $(CHILING_ROOT)/driver/directfb/MediaTek/gfx/gfx_common_if.h
	rm -rf $(CHILING_ROOT)/driver/directfb/MediaTek/mt53xx
	find -name "*.o" | xargs rm -rf
	find -name "*.so" | xargs rm -rf
	find -name "*.nm" | xargs rm -rf
	find -name "modules.order" | xargs rm -rf
	find -name "Module.markers" | xargs rm -rf
	find -name ".tmp_versions" | xargs rm -rf



install::
	@(if test -z "$(DESTDIR)"; then echo "**** DESTDIR not set! ****"; exit 1; fi)
	install -d $(DESTDIR)$(bindir)
	install -d $(DESTDIR)$(libdir)
	install -d $(DESTDIR)$(moddir)
	install -d $(DESTDIR)$(moddir)/inputdrivers
	install -d $(DESTDIR)$(moddir)/interfaces
	install -d $(DESTDIR)$(moddir)/gfxdrivers
	install -d $(DESTDIR)$(moddir)/systems
	install -d $(DESTDIR)$(moddir)/wm
	

#	install -c -m 755 $(TEST) $(TOOLS) $(DESTDIR)$(bindir)/

ifneq "$(STATIC_LIB)" "true"
	install -c -m 755 $(OBJ_ROOT)/libdirectfb.so $(DESTDIR)$(libdir)/
else
	rm -f $(OBJ_ROOT)/libdirectfb.so
endif

	@touch $(OBJ_ROOT)/chiling/driver/directfb/Kernel/linux-fusion/linux/drivers/char/fusion/fusion.ko
	install -c -m 755 $(OBJ_ROOT)/chiling/driver/directfb/Kernel/linux-fusion/linux/drivers/char/fusion/fusion.ko $(DESTDIR)$(bindir)/
ifneq "$(STATIC_LIB)" "true"
	$(STRIP) --strip-unneeded $(DESTDIR)$(libdir)/libdirectfb.so
	$(STRIP) --strip-unneeded $(addprefix $(DESTDIR)$(bindir)/, $(notdir $(TEST) $(TOOLS)) fusion.ko)
endif



DFBINC  = $(SOURCE)/DirectFB/include

