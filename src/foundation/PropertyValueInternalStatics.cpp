/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValueInternalStatics.cpp
****/

#include "pch.h"
#include "PropertyValueInternalStatics.h"
#include "SerializeUtil.h"
#include "GuidHelper.h"
#include <foundation/library/com_ptr_singleton.h>
#include "ActivationFactory.h"

#include <sstream>

#include <foundation/pv_util.h>
#include <foundation/errors.h>
#include <foundation/hstring_wrapper.h>
#include <foundation/library/string_util.h>
#include <foundation/library/util_stl.h>

#ifndef _WINDOWS
#include <inttypes.h>
#endif

// {DE7BD90F-06D8-46B8-AA9A-3D114FD1C983}
const GUID foundation::library::IID_IPropertyValueInternalStatics =
{ 0xde7bd90f, 0x6d8, 0x46b8,{ 0xaa, 0x9a, 0x3d, 0x11, 0x4f, 0xd1, 0xc9, 0x83 } };

using namespace foundation;

EXTERN_C FOUNDATION_API HRESULT  STDAPICALLTYPE PropertyValue_Deserialize(
    std::istream& is, 
    foundation::PropertyType type, 
    foundation::IPropertyValue **ppPropertyValue)
{
    return CPropertyValueInternalStatics::Deserialize(is, type, ppPropertyValue);
}

EXTERN_C FOUNDATION_API HRESULT  STDAPICALLTYPE PropertyValue_Serialize(
    std::ostream& os,
    foundation::PropertyType type,
    foundation::IPropertyValue *pPropertyValue)
{
    return CPropertyValueInternalStatics::Serialize(os, type, pPropertyValue);
}

template <class T>
static int CompareValues(const T& value1,const T& value2)
{
    if(value1 == value2)
    {
        return 0;
    }
    else if(value1 > value2)
    {
        return 1;
    }
    return -1;
}

__unused_attribute__
static bool isIntType(PropertyType type)
{
    return type >= PropertyType_UInt8 && type <= PropertyType_UInt64;
}

static bool isRealType(PropertyType type)
{
    return type == PropertyType_Single || type == PropertyType_Double;
}

static bool isValueType(PropertyType type)
{
    return type >= PropertyType_UInt8 && type <= PropertyType_Double;
}

template <typename IntType>
static HRESULT StringToInt(HSTRING hstr, IntType& value)
{
    CHAR_t *pEndPtr = nullptr;
    value = (IntType)_pal_strtol(_pal_GetStringRawBuffer(hstr, nullptr), &pEndPtr, 10);
    return (*pEndPtr != 0) ? E_TYPEMISMATCH : S_OK;
}

template <typename UIntType>
static HRESULT StringToUInt(LPCSTR_t pStr, UIntType& value)
{
    CHAR_t *pEndPtr = nullptr;
    value = (UIntType)_pal_strtoul(pStr, &pEndPtr, 10);
    return (*pEndPtr != 0) ? E_TYPEMISMATCH : S_OK;
}

static HRESULT StringToDate(LPCSTR_t pDate, SystemTime& value)
{
    std::vector<foundation::string_t> dateSplit = library::string_util::split_t(pDate, U("-"));
    if (dateSplit.size() != 3)
    {
        return E_TYPEMISMATCH;
    }
    _IFR_(StringToUInt(dateSplit[0].c_str(), value.wYear));
    _IFR_(StringToUInt(dateSplit[1].c_str(), value.wMonth));
    _IFR_(StringToUInt(dateSplit[2].c_str(), value.wDay));
    return S_OK;
}

static HRESULT StringToSystmeTime(LPCSTR_t pStr, SystemTime& value)
{
    std::vector<foundation::string_t> dateTimeSplit = library::string_util::split_t(pStr, U(" "));
    if (dateTimeSplit.size() != 2)
    {
        // no time part
        value.wHour = 0;
        value.wMinute = 0;
        value.wSecond = 0;
        value.wMilliseconds = 0;
        return StringToDate(pStr, value);
    }
    _IFR_(StringToDate(dateTimeSplit[0].c_str(), value));

    std::vector<foundation::string_t> timeSplit = library::string_util::split_t(dateTimeSplit[1].c_str(), U(":"));
    if (timeSplit.size() != 3)
    {
        return E_TYPEMISMATCH;
    }
    _IFR_(StringToUInt(timeSplit[0].c_str(), value.wHour));
    _IFR_(StringToUInt(timeSplit[1].c_str(), value.wMinute));

    std::vector<foundation::string_t> secondsSplit = library::string_util::split_t(timeSplit[2].c_str(), U("."));
    if (secondsSplit.size() != 2)
    {
        return E_TYPEMISMATCH;
    }
    _IFR_(StringToUInt(secondsSplit[0].c_str(), value.wSecond));
    _IFR_(StringToUInt(secondsSplit[1].c_str(), value.wMilliseconds));

    return S_OK;
}

static HRESULT StringToUInt(HSTRING hstr, UINT32& value)
{
    return StringToUInt(_pal_GetStringRawBuffer(hstr, nullptr), value);
}

static HRESULT CompareDateTimeWithValue(
    IPropertyValue *pDateTime,
    IPropertyValue *pValue,
    int *pCompareResult,
    bool isNone)
{
    foundation_assert(pCompareResult != nullptr);

    foundation::DateTime dt;
    IFR_ASSERT(pDateTime->GetDateTime(&dt));
    UINT64 value;
    IFR_ASSERT(pValue->GetUInt64(&value));
    int cmp = (int)((UINT64)dt.UniversalTime - value);
    *pCompareResult = isNone ? -cmp : cmp;
    return S_OK;

}

// Serialize an Array to a binary stream
template 
<
    typename TArrayWrapper,
    typename T,
    HRESULT  (STDMETHODCALLTYPE foundation::IPropertyValue::*pGetArray)(UINT32 *,T**)
>
static HRESULT SerializeArray(
    _In_ IPropertyValue *pPropertyValue,binary_serializer& bos)
{
    TArrayWrapper arrayWrapper;
    (pPropertyValue->*pGetArray)(arrayWrapper.GetSizeAddressOf(),arrayWrapper.GetBufferAddressOf());
    bos.serializeArray(arrayWrapper.GetSize(),arrayWrapper.GetBuffer());
    return S_OK;
}

// Deserialize an Array from a binray stream
template 
<
    typename TArrayWrapper,
    typename T,
    HRESULT  (STDMETHODCALLTYPE foundation::IPropertyValueStatics::*pCreateArray)(UINT32 ,T*,foundation::IInspectable **)
>
static HRESULT DeserializeArray(binary_deserializer& bis,foundation::IInspectable **ppValue)
{
    TArrayWrapper arrayWrapper;
    bis.deserializeArray(arrayWrapper.GetSizeAddressOf(),arrayWrapper.GetBufferAddressOf());
    return (pv_util::GetPropertyValueStatics()->*pCreateArray)(arrayWrapper.GetSize(),(T *)arrayWrapper.GetBuffer(),ppValue);
}

