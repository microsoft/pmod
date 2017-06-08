MY_LOCAL_PATH := $(call my-dir)

LOCAL_PATH := $(call my-dir)

# extra includes at the end of the Android.mk
include $(LOCAL_PATH)/pmod/Android.mk

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

SRC_ROOT         := $(LOCAL_PATH)/../../../..
SRC_ROOT_REL     := ../../../..

FOUNDATION_LIBRARY_SRC  := $(SRC_ROOT_REL)/foundation_library
MODEL_LIBRARY_SRC        := $(SRC_ROOT_REL)/model_library
JANSSON_SRC        := $(SRC_ROOT_REL)/jansson/src
CODE_GEN_SRC       := $(SRC_ROOT_REL)/__generated

LOCAL_MODULE    := pmod_lib

MAKE_FILES         := $(LOCAL_PATH)/../../../make_files


# Foundation Library Source Files
include $(MAKE_FILES)/foundation_library_sources.inc

# Model Library Source Files
include $(MAKE_FILES)/model_library_sources.inc

# jansson Source Files
include $(MAKE_FILES)/jansson_sources.inc


LOCAL_SRC_FILES := \
	$(SRC_ROOT_REL)/java/pmod/jni/android_lib/JniLoadLibrary.cpp \
    $(JANSSON_SRC_FILES) \
    $(FOUNDATION_LIBRARY_SRC_FILES) \
    $(FOUNDATION_LIBRARY_GENERATED_SRC_FILES) \
    $(MODEL_LIBRARY_SRC_FILES) \
    $(MODEL_LIBRARY_SRC_GENERATED_FILES) \

LOCAL_C_INCLUDES := \
    $(SRC_ROOT)/public/inc \
    $(SRC_ROOT)/inc \
    $(SRC_ROOT)/foundation \
    $(SRC_ROOT)/foundation_library \
    $(SRC_ROOT)/model \
    $(SRC_ROOT)/model_library \
    $(SRC_ROOT)/jansson/src \

LOCAL_CFLAGS += -std=gnu++0x -D NO_THREAD_CPP0x -D _HAS_CPP0X=1 -D DEBUG
LOCAL_LDLIBS := -llog

LOCAL_SHARED_LIBRARIES := pmod-prebuilt

include $(BUILD_SHARED_LIBRARY)
