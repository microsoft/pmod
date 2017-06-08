/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JClass.cpp
****/
#include "JClass.h"
#include "JVMEnv.h"
#include "JniUtility.h"

using namespace Java;

JClass::JClass(jclass jclazz, bool removeLocalRef) : JObject((jobject)jclazz, removeLocalRef)
{
}

JClass::JClass(jobject jobj)
{
    JNIEnv* env = GetEnv();
    m_jobj = nullptr;
    AttachLocalObj(env, env->GetObjectClass(jobj));
}

JClass::JClass(const char* lpClassName)
{
    JNIEnv* env = GetEnv();
    m_jobj = nullptr;
    AttachLocalObj(env, env->FindClass(lpClassName));
}

bool JClass::IsInstance(jobject jobj) const
{
    JNIEnv* env = GetEnv();
    return env->IsInstanceOf(jobj, (jclass)m_jobj) ? true:false;
}
