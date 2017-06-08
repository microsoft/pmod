/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JObject.h
****/

#pragma once

#include <jni.h>

namespace Java
{
    class JObject
    {
    public:
        JObject();
        JObject(jobject jobj, bool removeLocalRef = false);
        ~JObject();
    public:
        operator jobject () const {return m_jobj;};
        void operator = (const JObject& obj);
    protected:
        void AttachLocalObj(JNIEnv* env, jobject localObj);
        JNIEnv* GetEnv() const;
    protected:
        jobject m_jobj;
    };
}

