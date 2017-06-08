
MY_LOCAL_PATH := $(call my-dir)

LOCAL_PATH := $(MY_LOCAL_PATH)

# extra includes at the end of the Android.mk
include $(LOCAL_PATH)/pmod/Android.mk

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

ROOT         := $(LOCAL_PATH)/../../../../../..
ROOT_REL     := ../../../../../..

DEMO_APP_CPP_IMPL     := ../../../../src/cpp_impl

CODE_GEN_SRC     := $(ROOT_REL)/bin/samples/__generated/DemoApp

# C++ codegen needed
CODE_GEN_SRC_FILES := \
    $(CODE_GEN_SRC)/DemoAppTypeInfo.g.cpp \
    $(CODE_GEN_SRC)/DemoAppInterfaces.g.cpp \

# C++ Model Files needed
DEMOPP_SRC_FILES := \
    $(DEMO_APP_CPP_IMPL)/DemoApp.cpp \
    $(DEMO_APP_CPP_IMPL)/jni/JniDemoApp.cpp \


LOCAL_MODULE    := demoapp_impl

LOCAL_SRC_FILES := \
    $(CODE_GEN_SRC_FILES) \
    $(DEMOPP_SRC_FILES) \

LOCAL_C_INCLUDES := \
    $(ROOT)/src/public/inc \
    $(ROOT)/bin/samples/__generated/DemoApp \

LOCAL_CFLAGS += -std=gnu++0x -D _HAS_CPP0X=1
LOCAL_CPPFLAGS += -fexceptions

LOCAL_SHARED_LIBRARIES := pmod-prebuilt pmod_lib-prebuilt

include $(BUILD_SHARED_LIBRARY)

