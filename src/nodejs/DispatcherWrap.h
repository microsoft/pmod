/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DispatcherWrap.h
****/
#pragma once

#include "InspectableBaseWrap.h"
#include <foundation/library/interfaces/dispatcher.h>


class DispatcherWrap : public InspectableWrap<foundation::library::IDispatcher>
{
public:
    static void Init(v8::Handle<v8::Object> exports);
    static bool IsInstanceOf(v8::Isolate* isolate, v8::Local<v8::Value>& value)
    {
        return _template.Get(isolate)->HasInstance(value);
    }
protected:

private:
    explicit DispatcherWrap();
    ~DispatcherWrap();

    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;
};

