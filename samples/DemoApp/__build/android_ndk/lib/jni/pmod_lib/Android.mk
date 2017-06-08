
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := pmod_lib-prebuilt
LOCAL_SRC_FILES := ../../../../../../../bin/Product/android_ndk/pmod_lib/libs/armeabi/libpmod_lib.so
include $(PREBUILT_SHARED_LIBRARY)

