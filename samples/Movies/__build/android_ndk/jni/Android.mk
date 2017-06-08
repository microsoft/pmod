
MY_LOCAL_PATH := $(call my-dir)

LOCAL_PATH := $(MY_LOCAL_PATH)

# extra includes at the end of the Android.mk
include $(LOCAL_PATH)/pmod/Android.mk

LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)

ROOT         := $(LOCAL_PATH)/../../../../..
ROOT_REL     := ../../../../..

MOVIES_APP_CPP     := ../../../src/cpp

CODE_GEN_SRC     := $(ROOT_REL)/bin/samples/__generated/Movies

# C++ codegen needed
CODE_GEN_SRC_FILES := \
    $(CODE_GEN_SRC)/MoviesTypeInfo.g.cpp \
    $(CODE_GEN_SRC)/MoviesInterfaces.g.cpp \
    $(CODE_GEN_SRC)/MoviesFastInternal.g.cpp \

# C++ Model Files needed
MOVIES_SRC_FILES := \
    $(MOVIES_APP_CPP)/Movies.cpp \
    $(MOVIES_APP_CPP)/MoviesApiService.cpp \


LOCAL_MODULE    := movies

LOCAL_SRC_FILES := \
    $(CODE_GEN_SRC_FILES) \
    $(MOVIES_SRC_FILES) \

CPP_REST_REPO := \github\cpprestsdk
CPP_REST_SDK_INC := \
    $(CPP_REST_REPO)\Release\include \
    $(CPP_REST_REPO)\packages\boost.1.55.0.16\lib\native\include \
    $(CPP_REST_REPO)\packages\openssl-android.1.0.2.0\build\native\include \

LOCAL_C_INCLUDES := \
    $(ROOT)/src/public/inc \
    $(ROOT)/bin/samples/__generated/Movies \
    $(CPP_REST_SDK_INC) \


//LOCAL_CFLAGS += -std=gnu++0x -D _HAS_CPP0X=1
//  -D __GCC_ATOMIC_INT_LOCK_FREE=2

LOCAL_CPPFLAGS += -std=c++11 -frtti -funwind-tables -Wno-unused-local-typedef -D _HAS_CPP0X=1

LOCAL_SHARED_LIBRARIES := pmod-prebuilt pmod_lib-prebuilt

include $(BUILD_SHARED_LIBRARY)

