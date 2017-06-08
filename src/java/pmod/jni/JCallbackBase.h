/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JCallbackBase.h
****/
#pragma once

#include <assert.h>

#include <jni.h>
#include "JObject.h"


class JCallbackBase
{
protected:
    void setJObject(
        JNIEnv *env,
        jobject javaSinkObj,
        const char *callbackMethodName,
        const char *callbackMethodSignature
        )
    {
        m_javaSinkObj = javaSinkObj;
        jclass javaSinkClass = env->GetObjectClass(m_javaSinkObj);
        assert(javaSinkClass);
        m_javaSinkMethodId = env->GetMethodID(javaSinkClass, callbackMethodName, callbackMethodSignature);
        assert(m_javaSinkMethodId);
    }

    template<typename TResult>
    TResult InvokeCallback(
        JNIEnv* env,
        TResult(JNIEnv::*pCallMethod)(jobject jobj,jmethodID methodId,va_list args),
        ...)
    {
        assert(env != nullptr);

        va_list args;
        va_start(args, pCallMethod);

        TResult result = (env->*pCallMethod)(m_javaSinkObj, m_javaSinkMethodId, args);
        va_end(args);
        return result;
    }

    void InvokeCallback(
        JNIEnv* env,
        ...)
    {
        assert(env != nullptr);

        va_list args;
        va_start(args, env);

        env->CallVoidMethodV(m_javaSinkObj, m_javaSinkMethodId, args);
        va_end(args);
    }


private:
    Java::JObject     m_javaSinkObj;
    jmethodID         m_javaSinkMethodId;
};
