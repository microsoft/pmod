/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NodeJsHelper.cpp
****/
#include "pch.h"
#include "NodeJsHelper.h"
#include "DictionaryWrap.h"
#include "DispatcherWrap.h"
#include "ObservableObjectWrap.h"
#include "ObservableCollectionWrap.h"
#include "CommandWrap.h"
#include "AsyncOperationWrap.h"
#include <foundation/boxing_util.h>
#include <foundation/string_t.h>
#include <pal/library/trace_util.h>

using namespace v8;
using namespace foundation;
using namespace pmod;

extern uv_thread_t _MainThread;

uv_thread_t GetMainThread()
{
    return _MainThread;
}

bool IsMainThread()
{
    uv_thread_t self = uv_thread_self();
    return uv_thread_equal(&_MainThread, &self) ? true:false;
}

bool ThrowIfNumberOfArgs(v8::Isolate* isolate, const FunctionCallbackInfo<Value>& args,int expected)
{
    if (args.Length() < expected)
    {
        string_t errorMessage = pal::library::_Format_helper(
            U("Wrong number of arguments.Expected:%d Passed:%d"),
            expected,
            args.Length());

        ThrowErrorException(isolate, _TO_UTF8(errorMessage).c_str());
        return false;
    }
    return true;
}

bool ThrowIfArgumentFunction(
    v8::Isolate* isolate,
    const FunctionCallbackInfo<Value>& args,
    int index,
    Local<Function>& var)
{
    if (!ThrowIfNumberOfArgs(isolate, args, index + 1))
    {
        return false;
    }

    if (!args[index]->IsFunction())
    {
        string_t errorMessage = pal::library::_Format_helper(U("Argument:%d must be a function"), index);
        ThrowTypeErrorException(isolate, _TO_UTF8(errorMessage).c_str());
        return false;
    }
    var = Local<Function>::Cast(args[index]);
    return true;
}

bool ThrowIfArgumentValue(
    v8::Isolate* isolate,
    const FunctionCallbackInfo<Value>& args,
    int index,
    InspectablePtr& valuePtr,
    PropertyType expectedType)
{
    if (!ThrowIfNumberOfArgs(isolate, args, index + 1))
    {
        return false;
    }

    if (FAILED(ToFoundationValue(isolate, args[index], valuePtr.ReleaseAndGetAddressOf())))
    {
        string_t errorMessage = pal::library::_Format_helper(U("Wrong type for argument index:%d"),index);
        ThrowTypeErrorException(isolate, _TO_UTF8(errorMessage).c_str());
        return false;
    }

    if (valuePtr != nullptr && expectedType != PropertyType::PropertyType_Inspectable)
    {
        if (FAILED(pv_util::ChangeType(expectedType, valuePtr.GetAddressOfPtr())))
        {
            string_t errorMessage = pal::library::_Format_helper(U("Cast failed for argument index:%d expected:%d"), index, expectedType);
            ThrowTypeErrorException(isolate, _TO_UTF8(errorMessage).c_str());
            return false;
        }
    }
    return true;
}

void ThrowHResultException(v8::Isolate* isolate, const CHAR_t *msg,HRESULT hr)
{
    string_t errorMessage = pal::library::_Format_helper(U("%s.HResult error:0x%04x"), msg, hr);
    ThrowErrorException(isolate, _TO_UTF8(errorMessage).c_str());
}

#ifdef __UTF16_STRINGS

HStringPtr ToHstringPtr(Local<Value>& value)
{
    v8::String::Value key(value->ToString());
   return HStringPtr(reinterpret_cast<LPCSTR_t>(key.operator*()));
}

#else

HStringPtr ToHstringPtr(Local<Value>& value)
{
    v8::String::Utf8Value str(value->ToString());
    return HStringPtr(*str);
}

#endif

