/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationWrap.cpp
****/
#include "pch.h"
#include "AsyncOperationWrap.h"
#include "NodeJsHelper.h"
#include <foundation/library/async_operation_impl.h>

using namespace v8;
using namespace foundation;

Persistent<Function> AsyncOperationWrap::_constructor;
Persistent<v8::FunctionTemplate> AsyncOperationWrap::_template;

AsyncOperationWrap::AsyncOperationWrap(IAsyncOperation *p)
{
    this->_asyncOperationPtr = p;
    _connection.SetSink(this, &AsyncOperationWrap::OnCompleted);
}

AsyncOperationWrap::~AsyncOperationWrap() {
    this->_asyncOperationPtr.Release();
}

Local<Value> AsyncOperationWrap::Create(foundation::IAsyncOperation *p)
{
    Isolate* isolate = Isolate::GetCurrent();
    AsyncOperationWrap* obj = new AsyncOperationWrap(p);
    auto instance = Local<Function>::New(isolate, _constructor)->NewInstance();
    obj->Wrap(instance);
    return instance;
}

void AsyncOperationWrap::Init(Handle<Object> exports)
{
    InspectableBaseWrap::Init(
        exports,
        _constructor,
        _template,
        "AsyncOperation",
        SetPrototypeMethods);
}

void AsyncOperationWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
    ObjectWrap::SetPrototypeMethods(tpl);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getStatus", GetStatus);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getResult", GetResult);
    NODE_SET_PROTOTYPE_METHOD(tpl, "onCompleted", OnCompleted);
}

void AsyncOperationWrap::GetStatus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    AsyncOperationWrap* pWrapper = ObjectWrap::Unwrap<AsyncOperationWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_asyncOperationPtr))
    {
        return;
    }

    ResultStatus status;
    pWrapper->_asyncOperationPtr->GetStatus(&status);
    args.GetReturnValue().Set(Integer::New(isolate, (int32_t)status));
}

void AsyncOperationWrap::GetResult(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    AsyncOperationWrap* pWrapper = ObjectWrap::Unwrap<AsyncOperationWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_asyncOperationPtr))
    {
        return;
    }

    UINT32 timeout = (UINT32)-1;
    if (args.Length() == 1)
    {
        GetArgumentValue(isolate, args, 0, timeout);
    }

    InspectablePtr valuePtr;
    _NODEJS_ERROR_EXCP_(pWrapper->_asyncOperationPtr->GetResult(timeout, valuePtr.GetAddressOf()), U("IAsyncOperation::GetResult"));
    _NODEJS_RETURN_VALUE_EXCP_(valuePtr);
}

void AsyncOperationWrap::OnCompleted(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    Local<Function> localFunc;
    if (!ThrowIfArgumentFunction(isolate, args, 0, localFunc))
    {
        return;
    }
    AsyncOperationWrap* pWrapper = ObjectWrap::Unwrap<AsyncOperationWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_asyncOperationPtr))
    {
        return;
    }

    pWrapper->_onCompletedCallbacks.push_back(Persistent<Function>(isolate, localFunc));
    pWrapper->_connection.Attach(pWrapper->_asyncOperationPtr);
}

HRESULT AsyncOperationWrap::OnCompleted(foundation::IUnknown *pSender, ICompletedEventArgs *pEventArgs)
{
    if (IsMainThread())
    {
        this->OnCompletedCallback();
    }
    else
    {
        auto asyncHandle = new uv_async_t();
        asyncHandle->data = this;
        uv_async_init(uv_default_loop(), asyncHandle, _OnCompletedCallback);

        uv_async_send(asyncHandle);
    }
    return S_OK;
}

void AsyncOperationWrap::_OnCompletedCallback(uv_async_t* handle)
{
    AsyncOperationWrap* pAsyncOperationWrap = static_cast<AsyncOperationWrap*>(handle->data);
    CloseAndDeleteAsyncHandler(handle);
    pAsyncOperationWrap->OnCompletedCallback();
}

void AsyncOperationWrap::OnCompletedCallback()
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    for(auto iter = _onCompletedCallbacks.begin();iter != _onCompletedCallbacks.end();++iter)
    {
        auto cb = Local<Function>::New(isolate, (*iter));
        cb->Call(isolate->GetCurrentContext()->Global(), 0, nullptr);
    }
}
