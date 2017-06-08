
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := pmod-prebuilt
LOCAL_SRC_FILES := ../../../../../../bin/Product/android_ndk/pmod/libs/armeabi/libpmod.so
include $(PREBUILT_SHARED_LIBRARY)

