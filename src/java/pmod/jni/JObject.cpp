/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JObject.cpp
****/
#include "JObject.h"
#include "JVMEnv.h"
#include "JniUtility.h"

using namespace Java;

JObject::JObject() : m_jobj(nullptr)
{
}

JObject::JObject(jobject jobj, bool removeLocalRef)
{
    m_jobj = nullptr;
    if (jobj != nullptr)
    {
        JNIEnv* env = GetEnv();
        m_jobj = JniUtility::newGlobalRef(env, jobj);
        if (removeLocalRef)
        {
            env->DeleteLocalRef(jobj);
        }
    }
}

JObject::~JObject()
{
    if (m_jobj != nullptr)
    {
        JniUtility::deleteGlobalRef(m_jobj);
        m_jobj = nullptr;
    }
}

void JObject::operator = (const JObject& obj)
{
    if (this == &obj)
    {
        return;
    }

    JNIEnv* env = GetEnv();
    jobject localObj = nullptr;
    if (obj.m_jobj != nullptr)
    {
        localObj = env->NewLocalRef(obj.m_jobj);
    }
    
    AttachLocalObj(env, localObj);
}

void JObject::AttachLocalObj(JNIEnv* env, jobject localObj)
{
    if (m_jobj != nullptr)
    {
        JniUtility::deleteGlobalRef(m_jobj);
        m_jobj = nullptr;
    }
    
    if (localObj != nullptr)
    {
        m_jobj = JniUtility::newGlobalRef(env, localObj);
        env->DeleteLocalRef(localObj);
    }
}

JNIEnv* JObject::GetEnv() const
{
    JNIEnv* env = nullptr;
    JVMEnv::attachCurrentJNIEnv(&env);
    return env;
}
