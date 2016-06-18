# Copyright 2006 The Android Open Source Project
LOCAL_PATH:= $(call my-dir)

################## build iwlib ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := iwlib.c
LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE:= libiw
LOCAL_STATIC_LIBRARIES := libcutils libc libm
LOCAL_MODULE_TAGS := optional
include $(BUILD_STATIC_LIBRARY)

################## build iwpriv ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := iwpriv.c
#LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE := iwpriv
LOCAL_STATIC_LIBRARIES := libiw
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES) # install to system/xbin
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

################## build iwconfig ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := iwconfig.c
LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE := iwconfig
LOCAL_STATIC_LIBRARIES := libcutils libc libm libiw
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES) # install to system/xbin
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

################## build iwplist ###################
include $(CLEAR_VARS)
LOCAL_SRC_FILES := iwlist.c iwlib.h
LOCAL_CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Winline -MMD -fPIC
LOCAL_MODULE := iwlist
LOCAL_STATIC_LIBRARIES := libcutils libc libm libiw
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES) # install to system/xbin
include $(BUILD_EXECUTABLE)
