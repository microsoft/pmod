LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

SRC_ROOT         := $(LOCAL_PATH)/../../../../..
SRC_ROOT_REL     := ../../../../..

JANSSON_SRC        := $(SRC_ROOT_REL)/jansson/src

LOCAL_MODULE    := jannson_lib

MAKE_FILES         := $(LOCAL_PATH)/../../../../make_files

# jansson Source Files
include $(MAKE_FILES)/jansson_sources.inc

LOCAL_SRC_FILES := \
    $(JANSSON_SRC_FILES) \

LOCAL_C_INCLUDES := \
    $(SRC_ROOT)/jansson/src \

include $(BUILD_STATIC_LIBRARY)
