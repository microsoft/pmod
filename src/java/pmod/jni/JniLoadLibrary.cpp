/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JniLoadLibrary.cpp
****/
#include <jni.h>
#include "JVMEnv.h"

#if defined(DEBUG)
#include <foundation/library/debug_library.h>
using namespace foundation;
#endif

extern "C" {


JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved)
{
#if defined(DEBUG)
    debug_util::RemoveOptionsFlag(library::_OptionTrackTypeInfoLeaks);
#endif
    Java::JVMEnv::setJvm(vm);

    JNIEnv* env;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    return JNI_VERSION_1_6;
}

}
