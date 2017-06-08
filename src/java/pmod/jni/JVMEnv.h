/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JVMEnv.h
****/

#pragma once

#if defined(__GNUC__)
// This will prevent compiler erross due to GNU compilers
#define nullptr __null
#endif

#include <jni.h>

namespace Java
{
class JVMEnv
{
public:
    //+---------------------------------------------------------------------
    // Routine Description:
    //      Set JavaVM pointer.
    //
    // Arguments:
    //      jvm: The JavaVM pointer.
    //----------------------------------------------------------------------
    static void setJvm(JavaVM* jvm);

    //+---------------------------------------------------------------------
    // Routine Description:
    //      Get JavaVM pointer.
    //
    // Return value:
    //      The JavaVM pointer.
    //----------------------------------------------------------------------
    static JavaVM* getJvm();

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
    static int attachCurrentJNIEnv(JNIEnv** env);

    //+---------------------------------------------------------------------
    // Routine Description:
    //      detach current thread from Java VM, if the thread is already attached.
    //
    // Return value:
    //	   0 if successfully detached or don't need to detach, -1 for any errors.
    //----------------------------------------------------------------------
    static int detachCurrentJNIEnv();

private:
    // The pointer to JavaVM.
    static JavaVM* s_jvm;
};
}
