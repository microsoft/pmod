/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JClass.h
****/

#pragma once

#include <jni.h>
#include "JObject.h"

namespace Java
{
    class JClass : public JObject
    {
    public:
        JClass(){};
        JClass(jclass jclazz, bool removeLocalRef = false);
        JClass(jobject jobj);
        JClass(const char* lpClassName);
    public:
        operator jclass () const {return (jclass)m_jobj;};
        bool IsInstance(jobject jobj) const;
    };
}
