/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NotifyCollectionChangedEventArgs.cpp
****/
#include "NotifyCollectionChangedEventArgs.h"

#include "JniCoreUtils.h"
// propertymodel public headers
#include <pmod/interfaces.h>
#include <foundation/array_wrapper.h>

using namespace pmod;

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getAction
  (JNIEnv *env, jobject _this)
{
    INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs = extractInterfaceExpectedAndCast<INotifyCollectionChangedEventArgs>(env,_this);
    if(!pNotifyCollectionChangedEventArgs)
    {
        return (jint)0;
    }
    foundation::NotifyCollectionModelChangedAction action;
    HRESULT hr = pNotifyCollectionChangedEventArgs->GetAction(&action);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"INotifyCollectionChangedEventArgs::GetAction");
        return (jint)0;
    }
    return (jint)action;
}

JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getNewItems
  (JNIEnv *env, jobject _this)
{
    INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs = extractInterfaceExpectedAndCast<INotifyCollectionChangedEventArgs>(env,_this);
    if(!pNotifyCollectionChangedEventArgs)
    {
        return (jobjectArray)nullptr;
    }

    foundation::InspectableArrayWrapper values;
    HRESULT hr = pNotifyCollectionChangedEventArgs->GetNewItems(values.GetSizeAddressOf(),values.GetBufferAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"INotifyCollectionChangedEventArgs::GetNewItems");
        return (jobjectArray)nullptr;
    }
    if(values.GetSize() == 0)
    {
        return (jobjectArray)nullptr;
    }
    return returnValues(env,values.GetSize(),(foundation::IInspectable **)values.GetBuffer());
}

JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getOldItems
    (JNIEnv *env, jobject _this)
{
    INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs = extractInterfaceExpectedAndCast<INotifyCollectionChangedEventArgs>(env,_this);
    if(!pNotifyCollectionChangedEventArgs)
    {
        return (jobjectArray)nullptr;
    }

    foundation::InspectableArrayWrapper values;
    HRESULT hr = pNotifyCollectionChangedEventArgs->GetOldItems(values.GetSizeAddressOf(),values.GetBufferAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"INotifyCollectionChangedEventArgs::GetOldItems");
        return (jobjectArray)nullptr;
    }
    if(values.GetSize() == 0)
    {
        return (jobjectArray)nullptr;
    }
    return returnValues(env,values.GetSize(),(foundation::IInspectable **)values.GetBuffer());
}

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getNewStartingIndex
  (JNIEnv *env, jobject _this)
{
    INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs = extractInterfaceExpectedAndCast<INotifyCollectionChangedEventArgs>(env,_this);
    if(!pNotifyCollectionChangedEventArgs)
    {
        return (jint)-1;
    }
    UINT32 newStartingIndex;
    HRESULT hr = pNotifyCollectionChangedEventArgs->GetNewStartingIndex(&newStartingIndex);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"INotifyCollectionChangedEventArgs::GetNewStartingIndex");
        return (jint)-1;
    }
    return (jint)newStartingIndex;
}


JNIEXPORT jint JNICALL Java_com_microsoft_pmod_NotifyCollectionChangedEventArgs_getOldStartingIndex
    (JNIEnv *env, jobject _this)
{
    INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs = extractInterfaceExpectedAndCast<INotifyCollectionChangedEventArgs>(env,_this);
    if(!pNotifyCollectionChangedEventArgs)
    {
        return (jint)-1;
    }
    UINT32 oldStartingIndex;
    HRESULT hr = pNotifyCollectionChangedEventArgs->GetOldStartingIndex(&oldStartingIndex);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"INotifyCollectionChangedEventArgs::GetOldStartingIndex");
        return (jint)-1;
    }
    return (jint)oldStartingIndex;
}
