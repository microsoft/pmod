/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandImpl.cpp
****/
#include "CommandImpl.h"

#include "JniCoreUtils.h"
#include "JniConnection.h"

// propertymodel public headers
#include <pmod/interfaces.h>

using namespace pmod;

struct _CanExecuteChangedEventArgsClassName{
    static const char *GetEventArgsClassName()
    {
        return "com/microsoft/pmod/CanExecuteChangedEventArgs";
    }
};

typedef _TListenerProxy
<
    ICommand,
    ICanExecuteChangedEventHandler,
    ICanExecuteChangedEventArgs,
    CommandListenerBase,
    _CanExecuteChangedEventArgsClassName
> CCanExecuteChangedEventProxy;

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_BaseCommandImpl_Execute
  (JNIEnv *env, jobject _this, jobject jParameter)
{
    ICommand *pCommandModel = extractInterfaceExpectedAndCast<ICommand>(env,_this);
    if(!pCommandModel)
    {
        return (jobject)nullptr;
    }
    foundation::IInspectable *pParameter = extractInterfaceIf(env, jParameter);
    foundation::InspectablePtr spResult;
    HRESULT hr = pCommandModel->Execute(pParameter,spResult.GetAddressOf());
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"ICommand::Execute");
        return (jobject)nullptr;
    }
    if(spResult != nullptr)
    {
        returnValue(env,spResult);
    }
    return (jobject)nullptr;
}


JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_BaseCommandImpl_CanExecute
  (JNIEnv *env, jobject _this, jobject jParameter)
{
    ICommand *pCommandModel = extractInterfaceExpectedAndCast<ICommand>(env,_this);
    if(!pCommandModel)
    {
        return (jboolean)false;
    }
    foundation::IInspectable *pParameter = extractInterfaceIf(env, jParameter);
    bool bCanExecute;
    HRESULT hr = pCommandModel->CanExecute(pParameter,&bCanExecute);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"ICommand::CanExecute");
        return (jboolean)false;
    }
    return (jboolean)bCanExecute;
}

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_BaseCommandImpl_GetState
  (JNIEnv *env, jobject _this)
{
    ICommand *pCommandModel = extractInterfaceExpectedAndCast<ICommand>(env,_this);
    if(!pCommandModel)
    {
        return (jint)0;
    }
    UINT32 state;
    HRESULT hr = pCommandModel->GetState(&state);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"ICommand::GetState");
        return (jint)0;
    }
    return (jint)state;
}


JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_BaseCommandImpl_createListenerProxy
  (JNIEnv *env, jobject _this, jobject javaSinkObj)
{
    ICommand *pCommandModel = extractInterfaceExpectedAndCast<ICommand>(env,_this);
    if(!pCommandModel)
    {
        return (jlong)0;
    }
    return (jlong)CCanExecuteChangedEventProxy::CreateInstance<CCanExecuteChangedEventProxy>(env,javaSinkObj,pCommandModel);
}
