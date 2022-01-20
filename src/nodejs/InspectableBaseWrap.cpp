/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InspectableBaseWrap.cpp
****/
#include "pch.h"
#include "InspectableBaseWrap.h"
#include "NodeJsHelper.h"
#include <foundation/library/disposable_util.h>

using namespace v8;
using namespace foundation;

void InspectableBaseWrap::Init(
    v8::Handle<v8::Object> exports,
    Persistent<Function> &ctor,
    Persistent<FunctionTemplate> &function,
    const char *className,
    void(*_InitPrototypes)(v8::Local<v8::FunctionTemplate>& tpl),
    v8::FunctionCallback _new)
{
    Isolate* isolate = Isolate::GetCurrent();
    LocalContext context(isolate);

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, _new);
    tpl->SetClassName(String::NewFromUtf8(isolate, className).ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    function.Reset(isolate, tpl);

    // Prototype
    (*_InitPrototypes)(tpl);

    auto func = tpl->GetFunction(context.local());

    ctor.Reset(isolate, func.ToLocalChecked());
    exports->Set(context.local(), String::NewFromUtf8(isolate, className).ToLocalChecked(), func.ToLocalChecked());
}

void InspectableBaseWrap::SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl)
{
    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "dispose", Dispose);
}

void InspectableBaseWrap::Dispose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    bool dispose = false;
    if (args.Length() == 1)
    {
        GetArgumentValue(isolate, args, 0, dispose);
    }

    InspectableBaseWrap* pWrapper = ObjectWrap::Unwrap<InspectableBaseWrap>(args.Holder());
    foundation::IInspectable **pp = pWrapper->GetInspectablePtr();
    if (*pp != nullptr)
    {
        pWrapper->Dispose(dispose);
        if (dispose)
        {
            library::DisposeIf(*pp);
        }
        (*pp)->Release();
        *pp = nullptr;
    }
}

void InspectableBaseWrap::Dispose(bool dispose)
{
}

bool InspectableBaseWrap::ThrowIfGetInspectablePtr(v8::Isolate* isolate, foundation::IInspectable **pp)
{
    *pp = *this->GetInspectablePtr();
    return ThrowIfDisposed(isolate, *pp);
}

bool InspectableBaseWrap::ThrowIfDisposed(v8::Isolate* isolate, foundation::IInspectable *pInspectable)
{
    if (pInspectable == nullptr)
    {
        ThrowErrorException(isolate, "InspectableBaseWrap was disposed");
        return false;
    }
    return true;
}

void InspectableBaseWrap::CloseAndDeleteAsyncHandler(uv_async_t* handle)
{
    uv_close((uv_handle_t*)handle, _CloseAsyncHelper);
}

void InspectableBaseWrap::_CloseAsyncHelper(uv_handle_t* handle)
{
    auto asyncHandle = reinterpret_cast<uv_async_t *>(handle);
    delete asyncHandle;
}
