#
# Set warning error parse log/tools
#
export WARNING_LOG  = $(THIS_ROOT)/build.log
export ERROR_LOG    = $(THIS_ROOT)/build_fail.log
export CHK_ERR_WARN_SCRIPT = $(PROJECT_ROOT)/../mak/chk_warn_err.sh
export CHK_ERR_WARN_PARM = $(WARNING_LOG) $(ERROR_LOG)

#
# Set path to source obj library output path
#
export SOURCE_ROOT        ?= $(PROJECT_ROOT)/source
export SOURCE_LIB_ROOT    ?= $(PROJECT_ROOT)/lib
export TEST_FRAMEWORK_ROOT ?= $(PROJECT_ROOT)/Test

export OBJ_ROOT           ?= $(THIS_ROOT)/obj
export OUTPUT_ROOT        ?= $(THIS_ROOT)/output



export SOURCE_ROOT
export SOURCE_LIB_ROOT
export TEST_FRAMEWORK_ROOT

export PREFIX = $(OUTPUT_ROOT)



#my own code
export MY_SRC_LIB_ROOT    ?= $(THIS_ROOT)/lib

export MY_OBJ_ROOT        ?= $(THIS_ROOT)/obj
export MY_OUTPUT_ROOT        ?= $(THIS_ROOT)/output

export MY_SRC_LIB_ROOT



PWD                 := $(shell pwd)
export FLPRJ_ROOT   := $(PWD)
VM_LINUX_ROOT       ?= $(word 1, $(subst /vm_linux/,/vm_linux /, $(FLPRJ_ROOT)))
THIRDPARTY_ROOT     ?= $(word 1, $(subst /third_party/,/third_party /, $(FLPRJ_ROOT)))
THIRDPARTY_SRC_ROOT ?= $(THIRDPARTY_ROOT)/source
export MAK_ROOT     := $(THIRDPARTY_SRC_ROOT)/mak

#move to mak/target.mak, try get by uname for build platform information
CROSS_HOST         ?= i686-pc-linux-gnu
CROSS_TARGET       ?= armv6z-mediatek-linux-gnueabi

UT_PATH		   ?= $(FLPRJ_ROOT)/ut
include $(MAK_ROOT)/target.mak

THIRDPARTY_LIB_ROOT ?= $(THIRDPARTY_ROOT)/library/gnuarm-$(TOOL_CHAIN)$(VFP_SUFFIX)

ifneq ($(BUILD_CFG),debug)
else
endif
ALSA_TAR_BALL ?=alsa-lib-1.0.24.1.tar
ALSA_TAR_BALL_DIR = alsa-lib-1.0.24.1
ALSA_UTIL_DIR = alsa-utils-1.0.24.2


BUILD_TMPDIR = $(BUILD_DIR)/$(notdir $(CROSS_COMPILE)build)

