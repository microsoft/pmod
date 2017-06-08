# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

SRC_ROOT         := $(LOCAL_PATH)/../../../..
SRC_ROOT_REL     := ../../../..

COMMON_SRC         := $(SRC_ROOT_REL)/src
FOUNDATION_PAL_SRC     := $(SRC_ROOT_REL)/foundation_pal
UNIX_SRC        := $(SRC_ROOT_REL)/pal/unix
FOUNDATION_SRC  := $(SRC_ROOT_REL)/foundation
MODEL_SRC        := $(SRC_ROOT_REL)/model
PAL_SRC        := $(SRC_ROOT_REL)/pal
JNI_CORE_SRC        := $(SRC_ROOT_REL)/java/pmod/jni
CODE_GEN_SRC       := $(SRC_ROOT_REL)/__generated
PINVOKE_SRC     := $(SRC_ROOT_REL)/coreclr/cpp

LOCAL_MODULE    := pmod

MAKE_FILES         := $(LOCAL_PATH)/../../../make_files

# JNI Specific Source Files
include $(MAKE_FILES)/jni_sources.inc

# Source Files
include $(MAKE_FILES)/common_sources.inc
  
# Pal Files
include $(MAKE_FILES)/pal_sources.inc

# Foundation Source Files
include $(MAKE_FILES)/foundation_sources.inc

# Model Source Files
include $(MAKE_FILES)/model_sources.inc

# unix Source Files
include $(MAKE_FILES)/unix_sources.inc

# pinvoke files
include $(MAKE_FILES)/pinvoke_sources.inc

LOCAL_SRC_FILES := \
    $(SRC_ROOT_REL)/android/pal_trace_log.cpp \
    $(SRC_ROOT_REL)/android/mbstowcs.cpp \
    $(SRC_ROOT_REL)/android/wcstombs.cpp \
    $(JNI_CORE_SRC_FILES) \
    $(PAL_SRC_FILES) \
    $(COMMON_SRC_FILES) \
	$(UNIX_SRC_FILES) \
	$(FOUNDATION_PAL_SRC_FILES) \
    $(FOUNDATION_SRC_FILES) \
    $(FOUNDATION_GENERATED_SRC_FILES) \
    $(MODEL_SRC_FILES) \
    $(MODEL_SRC_GENERATED_FILES) \
    $(PINVOKE_SRC_FILES) \

LOCAL_C_INCLUDES := \
    $(SRC_ROOT)/public/inc \
    $(SRC_ROOT)/inc \
    $(SRC_ROOT)/foundation \
    $(SRC_ROOT)/model \
    $(SRC_ROOT)/jansson/src \
    $(SRC_ROOT)/java/pmod/jni \

LOCAL_CFLAGS += -std=gnu++0x -D NO_THREAD_CPP0x -D _HAS_CPP0X=1 -D DEBUG
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
