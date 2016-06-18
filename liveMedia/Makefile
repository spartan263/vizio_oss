CROSS_COMPILE =		/mtkoss/gnuarm/vfp_4.5.1_2.6.27_cortex-a9-rhel4/i686/bin/armv7a-mediatek451_001_vfp-linux-gnueabi-
COMPILE_OPTS =		$(INCLUDES) -I. -fPIC -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DENABLE_MTK_RTSP        
ifeq "$(DBG)" "true"
COMPILE_OPTS +=		-g -O0
else
COMPILE_OPTS +=		-O2
endif
C =			c
C_COMPILER =		$(CROSS_COMPILE)gcc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
OBJ =			o
LINK =			$(CROSS_COMPILE)g++ -o
LINK_OPTS =		
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		$(CROSS_COMPILE)g++ -shared -fPIC -o 
LIBRARY_LINK_OPTS =	$(LINK_OPTS)
LIB_SUFFIX =			so
#LIBRARY_LINK =		$(CROSS_COMPILE)ar cr 
#LIBRARY_LINK_OPTS =	$(LINK_OPTS)
#LIB_SUFFIX =			a
#LIBRARY_SHARE =		$(CROSS_COMPILE)g++ -shared -fPIC -o  
#LIBRARY_SHARE_OPTS =	
#LIB_SHARE_SUFFIX = so
LIBS_FOR_CONSOLE_APPLICATION =
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change

LIVEMEDIA_DIR = liveMedia
GROUPSOCK_DIR = groupsock
USAGE_ENVIRONMENT_DIR = UsageEnvironment
BASIC_USAGE_ENVIRONMENT_DIR = BasicUsageEnvironment

TESTPROGS_DIR = testProgs

MEDIA_SERVER_DIR = mediaServer

all:
	cd $(LIVEMEDIA_DIR) ; $(MAKE) DBG=$(DBG)
	cd $(GROUPSOCK_DIR) ; $(MAKE) DBG=$(DBG)
	cd $(USAGE_ENVIRONMENT_DIR) ; $(MAKE) DBG=$(DBG)
	cd $(BASIC_USAGE_ENVIRONMENT_DIR) ; $(MAKE) DBG=$(DBG)
	cd $(TESTPROGS_DIR) ; $(MAKE) DBG=$(DBG)
	cd $(MEDIA_SERVER_DIR) ; $(MAKE) DBG=$(DBG)

clean:
	cd $(LIVEMEDIA_DIR) ; $(MAKE) clean
	cd $(GROUPSOCK_DIR) ; $(MAKE) clean
	cd $(USAGE_ENVIRONMENT_DIR) ; $(MAKE) clean
	cd $(BASIC_USAGE_ENVIRONMENT_DIR) ; $(MAKE) clean
	cd $(TESTPROGS_DIR) ; $(MAKE) clean
	cd $(MEDIA_SERVER_DIR) ; $(MAKE) clean

export TOOL_CHAIN = 4.5.1
export ENABLE_CA9 = true
include ../mak/target.mak

ifndef OSS_PATH
OSS_PATH  =./../../../dtv_linux/mtk_util/library/gnuarm-4.5.1_vfp_ca9/RtspPlayer/wifi_display
endif

install:
	cp -rf BasicUsageEnvironment/*.so $(OSS_PATH)/
	cp -rf groupsock/*.so $(OSS_PATH)/
	cp -rf liveMedia/*.so $(OSS_PATH)/
	cp -rf UsageEnvironment/*.so $(OSS_PATH)/
ifeq "$(DBG)" "true"
	@echo "Building debug mode"
else
	$(CROSS_COMPILE)strip --strip-unneeded  $(OSS_PATH)/*.so
endif
