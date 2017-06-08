/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:jniCppRestSdk.cpp
****/
#include <jni.h>
#include <android/log.h>

void cpprest_init(JavaVM*);

extern "C" {

    JNIEXPORT jint JNICALL
    JNI_OnLoad(JavaVM* vm, void* reserved)
    {
        __android_log_write(ANDROID_LOG_DEBUG, "movies", "JNI_OnLoad");
        cpprest_init(vm);
        return JNI_VERSION_1_6;
    }
}
