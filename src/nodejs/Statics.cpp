/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Statics.cpp
****/
#include "pch.h"
#include "Statics.h"
#include "NodeJsHelper.h"
#include <pal/pal_load_library.h>
#include <foundation/activation_factory.h>

using namespace v8;
using namespace foundation;

void Statics::LoadLibraryInternal(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    HStringPtr moduleName;
    if (!GetArgumentValue(isolate, args, 0, moduleName))
    {
        return;
    }
    _pal_module_t hModule = _pal_LoadLibrary(moduleName.GetRawBuffer());
    if (!hModule)
    {
        ThrowErrorExceptionWithFormat(
            isolate,
            "Failed to load library:{0}",
            _TO_UTF8(moduleName).c_str(),
            nullptr);
    }
}

void Statics::GetActivationFactory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::GetCurrent();
    HandleScope scope(isolate);

    HStringPtr activationId;
    if (!GetArgumentValue(isolate, args, 0, activationId))
    {
        return;
    }    
    foundation::IInspectable *pActivationFactory = reinterpret_cast<foundation::IInspectable *>(GetActivationFactoryInstance(activationId.GetRawBuffer()));
    if (pActivationFactory == nullptr)
    {
        ThrowErrorExceptionWithFormat(isolate, "GetActivationFactoryInstance('%s')", activationId.GetRawBuffer());
    }
    ReturnInspectableValue(isolate, pActivationFactory, args);
}

void Statics::Init(Handle<Object> exports) {
  NODE_SET_METHOD(exports, "loadLibrary", LoadLibraryInternal);
  NODE_SET_METHOD(exports, "getActivationFactory", GetActivationFactory);
}

