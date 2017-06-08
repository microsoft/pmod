/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JsonStatics.cpp
****/
#include "pch.h"

#include "JsonStatics.h"

#include "jansson.h"
#include <foundation/pv_util.h>
#include <foundation/library/json_statics_util.h>
#include <foundation/library/dictionary_class.h>
#include "FoundationUtilHelper.h"
#include <foundation/library/com_ptr_singleton.h>
#include <foundation/library/base_adapter_host.h>

#include <map>
#include <sstream>

// {B7EB442C-AF9D-41B5-A44B-16EB07B02121}
const GUID foundation::library::IID_IJsonStatics =
{ 0xb7eb442c, 0xaf9d, 0x41b5,{ 0xa4, 0x4b, 0x16, 0xeb, 0x7, 0xb0, 0x21, 0x21 } };

using namespace foundation;

const char *_dtPrefix = "__dt:";

class _JsonAutoDecr
{
public:
    _JsonAutoDecr(json_t *json):
        _json(json)
    {
        foundation_assert(json);
    }
    ~_JsonAutoDecr()
    {
        json_decref(_json);
    }
private:
    json_t *_json;
};

static HRESULT ToJsonString(json_t *jsonObj, HSTRING *json);
static HRESULT FromJsonString(HSTRING json, json_t *&jsonObj);
static HRESULT CreateJsonError(
	const json_error_t& error,
	_COM_Outptr_ IDictionary **ppJsonError);
static HRESULT FromJsonFile(
	const char *path,
    IDictionary **ppJsonError,
	json_t *&jsonObj);
static HRESULT FromJsonString(
	const char *json_str,
    IDictionary **ppJsonError,
	json_t *&jsonObj);

//------------------------------------------------------------------------------
// ToJsonArray
// Helper to convert from WinRT Array into a Json
// Note: this will only work if cast is supported between the WinRT <-> Json Type
//------------------------------------------------------------------------------
template 
<
    typename TMoType,       // the modern type 
    typename TJsonType      // the Json native type
>
HRESULT ToJsonArray(
    IPropertyValue *pPropertyValue,
	HRESULT (STDMETHODCALLTYPE IPropertyValue::*pGetArray)(UINT32 *, TMoType **),
    json_t *(*jsonValueFactory)(TJsonType),
    _Out_ json_t *&json)
{
    UINT32 size;
    TMoType *pValues;

    HRESULT hr = (pPropertyValue->*pGetArray)(&size,&pValues);
    IFHR_ASSERT(hr);

    json = json_array();
    for(UINT32 i = 0;i < size;++i)
    {
        json_array_append_new(json,(*jsonValueFactory)((TJsonType)pValues[i]));
    }
    _pal_MemFree((LPVOID)pValues);
    return S_OK;
}
// forwards
static HRESULT IInspectableToJson(
    _In_ foundation::IInspectable *pInspectable,
    _Out_ json_t *&json);
static HRESULT JsonToInspectable(
    _In_ json_t *jsonObj,
    _Out_ foundation::IInspectable **ppInspectable);