LIB_LIST =      $(wildcard $(SOURCE_LIB_ROOT)/*.content)


 
.PHONY : all src src_ut  $(LIB_LIST)

#all : init src 
all : init src src_ut packing install

util:
	cd $(FLPRJ_ROOT)/$(ALSA_UTIL_DIR) && \
	CC="$(CC)" CFLAGS="$(CC_FLAG)" LD=$(CROSS_COMPILE)ld AS="$(AS)" AR="$(AR)" STRIP="$(STRIP)" NM="$(NM)" RANLIB="$(RANLIB)" ./configure --prefix=$(PREFIX) --exec-prefix=$(PREFIX) --host=i686-pc-linux-gnu --target=armv6z-mediatek-linux-gnueabi --with-alsa-prefix=$(PREFIX)/lib --with-alsa-inc-prefix=$(PREFIX)/include --with-udev-rules-dir=$(PREFIX) --with-asound-state-dir=$(PREFIX) --disable-alsamixer
	$(MAKE) -C $(FLPRJ_ROOT)/$(ALSA_UTIL_DIR) install; 
	
#CC="$(CC)" CFLAGS="$(CC_FLAG)" CXX="$(CXX)" CXXFLAGS="$(CC_FLAG)"  LD="$(LD)" CPP="$(CPP)" AS="$(AS)" AR="$(AR)" STRIP="$(STRIP)" NM="$(NM)" RANLIB="$(RANLIB)"
init:
	$(call if_dir_not_exist_fct,$(call path_cnvt,$(OUTPUT_ROOT)), $(MKDIR) $(MKDIR_FLAG) $(call path_cnvt,$(OUTPUT_ROOT));)
#	tar -xvf alsa-lib-1.0.24.1.tar
	cd $(FLPRJ_ROOT)/$(ALSA_TAR_BALL_DIR) && \
	CC="$(CC)" CFLAGS="$(CC_FLAG)" LD=$(CROSS_COMPILE)ld AS="$(AS)" AR="$(AR)" STRIP="$(STRIP)" NM="$(NM)" RANLIB="$(RANLIB)" ./configure --enable-shared=yes --enable-static=no --prefix=$(PREFIX) --host=$(CROSS_HOST) --target=$(CROSS_TARGET) --with-configdir2=/basic/lib/alsa-lib-1.0.24.1/share/alsa --with-plugindir2=/basic/lib/alsa-lib-1.0.24.1/lib/alsa-lib
#	cd $(FLPRJ_ROOT)/$(ALSA_TAR_BALL_DIR) && \
	CC="$(CC)" CFLAGS="$(CC_FLAG)" LD=$(CROSS_COMPILE)ld AS="$(AS)" AR="$(AR)" STRIP="$(STRIP)" NM="$(NM)" RANLIB="$(RANLIB)" ./configure --enable-shared=yes --enable-static=no --prefix=$(PREFIX) --host=$(CROSS_HOST) --target=$(CROSS_TARGET) --with-configdir=/basic/lib/alsa-lib-1.0.24.1/share/alsa --with-plugindir=/basic/lib/alsa-lib-1.0.24.1/lib/alsa-lib
#	CC="$(CC)" CFLAGS="$(CC_FLAG)" CXX="$(CXX)" CXXFLAGS="$(CC_FLAG)" LD="$(CC)" AS="$(AS)" AR="$(AR)" STRIP="$(STRIP)" NM="$(NM)" RANLIB="$(RANLIB)" \
	./configure --enable-shared=yes --enable-static=no --prefix=$(PREFIX) --host=$(CROSS_HOST) --target=$(CROSS_TARGET)

src:
	$(MAKE) -C $(FLPRJ_ROOT)/$(ALSA_TAR_BALL_DIR) all;
	$(MAKE) -C $(FLPRJ_ROOT)/$(ALSA_TAR_BALL_DIR) install; 

install:
	@echo '@@@@@@@@Release $(ALSA_TAR_BALL_DIR) starting.@@@@@@@@'
	@mkdir -p /$(VM_LINUX_ROOT)/third_party/library/gnuarm-$(TOOL_CHAIN)$(VFP_SUFFIX)/alsa/
	@mkdir -p /$(VM_LINUX_ROOT)/third_party/library/gnuarm-$(TOOL_CHAIN)$(VFP_SUFFIX)/alsa/$(ALSA_TAR_BALL_DIR)
	@echo 'cp -rf $(FLPRJ_ROOT)/output/*  /$(VM_LINUX_ROOT)/third_party/library/gnuarm-$(TOOL_CHAIN)$(VFP_SUFFIX)/alsa/$(ALSA_TAR_BALL_DIR)'
		@cp -rf $(FLPRJ_ROOT)/output/*  /$(VM_LINUX_ROOT)/third_party/library/gnuarm-$(TOOL_CHAIN)$(VFP_SUFFIX)/alsa/$(ALSA_TAR_BALL_DIR)
	@cp -rf $(FLPRJ_ROOT)/makefile.softlink  /$(VM_LINUX_ROOT)/third_party/library/gnuarm-$(TOOL_CHAIN)$(VFP_SUFFIX)/alsa/$(ALSA_TAR_BALL_DIR)/lib/Makefile
	@echo '@@@@@@@@Release $(ALSA_TAR_BALL_DIR) finished.@@@@@@@@'

src_ut :
	$(MAKE) -C $(FLPRJ_ROOT)/ut all; \
	$(call set_build_chk_fct,$(CHK_ERR_WARN_SCRIPT),$(CHK_ERR_WARN_PARM))
	@$(call if_file_exist_fct, $(ERROR_LOG), echo -e '\n\n----- Build Fail -----';cat $(ERROR_LOG);exit 1;)
	$(MAKE) linking

packing:
	$(LD) -Wall -o $(MY_OUTPUT_ROOT)/ut_main -L$(MY_SRC_LIB_ROOT) -lmy_ut -L$(OUTPUT_ROOT)/lib -lasound


linking :$(LIB_LIST)
	@echo "Archieve Library Done"

$(LIB_LIST) :
	@echo $(AR) -scru $(MY_SRC_LIB_ROOT)/$(notdir $(basename $(basename $@))).a `cat $@`
	$(AR) -scru $(MY_SRC_LIB_ROOT)/$(notdir $(basename $(basename $@))).a `cat $@`
#define check_build_dir
#if [ ! -d $(BUILD_TMPDIR) ]; then mkdir -p $(BUILD_TMPDIR); fi
#endef

#all_phony := zlib png jpeg freetype busybox

#.PHONY: $(all_phony) $(addprefix clean_,$(all_phony))

#all: $(all_phony)


clean : clean_src

clean_src :
	$(RM) -r $(RM_FLAG) $(call path_cnvt,$(OUTPUT_ROOT))
	$(RM) -r $(RM_FLAG) $(call path_cnvt,$(SOURCE_LIB_ROOT))
	$(RM) -r $(RM_FLAG) $(call path_cnvt,$(OBJ_DIR)) build.log

help:
	@echo 'linuxq make BUILD_CFG=rel TOOL_CHAIN=4.5.1 ENABLE_CA9=true init src src_ut packing install 2>&1 |tee log'
	@echo '##################  debug variable #########################'
	@echo CC="$(CC)" 
	@echo CFLAGS="$(CC_FLAG)"
	@echo  CXX="$(CXX)"
	@echo CXXFLAGS="$(CC_FLAG)" 
	@echo CPP="$(CPP)"
	@echo MAKE="$(MAKE)"
	@echo LIB_LIST="$(LIB_LIST)"
