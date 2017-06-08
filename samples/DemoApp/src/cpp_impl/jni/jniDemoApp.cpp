/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:jniDemoApp.cpp
****/
#include <jni.h>

extern "C" {
    int CreateDemoAppImpl(void **pp);

    JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_demoappui_DemoAppFactory_createInstance(JNIEnv *env, jclass)
    {
        void *p;
        CreateDemoAppImpl(&p);
        return (jlong)p;
    }


    JNIEXPORT jint JNICALL
    JNI_OnLoad(JavaVM* vm, void* reserved)
    {
        return JNI_VERSION_1_6;
    }
}
