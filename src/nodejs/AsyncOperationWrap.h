/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationWrap.h
****/
#pragma once

#include "ObjectWrap.h"
#include <foundation/interfaces/async_operation.h>
#include <foundation/async_operation_connection.h>
#include <uv.h>


class AsyncOperationWrap : public ObjectWrap
{
public:
    static void Init(v8::Handle<v8::Object> exports);
    static v8::Local<v8::Value> Create(foundation::IAsyncOperation *p);

    static bool IsInstanceOf(v8::Isolate* isolate, v8::Local<v8::Value>& value)
    {
        return _template.Get(isolate)->HasInstance(value);
    }
protected:
    foundation::IInspectable **GetInspectablePtr() override
    {
        return reinterpret_cast<foundation::IInspectable **>(_asyncOperationPtr.GetAddressOfPtr());
    }
private:
    foundation::ComPtr<foundation::IAsyncOperation> _asyncOperationPtr;
    foundation::AsyncOperationCompletedConnection _connection;

    std::vector<v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>>> _onCompletedCallbacks;

    explicit AsyncOperationWrap(foundation::IAsyncOperation *p);
    ~AsyncOperationWrap();

    HRESULT OnCompleted(foundation::IUnknown *pSender, foundation::ICompletedEventArgs *pEventArgs);
    void OnCompletedCallback();

    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);
    static void _OnCompletedCallback(uv_async_t* handle);

    static void GetStatus(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetResult(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void OnCompleted(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;
};

