/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventModelEventArgs.cpp
****/
#include "EventModelEventArgs.h"

#include "JniCoreUtils.h"
// propertymodel public headers
#include <pmod/interfaces.h>

using namespace pmod;

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_EventModelEventArgs_getEvent
    (JNIEnv *env, jobject _this)
{
    IEventModelEventArgs *pEventModelEventArgs = extractInterfaceExpectedAndCast<IEventModelEventArgs>(env, _this);
    if (!pEventModelEventArgs)
    {
        return (jint)0;
    }
    UINT32 eventId;
    HRESULT hr = pEventModelEventArgs->GetEvent(&eventId);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IEventModelEventArgs::GetEvent");
        return (jint)0;
    }
    return (jint)eventId;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_EventModelEventArgs_getEventArgs
  (JNIEnv *env, jobject _this)
{
    IEventModelEventArgs *pEventModelEventArgs = extractInterfaceExpectedAndCast<IEventModelEventArgs>(env, _this);
    if (!pEventModelEventArgs)
    {
        return (jobject)nullptr;
    }

    foundation::InspectablePtr spEventArgs;
    HRESULT hr = pEventModelEventArgs->GetEventArgs(spEventArgs.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IEventModelEventArgs::GetEventArgs");
        return (jobject)nullptr;
    }
    return returnValue(env, spEventArgs);
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
