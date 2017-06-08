/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyInfoImpl.cpp
****/
#include "PropertyInfoImpl.h"

#include "JniCoreUtils.h"

// propertymodel public headers
#include <pmod/interfaces.h>
#include <foundation/hstring_wrapper.h>

using namespace pmod;

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyInfo_getId
    (JNIEnv *env, jobject _this)
{
	foundation::IPropertyInfo *pPropertyInfo = extractInterfaceExpectedAndCast<foundation::IPropertyInfo>(env,_this);
    if(!pPropertyInfo)
    {
        return (jint)nullptr;
    }
    UINT32 propertyId;
    HRESULT hr = pPropertyInfo->GetId(&propertyId);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyInfo::GetId");
        return (jint)nullptr;
    }
    return (jint)propertyId;
}

JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_PropertyInfo_getName
    (JNIEnv *env, jobject _this)
{
	foundation::IPropertyInfo *pPropertyInfo = extractInterfaceExpectedAndCast<foundation::IPropertyInfo>(env,_this);
    if(!pPropertyInfo)
    {
        return (jstring)nullptr;
    }
    foundation::HStringPtr name;
    HRESULT hr = pPropertyInfo->GetName(name.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyInfo::GetName");
        return (jstring)nullptr;
    }
    return toString(env,name);
}


