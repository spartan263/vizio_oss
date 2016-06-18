INCLUDES = -I../UsageEnvironment/include -I../groupsock/include -I../liveMedia/include -I../BasicUsageEnvironment/include
##### Change the following for your environment: 
CROSS_COMPILE =		/mtkoss/gnuarm/vfp_4.5.1_2.6.27_cortex-a9-rhel4/i686/bin/armv7a-mediatek451_001_vfp-linux-gnueabi-
COMPILE_OPTS =		$(INCLUDES) -I. -fPIC -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
COMPILE_OPTS +=		-DENABLE_MTK_RTSP -DMTK_WFD_RTSP
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

MEDIA_SERVER = live555MediaServer$(EXE)

ALL = $(MEDIA_SERVER)
all: $(ALL)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

MEDIA_SERVER_OBJS = live555MediaServer.$(OBJ) DynamicRTSPServer.$(OBJ)

live555MediaServer,$(CPP):	DynamicRTSPServer.hh version.hh
DynamicRTSPServer.$(CPP):	DynamicRTSPServer.hh

USAGE_ENVIRONMENT_DIR = ../UsageEnvironment
USAGE_ENVIRONMENT_LIB = $(USAGE_ENVIRONMENT_DIR)/libUsageEnvironment.$(LIB_SUFFIX)
BASIC_USAGE_ENVIRONMENT_DIR = ../BasicUsageEnvironment
BASIC_USAGE_ENVIRONMENT_LIB = $(BASIC_USAGE_ENVIRONMENT_DIR)/libBasicUsageEnvironment.$(LIB_SUFFIX)
LIVEMEDIA_DIR = ../liveMedia
LIVEMEDIA_LIB = $(LIVEMEDIA_DIR)/libliveMedia.$(LIB_SUFFIX)
GROUPSOCK_DIR = ../groupsock
GROUPSOCK_LIB = $(GROUPSOCK_DIR)/libgroupsock.$(LIB_SUFFIX)
LOCAL_LIBS =	$(LIVEMEDIA_LIB) $(GROUPSOCK_LIB) \
		$(BASIC_USAGE_ENVIRONMENT_LIB) $(USAGE_ENVIRONMENT_LIB)
LIBS =			$(LOCAL_LIBS) $(LIBS_FOR_CONSOLE_APPLICATION)

live555MediaServer$(EXE):	$(MEDIA_SERVER_OBJS) $(LOCAL_LIBS)
	$(LINK)$@ $(CONSOLE_LINK_OPTS) $(MEDIA_SERVER_OBJS) $(LIBS)

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

install:
	cp -rf *.so $(THIRDPARTY_LIB_ROOT)/rtsp/
ifeq "$(DBG)" "true"
	@echo "Building debug mode"
else
	$(CROSS_COMPILE)strip --strip-unneeded  $(THIRDPARTY_LIB_ROOT)/rtsp/*.so	
endif

##### Any additional, platform-specific rules come here:
