/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyChangedEventArgs.cpp
****/
#include "PropertyChangedEventArgs.h"

#include "JniCoreUtils.h"
// propertymodel public headers
#include <pmod/interfaces.h>

using namespace pmod;

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyChangedEventArgs_getProperty
    (JNIEnv *env, jobject _this)
{
    IPropertyChangedEventArgs *pPropertyChangedEventArgs = extractInterfaceExpectedAndCast<IPropertyChangedEventArgs>(env,_this);
    if(!pPropertyChangedEventArgs)
    {
        return (jint)0;
    }
    UINT32 propertyId;
    HRESULT hr = pPropertyChangedEventArgs->GetProperty(&propertyId);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyChangedEventArgs::GetProperty");
        return (jint)0;
    }
    return (jint)propertyId;
}
JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_PropertyChangedEventArgs_getOldValue
  (JNIEnv *env, jobject _this)
{
    IPropertyChangedEventArgs *pPropertyChangedEventArgs = extractInterfaceExpectedAndCast<IPropertyChangedEventArgs>(env,_this);
    if(!pPropertyChangedEventArgs)
    {
        return (jobject)nullptr;
    }

    foundation::InspectablePtr spOldValue;
    HRESULT hr = pPropertyChangedEventArgs->GetOldValue(spOldValue.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyChangedEventArgs::GetOldValue");
        return (jobject)nullptr;
    }
    return returnValue(env,spOldValue);
}


JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_PropertyChangedEventArgs_getNewValue
  (JNIEnv *env, jobject _this)
{
    IPropertyChangedEventArgs *pPropertyChangedEventArgs = extractInterfaceExpectedAndCast<IPropertyChangedEventArgs>(env,_this);
    if(!pPropertyChangedEventArgs)
    {
        return (jobject)nullptr;
    }

    foundation::InspectablePtr  spNewValue;
    HRESULT hr = pPropertyChangedEventArgs->GetNewValue(spNewValue.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyChangedEventArgs::GetNewValue");
        return (jobject)nullptr;
    }
    return returnValue(env,spNewValue);
}
