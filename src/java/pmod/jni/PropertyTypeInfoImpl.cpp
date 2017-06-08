/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyTypeInfoImpl.cpp
****/
#include "PropertyTypeInfoImpl.h"

#include "JniCoreUtils.h"


using namespace pmod;
using namespace foundation;

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyTypeInfo_getPropertyTypeInternal
    (JNIEnv *env, jobject _this)
{
    IPropertyTypeInfo *pPropertyTypeInfo = extractInterfaceExpectedAndCast<IPropertyTypeInfo>(env,_this);
    if(!pPropertyTypeInfo)
    {
        return (jint)0;
    }
    PropertyType propertyType;
    HRESULT hr = pPropertyTypeInfo->GetPropertyType(&propertyType);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyTypeInfo::GetPropertyType");
        return (jint)0;
    }
    return (jint)propertyType;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_PropertyTypeInfo_getModelType
    (JNIEnv *env, jobject _this)
{
    IPropertyTypeInfo *pPropertyTypeInfo = extractInterfaceExpectedAndCast<IPropertyTypeInfo>(env,_this);
    if(!pPropertyTypeInfo)
    {
        return (jobject)nullptr;
    }
    foundation::IObjectTypeInfo *pObjectTypeInfo = nullptr;
    HRESULT hr = pPropertyTypeInfo->GetTypeInfo(&pObjectTypeInfo);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyTypeInfo::GetTypeInfo");
    }
    if (pObjectTypeInfo)
    {
        return returnObjectTypeInfo(env, pObjectTypeInfo);
    }
    return (jobject)nullptr;
}