// toString convert methods
// TODO: move to a shared location
static foundation::string_t toString(BYTE value)
{
    return FormatNumber(U("%d"),(int)value);
}
static foundation::string_t toString(INT16 value)
{
    return FormatNumber(U("%d"),(int)value);
}
static foundation::string_t toString(UINT16 value)
{
    return FormatNumber(U("%u"),(int)value);
}
static foundation::string_t toString(INT32 value)
{
    return FormatNumber(U("%d"),value);
}
static foundation::string_t toString(UINT32 value)
{
    return FormatNumber(U("%u"),value);
}
static foundation::string_t toString(INT64 value)
{
#ifdef _WINDOWS
    return FormatNumber(U("%lli"),value);
#elif defined(PRId64)
    return FormatNumber("%" PRId64"",value);
#else
    return FormatNumber("%lld", value);
#endif
}
static foundation::string_t toString(UINT64 value)
{
#ifdef _WINDOWS
    return FormatNumber(U("%llu"),value);
#elif defined(PRIu64)
    return FormatNumber("%" PRIu64"",value);
#else
    return FormatNumber("%llu", value);
#endif
}
static foundation::string_t toString(DOUBLE value)
{
    return FormatNumber(U("%f"),value);
}

static foundation::string_t toString(bool value)
{
    return value ? U("true") : U("false");
}

static foundation::string_t toString(HSTRING value)
{
    return _pal_GetStringRawBuffer(value,nullptr);
}

foundation::string_t toString(const SystemTime& value)
{
    return FormatNumber(U("%04d-%02d-%02d %02d:%02d:%02d.%03d"), 
        value.wYear,
        value.wMonth,
        value.wDay,
        value.wHour,
        value.wMinute,
        value.wSecond,
        value.wMilliseconds);
}

// convert an Array to string using templatized parameters
template 
<
    typename TArrayWrapper,
    typename T,
    HRESULT  (STDMETHODCALLTYPE foundation::IPropertyValue::*pGetArray)(UINT32 *,T**)
>
static foundation::string_t toString(
    _In_ IPropertyValue *pPropertyValue)
{
    foundation::string_t str = U("[");
    TArrayWrapper arrayWrapper;
    (pPropertyValue->*pGetArray)(arrayWrapper.GetSizeAddressOf(),arrayWrapper.GetBufferAddressOf());
    for(UINT32 index = 0;index < arrayWrapper.GetSize(); ++index)
    {
        if(index > 0)
        {
            str += U(",");
        }
        str += toString(arrayWrapper[index]);
    }
    str += U("]");
    return str;
}
// Our internal statics singleton 
library::IPropertyValueInternalStatics *CPropertyValueInternalStatics::GetInstance()
{
	return library::_GetFactoryInstance<CPropertyValueInternalStatics, library::IPropertyValueInternalStatics>();
}

HRESULT CPropertyValueInternalStatics::GetOrCreateInstance(library::IPropertyValueInternalStatics **ppPropertyValueStatics)
{
    IFCPTR_ASSERT(ppPropertyValueStatics);
    *ppPropertyValueStatics = GetInstance();
    (*ppPropertyValueStatics)->AddRef();
    return S_OK;
}

HRESULT CPropertyValueInternalStatics::Serialize(
    std::ostream& os,
    foundation::IPropertyValue *pPropertyValue)
{
    foundation_assert(pPropertyValue);

    binary_serializer bos(os);

    PropertyType type;
    pPropertyValue->get_Type(&type);
    
    bos.serializeUInt16((UINT16)type);

    return Serialize(os,type,pPropertyValue);
}



HRESULT CPropertyValueInternalStatics::Serialize(
    std::ostream& os,
    PropertyType type,
    IPropertyValue *pPropertyValue)
{
    foundation_assert(pPropertyValue);

    binary_serializer bos(os);

    switch (type)
    {
    case PropertyType_Empty:
        break;
    case PropertyType_UInt8:
        {
            UINT8 value;
            pPropertyValue->GetUInt8(&value);
            bos.serializeByte(value);
        }
        break;
    case PropertyType_Int16:
        {
            INT16 value;
            pPropertyValue->GetInt16(&value);
            bos.serializeInt16(value);
        }
        break;
    case PropertyType_UInt16:
        {
            UINT16 value;
            pPropertyValue->GetUInt16(&value);
            bos.serializeUInt16(value);
        }
        break;
    case PropertyType_Int32:
        {
            INT32 value;
            pPropertyValue->GetInt32(&value);
            bos.serializeInt(value);
        }
        break;
    case PropertyType_UInt32:
        {
            UINT32 value;
            pPropertyValue->GetUInt32(&value);
            bos.serializeUInt(value);
        }
        break;
    case PropertyType_Int64:
        {
            INT64 value;
            pPropertyValue->GetInt64(&value);
            bos.serializeInt64(value);
        }
        break;
    case PropertyType_UInt64:
        {
            UINT64 value;
            pPropertyValue->GetUInt64(&value);
            bos.serializeUInt64(value);
        }
        break;
    case PropertyType_Single:
        {
            FLOAT value;
            pPropertyValue->GetSingle(&value);
            bos.serializeSingle(value);
        }
        break;
    case PropertyType_Double:
        {
            DOUBLE value;
            pPropertyValue->GetDouble(&value);
            bos.serializeDouble(value);
        }
        break;
    case PropertyType_Char16:
        {
            WCHAR value;
            pPropertyValue->GetChar16(&value);
            bos.serializeChar16(value);
        }
        break;
    case PropertyType_Boolean:
        {
            boolean boolValue;
            pPropertyValue->GetBoolean(&boolValue);
            bos.serializeBoolean(boolValue ? true:false);
        }
        break;
    case PropertyType_String:
        {
            foundation::HStringPtr hstr;
            pPropertyValue->GetString(hstr.GetAddressOf());
            bos.serializeString(hstr.GetRawBuffer());
        }
        break;
    case PropertyType_Guid:
        {
            IID iid;
            pPropertyValue->GetGuid(&iid);
            bos.serializeIID(iid);
        }
        break;
    case PropertyType_UInt8Array:
        {
            SerializeArray<foundation::UInt8ArrayWrapper,BYTE,&IPropertyValue::GetUInt8Array>(pPropertyValue,bos);
        }
        break;
    case PropertyType_Int16Array:
        {
            SerializeArray<foundation::Int16ArrayWrapper,INT16,&IPropertyValue::GetInt16Array>(pPropertyValue,bos);
        }
        break;
    case PropertyType_UInt16Array:
        {
            SerializeArray<foundation::UInt16ArrayWrapper,UINT16,&IPropertyValue::GetUInt16Array>(pPropertyValue,bos);
        }
        break;
    case PropertyType_Int32Array:
        {
            SerializeArray<foundation::Int32ArrayWrapper,INT32,&IPropertyValue::GetInt32Array>(pPropertyValue,bos);
        }
        break;
    case PropertyType_UInt32Array:
        {
            SerializeArray<foundation::UInt32ArrayWrapper,UINT32,&IPropertyValue::GetUInt32Array>(pPropertyValue,bos);
        }
        break;
    case PropertyType_Int64Array:
        {
            SerializeArray<foundation::Int64ArrayWrapper,INT64,&IPropertyValue::GetInt64Array>(pPropertyValue,bos);
        }
        break;
    case PropertyType_UInt64Array:
        {
            SerializeArray<foundation::UInt64ArrayWrapper,UINT64,&IPropertyValue::GetUInt64Array>(pPropertyValue,bos);
        }
        break;
    case PropertyType_SingleArray:
        {
            SerializeArray<foundation::SingleArrayWrapper,FLOAT,&IPropertyValue::GetSingleArray>(pPropertyValue,bos);
        }
        break;
    case PropertyType_DoubleArray:
        {
            SerializeArray<foundation::DoubleArrayWrapper,DOUBLE,&IPropertyValue::GetDoubleArray>(pPropertyValue,bos);
        }
        break;
    case PropertyType_BooleanArray:
        {
            SerializeArray<foundation::ArrayWrapper<boolean>,boolean,&IPropertyValue::GetBooleanArray>(pPropertyValue,bos);
        }
        break;
    case PropertyType_StringArray:
        {
            foundation::HStringArrayWrapper ArrayWrapper;
            IFR_ASSERT(pPropertyValue->GetStringArray(ArrayWrapper.GetSizeAddressOf(), ArrayWrapper.GetBufferAddressOf()));
            bos.serializeValue(ArrayWrapper.GetSize());
            for (UINT32 index = 0; index < ArrayWrapper.GetSize(); ++index)
            {
                bos.serializeString(_pal_GetStringRawBuffer(ArrayWrapper[index],nullptr));
            }
        }
        break;
    case PropertyType_DateTime:
        {
            foundation::DateTime dt;
            pPropertyValue->GetDateTime(&dt);
            bos.get_ostream().write((const char *)&dt,sizeof(foundation::DateTime));
        }
        break;
    case PropertyType_Inspectable:
    case PropertyType_TimeSpan:
    case PropertyType_Point:
    case PropertyType_Size:
    case PropertyType_Rect:
    default:
        foundation_assert(false);
        return E_NOTIMPL;
    }
    return S_OK;
}