static json_t *toJson(HSTRING hstr)
{
    return json_string(_TO_UTF8_STR(hstr));
}
//------------------------------------------------------------------------------
// CoreDictionaryToJson
// Helper to convert from IDictionary -> json_t
//------------------------------------------------------------------------------
static HRESULT CoreDictionaryToJson(
    _In_ IDictionary *pDictionary,
    _Out_ json_t *&jsonObj) 
{
    foundation_assert(pDictionary);

    foundation::ArrayRefCountWrapper<IDictionaryPair> properties;
    IFR_ASSERT(pDictionary->GetView(properties.GetSizeAddressOf(),properties.GetBufferAddressOf()));

    jsonObj = json_object();
    for(UINT32 index = 0 ;index < properties.GetSize();++index)
    {
        foundation::HStringPtr key;
        IFR_ASSERT(properties[index]->GetKey(key.GetAddressOf()));

        foundation::InspectablePtr spValue;
        IFR_ASSERT(properties[index]->GetValue(spValue.GetAddressOf()));
        json_t *jsonValue = nullptr;
        IFR_ASSERT(IInspectableToJson(spValue,jsonValue));
        json_object_set_new(jsonObj, _TO_UTF8_STR((HSTRING)key), jsonValue);
    }
    return S_OK;
}
//------------------------------------------------------------------------------
// JsonToCoreDictionary
// Helper to convert from json_t  -> IDictionary
//------------------------------------------------------------------------------
static HRESULT JsonToCoreDictionary(
    _In_ json_t *jsonObj,
    _Out_ IDictionary **ppDictionary) 
{
    HRESULT hr;
    foundation::ComPtr<IDictionary>                  spCoreDictionary;
    IFHR_ASSERT(library::CreateDictionaryClass(spCoreDictionary.GetAddressOf()));

    const char *key;
    json_t *value;
    json_object_foreach(jsonObj,key,value)
    {
        foundation::InspectablePtr spValue;
        IFHR_ASSERT(JsonToInspectable(value,spValue.GetAddressOf()));
        bool replaced;
        IFHR_ASSERT(spCoreDictionary->Insert(foundation::HStringRef(_TO_PAL_STR(key)),spValue,&replaced));
    }
    *ppDictionary = spCoreDictionary.Detach();
    return S_OK;
}
//------------------------------------------------------------------------------
// InspectableArrayToJson
// Helper to convert from InspectableArrayWrapper -> json_t
//------------------------------------------------------------------------------
static HRESULT InspectableArrayToJson(
    _In_ UINT32 size,
    _In_ foundation::IInspectable **ppInspectables,
    _Out_ json_t *&json)
{
    HRESULT hr;
    json = json_array();
    for(UINT32 i = 0;i < size;++i)
    {
        json_t *jsonObj = nullptr;
        hr = IInspectableToJson(ppInspectables[i],jsonObj);
        IFHR_ASSERT(hr);
        json_array_append_new(json,jsonObj);
    }
    return S_OK;
}

