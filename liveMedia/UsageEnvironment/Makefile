INCLUDES = -Iinclude -I../groupsock/include
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

ifndef OSS_PATH
OSS_PATH  =./../../../../dtv_linux/mtk_util/library/gnuarm-4.5.1_vfp_ca9/RtspPlayer/wifi_display
endif
##### End of variables to change

USAGE_ENVIRONMENT_LIB = libUsageEnvironment.$(LIB_SUFFIX)
ALL = $(USAGE_ENVIRONMENT_LIB)
all:	$(ALL)

OBJS = UsageEnvironment.$(OBJ) HashTable.$(OBJ) strDup.$(OBJ)

$(USAGE_ENVIRONMENT_LIB): $(OBJS)
	$(LIBRARY_LINK)$@ $(LIBRARY_LINK_OPTS) $(OBJS)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

UsageEnvironment.$(CPP):	include/UsageEnvironment.hh
include/UsageEnvironment.hh:	include/UsageEnvironment_version.hh include/Boolean.hh include/strDup.hh
HashTable.$(CPP):		include/HashTable.hh
include/HashTable.hh:		include/Boolean.hh
strDup.$(CPP):			include/strDup.hh

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

install:
	cp -rf *.so $(OSS_PATH)/
ifeq "$(DBG)" "true"
	@echo "Building debug mode"
else
	$(CROSS_COMPILE)strip --strip-unneeded  $(OSS_PATH)/*.so	
endif

##### Any additional, platform-specific rules come here:
