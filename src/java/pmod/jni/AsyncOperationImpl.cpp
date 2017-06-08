/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationImpl.cpp
****/
#include "AsyncOperationImpl.h"

#include "JniCoreUtils.h"
#include "JniConnection.h"

// propertymodel public headers
#include <foundation/interfaces.h>
#include <foundation/errors.h>
#include <foundation/async_operation_connection.h>
#include <foundation/type_info_statics_util.h>
#include <foundation/hstring_wrapper.h>

using namespace foundation;

struct _CompletedEventArgsClassName{
    static const char *GetEventArgsClassName()
    {
        return "com/microsoft/pmod/CompletedEventArgs";
    }
};

typedef _TListenerProxy
<
    IAsyncOperation,
    ICompletedEventHandler,
    ICompletedEventArgs,
    AsyncOperationCompletedListenerBase,
    _CompletedEventArgsClassName
> CCompletedEventProxy;

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_GetResult
(JNIEnv *env, jobject _this, jint timeout)
{
    IAsyncOperation *pAsyncOperation = extractInterfaceExpectedAndCast<IAsyncOperation>(env,_this);
    if (pAsyncOperation == nullptr)
    {
        return (jobject)nullptr;
    }

    foundation::InspectablePtr spResult;
    HRESULT hr = pAsyncOperation->GetResult((UINT32)timeout, spResult.GetAddressOf());
    if (FAILED(hr))
    {
        ThrowComFail(env,hr,"IAsyncOperation::GetResult");
        return (jobject)nullptr;
    }

    if (spResult != nullptr)
    {
        return returnValue(env,spResult);
    }

    return (jobject)nullptr;
}

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_GetStatus
  (JNIEnv *env, jobject _this)
{
    IAsyncOperation *pAsyncOperation = extractInterfaceExpectedAndCast<IAsyncOperation>(env, _this);
    if (pAsyncOperation == nullptr)
    {
        return (jint)ResultStatus::Error;
    }

    ResultStatus status;
    HRESULT hr = pAsyncOperation->GetStatus(&status);
    if (FAILED(hr))
    {
        ThrowComFail(env,hr,"IAsyncOperation::GetStatus");
        return (jint)ResultStatus::Error;
    }

    return (jint)status;
}


JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_createListenerProxy
  (JNIEnv *env, jobject _this, jobject javaSinkObj)
{
    IAsyncOperation *pAsyncOperation = extractInterfaceExpectedAndCast<IAsyncOperation>(env, _this);
    if(!pAsyncOperation)
    {
        return (jlong)0;
    }

    return (jlong)CCompletedEventProxy::CreateInstance<CCompletedEventProxy>(env,javaSinkObj,pAsyncOperation);
}


JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_Cancel
  (JNIEnv *env, jobject _this)
{
    IAsyncOperation *pAsyncOperation = extractInterfaceExpectedAndCast<IAsyncOperation>(env, _this);
    if (pAsyncOperation == nullptr)
    {
        return (jboolean)false;
    }

    ResultStatus status;
    HRESULT hr = pAsyncOperation->GetStatus(&status);
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IAsyncOperation::GetStatus");
        return (jboolean)false;
    }

    if (status != ResultStatus::Started)
    {
        return (jboolean)false;
    }

    hr = pAsyncOperation->Cancel();
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IAsyncOperation::Cancel");
        return (jboolean)false;
    }

    return (jboolean)true;
}


JNIEXPORT jint JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_Wait
  (JNIEnv *env, jobject _this, jint timeout)
{
    IAsyncOperation *pAsyncOperation = extractInterfaceExpectedAndCast<IAsyncOperation>(env, _this);
    if (pAsyncOperation == nullptr)
    {
        return (jint)ResultStatus::Error;
    }

    ResultStatus status;
    HRESULT hr = pAsyncOperation->GetStatus(&status);
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IAsyncOperation::GetStatus");
        return (jint)ResultStatus::Error;
    }

    if (status != ResultStatus::Started)
    {
        return (jint)status;
    }

    hr = pAsyncOperation->Wait((UINT32)timeout, &status);
    if (hr == foundation::E_ASYNC_OPERATION_TIMEOUT)
    {
        return (jint)-1;
    }

    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IAsyncOperation::Wait");
        return (jint)ResultStatus::Error;
    }

    return (jint)status;
}

JNIEXPORT jstring JNICALL Java_com_microsoft_pmod_BaseAsyncOperationImpl_getError
  (JNIEnv *env, jobject _this)
{
    IAsyncOperation *pAsyncOperation = extractInterfaceExpectedAndCast<IAsyncOperation>(env, _this);
    if (pAsyncOperation == nullptr)
    {
        return (jstring)nullptr;
    }

    HRESULT hrError(S_OK);
    HRESULT hr = pAsyncOperation->GetError(&hrError);
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IAsyncOperation::GetError");
        return (jstring)nullptr;
    }
    
    foundation::HStringPtr error;
    hr = GetEnumValueName(ErrorsEnum::TypeId, hrError, error.GetAddressOf());
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "GetEnumValueName");
        return (jstring)nullptr;
    }
    
    return toString(env, error);
}  