HRESULT ToFoundationValue(v8::Isolate* isolate, Local<Value>& value, foundation::IInspectable **ppValue)
{
    if (value->IsNull())
    {
        *ppValue = nullptr;
        return S_OK;
    }
    else if (value->IsUndefined())
    {
        return pv_util::CreateEmptyValue(ppValue);
    }
    if (value->IsFalse())
    {
        return pv_util::CreateBooleanValue(false, ppValue);
    }
    else if (value->IsTrue())
    {
        return pv_util::CreateBooleanValue(true, ppValue);
    }
    else if (value->IsInt32())
    {
        return pv_util::CreateInt32Value(value->Int32Value(), ppValue);
    }
    else if (value->IsUint32())
    {
        return pv_util::CreateInt32Value(value->Uint32Value(), ppValue);
    }
    else if (value->IsNumber())
    {
        return pv_util::CreateDoubleValue(value->NumberValue(), ppValue);
    }
    else if (value->IsString())
    {
        return pv_util::CreateStringValue(ToHstringPtr(value), ppValue);
    }
    else if (value->IsObject())
    {
        if (DictionaryWrap::IsInstanceOf(isolate, value) ||
            DispatcherWrap::IsInstanceOf(isolate, value) ||
            ObjectDispatchWrap::IsInstanceOf(isolate, value) ||
            ObservableObjectWrap::IsInstanceOf(isolate, value) ||
            ObservableCollectionWrap::IsInstanceOf(isolate, value) ||
            CommandWrap::IsInstanceOf(isolate, value))
        {
            InspectableBaseWrap* pInspectableBaseWrap = node::ObjectWrap::Unwrap<InspectableBaseWrap>(value->ToObject());
            foundation::IInspectable **pp = pInspectableBaseWrap->GetInspectablePtr();
            *ppValue = *pp;
            if (*pp != nullptr)
            {
                (*pp)->AddRef();
            }
            return S_OK;
        }
    }
    return E_TYPEMISMATCH;
}

HRESULT InspectableArrayToLocalValue(
    v8::Isolate* isolate,
    foundation::InspectableArrayWrapper &array,
    v8::Local<v8::Value>& localValue)
{
    // Create a new empty array.
    Handle<Array> hArray = Array::New(isolate, (int)array.GetSize());

    // Return an empty result if there was an error creating the array.
    if (hArray.IsEmpty())
    {
        return E_OUTOFMEMORY;
    }
    for (UINT32 index = 0; index < array.GetSize(); ++index)
    {
        Local<Value> localValue;
        _IFR_(InspectableToLocalValue(isolate, array[index], localValue));
        hArray->Set(index, localValue);
    }

    localValue = hArray;

    return S_OK;
}

HRESULT ReturnInspectableValue(v8::Isolate* isolate, foundation::IInspectable *pValue, const FunctionCallbackInfo<Value>& args)
{
    Local<Value> localValue;
    _IFR_(InspectableToLocalValue(isolate, pValue, localValue));
    args.GetReturnValue().Set(localValue);
    return S_OK;
}

HRESULT ReturnInspectableArray(v8::Isolate* isolate, foundation::InspectableArrayWrapper &array, const FunctionCallbackInfo<Value>& args)
{
    Local<Value> localValue;
    _IFR_(InspectableArrayToLocalValue(isolate, array, localValue));
    args.GetReturnValue().Set(localValue);
    return S_OK;
}

