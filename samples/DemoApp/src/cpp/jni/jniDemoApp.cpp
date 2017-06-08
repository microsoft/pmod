/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:jniDemoApp.cpp
****/
#include <jni.h>
#include <foundation/library/logger_util.h>
using namespace foundation;


extern "C" {

JNIIMPORT int JNICALL DemoAppRegister();

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
    logger_util::set_category_level(foundation::LoggingLevel::Debug);

    DemoAppRegister();
    return JNI_VERSION_1_6;
}

}
