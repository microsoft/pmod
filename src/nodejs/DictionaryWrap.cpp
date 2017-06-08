/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryWrap.cpp
****/
#include "pch.h"
#include "DictionaryWrap.h"
#include "NodeJsHelper.h"
#include <foundation/library/dictionary_class.h>

using namespace v8;
using namespace foundation;
using namespace foundation::library;

Persistent<Function> DictionaryWrap::_constructor;
v8::Persistent<v8::FunctionTemplate> DictionaryWrap::_template;

DictionaryWrap::DictionaryWrap()  {
    CreateDictionaryClass(this->_ptr.GetAddressOf());
}

DictionaryWrap::DictionaryWrap(foundation::IDictionary *p):
    InspectableWrap<foundation::IDictionary>(p)
{
}

DictionaryWrap::~DictionaryWrap() {
}

v8::Local<v8::Value> DictionaryWrap::Create(foundation::IDictionary *p)
{
    Isolate* isolate = Isolate::GetCurrent();
    DictionaryWrap* obj = new DictionaryWrap(p);
    auto instance = Local<Function>::New(isolate, _constructor)->NewInstance();
    obj->Wrap(instance);
    return instance;
}

void DictionaryWrap::Init(Handle<Object> exports) 
{
    InspectableBaseWrap::Init(
        exports,
        _constructor,
        _template,
        "Dictionary",
        SetPrototypeMethods,
        New);
}

void DictionaryWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
    InspectableBaseWrap::SetPrototypeMethods(tpl);
    NODE_SET_PROTOTYPE_METHOD(tpl, "lookup", Lookup);
    NODE_SET_PROTOTYPE_METHOD(tpl, "insert", Insert);
}

void DictionaryWrap::New(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = Isolate::GetCurrent();
  HandleScope scope(isolate);

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new DictionaryWrap()`
    DictionaryWrap* obj = new DictionaryWrap();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 1;
    Local<Value> argv[argc] = { args[0] };
    Local<Function> cons = Local<Function>::New(isolate, _constructor);
    args.GetReturnValue().Set(cons->NewInstance(argc, argv));
  }
}

void DictionaryWrap::Insert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    DictionaryWrap* pWrapper = ObjectWrap::Unwrap<DictionaryWrap>(args.Holder());
    if (!pWrapper->ThrowIfDisposed(isolate))
    {
        return;
    }

    HStringPtr hKey;
    if (!GetArgumentValue(isolate, args, 0, hKey))
    {
        return;
    }
    InspectablePtr valuePtr;
    if (!ThrowIfArgumentValue(isolate, args,1, valuePtr))
    {
        return;
    }
    bool replaced;
    _NODEJS_ERROR_EXCP_(pWrapper->_ptr->Insert(hKey, valuePtr, &replaced), U("IDictionary::Insert"));

    args.GetReturnValue().Set(Boolean::New(isolate, replaced));
}

void DictionaryWrap::Lookup(const FunctionCallbackInfo<Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    DictionaryWrap* pWrapper = ObjectWrap::Unwrap<DictionaryWrap>(args.Holder());
    if (!pWrapper->ThrowIfDisposed(isolate))
    {
        return;
    }

    HStringPtr hKey;
    if (!GetArgumentValue(isolate, args, 0, hKey))
    {
        return;
    }
    InspectablePtr valuePtr;
    _NODEJS_ERROR_EXCP_(pWrapper->_ptr->Lookup(hKey, valuePtr.GetAddressOf()), U("IDictionary::Lookup"));
    _NODEJS_RETURN_VALUE_EXCP_(valuePtr);
}
