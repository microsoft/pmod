/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Dispatcher.h
****/
#include <jni.h>

#ifndef _Included_com_microsoft_pmod_Dispatcher
#define _Included_com_microsoft_pmod_Dispatcher
#ifdef __cplusplus
extern "C" {
#endif


    JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dispatcher_createDispatcherAdapter
        (JNIEnv *env, jobject _this);

    JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dispatcher_invokeDispatchable
        (JNIEnv *env, jclass jclass, jlong dispatchable);

#ifdef __cplusplus
}
#endif
#endif
