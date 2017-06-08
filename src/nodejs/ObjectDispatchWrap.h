/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchWrap.h
****/
#pragma once

#include "ObjectWrap.h"
#include <foundation/interfaces/object_dispatch.h>

class ObjectDispatchWrap : public ObjectWrap
{
public:
    static void Init(v8::Handle<v8::Object> exports);
    static v8::Local<v8::Value> Create(foundation::IObjectDispatch *p);

    static bool IsInstanceOf(v8::Isolate* isolate, v8::Local<v8::Value>& value)
    {
        return _template.Get(isolate)->HasInstance(value);
    }
protected:
    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);

    foundation::IInspectable **GetInspectablePtr() override
    {
        return reinterpret_cast<foundation::IInspectable **>(_objectDisptachPtr.GetAddressOfPtr());
    }
    explicit ObjectDispatchWrap(foundation::IObjectDispatch *p);
    ~ObjectDispatchWrap();

private:
    foundation::ComPtr<foundation::IObjectDispatch> _objectDisptachPtr;

    static void GetProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void InvokeMethod(const v8::FunctionCallbackInfo<v8::Value>& args);

    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;
};