//------------------------------------------------------------------------------
// PropertyValueToJson
// Helper to convert from IPropertyValue -> json_t
//------------------------------------------------------------------------------
static HRESULT PropertyValueToJson(
    _In_ foundation::IPropertyValue *pPropertyValue,
    _Out_ json_t *&json)
{
    foundation_assert(pPropertyValue);
    PropertyType propertyType;
    HRESULT hr;

    IFHR_ASSERT(pPropertyValue->get_Type(&propertyType));
    switch(propertyType)
    {
    case PropertyType_Empty:
        json = json_null();
        break;
    case PropertyType_UInt8:
        {
            UINT8 value;
            pPropertyValue->GetUInt8(&value);
            json = json_integer((json_int_t)value);
        }
        break;
    case PropertyType_Int16:
        {
            INT16 value;
            pPropertyValue->GetInt16(&value);
            json = json_integer((json_int_t)value);
        }
        break;
    case PropertyType_UInt16:
        {
            UINT16 value;
            pPropertyValue->GetUInt16(&value);
            json = json_integer((json_int_t)value);
        }
        break;
    case PropertyType_Int32:
        {
            INT32 value;
            pPropertyValue->GetInt32(&value);
            json = json_integer((json_int_t)value);
        }
        break;
    case PropertyType_UInt32:
        {
            UINT32 value;
            pPropertyValue->GetUInt32(&value);
            json = json_integer((json_int_t)value);
        }
        break;
    case PropertyType_Int64:
        {
            INT64 value;
            pPropertyValue->GetInt64(&value);
            json = json_integer((json_int_t)value);
        }
        break;
    case PropertyType_UInt64:
        {
            UINT64 value;
            pPropertyValue->GetUInt64(&value);
            json = json_integer((json_int_t)value);
        }
        break;
    case PropertyType_Single:
        {
            FLOAT value;
            pPropertyValue->GetSingle(&value);
            json = json_real(value);
        }
        break;
    case PropertyType_Double:
        {
            DOUBLE value;
            pPropertyValue->GetDouble(&value);
            json = json_real(value);
        }
        break;
    case PropertyType_Char16:
        {
            WCHAR value;
            pPropertyValue->GetChar16(&value);
            json = json_integer((json_int_t)value);
        }
        break;
    case PropertyType_Boolean:
        {
            boolean bVal;
            pPropertyValue->GetBoolean(&bVal);
            json = json_boolean(bVal);
        }
        break;
    case PropertyType_String:
        {
            foundation::HStringPtr hstr;
            pPropertyValue->GetString(hstr.GetAddressOf());
            json = toJson(hstr);
        }
        break;
    case PropertyType_Guid:
        {
            IID iid;
            pPropertyValue->GetGuid(&iid);
            HStringPtr hstr;
            pv_util::ToString(iid,hstr.GetAddressOf());
            json = json_string(foundation::to_utf8_string(hstr).c_str());
        }
        break;
    case PropertyType_DateTime:
        {
            DateTime dt;
            pPropertyValue->GetDateTime(&dt);

            std::ostringstream dtStream;
            dtStream << _dtPrefix;
            dtStream << dt.UniversalTime;

            json = json_string(dtStream.str().c_str());
        }
        break;
    case PropertyType_UInt8Array:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetUInt8Array,json_integer,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_Int16Array:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetInt16Array,json_integer,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_UInt16Array:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetUInt16Array,json_integer,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_Int32Array:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetInt32Array,json_integer,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_UInt32Array:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetUInt32Array,json_integer,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_Int64Array:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetInt64Array,json_integer,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_UInt64Array:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetUInt64Array,json_integer,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_SingleArray:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetSingleArray,json_real,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_DoubleArray:
        {
            hr = ToJsonArray(pPropertyValue,&IPropertyValue::GetDoubleArray,json_real,json);
            IFHR_ASSERT(hr);
        }
        break;
    case PropertyType_BooleanArray:
        {
            UINT32 size;
            boolean *pValues;

            hr = pPropertyValue->GetBooleanArray(&size,&pValues);
            IFHR_ASSERT(hr);

            json = json_array();
            for(UINT32 i = 0;i < size;++i)
            {
                json_array_append_new(json,json_boolean(pValues[i]));
            }
            _pal_MemFree((LPVOID)pValues);
        }
        break;
    case PropertyType_StringArray:
        {
            foundation::HStringArrayWrapper strArrayWrapper;

            hr = pPropertyValue->GetStringArray(
                strArrayWrapper.GetSizeAddressOf(),
                strArrayWrapper.GetBufferAddressOf());
            IFHR_ASSERT(hr);

            json = json_array();
            for(UINT32 i = 0;i < strArrayWrapper.GetSize();++i)
            {
                json_array_append_new(json,json_string(_TO_UTF8_STR(strArrayWrapper[i])));
            }
        }
        break;
    case PropertyType_InspectableArray:
        {
            foundation::InspectableArrayWrapper objArrayWrapper;

            IFHR_ASSERT(pPropertyValue->GetInspectableArray(
                objArrayWrapper.GetSizeAddressOf(),
                objArrayWrapper.GetBufferAddressOf()));

            IFHR_ASSERT(InspectableArrayToJson(
                objArrayWrapper.GetSize(),
                (foundation::IInspectable **)objArrayWrapper.GetBuffer(),
                json));
        }
        break;
    default:
        foundation_assert(false);
        return E_NOTIMPL;
    }
    return S_OK;
}
//------------------------------------------------------------------------------
// JsonToPropertyValue
// Helper to convert from json_t -> IPropertyValue
//------------------------------------------------------------------------------
static HRESULT JsonToPropertyValue(
    _In_ json_t *jsonObj,
    _Out_ IPropertyValue **ppPropertyValue
    )
{
    foundation_assert(jsonObj);
    if(json_is_integer(jsonObj))
    {
        return pv_util::CreateInt64Value(json_integer_value(jsonObj),(foundation::IInspectable **)ppPropertyValue);
    }
    else if(json_is_real(jsonObj))
    {
        return pv_util::CreateDoubleValue(json_real_value(jsonObj),(foundation::IInspectable **)ppPropertyValue);
    }
    else if(json_is_boolean(jsonObj))
    {
        return pv_util::CreateBooleanValue(json_is_true(jsonObj),(foundation::IInspectable **)ppPropertyValue);
    }
    else if(json_is_string(jsonObj))
    {
        const char *pstr = json_string_value(jsonObj);
        if(strncmp(pstr,_dtPrefix,strlen(_dtPrefix)) == 0)
        {
            // date time found encoded as a json string
            DateTime dt;

            std::stringstream is(pstr + strlen(_dtPrefix));
            is >> dt.UniversalTime;
            return pv_util::CreateDateTimeValue(dt,(foundation::IInspectable **)ppPropertyValue);
        }
        else
        {
            return pv_util::CreateStringValue(_TO_PAL_STR(pstr),(foundation::IInspectable **)ppPropertyValue);
        }
    }
    else if(json_is_array(jsonObj))
    {
        // TODO: we may want to support type safe array instead of foundation::IInspectable
        _Inspectable_ptr_vector_type spArrayValues;
        size_t size = json_array_size(jsonObj);
        for(size_t i = 0;i < size; ++i)
        {
            foundation::InspectablePtr spValue;
            IFR_ASSERT(JsonToInspectable(
                    json_array_get(jsonObj,i),
                    spValue.GetAddressOf()));
            spArrayValues.push_back(spValue);
        }
        return pv_util::CreateInspectableArrayValue(
            (UINT32)size,
            size ? spArrayValues.front().GetAddressOfPtr():nullptr,
            (foundation::IInspectable **)ppPropertyValue);
    }

    foundation_assert(false);
    return E_UNEXPECTED;
}
//------------------------------------------------------------------------------
// IInspectableToJson
// Helper to convert from foundation::IInspectable -> json_t
//------------------------------------------------------------------------------
static HRESULT IInspectableToJson(
    _In_ foundation::IInspectable *pInspectable,
    _Out_ json_t *&json)
{
    if(pInspectable == nullptr)
    {
        json = json_null();
        return S_OK;
    }

    PropertyValuePtr spAsPropertyValue;
    if(SUCCEEDED(pv_util::CastToPropertyValue(
            pInspectable,
            spAsPropertyValue.GetAddressOf())))
    {
        return PropertyValueToJson(spAsPropertyValue,json);
    }
    // we only expect to resolve IDictionary interfaces
    foundation::ComPtr<IDictionary>   spCoreDictionary;
    IFR_ASSERT(foundation::QueryInterface(pInspectable,spCoreDictionary.GetAddressOf()));

    return CoreDictionaryToJson(spCoreDictionary,json);
}

