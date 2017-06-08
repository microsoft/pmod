/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JsonUtil.cpp
****/

#include "JniCoreUtils.h"
#include "JVMEnv.h"

#include <foundation/library/json_statics_util.h>

using namespace foundation;

// exported entry points
extern "C" {
    JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_JsonUtil_toJsonInternal
        (JNIEnv *env, jobject thiz, jobject propertyValue);
    JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_JsonUtil_fromJsonInternal
        (JNIEnv *env, jobject thiz, jstring jJson);
}

JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_JsonUtil_toJsonInternal
(JNIEnv *env, jobject thiz, jobject propertyValue)
{
    foundation::IInspectable *pValue = extractInterfaceIf(env, propertyValue);

    HStringPtr json;
    if (ThrowIfComFail(
        env,
        json_util::ToJson(pValue, json.GetAddressOf()),
        "foundation::json_util::ToJson"))
    {
        return (jstring)nullptr;
    }
    return toString(env, json);
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_JsonUtil_fromJsonInternal
(JNIEnv *env, jobject thiz, jstring jJson)
{
    HStringPtr json = toHStringPtr(env, jJson);
    foundation::InspectablePtr valuePtr;
    if (ThrowIfComFail(
        env,
        foundation::json_util::FromJson(json, valuePtr),
        "foundation::json_util::FromJson"))
    {
        return (jobject)nullptr;
    }
    return returnValue(env, valuePtr);
}

