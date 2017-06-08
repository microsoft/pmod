/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NodeJsHelper.h
****/
#pragma once

#include <foundation/hstring_wrapper.h>
#include <foundation/com_ptr.h>
#include <foundation/boxing_util.h>
#include <foundation/library/string_util.h>

#include <uv.h>

uv_thread_t GetMainThread();
bool IsMainThread();

#ifdef __UTF16_STRINGS
#define _TO_UTF8(string_t) foundation::to_utf8_string(string_t)
#else
#define _TO_UTF8(string_t) string_t
#endif

foundation::HStringPtr ToHstringPtr(v8::Local<v8::Value>& value);
HRESULT ToFoundationValue(v8::Isolate* isolate, v8::Local<v8::Value>& value, foundation::IInspectable **ppValue);
HRESULT InspectableToLocalValue(v8::Isolate* isolate, foundation::IInspectable *pValue, v8::Local<v8::Value>& localValue);

HRESULT ReturnInspectableValue(v8::Isolate* isolate, foundation::IInspectable *pValue, const v8::FunctionCallbackInfo<v8::Value>& args);
HRESULT ReturnInspectableArray(v8::Isolate* isolate, foundation::InspectableArrayWrapper &array, const v8::FunctionCallbackInfo<v8::Value>& args);

bool ThrowIfNumberOfArgs(v8::Isolate* isolate, const v8::FunctionCallbackInfo<v8::Value>& args, int expected);
bool ThrowIfArgumentValue(
    v8::Isolate* isolate,
    const v8::FunctionCallbackInfo<v8::Value>& args,
    int index,
    foundation::InspectablePtr& valuePtr,
    foundation::PropertyType expectedType = foundation::PropertyType::PropertyType_Inspectable);
bool ThrowIfArgumentFunction(
    v8::Isolate* isolate,
    const v8::FunctionCallbackInfo<v8::Value>& args,
    int index,
    v8::Local<v8::Function>& var);

static inline void ThrowErrorException(v8::Isolate* isolate,const char* errorMsg)
{
    isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, errorMsg)));
}

static inline void ThrowErrorExceptionWithFormat(v8::Isolate* isolate, const char* _Format, ...)
{
    va_list args;
    va_start(args, _Format);     /* Initialize variable arguments. */
    ThrowErrorException(
        isolate,
        foundation::library::string_util::_format<std::string, std::ostringstream>(_Format, args).c_str());
}

void ThrowHResultException(v8::Isolate* isolate, const CHAR_t *msg, HRESULT hr);

static inline void ThrowTypeErrorException(v8::Isolate* isolate, const char* errorMsg)
{
    isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, errorMsg)));
}

template <class T>
bool GetArgumentValue(
    v8::Isolate* isolate,
    const v8::FunctionCallbackInfo<v8::Value>& args,
    int index,
    T& value)
{
    foundation::InspectablePtr valuePtr;
    if (!ThrowIfArgumentValue(
        isolate,
        args,
        index,
        valuePtr,
        foundation::GetPropertyType<T>()))
    {
        return false;
    }
    foundation::GetValue(valuePtr, value);
    return true;
}


#define _NODEJS_TYPE_EXCP_(x,msg) { if(FAILED((x))) { ThrowTypeErrorException(isolate, msg); return; } }
#define _NODEJS_ERROR_EXCP_(x,msg) {HRESULT hr_IFR = (x); if(FAILED(hr_IFR)) { ThrowHResultException(isolate, msg, hr_IFR); return; } }

#define _NODEJS_RETURN_VALUE_EXCP_(p) { if(FAILED(ReturnInspectableValue(isolate,p,args))) { ThrowErrorException(isolate,"NodeJsHelper::ReturnInspectableValue"); return; } }


