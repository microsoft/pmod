/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventModelEventArgs.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_EventModelEventArgs
#define _Included_com_microsoft_pmod_EventModelEventArgs
#ifdef __cplusplus
extern "C" {
#endif

    JNIEXPORT jint JNICALL Java_com_microsoft_pmod_EventModelEventArgs_getEvent
        (JNIEnv *env, jobject _this);

    JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_EventModelEventArgs_getEventArgs
        (JNIEnv *env, jobject _this);

#ifdef __cplusplus
}
#endif
#endif
