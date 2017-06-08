/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectTypeInfo.cpp
****/
#include "ObjectTypeInfo.h"

#include "JniCoreUtils.h"

// propertymodel public headers
#include <pmod/interfaces.h>
#include <foundation/hstring_wrapper.h>

using namespace pmod;

JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_ObjectTypeInfo_getName
    (JNIEnv *env, jobject _this)
{
    foundation::IObjectTypeInfo *pObjectTypeInfo = extractInterfaceExpectedAndCast<foundation::IObjectTypeInfo>(env, _this);
    if (!pObjectTypeInfo)
    {
        return (jstring)nullptr;
    }
    foundation::HStringPtr name;
    HRESULT hr = pObjectTypeInfo->GetName(name.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IModelTypeInfo::GetName");
        return (jstring)nullptr;
    }
    return toString(env,name);
}

JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_ObjectTypeInfo_getType
    (JNIEnv *env, jobject _this)
{
    foundation::IObjectTypeInfo *pObjectTypeInfo = extractInterfaceExpectedAndCast<foundation::IObjectTypeInfo>(env, _this);
    if (!pObjectTypeInfo)
    {
        return (jstring)nullptr;
    }
    IID iidType;
    HRESULT hr = pObjectTypeInfo->GetType(&iidType);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObjectTypeInfo::GetType");
        return (jstring)nullptr;
    }
    return toString(env,iidType);
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_ObjectTypeInfo_getBaseType
    (JNIEnv *env, jobject _this)
{
    foundation::IObjectTypeInfo *pObjectTypeInfo = extractInterfaceExpectedAndCast<foundation::IObjectTypeInfo>(env, _this);
    if (!pObjectTypeInfo)
    {
        return (jobject)nullptr;
    }
    foundation::IObjectTypeInfo *pObjectTypeInfoBase;
    HRESULT hr = pObjectTypeInfo->GetBaseType(&pObjectTypeInfoBase);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IObjectTypeInfo::GetBaseType");
        return (jobject)nullptr;
    }
    if (pObjectTypeInfoBase)
    {
        return returnObjectTypeInfo(env, pObjectTypeInfoBase);
    }
    return (jobject)nullptr;
}