HRESULT CPropertyValueInternalStatics::Deserialize(
    std::istream& is,
    IPropertyValue **ppPropertyValue)
{
    foundation_assert(ppPropertyValue);

    binary_deserializer bis(is);

    foundation::InspectablePtr spValue;
    PropertyType type = (PropertyType)bis.deserializeUInt16();
    return Deserialize(is,type,ppPropertyValue);
}

HRESULT CPropertyValueInternalStatics::Deserialize(
    std::istream& is,
    PropertyType type,
    IPropertyValue **ppPropertyValue)
{
    foundation_assert(ppPropertyValue);

    binary_deserializer bis(is);
    foundation::InspectablePtr spValue;

    switch (type)
    {
    case PropertyType_Empty:
        IFR_ASSERT(pv_util::CreateEmptyValue(spValue.GetAddressOf()));
        break;
    case PropertyType_UInt8:
        {
            UINT8 value = bis.deserializeByte();
            IFR_ASSERT(pv_util::CreateUInt8Value(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Int16:
        {
            INT16 value = bis.deserializeInt16();
            IFR_ASSERT(pv_util::CreateInt16Value(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_UInt16:
        {
            UINT16 value = bis.deserializeUInt16();
            IFR_ASSERT(pv_util::CreateUInt16Value(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Int32:
        {
            INT32 value = bis.deserializeInt();
            IFR_ASSERT(pv_util::CreateInt32Value(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_UInt32:
        {
            UINT32 value = bis.deserializeUInt();
            IFR_ASSERT(pv_util::CreateUInt32Value(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Int64:
        {
            INT64 value = bis.deserializeInt64();
            IFR_ASSERT(pv_util::CreateInt64Value(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_UInt64:
        {
            UINT64 value = bis.deserializeUInt64();
            IFR_ASSERT(pv_util::CreateUInt64Value(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Single:
        {
            FLOAT value = bis.deserializeSingle();
            IFR_ASSERT(pv_util::CreateSingleValue(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Double:
        {
            DOUBLE value = bis.deserializeDouble();
            IFR_ASSERT(pv_util::CreateDoubleValue(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Char16:
        {
            WCHAR value = bis.deserializeChar16();
            IFR_ASSERT(pv_util::CreateChar16Value(value,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Boolean:
        {
            boolean boolValue = bis.deserializeBoolean();
            IFR_ASSERT(pv_util::CreateBooleanValue(boolValue,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_String:
        {
            foundation::string_t s = bis.deserializeString();
            IFR_ASSERT(pv_util::CreateStringValue(s.c_str(),spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Guid:
        {
            IID iid = bis.deserializeIID();
            IFR_ASSERT(pv_util::CreateGuidValue(iid,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_DateTime:
        {
            foundation::DateTime dt;
            bis.get_istream().read((char *)&dt,sizeof(foundation::DateTime));
            IFR_ASSERT(pv_util::CreateDateTimeValue(dt,spValue.GetAddressOf()));
        }
        break;
    case PropertyType_UInt8Array:
        {
            DeserializeArray<foundation::UInt8ArrayWrapper,BYTE,&IPropertyValueStatics::CreateUInt8Array>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_Int16Array:
        {
            DeserializeArray<foundation::Int16ArrayWrapper,INT16,&IPropertyValueStatics::CreateInt16Array>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_UInt16Array:
        {
            DeserializeArray<foundation::UInt16ArrayWrapper,UINT16,&IPropertyValueStatics::CreateUInt16Array>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_Int32Array:
        {
            DeserializeArray<foundation::Int32ArrayWrapper,INT32,&IPropertyValueStatics::CreateInt32Array>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_UInt32Array:
        {
            DeserializeArray<foundation::UInt32ArrayWrapper,UINT32,&IPropertyValueStatics::CreateUInt32Array>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_Int64Array:
        {
            DeserializeArray<foundation::Int64ArrayWrapper,INT64,&IPropertyValueStatics::CreateInt64Array>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_UInt64Array:
        {
            DeserializeArray<foundation::UInt64ArrayWrapper,UINT64,&IPropertyValueStatics::CreateUInt64Array>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_SingleArray:
        {
            DeserializeArray<foundation::SingleArrayWrapper,FLOAT,&IPropertyValueStatics::CreateSingleArray>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_DoubleArray:
        {
            DeserializeArray<foundation::DoubleArrayWrapper,DOUBLE,&IPropertyValueStatics::CreateDoubleArray>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_BooleanArray:
        {
            DeserializeArray<foundation::ArrayWrapper<boolean>,boolean,&IPropertyValueStatics::CreateBooleanArray>(bis,spValue.GetAddressOf());
        }
        break;
    case PropertyType_StringArray:
        {
            UINT32 size = bis.deserializeUInt();
            std::vector<foundation::HStringPtr> hstring_array;
            hstring_array.resize(size);
            for (UINT32 index = 0; index < hstring_array.size(); ++index)
            {
                foundation::string_t str = bis.deserializeString();
                hstring_array[index] = foundation::HStringPtr(str.c_str());
            }
            IFR_ASSERT(pv_util::CreateStringArrayValue(
                (UINT32)hstring_array.size(), 
                hstring_array.size() ? (HSTRING *)hstring_array.front().GetAddressOfPtr() : nullptr, spValue.GetAddressOf()));
        }
        break;
    case PropertyType_Inspectable:
    case PropertyType_TimeSpan:
    case PropertyType_Point:
    case PropertyType_Size:
    case PropertyType_Rect:
    default:
        foundation_assert(false);
        return E_NOTIMPL;
    }
    foundation_assert(spValue != nullptr);
    IFR_ASSERT(foundation::QueryInterface(spValue,foundation_IID_IPropertyValue,ppPropertyValue));
    return S_OK;
}

foundation::string_t CPropertyValueInternalStatics::ToString(DateTime& dateTime)
{
    SystemTime systemTime;
    _pal_DateTimeToSystemTime(dateTime, &systemTime);
    return toString(systemTime);
}

foundation::string_t CPropertyValueInternalStatics::ToString(IPropertyValue *pPropertyValue)
{
    foundation_assert(pPropertyValue);

    foundation::PropertyType type;
    pPropertyValue->get_Type(&type);
    if(type == PropertyType_Empty)
    {
        return U("(null)");
    }
    else if(type == PropertyType_UInt8)
    {
        BYTE value;
        pPropertyValue->GetUInt8(&value);
        return toString(value);
    }
    else if(type == PropertyType_Int16)
    {
        INT16 value;
        pPropertyValue->GetInt16(&value);
        return toString(value);
    }
    else if(type == PropertyType_UInt16)
    {
        UINT16 value;
        pPropertyValue->GetUInt16(&value);
        return toString(value);
    }
    else if(type == PropertyType_Int32)
    {
        INT32 value;
        pPropertyValue->GetInt32(&value);
        return toString(value);
    }
    else if(type == PropertyType_UInt32)
    {
        UINT32 value;
        pPropertyValue->GetUInt32(&value);
        return toString(value);
    }
    else if(type == PropertyType_Int64)
    {
        INT64 value;
        pPropertyValue->GetInt64(&value);
        return toString(value);
    }
    else if(type == PropertyType_UInt64)
    {
        UINT64 value;
        pPropertyValue->GetUInt64(&value);
        return toString(value);

    }
    else if(type == PropertyType_Single)
    {
        FLOAT value;
        pPropertyValue->GetSingle(&value);
        return toString(value);
    }
    else if(type == PropertyType_Double)
    {
        DOUBLE value;
        pPropertyValue->GetDouble(&value);
        return toString(value);
    }
    else if(type == PropertyType_Boolean)
    {
        boolean value;
        pPropertyValue->GetBoolean(&value);
        return toString(value ? true:false);
    }
    else if(type == PropertyType_String)
    {
        foundation::HStringPtr hstr;
        pPropertyValue->GetString(hstr.GetAddressOf());
        return toString(hstr);
    }
    else if (type == PropertyType_DateTime)
    {
        foundation::DateTime dateTime;
        pPropertyValue->GetDateTime(&dateTime);
        return ToString(dateTime);
    }
    else if (type == PropertyType_Guid)
    {
        IID iid;
        pPropertyValue->GetGuid(&iid);
        return _TO_PAL_STR(::ToString(iid).c_str());
    }
    else if(type == PropertyType_Int32Array)
    {
        return toString<foundation::Int32ArrayWrapper,INT32,&IPropertyValue::GetInt32Array>(pPropertyValue);
    }
    else if(type == PropertyType_UInt8Array)
    {
        return toString<foundation::UInt8ArrayWrapper,BYTE,&IPropertyValue::GetUInt8Array>(pPropertyValue);
    }
    else if(type == PropertyType_Int16Array)
    {
        return toString<foundation::Int16ArrayWrapper,INT16,&IPropertyValue::GetInt16Array>(pPropertyValue);
    }
    else if(type == PropertyType_UInt16Array)
    {
        return toString<foundation::UInt16ArrayWrapper,UINT16,&IPropertyValue::GetUInt16Array>(pPropertyValue);
    }
    else if(type == PropertyType_Int32Array)
    {
        return toString<foundation::Int32ArrayWrapper,INT32,&IPropertyValue::GetInt32Array>(pPropertyValue);
    }
    else if(type == PropertyType_UInt32Array)
    {
        return toString<foundation::UInt32ArrayWrapper,UINT32,&IPropertyValue::GetUInt32Array>(pPropertyValue);
    }
    else if(type == PropertyType_Int64Array)
    {
        return toString<foundation::Int64ArrayWrapper,INT64,&IPropertyValue::GetInt64Array>(pPropertyValue);
    }
    else if(type == PropertyType_UInt64Array)
    {
        return toString<foundation::UInt64ArrayWrapper,UINT64,&IPropertyValue::GetUInt64Array>(pPropertyValue);
    }
    else if(type == PropertyType_SingleArray)
    {
        return toString<foundation::SingleArrayWrapper,FLOAT,&IPropertyValue::GetSingleArray>(pPropertyValue);
    }
    else if(type == PropertyType_DoubleArray)
    {
        return toString<foundation::DoubleArrayWrapper,DOUBLE,&IPropertyValue::GetDoubleArray>(pPropertyValue);
    }
    else if(type == PropertyType_BooleanArray)
    {
        return toString<foundation::ArrayWrapper<boolean>,boolean,&IPropertyValue::GetBooleanArray>(pPropertyValue);
    }
    else if(type == PropertyType_StringArray)
    {
        return toString<foundation::HStringArrayWrapper,HSTRING,&IPropertyValue::GetStringArray>(pPropertyValue);
    }
    else
    {
        return U("[???]");
    }
}


STDMETHODIMP CPropertyValueInternalStatics::ComparePropertyValues(
    _In_ IPropertyValue *pPropertyValue1,
    _In_ IPropertyValue *pPropertyValue2,
    _Out_ int *pCompareResult)
{
    foundation_assert(pPropertyValue1);
    foundation_assert(pPropertyValue2);

    PropertyType type1,type2;
    IFR_ASSERT(pPropertyValue1->get_Type(&type1));
    IFR_ASSERT(pPropertyValue2->get_Type(&type2));

    *pCompareResult = -1;
    if(type1 != type2)
    {
        if(isValueType(type1) && isValueType(type2))
        {
            if(isRealType(type1) || isRealType(type2))
            {
                type1 = PropertyType_Double;
            }
            else
            {
                // TODO: we should select the proper unsigned type when needed
                type1 = PropertyType_Int64;
            }
        }
        else if (isValueType(type2) && type1 == PropertyType_DateTime)
        {
            return CompareDateTimeWithValue(pPropertyValue1, pPropertyValue2, pCompareResult,false);
        }
        else if (isValueType(type1) && type2 == PropertyType_DateTime)
        {
            return CompareDateTimeWithValue(pPropertyValue2, pPropertyValue1, pCompareResult,true);
        }
        else
        {
            // cast will fail when getting values
            return E_UNEXPECTED;
        }
    }

    switch (type1)
    {
    case PropertyType_Empty:
        *pCompareResult = 0;
        break;
    case PropertyType_UInt8:
        {
            BYTE value1;
            BYTE value2;
            IFR_ASSERT(pPropertyValue1->GetUInt8(&value1));
            IFR_ASSERT(pPropertyValue2->GetUInt8(&value2));
            *pCompareResult = value1 - value2;
        }
        break;
    case PropertyType_Int16:
        {
            INT16 value1;
            INT16 value2;
            IFR_ASSERT(pPropertyValue1->GetInt16(&value1));
            IFR_ASSERT(pPropertyValue2->GetInt16(&value2));
            *pCompareResult = value1 - value2;
        }
        break;
    case PropertyType_UInt16:
        {
            UINT16 value1;
            UINT16 value2;
            IFR_ASSERT(pPropertyValue1->GetUInt16(&value1));
            IFR_ASSERT(pPropertyValue2->GetUInt16(&value2));
            *pCompareResult = value1 - value2;
        }
        break;
    case PropertyType_Int32:
        {
            INT32 value1;
            INT32 value2;
            IFR_ASSERT(pPropertyValue1->GetInt32(&value1));
            IFR_ASSERT(pPropertyValue2->GetInt32(&value2));
            *pCompareResult = value1 - value2;
        }
        break;
    case PropertyType_UInt32:
        {
            UINT32 value1;
            UINT32 value2;
            IFR_ASSERT(pPropertyValue1->GetUInt32(&value1));
            IFR_ASSERT(pPropertyValue2->GetUInt32(&value2));
            *pCompareResult = value1 - value2;
        }
        break;
    case PropertyType_Int64:
        {
            INT64 value1;
            INT64 value2;
            IFR_ASSERT(pPropertyValue1->GetInt64(&value1));
            IFR_ASSERT(pPropertyValue2->GetInt64(&value2));
            *pCompareResult = CompareValues(value1,value2);
        }
        break;
    case PropertyType_UInt64:
        {
            UINT64 value1;
            UINT64 value2;
            IFR_ASSERT(pPropertyValue1->GetUInt64(&value1));
            IFR_ASSERT(pPropertyValue2->GetUInt64(&value2));
            *pCompareResult = CompareValues(value1,value2);
        }
        break;
    case PropertyType_Single:
        {
            FLOAT value1;
            FLOAT value2;
            IFR_ASSERT(pPropertyValue1->GetSingle(&value1));
            IFR_ASSERT(pPropertyValue2->GetSingle(&value2));
            *pCompareResult = CompareValues(value1,value2);
        }
        break;
    case PropertyType_Double:
        {
            DOUBLE value1;
            DOUBLE value2;
            IFR_ASSERT(pPropertyValue1->GetDouble(&value1));
            IFR_ASSERT(pPropertyValue2->GetDouble(&value2));
            *pCompareResult = CompareValues(value1,value2);
        }
        break;
    case PropertyType_Char16:
        {
            WCHAR value1;
            WCHAR value2;
            IFR_ASSERT(pPropertyValue1->GetChar16(&value1));
            IFR_ASSERT(pPropertyValue2->GetChar16(&value2));
            *pCompareResult = value1 - value2;
        }
        break;
    case PropertyType_Boolean:
        {
            boolean value1;
            boolean value2;
            IFR_ASSERT(pPropertyValue1->GetBoolean(&value1));
            IFR_ASSERT(pPropertyValue2->GetBoolean(&value2));
            *pCompareResult = value1 - value2;
        }
        break;
    case PropertyType_String:
        {
            foundation::HStringPtr value1;
            foundation::HStringPtr value2;
            IFR_ASSERT(pPropertyValue1->GetString(value1.GetAddressOf()));
            IFR_ASSERT(pPropertyValue2->GetString(value2.GetAddressOf()));
            IFR_ASSERT(_pal_CompareStringOrdinal(value1, value2, pCompareResult));
        }
        break;
    case PropertyType_Inspectable:
        foundation_assert(false);
        break;
    case PropertyType_DateTime:
        {
            foundation::DateTime dt1,dt2;
            IFR_ASSERT(pPropertyValue1->GetDateTime(&dt1));
            IFR_ASSERT(pPropertyValue2->GetDateTime(&dt2));
            *pCompareResult = (int)(dt1.UniversalTime - dt2.UniversalTime);
        }
        break;
    case PropertyType_TimeSpan:
        {
            foundation::TimeSpan t1, t2;
            IFR_ASSERT(pPropertyValue1->GetTimeSpan(&t1));
            IFR_ASSERT(pPropertyValue2->GetTimeSpan(&t2));
            *pCompareResult = (int)(t1.Duration - t2.Duration);
        }
        break;
    case PropertyType_Guid:
        {
            GUID value1;
            GUID value2;
            IFR_ASSERT(pPropertyValue1->GetGuid(&value1));
            IFR_ASSERT(pPropertyValue2->GetGuid(&value2));
            if (value1 == value2)
            {
                *pCompareResult = 0;
            }
            else
            {
                *pCompareResult = foundation::library::GuidLessThan::compare(value1, value2) ? -1 : 1;
            }
        }
        break;
    default:
        return E_NOTIMPL;
    }
    return S_OK;
}

STDMETHODIMP CPropertyValueInternalStatics::CompareInspectableValues(
    _In_opt_ foundation::IInspectable *value1,
    _In_opt_ foundation::IInspectable *value2,
    _Out_ int *pCompareResult)
{
    IFCPTR_ASSERT(pCompareResult);

    *pCompareResult = -1;

    // scalar comparision
    if(value1 == value2)
    {
        *pCompareResult = 0;
        return S_OK;
    }

    // check if one is null and the opther not
    if(value1 == nullptr && value2 != nullptr)
    {
        // comparission == -1
        return S_OK;
    }
    else if(value1 != nullptr && value2 == nullptr)
    {
        *pCompareResult = 1;
        return S_OK;
    }

    PropertyValuePtr spPropertyValue1,spPropertyValue2;
    if(value1)
    {
        IGNOREHR(pv_util::CastToPropertyValue(value1,spPropertyValue1.GetAddressOf()));
    }
    if(value2)
    {
        IGNOREHR(pv_util::CastToPropertyValue(value2,spPropertyValue2.GetAddressOf()));
    }
    // check null conditions
    if(
        (value1 == nullptr && spPropertyValue2 && pv_util::IsValueEmpty(spPropertyValue2))
        ||
        (value2 == nullptr && spPropertyValue1 && pv_util::IsValueEmpty(spPropertyValue1))
        )
    {
        *pCompareResult = 0;
        return S_OK;
    }
    // QI again to foundation::IUnknown inc case the object is aggregated
    foundation::ComPtr<foundation::IUnknown> spUnk1;
    foundation::ComPtr<foundation::IUnknown> spUnk2;
    if(value1)
    {
        IFR_ASSERT(value1->QueryInterface(IID_IUnknown,(void **)&spUnk1));
    }
    if(value2)
    {
        IFR_ASSERT(value2->QueryInterface(IID_IUnknown, (void **)&spUnk2));
    }

    if(spUnk1 == spUnk2)
    {
        *pCompareResult = 0;
        return S_OK;
    }

    if(spPropertyValue1 != nullptr && spPropertyValue2 != nullptr)
    {
        _IFR_(ComparePropertyValues(spPropertyValue1, spPropertyValue2, pCompareResult));
        return S_OK;
    }

    if(spUnk1 != nullptr && spUnk2 != nullptr)
    {
        // compare pointer assignments
        *pCompareResult = (int)(spUnk1.Get() - spUnk2.Get());
    }
    return S_OK;
}

HRESULT CPropertyValueInternalStatics::CreateDefaultValueInternal(
    foundation::PropertyType type,
    /*_COM_Outptr_result_maybenull_*/ foundation::IInspectable **ppValue)
{
    if(type & 0x400)
    {
        // arrays by default will be null
        *ppValue = nullptr;
        return S_OK;
    }

    switch(type)
    {
    case PropertyType_UInt8:
        pv_util::CreateUInt8Value(0,ppValue);
        break;
    case PropertyType_Int16:
        pv_util::CreateInt16Value(0,ppValue);
        break;
    case PropertyType_Int32:
        pv_util::CreateInt32Value(0,ppValue);
        break;
    case PropertyType_Int64:
        pv_util::CreateInt64Value(0,ppValue);
        break;
    case PropertyType_UInt16:
        pv_util::CreateUInt16Value(0,ppValue);
        break;
    case PropertyType_UInt32:
        pv_util::CreateUInt32Value(0,ppValue);
        break;
    case PropertyType_UInt64:
        pv_util::CreateUInt64Value(0,ppValue);
        break;
    case PropertyType_Single:
        pv_util::CreateSingleValue((FLOAT)0.0,ppValue);
        break;
    case PropertyType_Double:
        pv_util::CreateDoubleValue(0.0,ppValue);
        break;
    case PropertyType_Boolean:
        pv_util::CreateBooleanValue(false,ppValue);
        break;
    case PropertyType_String:
        pv_util::CreateStringValue((HSTRING)nullptr,ppValue);
        break;
    case PropertyType_DateTime:
        {
            DateTime dt;
            dt.UniversalTime = 0;
            pv_util::CreateDateTimeValue(dt,ppValue);
        }
        break;
    case PropertyType_Guid:
        pv_util::CreateGuidValue(GUID_NULL, ppValue);
        break;
    case PropertyType_Inspectable:
        *ppValue = nullptr;
        break;
    default:
        // TODO: better error code when not supported
        return E_UNEXPECTED;
    }
    return S_OK;
}

template 
<
    typename TArrayWrapper,
    typename T,
    HRESULT  (STDMETHODCALLTYPE foundation::IPropertyValue::*pGetValue)(T*),
    HRESULT  (STDMETHODCALLTYPE foundation::IPropertyValueStatics::*pCreateArray)(UINT32 ,T*,foundation::IInspectable **)
>
static HRESULT CreateCastArray(
    UINT32 size,
    foundation::IInspectable **ppValues,
    foundation::IInspectable **ppArrayValue)
{
    TArrayWrapper arrayWrapper;
    T *pBuffer = (T*)_pal_MemAlloc(size*sizeof(T));
    // feed the array wrapper
    *arrayWrapper.GetBufferAddressOf() = pBuffer;
    *arrayWrapper.GetSizeAddressOf() = size;
    for(UINT32 index = 0;index < size;++index)
    {
        PropertyValuePtr spPropertyValue;
        IFR_ASSERT(pv_util::CastToPropertyValue(ppValues[index],spPropertyValue.GetAddressOf()));
        IFR_ASSERT((spPropertyValue->*pGetValue)(pBuffer));
        ++pBuffer;
    }
    return (pv_util::GetPropertyValueStatics()->*pCreateArray)(
        arrayWrapper.GetSize(),
        (T *)arrayWrapper.GetBuffer(),
        ppArrayValue);
}

template<typename TArrayWrapper>
static HRESULT CreateObjectArray(
    foundation::IInspectable *p_value,
    foundation::IInspectable **ppArrayValue)
{
    TArrayWrapper arrayWrapper;
    IFR_ASSERT(pv_util::GetValue(p_value,
        arrayWrapper.GetSizeAddressOf(),
        arrayWrapper.GetBufferAddressOf()));

    std::vector<foundation::InspectablePtr> object_array;
    object_array.resize(arrayWrapper.GetSize());

    for (UINT32 index = 0; index < arrayWrapper.GetSize(); ++index)
    {
        object_array[index] = pv_util::CreateValue(arrayWrapper[index]);
    }
    return pv_util::CreateInspectableArrayValue(
        (UINT32)object_array.size(),
        object_array.size() ? (foundation::IInspectable **)object_array.front().GetAddressOfPtr(): nullptr,
        ppArrayValue
        );
}

static HRESULT CreateInspectableObjectArray(
    foundation::PropertyType propertyType,
    foundation::IInspectable *pValue,
    foundation::IInspectable **ppValue)
{
    switch (propertyType)
    {
    case PropertyType_Int16:
        return CreateObjectArray<foundation::Int16ArrayWrapper>(pValue, ppValue);
    case PropertyType_UInt16:
        return CreateObjectArray<foundation::UInt16ArrayWrapper>(pValue, ppValue);
    case PropertyType_Int32:
        return CreateObjectArray<foundation::Int32ArrayWrapper>(pValue, ppValue);
    case PropertyType_UInt32:
        return CreateObjectArray<foundation::UInt32ArrayWrapper>(pValue, ppValue);
    case PropertyType_Int64:
        return CreateObjectArray<foundation::Int64ArrayWrapper>(pValue, ppValue);
    case PropertyType_UInt64:
        return CreateObjectArray<foundation::UInt64ArrayWrapper>(pValue, ppValue);
    case PropertyType_String:
        return CreateObjectArray<foundation::HStringArrayWrapper>(pValue, ppValue);
    case PropertyType_Boolean:
        return CreateObjectArray<foundation::BooleanArrayWrapper>(pValue, ppValue);
    default:
        foundation_assert(false);
        return E_NOTIMPL;
    }
    return S_OK;
}

HRESULT CPropertyValueInternalStatics::CreateArrayFromInspectableArray(
    foundation::PropertyType propertyType,
    _In_  foundation::IPropertyValue *pValue,
    foundation::IInspectable **ppValue)
{
    foundation::InspectableArrayWrapper objectArrayWrapper;
    IFR_ASSERT(pValue->GetInspectableArray(
        objectArrayWrapper.GetSizeAddressOf(),
        objectArrayWrapper.GetBufferAddressOf()));

    foundation::IInspectable **pBuffer = (foundation::IInspectable **)objectArrayWrapper.GetBuffer();
    for (UINT32 index = 0; index < objectArrayWrapper.GetSize(); ++index)
    {
        IFR_ASSERT(ChangeTypeInternal(propertyType, pBuffer));
        ++pBuffer;
    }
    // move to first item
    pBuffer = (foundation::IInspectable **)objectArrayWrapper.GetBuffer();
    switch (propertyType)
    {
    case PropertyType_UInt8:
        return CreateCastArray<
            foundation::UInt8ArrayWrapper,
            BYTE,
            &foundation::IPropertyValue::GetUInt8,
            &foundation::IPropertyValueStatics::CreateUInt8Array
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_Int16:
        return CreateCastArray<
            foundation::Int16ArrayWrapper,
            INT16,
            &foundation::IPropertyValue::GetInt16,
            &foundation::IPropertyValueStatics::CreateInt16Array
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_UInt16:
        return CreateCastArray<
            foundation::UInt16ArrayWrapper,
            UINT16,
            &foundation::IPropertyValue::GetUInt16,
            &foundation::IPropertyValueStatics::CreateUInt16Array
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_Int32:
        return CreateCastArray<
            foundation::Int32ArrayWrapper,
            INT32,
            &foundation::IPropertyValue::GetInt32,
            &foundation::IPropertyValueStatics::CreateInt32Array
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_UInt32:
        return CreateCastArray<
            foundation::UInt32ArrayWrapper,
            UINT32,
            &foundation::IPropertyValue::GetUInt32,
            &foundation::IPropertyValueStatics::CreateUInt32Array
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_Int64:
        return CreateCastArray<
            foundation::Int64ArrayWrapper,
            INT64,
            &foundation::IPropertyValue::GetInt64,
            &foundation::IPropertyValueStatics::CreateInt64Array
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_UInt64:
        return CreateCastArray<
            foundation::UInt64ArrayWrapper,
            UINT64,
            &foundation::IPropertyValue::GetUInt64,
            &foundation::IPropertyValueStatics::CreateUInt64Array
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_Single:
        return CreateCastArray<
            foundation::SingleArrayWrapper,
            FLOAT,
            &foundation::IPropertyValue::GetSingle,
            &foundation::IPropertyValueStatics::CreateSingleArray
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_Double:
        return CreateCastArray<
            foundation::DoubleArrayWrapper,
            DOUBLE,
            &foundation::IPropertyValue::GetDouble,
            &foundation::IPropertyValueStatics::CreateDoubleArray
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_Boolean:
        return CreateCastArray<
            foundation::BooleanArrayWrapper,
            boolean,
            &foundation::IPropertyValue::GetBoolean,
            &foundation::IPropertyValueStatics::CreateBooleanArray
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    case PropertyType_String:
        return CreateCastArray<
            foundation::HStringArrayWrapper,
            HSTRING,
            &foundation::IPropertyValue::GetString,
            &foundation::IPropertyValueStatics::CreateStringArray
        >(objectArrayWrapper.GetSize(), pBuffer, ppValue);
        break;
    default:
        break;
    }
    foundation_assert(false);
    return E_UNEXPECTED;
}

HRESULT CPropertyValueInternalStatics::CastToArray(
        _In_    foundation::PropertyType  targetArrayType,
        _In_    foundation::IPropertyValue *pValue,
        _Inout_ foundation::IInspectable **ppValue)
{
    foundation_assert((targetArrayType & 0x400) != 0);

    foundation::PropertyType propertyType;
    IFR_ASSERT(pValue->get_Type(&propertyType));

    if( (propertyType & 0x400) == 0)
    {
        return foundation::E_ARRAY_EXPECTED;
    }

    foundation::PropertyType sourceItemType = (foundation::PropertyType)(propertyType & 0xff);
    foundation::PropertyType itemType = (foundation::PropertyType)(targetArrayType & 0xff);

    if(propertyType == PropertyType_InspectableArray)
    {
        return CreateArrayFromInspectableArray(itemType, pValue, ppValue);
    }
    else if (targetArrayType == PropertyType_InspectableArray)
    {
        return CreateInspectableObjectArray(
            sourceItemType,
            pValue, 
            ppValue);
    }
    else
    {
        // attempt conversion to Object Array
        foundation::InspectablePtr object_array_ptr;
        _IFR_(CreateInspectableObjectArray(
            sourceItemType,
            pValue,
            object_array_ptr.GetAddressOf()));

        foundation::PropertyValuePtr PropertyValuePtr;
        IFR_ASSERT(pv_util::CastToPropertyValue(object_array_ptr, PropertyValuePtr.GetAddressOf()));
        return CreateArrayFromInspectableArray(itemType, PropertyValuePtr.Get(), ppValue);
    }
}

HRESULT CPropertyValueInternalStatics::ChangeTypeInternal(
    _In_    foundation::PropertyType type,
    _Inout_ foundation::IInspectable **ppValue
    )
{
    foundation_assert(ppValue);
    // for null values we will return a 'default' value
    if(*ppValue == nullptr)
    {
        return CreateDefaultValueInternal(type, ppValue);
    }

    foundation::ComPtr<foundation::IPropertyValue> spPropertyValue;
    IFR_ASSERT(foundation::QueryInterface(*ppValue,foundation_IID_IPropertyValue,spPropertyValue.GetAddressOf()));

    foundation::PropertyType valuePropertyType;
    IFR_ASSERT(spPropertyValue->get_Type(&valuePropertyType));

    if(type == valuePropertyType)
    {
        return S_OK;
    }
    foundation::IInspectable *pValue = *ppValue;

    if(type & 0x400)
    {
        foundation::IInspectable *pArrayValue = nullptr;
        // we need array cast
        _IFR_(CastToArray(type, spPropertyValue, &pArrayValue));
        *ppValue = pArrayValue;
    }
    else if (valuePropertyType == PropertyType_Boolean)
    {
        boolean boolValue;
        spPropertyValue->GetBoolean(&boolValue);
        switch (type)
        {
        case PropertyType_UInt8:
            IFR_ASSERT(pv_util::CreateUInt8Value(boolValue ? 1:0, ppValue));
            break;
        case PropertyType_Int16:
            IFR_ASSERT(pv_util::CreateInt16Value(boolValue ? 1 : 0, ppValue));
            break;
        case PropertyType_UInt16:
            IFR_ASSERT(pv_util::CreateUInt16Value(boolValue ? 1 : 0, ppValue));
            break;
        case PropertyType_Int32:
            IFR_ASSERT(pv_util::CreateInt32Value(boolValue ? 1 : 0, ppValue));
            break;
        case PropertyType_UInt32:
            IFR_ASSERT(pv_util::CreateUInt32Value(boolValue ? 1 : 0, ppValue));
            break;
        case PropertyType_Int64:
            IFR_ASSERT(pv_util::CreateInt64Value(boolValue ? 1 : 0, ppValue));
            break;
        case PropertyType_UInt64:
            IFR_ASSERT(pv_util::CreateUInt64Value(boolValue ? 1 : 0, ppValue));
            break;
        case PropertyType_String:
            IFR_ASSERT(pv_util::CreateStringValue(boolValue ? U("true") : U("false"), ppValue));
            break;
        default:
            return E_WRONG_TYPE;
        }
    }
    else
    {
    switch (type)
    {
    case PropertyType_UInt8:
        {
            BYTE value;
            _IFR_(spPropertyValue->GetUInt8(&value));
            IFR_ASSERT(pv_util::CreateUInt8Value(value,ppValue));
        }
        break;
    case PropertyType_Int16:
        {
            INT16 value;
            _IFR_(spPropertyValue->GetInt16(&value));
            IFR_ASSERT(pv_util::CreateInt16Value(value,ppValue));
        }
        break;
    case PropertyType_UInt16:
        {
            UINT16 value;
            _IFR_(spPropertyValue->GetUInt16(&value));
            IFR_ASSERT(pv_util::CreateUInt16Value(value,ppValue));
        }
        break;
    case PropertyType_Int32:
        {
            INT32 value;
            if (valuePropertyType == PropertyType_String)
            {
                foundation::HStringPtr hstr;
                _IFR_(spPropertyValue->GetString(hstr.GetAddressOf()));
                _IFR_(StringToInt(hstr, value));
            }
            else
            {
                _IFR_(spPropertyValue->GetInt32(&value));
            }
            IFR_ASSERT(pv_util::CreateInt32Value(value,ppValue));
        }
        break;
    case PropertyType_UInt32:
        {
            UINT32 value;
            if (valuePropertyType == PropertyType_String)
            {
                foundation::HStringPtr hstr;
                _IFR_(spPropertyValue->GetString(hstr.GetAddressOf()));
                _IFR_(StringToUInt(hstr, value));
            }
            else
            {
                _IFR_(spPropertyValue->GetUInt32(&value));
            }
            IFR_ASSERT(pv_util::CreateUInt32Value(value,ppValue));
        }
        break;
    case PropertyType_Int64:
        {
            INT64 value;
            _IFR_(spPropertyValue->GetInt64(&value));
            IFR_ASSERT(pv_util::CreateInt64Value(value,ppValue));
        }
        break;
    case PropertyType_UInt64:
        {
            UINT64 value;
            _IFR_(spPropertyValue->GetUInt64(&value));
            IFR_ASSERT(pv_util::CreateUInt64Value(value,ppValue));
        }
        break;
    case PropertyType_Single:
        {
            FLOAT value;
            if (valuePropertyType == PropertyType_Double)
            {
                DOUBLE doubleValue;
                _IFR_(spPropertyValue->GetDouble(&doubleValue));
                value = static_cast<FLOAT>(doubleValue);
            }
            else if (
                valuePropertyType == PropertyType_UInt8 ||
                valuePropertyType == PropertyType_UInt16 ||
                valuePropertyType == PropertyType_UInt32 ||
                valuePropertyType == PropertyType_UInt64)
            {
                UINT64 uint64;
                _IFR_(spPropertyValue->GetUInt64(&uint64));
                value = static_cast<FLOAT>(uint64);
            }
            else if (
                valuePropertyType == PropertyType_Int16 ||
                valuePropertyType == PropertyType_Int32 ||
                valuePropertyType == PropertyType_Int64)
            {
                INT64 int64;
                _IFR_(spPropertyValue->GetInt64(&int64));
                value = static_cast<FLOAT>(int64);
            }
            else
            {
                _IFR_(spPropertyValue->GetSingle(&value));
            }
            IFR_ASSERT(pv_util::CreateSingleValue(value,ppValue));
        }
        break;
    case PropertyType_Double:
        {
            DOUBLE value;
            _IFR_(spPropertyValue->GetDouble(&value));
            IFR_ASSERT(pv_util::CreateDoubleValue(value,ppValue));
        }
        break;
    case PropertyType_Char16:
        {
            WCHAR value;
            _IFR_(spPropertyValue->GetChar16(&value));
            IFR_ASSERT(pv_util::CreateChar16Value(value,ppValue));
        }
        break;
    case PropertyType_Boolean:
        {
            if (valuePropertyType == PropertyType_String)
            {
                foundation::HStringPtr value;
                spPropertyValue->GetString(value.GetAddressOf());

                if (_pal_strcmp(value.GetRawBuffer(), U("true")) == 0)
                {
                    IFR_ASSERT(pv_util::CreateBooleanValue(true, ppValue));
                }
                else if (_pal_strcmp(value.GetRawBuffer(), U("false")) == 0)
                {
                    IFR_ASSERT(pv_util::CreateBooleanValue(false, ppValue));
                }
                else
                {
                    return E_WRONG_TYPE;
                }
            }
            else
            {
                UINT8 value;
                _IFR_(spPropertyValue->GetUInt8(&value));
                IFR_ASSERT(pv_util::CreateBooleanValue(value ? true : false, ppValue));
            }
        }
        break;
    case PropertyType_String:
    case PropertyType_Guid:
        {
            if (type == PropertyType_Guid)
            {
                foundation::HStringPtr value;
                _IFR_(spPropertyValue->GetString(value.GetAddressOf()));

                IID iid;
                if (!::FromString(foundation::to_utf8_string(value).c_str(), iid))
                {
                    _IFR_(E_WRONG_TYPE);
                }
                IFR_ASSERT(pv_util::CreateGuidValue(iid, ppValue));
            }
            else // target is String
            {
                foundation::string_t string_value = ToString(spPropertyValue);
                IFR_ASSERT(pv_util::CreateStringValue(string_value.c_str(), ppValue));
            }
        }
        break;
    case PropertyType_DateTime:
        {
            foundation::HStringPtr value;
            _IFR_(spPropertyValue->GetString(value.GetAddressOf()));

            SystemTime systemTime;
            _IFR_(StringToSystmeTime(_pal_GetStringRawBuffer(value, nullptr), systemTime));

            DateTime dateTime;
            _IFR_(_pal_SystemTimeToDateTime(systemTime, &dateTime));

            IFR_ASSERT(pv_util::CreateDateTimeValue(dateTime,ppValue));
        }
        break;
    case PropertyType_Inspectable:
        {
            if(valuePropertyType == foundation::PropertyType_Empty)
            {
                // convert Empt type to a null value
                *ppValue = nullptr;
            }
            else
            {
                return E_UNEXPECTED;
            }
        }
        break;
    case PropertyType_TimeSpan:
    default:
        return E_NOTIMPL;
    }
    }
    // if we arrive here a new values is being returned in ppValue
    // so we Release the old value reference
    pValue->Release();
    return S_OK;
}

STDMETHODIMP CPropertyValueInternalStatics::ChangeType(
    _In_    foundation::PropertyType type,
    _Inout_ foundation::IInspectable **ppValue
    )
{
    return ChangeTypeInternal(type,ppValue);
}

STDMETHODIMP CPropertyValueInternalStatics::ToString(
    _In_ IPropertyValue *pPropertyValue,
    HSTRING *hstring)
{
    IFCPTR_ASSERT(pPropertyValue);
    IFCPTR_ASSERT(hstring);
    foundation::string_t str = ToString(pPropertyValue);
    return _pal_CreateString(str.c_str(),(UINT32)str.size(),hstring);
}

STDMETHODIMP CPropertyValueInternalStatics::Serialize(
    _In_ IPropertyValue *pPropertyValue,
    _Out_ UINT32 *size,
    _Outptr_ UINT8 **data)
{
    IFCPTR_ASSERT(pPropertyValue);
    IFCPTR_ASSERT(size);
    IFCPTR_ASSERT(data);

    std::stringstream os;
    IFR_ASSERT(Serialize(os,pPropertyValue));
    IFR_ASSERT(StringStreamToArray(os, size, data));
    return S_OK;
}

STDMETHODIMP CPropertyValueInternalStatics::Deserialize(
    _In_ UINT32 size,
    _In_ UINT8 *data,
    _COM_Outptr_ IPropertyValue **ppPropertyValue)
{
    std::string buffer((const char *)data,size);
    std::stringstream is(buffer);

    IFR_ASSERT(Deserialize(is, ppPropertyValue));
    return S_OK;
}

STDMETHODIMP CPropertyValueInternalStatics::CreateDefaultValue(
    foundation::PropertyType type,
    foundation::IInspectable **ppValue)
{
    return CPropertyValueInternalStatics::CreateDefaultValueInternal(type, ppValue);
}

EXTERN_C FOUNDATION_API HRESULT  STDAPICALLTYPE GetPropertyValueStatics(
    _COM_Outptr_ foundation::IPropertyValueStatics **ppPropertyValueStatics);


foundation::IPropertyValueStatics *GetPropertyValueStaticsCallback()
{
    return library::_GetFactoryInstance<foundation::IPropertyValueStatics>([]() {
        foundation::IPropertyValueStatics *pPropertyValueStatics = nullptr;
        IF_ASSERT(::GetPropertyValueStatics(&pPropertyValueStatics));
        return pPropertyValueStatics;
    });
}

static _ActivationFactoryEntry activationEntries[] = {
    {
        U("foundation.PropertyValueStatics"),
        reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&GetPropertyValueStaticsCallback)
    },
    {
        U("foundation.PropertyValueStaticsInternal"),
        reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CPropertyValueInternalStatics::GetInstance)
    },
    {
        nullptr,
        nullptr,
    }
};

static _RegisterActivationFactory _propertyValueStaticsRegister(activationEntries);
