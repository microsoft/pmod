/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JniUtility.cpp
****/

#include "JniUtility.h"
#include "JVMEnv.h"
#include "JClass.h"
#include "JString.h"

using namespace Java;

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
jobject JniUtility::newGlobalRef(JNIEnv* env, jobject jobj)
{
    if (env == NULL)
    {
        return NULL;
    }

    return env->NewGlobalRef(jobj);
}

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
jobject JniUtility::newGlobalRef(jobject jobj)
{
    if (jobj != NULL)
    {
        JNIEnv* env;
        if (JVMEnv::attachCurrentJNIEnv(&env) == 0)
        {
            return JniUtility::newGlobalRef(env, jobj);
        }
    }

    return NULL;
}

//+---------------------------------------------------------------------
// Routine Description:
//      Delete JNI global reference.
//
// Arguments:
//      jobj: The java global reference which will be deleted.
//----------------------------------------------------------------------
void JniUtility::deleteGlobalRef(jobject jobj)
{
    if (jobj != NULL)
    {
        JNIEnv* env;
        if (JVMEnv::attachCurrentJNIEnv(&env) == 0)
        {
            env->DeleteGlobalRef(jobj);
        }
    }
}

JString JniUtility::getClassName(JNIEnv* env, jobject jobj)
{
    JClass objClass(jobj);
    JClass clazz("java/lang/Class");
    static jmethodID methodID = env->GetMethodID((jclass)clazz, "getName", "()Ljava/lang/String;");
    return JString((jstring)env->CallObjectMethod(objClass, methodID), true);
}

JString JniUtility::getClassName(jobject jobj)
{
    JNIEnv* env = NULL;
    JVMEnv::attachCurrentJNIEnv(&env);
    return getClassName(env, jobj);
}

void JniUtility::clearJavaException(JNIEnv* env)
{
    env->ExceptionClear();
}

bool JniUtility::retrieveJavaException(JNIEnv* env, bool clearException, JObject& exception)
{
    jobject objException = env->ExceptionOccurred();
    if (objException != NULL)
    {
        env->ExceptionClear();
        // have to clear exception before call JObject
        // Workaround: throw exception back after JObject is called.
        exception = JObject(objException, true);
        if (!clearException && objException != NULL)
        {
            env->Throw((jthrowable)objException);
        }
    }   

    return (objException != NULL);
}
