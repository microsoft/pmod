/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryWrap.h
****/
#pragma once

#include "InspectableBaseWrap.h"
#include <foundation/dictionary_ptr.h>


class DictionaryWrap : public InspectableWrap<foundation::IDictionary>
{
public:
    static void Init(v8::Handle<v8::Object> exports);
    static v8::Local<v8::Value> Create(foundation::IDictionary *p);


    static bool IsInstanceOf(v8::Isolate* isolate, v8::Local<v8::Value>& value)
    {
        return _template.Get(isolate)->HasInstance(value);
    }

private:
    explicit DictionaryWrap();
    explicit DictionaryWrap(foundation::IDictionary *p);
    ~DictionaryWrap();
    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);
    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Lookup(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void HasKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Insert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> _constructor;
    static v8::Persistent<v8::FunctionTemplate> _template;
};