//------------------------------------------------------------------------------
// JsonToInspectable
// Helper to convert from json_t -> foundation::IInspectable
//------------------------------------------------------------------------------
static HRESULT JsonToInspectable(
    _In_ json_t *jsonObj,
    _Out_ foundation::IInspectable **ppInspectable)
{
    foundation_assert(ppInspectable);
    foundation_assert(jsonObj);

    if(json_is_null(jsonObj))
    {
        *ppInspectable = nullptr;
        return S_OK;
    }
    if(json_is_object(jsonObj))
    {
        return JsonToCoreDictionary(jsonObj,(IDictionary **)ppInspectable);
    }
    return JsonToPropertyValue(jsonObj,(IPropertyValue **)ppInspectable);
}
//------------------------------------------------------------------------------
// class CJsonStreamProvider
// Singleton to implement the json stream provider
//------------------------------------------------------------------------------
foundation::IStreamProvider *CJsonStreamProvider::GetInstance()
{
    return library::_GetFactoryInstance<CJsonStreamProvider, foundation::IStreamProvider>();
}

HRESULT CJsonStreamProvider::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    if (iid == foundation::IStreamProvider::GetIID())
    {
        *ppInterface = static_cast<foundation::IStreamProvider *>(this);
    }
    else
    {
        return _CJsonStreamProvider_Base::QueryInterfaceImpl(iid, ppInterface);
    }
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CJsonStreamProvider::Serialize(foundation::IInspectable *pValue, foundation::IInspectable **ppValue)
{
    foundation::HStringPtr json;
    IFR_ASSERT(json_util::GetJsonStatics()->ToJson(pValue, json.GetAddressOf()));
    IFR_ASSERT(foundation::pv_util::CreateStringValue(json, ppValue));
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CJsonStreamProvider::Deserialize(foundation::IInspectable *pValue, foundation::IInspectable **ppValue)
{
    foundation::HStringPtr json;
    IFR_ASSERT(foundation::pv_util::GetValue(pValue, &json));
    return json_util::GetJsonStatics()->FromJson(json, ppValue);
}

library::IJsonStatics *CJsonStatics::GetInstance()
{
    return library::_GetFactoryInstance<CJsonStatics, IJsonStatics>();
}

HRESULT STDMETHODCALLTYPE CJsonStatics::Parse(
        _In_ const char *json_str,
        /*_COM_Outptr_opt_result_maybenull_*/ IDictionary **ppJsonError,
        _COM_Outptr_ foundation::IInspectable **ppInspectable)
{
    IFCPTR_COM_OUT_PTR_ASSERT(ppInspectable);
    json_t *jsonObj = nullptr;
    _IFR_(FromJsonString(json_str, ppJsonError, jsonObj));

    foundation::InspectablePtr spObject;
    HRESULT hr = JsonToInspectable(jsonObj,spObject.GetAddressOf());
    json_decref(jsonObj);
    IFHR_ASSERT(hr);
    *ppInspectable = spObject.Detach();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CJsonStatics::ParseFromFile(
    _In_ const char *file_path,
    /*_COM_Outptr_opt_result_maybenull_*/ IDictionary **ppJsonError,
    _COM_Outptr_ foundation::IInspectable **ppInspectable)
{
    IFCPTR_COM_OUT_PTR_ASSERT(ppInspectable);

    json_t *jsonObj = nullptr;
    _IFR_(FromJsonFile(file_path, ppJsonError, jsonObj));

    foundation::InspectablePtr spObject;
    HRESULT hr = JsonToInspectable(jsonObj, spObject.GetAddressOf());
    json_decref(jsonObj);
    IFHR_ASSERT(hr);
    *ppInspectable = spObject.Detach();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CJsonStatics::ToJson(
        _In_ foundation::IInspectable *pInspectable,
        _Out_ HSTRING *json)
{
    json_t *jsonObj = nullptr;

    IFR_ASSERT(IInspectableToJson(pInspectable,jsonObj));
    foundation_assert(jsonObj);

    return ToJsonString(jsonObj,json);
}

HRESULT STDMETHODCALLTYPE CJsonStatics::FromJson(
        _In_ HSTRING json,
        _COM_Outptr_ foundation::IInspectable **ppInspectable)
{
    json_t *jsonObj = nullptr;
    IFR_ASSERT(FromJsonString(json,jsonObj));

    foundation::InspectablePtr spObject;
    HRESULT hr = JsonToInspectable(jsonObj,spObject.GetAddressOf());
    json_decref(jsonObj);
    IFHR_ASSERT(hr);
    *ppInspectable = spObject.Detach();
    return S_OK;
}
HRESULT STDMETHODCALLTYPE CJsonStatics::GetStreamProvider(
    _COM_Outptr_ foundation::IStreamProvider **ppStreamProvider)
{
    IFCPTR_ASSERT(ppStreamProvider);

    *ppStreamProvider = CJsonStreamProvider::GetInstance();
    (*ppStreamProvider)->AddRef();
    return S_OK;
}

HRESULT CJsonStatics::QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
{
    if (iid == IJsonStatics::GetIID())
    {
        *ppInterface = static_cast<IJsonStatics *>(this);
    }
    else
    {
        return _Base_Type::QueryInterfaceImpl(iid, ppInterface);
    }
    return S_OK;
}

HRESULT ToJsonString(json_t *jsonObj,HSTRING *json)
{
    char *json_str = json_dumps(jsonObj, JSON_INDENT(1) | JSON_PRESERVE_ORDER | JSON_ENCODE_ANY | JSON_ESCAPE_SLASH);
    foundation_assert(json_str);
    if(!json_str)
    {
        return E_UNEXPECTED;
    }
    json_decref(jsonObj);
    HRESULT hr;
#ifdef __UTF16_STRINGS
    // convert to wide string and return
    foundation::pal_wstring json_wstr = foundation::to_pal_wstring(json_str);
    hr = _pal_CreateString(json_wstr.get(),(UINT32)_pal_wcslen(json_wstr.get()),json);
#else
    hr = _pal_CreateString(json_str,(UINT32)_pal_strlen(json_str),json);
#endif
    free((void *)json_str);
    return hr;
}

HRESULT FromJsonString(HSTRING json,json_t *&jsonObj)
{
    std::string json_str = foundation::to_utf8_string(json);
    return FromJsonString(json_str.c_str(),nullptr,jsonObj);
}

HRESULT CreateJsonError(
    const json_error_t& error,
    _COM_Outptr_ IDictionary **ppJsonError)
{
    foundation_assert(ppJsonError);

    foundation::ComPtr<IDictionary> spJsonError;

    // register the JsonError type info
    IFR_ASSERT(library::CreateDictionaryClass(spJsonError.GetAddressOf()));

    bool replace;
    // Property_Line
    spJsonError->Insert(
        HStringPtr(U("Line")),
        pv_util::CreateValue(error.line), &replace);
    // Property_Column
    spJsonError->Insert(
        HStringPtr(U("Column")),
        pv_util::CreateValue(error.column), &replace);
    // Property_Position
    spJsonError->Insert(
        HStringPtr(U("Position")),
        pv_util::CreateValue(error.position), &replace);
    // Property_Source
    spJsonError->Insert(
        HStringPtr(U("Source")),
        pv_util::CreateValue(foundation::HStringPtr(error.source)), &replace);
    // Property_Position
    spJsonError->Insert(
        HStringPtr(U("Text")),
        pv_util::CreateValue(foundation::HStringPtr(error.text)), &replace);

    *ppJsonError = spJsonError.Detach();
    return S_OK;
}

HRESULT FromJsonFile(
    const char *path,
    IDictionary **ppJsonError,
    json_t *&jsonObj)
{
    if (ppJsonError != nullptr)
    {
        *ppJsonError = nullptr;
    }
    json_error_t error;
    jsonObj = json_load_file(path, JSON_DECODE_ANY, &error);
    if (jsonObj == nullptr)
    {
        if (ppJsonError != nullptr)
        {
            IFR_ASSERT(CreateJsonError(error, ppJsonError));
        }
        return FOUNDATION_E_JSON_PARSER;
    }
    return S_OK;
}

HRESULT FromJsonString(
    const char *json_str,
    IDictionary **ppJsonError,
    json_t *&jsonObj)
{
    if (ppJsonError != nullptr)
    {
        *ppJsonError = nullptr;
    }
    json_error_t error;
    jsonObj = json_loads(json_str, JSON_DECODE_ANY, &error);
    if(jsonObj == nullptr)
    {
        if(ppJsonError != nullptr)
        {
            IFR_ASSERT(CreateJsonError(error, ppJsonError));
        }
        return FOUNDATION_E_JSON_PARSER;
    }
    return S_OK;
}


library::IJsonStatics *GetJsonStaticsInternal()
{
    return CJsonStatics::GetInstance();
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.JsonStreamProvider"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CJsonStreamProvider::GetInstance)
    );

static foundation::library::_RegisterActivationFactory _serviceRegister2(
    U("foundation.JsonStatics"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&GetJsonStaticsInternal)
    );