HRESULT InspectableToLocalValue(
    v8::Isolate* isolate,
    foundation::IInspectable *pValue,
    Local<Value>& localValue)
{
    if (pValue == nullptr)
    {
        localValue = Null(isolate);
        return S_OK;
    }

    IAsyncOperation  *pAsyncOperation;
    IObjectDispatch  *pPropertyData;
    IObservableObject  *pObservableObject;
    ICommand  *pCommand;
    IObservableCollection  *pObservableCollection;
    IPropertyValue  *pPropertyValue;
    IDictionary *pDictionary;

    if (SUCCEEDED(foundation::QueryWeakReference(pValue, IID_IPropertyValue, &pPropertyValue)))
    {
        PropertyType propertyType;
        pPropertyValue->get_Type(&propertyType);
        switch (propertyType)
        {
        case foundation::PropertyType_Empty:
            break;
        case foundation::PropertyType_UInt8:
            break;
        case foundation::PropertyType_Int16:
        case foundation::PropertyType_Int32:
        {
            INT32 value;
            _IFR_(pv_util::GetInt32Value(pValue, &value));
            localValue = Integer::New(isolate, value);
            return S_OK;
        }
        case foundation::PropertyType_UInt16:
        case foundation::PropertyType_UInt32:
        {
            UINT32 value;
            _IFR_(pv_util::GetUInt32Value(pValue, &value));
            localValue = Uint32::New(isolate, value);
            return S_OK;
        }

        break;
        case foundation::PropertyType_Int64:
            break;
        case foundation::PropertyType_UInt64:
            break;
        case foundation::PropertyType_Single:
            break;
        case foundation::PropertyType_Double:
            break;
        case foundation::PropertyType_Char16:
            break;
        case foundation::PropertyType_Boolean:
        {
            boolean value;
            _IFR_(pv_util::GetBooleanValue(pValue, &value));
            localValue = Boolean::New(isolate, value ? true : false);
            return S_OK;
        }
        case foundation::PropertyType_String:
        {
            HStringPtr value;
            _IFR_(pv_util::GetStringValue(pValue, value.GetAddressOf()));
#ifdef __UTF16_STRINGS
            localValue = v8::String::NewFromUtf8(isolate, foundation::to_utf8_string(value).c_str());
#else
            localValue = v8::String::NewFromUtf8(isolate, value.GetRawBuffer().c_str());
#endif
            return S_OK;
        }
        case foundation::PropertyType_Inspectable:
            break;
        case foundation::PropertyType_DateTime:
            break;
        case foundation::PropertyType_TimeSpan:
            break;
        case foundation::PropertyType_Guid:
            break;
        case foundation::PropertyType_Point:
            break;
        case foundation::PropertyType_Size:
            break;
        case foundation::PropertyType_Rect:
            break;
        case foundation::PropertyType_OtherType:
            break;
        case foundation::PropertyType_UInt8Array:
            break;
        case foundation::PropertyType_Int16Array:
            break;
        case foundation::PropertyType_UInt16Array:
            break;
        case foundation::PropertyType_Int32Array:
            break;
        case foundation::PropertyType_UInt32Array:
            break;
        case foundation::PropertyType_Int64Array:
            break;
        case foundation::PropertyType_UInt64Array:
            break;
        case foundation::PropertyType_SingleArray:
            break;
        case foundation::PropertyType_DoubleArray:
            break;
        case foundation::PropertyType_Char16Array:
            break;
        case foundation::PropertyType_BooleanArray:
            break;
        case foundation::PropertyType_StringArray:
            break;
        case foundation::PropertyType_InspectableArray:
            break;
        case foundation::PropertyType_DateTimeArray:
            break;
        case foundation::PropertyType_TimeSpanArray:
            break;
        case foundation::PropertyType_GuidArray:
            break;
        case foundation::PropertyType_PointArray:
            break;
        case foundation::PropertyType_SizeArray:
            break;
        case foundation::PropertyType_RectArray:
            break;
        case foundation::PropertyType_OtherTypeArray:
            break;
        default:
            break;
        }
    }
    else if (SUCCEEDED(foundation::QueryWeakReference(pValue, &pAsyncOperation)))
    {
        localValue = AsyncOperationWrap::Create(pAsyncOperation);

        return S_OK;
    }
    else if (SUCCEEDED(foundation::QueryWeakReference(pValue, &pCommand)))
    {
        localValue = CommandWrap::Create(pCommand);
        return S_OK;
    }
    else if (SUCCEEDED(foundation::QueryWeakReference(pValue, &pObservableCollection)))
    {
        localValue = ObservableCollectionWrap::Create(pObservableCollection);
        return S_OK;
    }
    else if (SUCCEEDED(foundation::QueryWeakReference(pValue, &pObservableObject)))
    {
        localValue = ObservableObjectWrap::Create(pObservableObject);
        return S_OK;
    }
    else if (SUCCEEDED(foundation::QueryWeakReference(pValue, &pPropertyData)))
    {
        localValue = ObjectDispatchWrap::Create(pPropertyData);
        return S_OK;
    }
    else if (SUCCEEDED(foundation::QueryWeakReference(pValue, &pDictionary)))
    {
        localValue = DictionaryWrap::Create(pDictionary);
        return S_OK;
    }
    return E_NOTIMPL;
}


