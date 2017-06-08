/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JniUtility.h
****/

#pragma once

#include <jni.h>

namespace Java
{

class JString;
class JObject;

class JniUtility
{
public:
    //+---------------------------------------------------------------------
    // Routine Description:
    //      Create JNI global reference.
    //
    // Arguments:
    //      env: The JNIEnv pointer.
    //      jobj: The java object which will be globally referenced.
    //
    // Return value:
    //      The global reference.
    //----------------------------------------------------------------------
    static jobject newGlobalRef(JNIEnv* env, jobject jobj);

    //+---------------------------------------------------------------------
    // Routine Description:
    //      Create JNI global reference.
    //
    // Arguments:
    //      jobj: The java object which will be globally referenced.
    //
    // Return value:
    //      The global reference.
    //----------------------------------------------------------------------
    static jobject newGlobalRef(jobject jobj);

    //+---------------------------------------------------------------------
    // Routine Description:
    //      Delete JNI global reference.
    //
    // Arguments:
    //      jobj: The java global reference which will be deleted.
    //----------------------------------------------------------------------
    static void deleteGlobalRef(jobject jobj);

    //+---------------------------------------------------------------------
    // Routine Description:
    //      Get class name of the java object.
    //
    // Arguments:
    //      jobj: The java object.
    //----------------------------------------------------------------------
    static JString getClassName(JNIEnv* env, jobject jobj);
    static JString getClassName(jobject jobj);

    //+---------------------------------------------------------------------
    // Routine Description:
    //      Clear java exception if there is an exception.
    //
    // Arguments:
    //      
    //----------------------------------------------------------------------
    static void clearJavaException(JNIEnv* env);

    
    //+---------------------------------------------------------------------
    // Routine Description:
    //      Retrieve java exception if there is.
    //
    // Arguments:
    //      clearException: whether the exception will be clearred.
    //      exception: return java exception
    // Return value:
    //      true if there is an exception, otherwise false.
    //----------------------------------------------------------------------
    static bool retrieveJavaException(JNIEnv* env, bool clearException, JObject& exception);
};
}
