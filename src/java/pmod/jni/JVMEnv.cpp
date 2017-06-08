/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JVMEnv.cpp
****/
#include "JVMEnv.h"
#include <assert.h>

#ifdef _WINDOWS
#define CAST_TO_JVM (void **)
#else
#define CAST_TO_JVM 
#endif

using namespace Java;

extern "C" void setCoreJvm(JavaVM* jvm)
{
    JVMEnv::setJvm(jvm);
}

JavaVM* JVMEnv::s_jvm = nullptr;

//+---------------------------------------------------------------------
// Routine Description:
//      Set JavaVM pointer.
//
// Arguments:
//      jvm: The JavaVM pointer.
//----------------------------------------------------------------------
void JVMEnv::setJvm(JavaVM* jvm)
{
    JVMEnv::s_jvm = jvm;
}

//+---------------------------------------------------------------------
// Routine Description:
//      Get JavaVM pointer.
//
// Return value:
//      The JavaVM pointer.
//----------------------------------------------------------------------
JavaVM* JVMEnv::getJvm()
{
    return JVMEnv::s_jvm;
}

//+---------------------------------------------------------------------
// Routine Description:
//      attach current thread to Java VM, and get the JNIEnv pointer.
//
// Arguments:
//      env: The pointer to JavaVM pointer, which will be set.
//
// Return value:
//      0 if successfully, -1 otherwise.
//----------------------------------------------------------------------
int JVMEnv::attachCurrentJNIEnv(JNIEnv** env)
{
    assert(JVMEnv::s_jvm != nullptr);

    // get jni env
    jint ret = JVMEnv::s_jvm->GetEnv((void**)env, JNI_VERSION_1_6);

    if (ret == JNI_OK)
    {
        return 0;
    }
    else if (ret == JNI_EDETACHED)
    {
        return JVMEnv::s_jvm->AttachCurrentThread(CAST_TO_JVM env, nullptr);
    }
    return JNI_ERR;
}

//+---------------------------------------------------------------------
// Routine Description:
//      detach current thread from Java VM, if the thread is already attached.
//
// Return value:
//      0 if successfully detached or don't need to detach, -1 for any errors.
//----------------------------------------------------------------------
int JVMEnv::detachCurrentJNIEnv()
{
    assert(JVMEnv::s_jvm != nullptr);

    int rc = 0;
    JNIEnv *env;
    if (JVMEnv::s_jvm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_EDETACHED)
    {
        // DetachCurrentThread returns JNI_OK (0) for success
        rc = (JVMEnv::s_jvm->DetachCurrentThread() == JNI_OK)? 0 : -1;
    }

    return rc;
}

