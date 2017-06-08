/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchWrap.cpp
****/
#include "pch.h"
#include "ObjectDispatchWrap.h"
#include "NodeJsHelper.h"

using namespace v8;
using namespace foundation;
using namespace foundation::library;

Persistent<Function> ObjectDispatchWrap::_constructor;
Persistent<v8::FunctionTemplate> ObjectDispatchWrap::_template;

ObjectDispatchWrap::ObjectDispatchWrap(IObjectDispatch *p)
{
    this->_objectDisptachPtr = p;
}

ObjectDispatchWrap::~ObjectDispatchWrap() {
    this->_objectDisptachPtr.Release();
}

v8::Local<v8::Value> ObjectDispatchWrap::Create(IObjectDispatch *p)
{
    Isolate* isolate = Isolate::GetCurrent();
    ObjectDispatchWrap* obj = new ObjectDispatchWrap(p);
    auto instance = Local<Function>::New(isolate, _constructor)->NewInstance();
    obj->Wrap(instance);
    return instance;
}

void ObjectDispatchWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
    ObjectWrap::SetPrototypeMethods(tpl);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getProperty", GetProperty);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setProperty", SetProperty);
    NODE_SET_PROTOTYPE_METHOD(tpl, "invokeMethod", InvokeMethod);
}

void ObjectDispatchWrap::Init(Handle<Object> exports)
{
    InspectableBaseWrap::Init(
        exports,
        _constructor,
        _template,
        "PropertyData",
        SetPrototypeMethods);
}

void ObjectDispatchWrap::GetProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    ObjectDispatchWrap* pWrapper = ObjectWrap::Unwrap<ObjectDispatchWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_objectDisptachPtr))
    {
        return;
    }

    UINT32 propertyId;
    if (!GetArgumentValue(isolate, args, 0, propertyId))
    {
        return;
    }
    InspectablePtr valuePtr;
    _NODEJS_ERROR_EXCP_(pWrapper->_objectDisptachPtr->GetProperty(propertyId, valuePtr.GetAddressOf()), U("IObjectDispatch::GetProperty"));
    _NODEJS_RETURN_VALUE_EXCP_(valuePtr);
}

void ObjectDispatchWrap::SetProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    ObjectDispatchWrap* pWrapper = ObjectWrap::Unwrap<ObjectDispatchWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_objectDisptachPtr))
    {
        return;
    }

    UINT32 propertyId;
    if (!GetArgumentValue(isolate, args, 0, propertyId))
    {
        return;
    }
    InspectablePtr valuePtr;
    if (!ThrowIfArgumentValue(isolate, args, 1, valuePtr))
    {
        return;
    }
    _NODEJS_ERROR_EXCP_(pWrapper->_objectDisptachPtr->SetProperty(propertyId, valuePtr), U("IObjectDispatch::SetProperty"));
}

void ObjectDispatchWrap::InvokeMethod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);
    ObjectDispatchWrap* pWrapper = ObjectWrap::Unwrap<ObjectDispatchWrap>(args.Holder());
    if (!ThrowIfDisposed(isolate, pWrapper->_objectDisptachPtr))
    {
        return;
    }

    UINT32 methodId;
    if (!GetArgumentValue(isolate, args, 0, methodId))
    {
        return;
    }
    std::vector< InspectablePtr> parameters;
    for (int index = 1; index < args.Length(); ++index)
    {
        InspectablePtr valuePtr;
        if (!ThrowIfArgumentValue(isolate, args, index, valuePtr))
        {
            return;
        }
        parameters.push_back(valuePtr);
    }

    foundation::InspectablePtr resultPtr;
    HRESULT hr = pWrapper->_objectDisptachPtr->InvokeMethod(
        (UINT32)methodId,
        (UINT32)parameters.size(),
        parameters.size() > 0 ? parameters.front().GetAddressOfPtr() : nullptr,
        resultPtr.GetAddressOf());

    _NODEJS_ERROR_EXCP_(hr, U("IObservableObject::InvokeMethod"));
    _NODEJS_RETURN_VALUE_EXCP_(resultPtr);
}
