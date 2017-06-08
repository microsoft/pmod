/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CanExecuteChangedEventArgs.cpp
****/
#include "CanExecuteChangedEventArgs.h"

#include "JniCoreUtils.h"
// propertymodel public headers
#include <pmod/interfaces.h>

using namespace pmod;

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_CanExecuteChangedEventArgs_getOldState
  (JNIEnv *env, jobject _this)
{
    ICanExecuteChangedEventArgs *pCanExecuteChangedEventArgs = extractInterfaceExpectedAndCast<ICanExecuteChangedEventArgs>(env,_this);
    if(!pCanExecuteChangedEventArgs)
    {
        return (jint)0;
    }
    UINT32 oldState;
    HRESULT hr = pCanExecuteChangedEventArgs->GetOldState(&oldState);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"ICanExecuteChangedEventArgs::GetOldState");
        return (jint)0;
    }
    return (jint)oldState;
}

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_CanExecuteChangedEventArgs_getNewState
  (JNIEnv *env, jobject _this)
{
    ICanExecuteChangedEventArgs *pCanExecuteChangedEventArgs = extractInterfaceExpectedAndCast<ICanExecuteChangedEventArgs>(env,_this);
    if(!pCanExecuteChangedEventArgs)
    {
        return (jint)0;
    }
    UINT32 newState;
    HRESULT hr = pCanExecuteChangedEventArgs->GetNewState(&newState);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"ICanExecuteChangedEventArgs::GetNewState");
        return (jint)0;
    }
    return (jint)newState;
}
