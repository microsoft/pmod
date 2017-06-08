/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectWrap.cpp
****/
#include "pch.h"
#include "ObjectWrap.h"
#include "NodeJsHelper.h"

using namespace v8;
using namespace foundation;

void ObjectWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
    InspectableBaseWrap::SetPrototypeMethods(tpl);
    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "getUniqueId", GetUniqueId);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getProcessId", GetProcessId);
}

void ObjectWrap::GetUniqueId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    ObjectWrap* pWrapper = node::ObjectWrap::Unwrap<ObjectWrap>(args.Holder());
    IObject *pObject;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObject))
    {
        return;
    }

    UINT32 uniqueId;
    pObject->GetUniqueId(&uniqueId);
    args.GetReturnValue().Set(Integer::New(isolate, uniqueId));
}

void ObjectWrap::GetProcessId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    ObjectWrap* pWrapper = node::ObjectWrap::Unwrap<ObjectWrap>(args.Holder());
    IObject *pObject;
    if (!pWrapper->ThrowIfGetPtr(isolate, &pObject))
    {
        return;
    }

    UINT32 processId;
    pObject->GetProcessId(&processId);
    args.GetReturnValue().Set(Integer::New(isolate, processId));
}
