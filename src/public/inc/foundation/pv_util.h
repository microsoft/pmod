/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pv_util.h
****/
#pragma once

#include <foundation/assert.h>
#include <foundation/errors.h>
#include <foundation/activation_factory.h>

#if _HAS_CPP0X
#include <type_traits>
#endif

#include <foundation/interfaces/property_value.h>
#include <foundation/library/interfaces/property_value_statics.h>

#include <foundation/hstring_wrapper.h>
#include <foundation/array_wrapper.h>
#include <foundation/value_alloc.h>

#include "com_ptr.h"
#include "macros.h"

namespace foundation
{

    template <class T>
    inline PropertyType GetPropertyType();

    template <>
    inline PropertyType GetPropertyType<void>()
    {
        return PropertyType_Empty;
    }
    template <>
    inline PropertyType GetPropertyType<UINT8>()
    {
        return PropertyType_UInt8;
    }
    template <>
    inline PropertyType GetPropertyType<INT16>()
    {
        return PropertyType_Int16;
    }
    template <>
    inline PropertyType GetPropertyType<UINT16>()
    {
        return PropertyType_UInt16;
    }
    template <>
    inline PropertyType GetPropertyType<INT32>()
    {
        return PropertyType_Int32;
    }
    template <>
    inline PropertyType GetPropertyType<UINT32>()
    {
        return PropertyType_UInt32;
    }
    template <>
    inline PropertyType GetPropertyType<INT64>()
    {
        return PropertyType_Int64;
    }
    template <>
    inline PropertyType GetPropertyType<UINT64>()
    {
        return PropertyType_UInt64;
    }
    template <>
    inline PropertyType GetPropertyType<FLOAT>()
    {
        return PropertyType_Single;
    }
    template <>
    inline PropertyType GetPropertyType<DOUBLE>()
    {
        return PropertyType_Double;
    }
    template <>
    inline PropertyType GetPropertyType<WCHAR>()
    {
        return PropertyType_Char16;
    }
    template <>
    inline PropertyType GetPropertyType<bool>()
    {
        return PropertyType_Boolean;
    }
    template <>
    inline PropertyType GetPropertyType<HSTRING>()
    {
        return PropertyType_String;
    }
    template <>
    inline PropertyType GetPropertyType<HStringPtr>()
    {
        return PropertyType_String;
    }
    template <>
    inline PropertyType GetPropertyType<DateTime>()
    {
        return PropertyType_DateTime;
    }
    template <>
    inline PropertyType GetPropertyType<TimeSpan>()
    {
        return PropertyType_TimeSpan;
    }
    template <>
    inline PropertyType GetPropertyType<SystemTime>()
    {
        return PropertyType_DateTime;
    }
    template <>
    inline PropertyType GetPropertyType<GUID>()
    {
        return PropertyType_Guid;
    }
    template <>
    inline PropertyType GetPropertyType<IInspectable *>()
    {
        return PropertyType_Inspectable;
    }

    template <>
    inline PropertyType GetPropertyType<UInt32ArrayWrapper>()
    {
        return PropertyType_UInt32Array;
    }

    template <>
    inline PropertyType GetPropertyType<HStringArrayWrapper>()
    {
        return PropertyType_StringArray;
    }




typedef ComPtr<foundation::IPropertyValue> PropertyValuePtr;

class pv_util
{
public:
    static foundation::IPropertyValueStatics *GetPropertyValueStatics()
    {
        return GetActivationFactoryWithCast<foundation::IPropertyValueStatics>(U("foundation.PropertyValueStatics"));
    }

    static HRESULT CastToPropertyValue(
        _In_ foundation::IInspectable *pValue,
        _COM_Outptr_ foundation::IPropertyValue **ppPropertyValue)
    {
        foundation_assert(pValue);
        foundation_assert(ppPropertyValue);

        return pValue->QueryInterface(foundation_IID_IPropertyValue, (void**)ppPropertyValue);
    }

    static PropertyValuePtr ToPropertyValuePtr(_In_ foundation::IInspectable *pValue)
    {
        PropertyValuePtr propertyvalue_ptr;
        HRESULT hr = CastToPropertyValue(pValue, propertyvalue_ptr.GetAddressOf());
        foundation_assert(SUCCEEDED(hr));
        return propertyvalue_ptr;
    }

    static int CompareInspectableValues(
        _In_opt_ foundation::IInspectable *value1, 
        _In_opt_ foundation::IInspectable *value2)
    {
        int result;
        IGNOREHR(GetPropertyValueStaticsInternal()->CompareInspectableValues(value1, value2, &result));
        return result;
    }

    static int CompareInspectableValues(
        _In_opt_ foundation::IInspectable *value1, 
        _In_opt_ foundation::IInspectable *value2, 
        _In_opt_ void *Context)
    {
        UNREFERENCED_PARAMETER(Context);
        return CompareInspectableValues(value1, value2);
    }


    static HRESULT ToString(_In_ foundation::IPropertyValue *pPropertyValue, _Outptr_result_maybenull_ HSTRING *hstring)
    {
        return GetPropertyValueStaticsInternal()->ToString(pPropertyValue, hstring);
    }

    static HRESULT ToString(_In_ foundation::IInspectable *pValue, _Outptr_result_maybenull_ HSTRING *hstring)
    {
        PropertyValuePtr propertyvalue_ptr;
        IFR_ASSERT(CastToPropertyValue(pValue, propertyvalue_ptr.GetAddressOf()));
        return ToString(propertyvalue_ptr, hstring);
    }

    static HRESULT ToString(GUID& iid, _Outptr_result_maybenull_ HSTRING *hstring)
    {
        InspectablePtr value;
        IFR_ASSERT(CreateGuidValue(iid, value.GetAddressOf()));
        IFR_ASSERT(ChangeType(PropertyType::PropertyType_String, value.GetAddressOfPtr()));
        return GetValue(value, hstring);
    }

    static HRESULT ToGuid(HSTRING hstr,IID& iid)
    {
        InspectablePtr value;
        IFR_ASSERT(CreateStringValue(hstr, value.GetAddressOf()));
        IFR_ASSERT(ChangeType(PropertyType::PropertyType_Guid, value.GetAddressOfPtr()));
        return GetValue(value, &iid);
    }

    static HRESULT ToString(const SystemTime& dt, _Outptr_result_maybenull_ HSTRING *hstring)
    {
        InspectablePtr value;
        IFR_ASSERT(CreateSystemTimeValue(dt, value.GetAddressOf()));
        IFR_ASSERT(ChangeType(PropertyType::PropertyType_String, value.GetAddressOfPtr()));
        return GetValue(value, hstring);
    }

    static HRESULT Serialize(
        _In_ foundation::IPropertyValue *pPropertyValue,
        _Out_ UINT32 *size,
        _Outptr_ UINT8 **data)
    {
        return GetPropertyValueStaticsInternal()->Serialize(pPropertyValue, size, data);
    }

    static HRESULT Deserialize(
        _In_ UINT32 size,
        _In_reads_(size) UINT8 *data,
        _COM_Outptr_ foundation::IPropertyValue **ppPropertyValue)
    {
        return GetPropertyValueStaticsInternal()->Deserialize(size, data, ppPropertyValue);
    }

    static HRESULT ChangeType(
        _In_  foundation::PropertyType type, 
        _Inout_ foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStaticsInternal()->ChangeType(type, ppValue);
    }

    static int NonCompareInspectableValues(
        _In_opt_ foundation::IInspectable *value1, 
        _In_opt_ foundation::IInspectable *value2)
    {
        return -CompareInspectableValues(value1, value2);
    }

    static int NonCompareInspectableValues(
        _In_opt_ foundation::IInspectable *value1, 
        _In_opt_ foundation::IInspectable *value2,
        _In_opt_ void *Context)
    {
        return -CompareInspectableValues(value1, value2, Context);
    }

    static HRESULT GetType(
        _In_ foundation::IInspectable *value, 
        _Always_(_Out_) foundation::PropertyType *pType)
    {
        foundation_assert(value);
        foundation_assert(pType);

        PropertyValuePtr propertyvalue_ptr;
        if (SUCCEEDED(CastToPropertyValue(value, propertyvalue_ptr.GetAddressOf())))
        {
            IFR_ASSERT(propertyvalue_ptr->get_Type(pType));
        }
        else
        {
            // not a property value
            *pType = foundation::PropertyType_Inspectable;
        }

        return S_OK;
    }

    static HRESULT VerifyType(
        _In_ foundation::IInspectable *value, 
        _In_ foundation::PropertyType type)
    {
        foundation::PropertyType valueType;
        IFR_ASSERT(GetType(value, &valueType));
        return valueType == type ? S_OK: E_FAIL;
    }

    static bool IsValueEmpty(_In_opt_ foundation::IInspectable *pValue)
    {
        foundation::PropertyType propertyType = foundation::PropertyType_Empty;
        if (pValue != nullptr)
        {
            GetType(pValue, &propertyType);
        }

        return propertyType == foundation::PropertyType_Empty;
    }

    static bool IsValueEmpty(_In_ foundation::IPropertyValue *pPropertyValue)
    {
        foundation::PropertyType type = foundation::PropertyType_Empty;
        if (pPropertyValue != nullptr)
        {
            pPropertyValue->get_Type(&type);
        }

        return type == foundation::PropertyType_Empty;
    }

    static bool IsArrayType(foundation::PropertyType type)
    {
        return (type & 0x400) != 0;
    }

    static bool IsValueArray(_In_opt_ foundation::IInspectable *pValue)
    {
        foundation::PropertyType propertyType = foundation::PropertyType_Empty;
        if (pValue != nullptr)
        {
            GetType(pValue, &propertyType);
        }

        return IsArrayType(propertyType);
    }

    static InspectablePtr CreateValue(_In_opt_ foundation::IInspectable *pValue)
    {
        return InspectablePtr(pValue);
    }

    static InspectablePtr CreateValue()
    {
        InspectablePtr spEmpty;
        CreateEmptyValue(spEmpty.GetAddressOf());
        return spEmpty;
    }

    static InspectablePtr CreateValue(BYTE value)
    {
        return CreateValueInternal<BYTE, CreateUInt8Value>(value);
    }
    static InspectablePtr CreateValue(INT16 value)
    {
        return CreateValueInternal<INT16, CreateInt16Value>(value);
    }
    static InspectablePtr CreateValue(UINT16 value)
    {
        return CreateValueInternal<UINT16, CreateUInt16Value>(value);
    }
    static InspectablePtr CreateValue(INT32 value)
    {
        return CreateValueInternal<INT32, CreateInt32Value>(value);
    }
    static InspectablePtr CreateValue(UINT32 value)
    {
        return CreateValueInternal<UINT32, CreateUInt32Value>(value);
    }
    static InspectablePtr CreateValue(INT64 value)
    {
        return CreateValueInternal<INT64, CreateInt64Value>(value);
    }
    static InspectablePtr CreateValue(UINT64 value)
    {
        return CreateValueInternal<UINT64, CreateUInt64Value>(value);
    }
    static InspectablePtr CreateValue(bool value)
    {
        return CreateValueInternal<boolean, CreateBooleanValue>(value);
    }
    static InspectablePtr CreateValue(LPCSTR_t value)
    {
        foundation::HStringRef hstring(value);
        return CreateValue((HSTRING)hstring);
    }

#ifdef __UTF16_STRINGS
	static InspectablePtr CreateValue(const char* value)
	{
	return CreateValue(foundation::HStringPtr(value));
	}
#endif

    static InspectablePtr CreateValue(HSTRING value)
    {
        return CreateValueInternal<HSTRING, CreateStringValue>(value);
    }
    static InspectablePtr CreateValue(DOUBLE value)
    {
        return CreateValueInternal<DOUBLE, CreateDoubleValue>(value);
    }
    static InspectablePtr CreateValue(FLOAT value)
    {
        return CreateValueInternal<FLOAT, CreateSingleValue>(value);
    }
    static InspectablePtr CreateValue(GUID value)
    {
        return CreateValueInternal<GUID, CreateGuidValue>(value);
    }
    static InspectablePtr CreateValue(foundation::DateTime value)
    {
        return CreateValueInternal<foundation::DateTime, CreateDateTimeValue>(value);
    }
    static InspectablePtr CreateValue(const foundation::SystemTime& value)
    {
        foundation::DateTime dateTime;
        _pal_SystemTimeToDateTime(value, &dateTime);
        return CreateValue(dateTime);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) BYTE* values)
    {
        return CreateValueArrayInternal<BYTE, CreateUInt8ArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) INT16* values)
    {
        return CreateValueArrayInternal<INT16, CreateInt16ArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) UINT16* values)
    {
        return CreateValueArrayInternal<UINT16, CreateUInt16ArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) INT32* values)
    {
        return CreateValueArrayInternal<INT32, CreateInt32ArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) UINT32* values)
    {
        return CreateValueArrayInternal<UINT32, CreateUInt32ArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) INT64* values)
    {
        return CreateValueArrayInternal<INT64, CreateInt64ArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) UINT64* values)
    {
        return CreateValueArrayInternal<UINT64, CreateUInt64ArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) FLOAT* values)
    {
        return CreateValueArrayInternal<FLOAT, CreateSingleArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) DOUBLE* values)
    {
        return CreateValueArrayInternal<DOUBLE, CreateDoubleArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) WCHAR* values)
    {
        return CreateValueArrayInternal<WCHAR, CreateChar16ArrayValue>(size, values);
    }

    template <class T>
    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) T** ppValues)
    {
        InspectablePtr spValue;
        CreateInspectableArrayValue(size, (foundation::IInspectable **)ppValues, spValue.GetAddressOf());
        foundation_assert(spValue != nullptr);
        return spValue;
    }

    ///
    /// @brief box an array of Enum values into a foundation UINT32 array
    ///
    template <class T>
    static InspectablePtr CreateValue(
        UINT32 size,
        _In_reads_opt_(size) const T* values
#if _HAS_CPP0X
        , typename std::enable_if<std::is_enum<T>::value>::type* = nullptr
#endif
        )
    {
        PalBuffer<UINT32> boxed_enum_values_buffer(size);
        if (values != nullptr)
        {
            for (UINT32 index = 0; index < size; ++index)
            {
                *(boxed_enum_values_buffer.get() + index) = (UINT32)values[index];
            }
        }
        return pv_util::CreateValue(size, boxed_enum_values_buffer.get());
    }

#if !defined(_WINDOWS)
    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) boolean* values)
    {
        return CreateValueArrayInternal<boolean, CreateBooleanArrayValue>(size, values);
    }
#else
    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) bool* values)
    {
        PalBuffer<boolean> boolean_buffer(size);
        if (values != nullptr)
        {
            for (UINT32 index = 0; index < size; ++index)
            {
                *(boolean_buffer.get() + index) = values[index];
            }
        }
        InspectablePtr valuePtr;
        CreateBooleanArrayValue(size, boolean_buffer.get(), valuePtr.GetAddressOf());
        return valuePtr;
    }
#endif

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) HSTRING* values)
    {
        return CreateValueArrayInternal<HSTRING, CreateStringArrayValue>(size, values);
    }

    static InspectablePtr CreateValue(UINT32 size, _In_reads_opt_(size) HStringPtr* values)
    {
        return CreateValue(size, (HSTRING*)values);
    }

    static HRESULT GetValue(
        _In_opt_ foundation::IInspectable *pInspectable, 
        _COM_Outptr_result_maybenull_ foundation::IInspectable **ppValue)
    {
        foundation_assert(ppValue);
        *ppValue = nullptr;

        if (pInspectable != nullptr)
        {
            *ppValue = pInspectable;
            (*ppValue)->AddRef();
        }

        return S_OK;
    }

    template <class T>
    static HRESULT GetValue(
        _In_opt_ foundation::IInspectable *pInspectable, 
        _COM_Outptr_result_maybenull_ T **ppValue)
    {
        foundation_assert(ppValue);
        *ppValue = nullptr;

        if (pInspectable != nullptr)
        {
            IFR_ASSERT(QueryInterface(pInspectable, ppValue));
        }

        return S_OK;
    }

    template <class T>
    static HRESULT GetValue(
        _In_opt_ foundation::IInspectable *pInspectable, 
        _Out_ T* pValue)
    {
        foundation_assert(pValue);
        *pValue = (T)-1;

        if (pInspectable != nullptr)
        {
            UINT32 enumValue;
            IFR_ASSERT(GetUInt32Value(pInspectable, &enumValue));
            *pValue = (T)enumValue;
        }

        return S_OK;
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ BYTE* value)
    {
        return GetUInt8Value(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ INT16 *value)
    {
        return GetInt16Value(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT16* value)
    {
        return GetUInt16Value(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ INT32* value)
    {
        return GetInt32Value(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32* value)
    {
        return GetUInt32Value(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ INT64* value)
    {
        return GetInt64Value(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT64* value)
    {
        return GetUInt64Value(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ bool* value)
    {
        boolean booleanValue;
        IFR_ASSERT(GetBooleanValue(pInspectable, &booleanValue));
        * value = booleanValue ? true:false;
        return S_OK;
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Outptr_result_maybenull_ HSTRING* value)
    {
        return GetStringValue(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ HStringPtr* value)
    {
        IFCPTR_ASSERT(value);
        return GetStringValue(pInspectable, value->ReleaseAndGetAddressOf());
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ DOUBLE* value)
    {
        return GetDoubleValue(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ FLOAT* value)
    {
        return GetSingleValue(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ IID* value)
    {
        return GetGuidValue(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ foundation::DateTime* value)
    {
        return GetDateTimeValue(pInspectable, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ foundation::SystemTime* value)
    {
        foundation::DateTime dateTime;
        _IFR_(GetValue(pInspectable, &dateTime));
        return _pal_DateTimeToSystemTime(dateTime, value);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ BYTE **ppValues)
    {
        return GetUInt8Array(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ INT16 **ppValues)
    {
        return GetInt16Array(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ UINT16 **ppValues)
    {
        return GetUInt16Array(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ INT32 **ppValues)
    {
        return GetInt32Array(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ UINT32 **ppValues)
    {
        return GetUInt32Array(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ INT64 **ppValues)
    {
        return GetInt64Array(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ UINT64 **ppValues)
    {
        return GetUInt64Array(pInspectable, size, ppValues);
    }

#if !defined(_WINDOWS)
    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ boolean **ppValues)
    {
        return GetBooleanArray(pInspectable, size, ppValues);
    }
#else
    static HRESULT GetValue(
        _In_ foundation::IInspectable *pInspectable, 
        _Out_ UINT32 *pSize,
        _Outptr_ bool **ppValues)
    {
        *pSize = 0;
        *ppValues = nullptr;

        foundation::BooleanArrayWrapper arrayWrapper;
        IFR_ASSERT(GetBooleanArray(
            pInspectable, 
            arrayWrapper.GetSizeAddressOf(), 
            arrayWrapper.GetBufferAddressOf()));

        bool *pValues = (bool*)_pal_MemAlloc(arrayWrapper.GetSize() * sizeof(bool));
        if (!pValues)
        {
            return E_OUTOFMEMORY;
        }
#pragma warning ( disable: 6386 )

        for(UINT32 index = 0;index < arrayWrapper.GetSize();++index)
        {
            pValues[index] = arrayWrapper[index] ? true : false;
        }

        *pSize = arrayWrapper.GetSize();
        *ppValues = pValues;
        return S_OK;
    }
#endif

#pragma warning( push )
#pragma warning( disable : 6001 )
    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ HSTRING **ppValues)
    {
        return GetStringArray(pInspectable, size, ppValues);
    }
#pragma warning( pop )

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ FLOAT **ppValues)
    {
        return GetSingleArray(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ DOUBLE **ppValues)
    {
        return GetDoubleArray(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ WCHAR **ppValues)
    {
        return GetChar16Array(pInspectable, size, ppValues);
    }

    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ foundation::IInspectable ***ppValues)
    {
        return GetInspectableArray(pInspectable, size, ppValues);
    }

    template <class T>
    static HRESULT GetValueArray(
        _In_ foundation::IInspectable *pInspectable,
        _Out_ UINT32 *pSize,
        _Outptr_ T **ppValues)
    {
        *ppValues = nullptr;
        *pSize = 0;

        InspectablePtr ptr(pInspectable);
        PropertyType arrayPropertyType = (PropertyType)(0x400 + GetPropertyType<T>());
        _IFR_(pv_util::ChangeType(arrayPropertyType, ptr.GetAddressOfPtr()));

        return GetValue(ptr, pSize, ppValues);
    }

    ///
    /// @brief unbox a value into a Enum type based array.
    /// The boxed value has to be a UINT32 array or an Inspectable Array of IEnumValue(s)
    ///
    template <class T>
    static HRESULT GetValue(
        _In_ foundation::IInspectable *pInspectable,
        _Out_ UINT32 *pSize,
        __RPC__deref_out_ecount_full_opt(*pSize) T **ppValues)
    {
        *ppValues = nullptr;
        *pSize = 0;

        foundation::PropertyType propertyType;
        IFR_ASSERT(pv_util::GetType(pInspectable, &propertyType));

        UINT32 size(0);
        T *pBuffer = nullptr;

        if (propertyType == foundation::PropertyType_UInt32Array)
        {
            UInt32ArrayWrapper boxed_values_wrapper;
            IFR_ASSERT(GetValue(
                pInspectable,
                boxed_values_wrapper.GetSizeAddressOf(),
                boxed_values_wrapper.GetBufferAddressOf()));
            size = boxed_values_wrapper.GetSize();

            pBuffer = (T *)_pal_MemAlloc(size * sizeof(T));
            if (pBuffer == nullptr)
            {
                return E_OUTOFMEMORY;
            }

            for (UINT32 index = 0; index < size; ++index)
            {
                pBuffer[index] = (T)boxed_values_wrapper[index];
            }
        }
        else
        {
            InspectableArrayWrapper boxed_values_wrapper;
            IFR_ASSERT(pv_util::GetValue(
                pInspectable,
                boxed_values_wrapper.GetSizeAddressOf(),
                boxed_values_wrapper.GetBufferAddressOf()));

            size = boxed_values_wrapper.GetSize();
            pBuffer = (T *)_pal_MemAlloc(size * sizeof(T));
            if (pBuffer == nullptr)
            {
                return E_OUTOFMEMORY;
            }

            for (UINT32 index = 0; index < size; ++index)
            {
                UINT32 enumValue = 0;
                if (boxed_values_wrapper[index] != nullptr)
                {
                    foundation::IEnumValue *pEnumValue;
                    foundation::QueryWeakReference(boxed_values_wrapper[index], &pEnumValue);
                    pEnumValue->GetValue(&enumValue);
                }
                pBuffer[index] = (T)enumValue;
            }
        }

        // assign ownership back
        *pSize = size;
        *ppValues = pBuffer;

        return S_OK;
    }

    template <class T>
    static HRESULT GetValue(
        _In_ foundation::IInspectable *pInspectable, 
        _Out_ UINT32 *pSize,
        const IID& iid, 
        T ***ppValues)
    {
        if (pInspectable == nullptr)
        {
            *pSize = 0;
            *ppValues = nullptr;
            return S_OK;
        }

        foundation::InspectableArrayWrapper objectArrayWrapper;
        IFR_ASSERT(GetInspectableArray(
            pInspectable,
            objectArrayWrapper.GetSizeAddressOf(),
            objectArrayWrapper.GetBufferAddressOf()));

        T **pBuffer = (T**)_pal_MemAlloc(objectArrayWrapper.GetSize() * sizeof(T*));
        if (pBuffer == nullptr)
        {
            *pSize = 0;
            return E_OUTOFMEMORY;
        }
#pragma warning ( disable: 6386 )

        for(UINT32 index = 0;index < objectArrayWrapper.GetSize();++index)
        {
            HRESULT hr = objectArrayWrapper[index]->QueryInterface(iid, (void**)(pBuffer+index));
            if(FAILED(hr))
            {
                _pal_MemFree((LPVOID)pBuffer);
                IFR_ASSERT(hr);
            }
        }

        *pSize = objectArrayWrapper.GetSize();
        *ppValues = pBuffer;
        return S_OK;
    }

    template <class T>
    static HRESULT GetValue(_In_ foundation::IInspectable *pInspectable, UINT32 *size, T ***ppValues)
    {
        return GetValue(pInspectable, size, GetIIDOf<T>(), ppValues);
    }

    static HRESULT CreateDefaultValue(
        PropertyType propertyType,
        _COM_Outptr_ foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStaticsInternal()->CreateDefaultValue(propertyType,ppValue);
    }

    static HRESULT CreateEmptyValue(__RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateEmpty(ppValue);
    }
    static HRESULT CreateUInt8Value(BYTE value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateUInt8(value, ppValue);
    }
    static HRESULT CreateInt16Value(INT16 value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateInt16(value, ppValue);
    }
    static HRESULT CreateUInt16Value(UINT16 value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateUInt16(value, ppValue);
    }
    static HRESULT CreateInt32Value(INT32 value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateInt32(value, ppValue);
    }
    static HRESULT CreateUInt32Value(UINT32 value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateUInt32(value, ppValue);
    }
    static HRESULT CreateInt64Value(INT64 value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateInt64(value, ppValue);
    }
    static HRESULT CreateUInt64Value(UINT64 value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateUInt64(value, ppValue);
    }
    static HRESULT CreateDoubleValue(DOUBLE value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateDouble(value, ppValue);
    }
    static HRESULT CreateSingleValue(FLOAT value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateSingle(value, ppValue);
    }
    static HRESULT CreateChar16Value(WCHAR value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateChar16(value, ppValue);
    }
    static HRESULT CreateBooleanValue(boolean value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateBoolean(value, ppValue);
    }
    static HRESULT CreateGuidValue(GUID value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateGuid(value, ppValue);
    }
    static HRESULT CreateDateTimeValue(foundation::DateTime value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateDateTime(value, ppValue);
    }
    static HRESULT CreateSystemTimeValue(const foundation::SystemTime& value, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        IFCPTR_ASSERT(ppValue);
        *ppValue = nullptr;

        foundation::DateTime dateTime;
        _IFR_(_pal_SystemTimeToDateTime(value, &dateTime));
        return CreateDateTimeValue(dateTime, ppValue);
    }
    static HRESULT CreateStringValue(LPCSTR_t str, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        foundation::HStringRef hStringRef(str);
        return GetPropertyValueStatics()->CreateString(hStringRef, ppValue);
    }
    static HRESULT CreateStringValue(HSTRING hString, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return GetPropertyValueStatics()->CreateString(hString, ppValue);
    }

    // Array support
    static HRESULT CreateUInt8ArrayValue(UINT32 size, _In_opt_ BYTE *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateUInt8Array, size, pValues, ppValue);
    }

    static HRESULT CreateInt16ArrayValue(UINT32 size, _In_opt_ INT16 *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateInt16Array, size, pValues, ppValue);
    }

    static HRESULT CreateUInt16ArrayValue(UINT32 size, _In_opt_ UINT16 *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateUInt16Array, size, pValues, ppValue);
    }

    static HRESULT CreateInt32ArrayValue(UINT32 size, _In_opt_ INT32 *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateInt32Array, size, pValues, ppValue);
    }

    static HRESULT CreateUInt32ArrayValue(UINT32 size, _In_opt_ UINT32 *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateUInt32Array, size, pValues, ppValue);
    }

    static HRESULT CreateInt64ArrayValue(UINT32 size, _In_opt_ INT64 *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateInt64Array, size, pValues, ppValue);
    }

    static HRESULT CreateUInt64ArrayValue(UINT32 size, _In_opt_ UINT64 *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateUInt64Array, size, pValues, ppValue);
    }

    static HRESULT CreateBooleanArrayValue(UINT32 size, _In_opt_ boolean *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateBooleanArray, size, pValues, ppValue);
    }

    static HRESULT CreateGuidArrayValue(UINT32 size, _In_opt_ GUID *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateGuidArray, size, pValues, ppValue);
    }

    static HRESULT CreateDateTimeArrayValue(UINT32 size, _In_opt_ foundation::DateTime *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateDateTimeArray, size, pValues, ppValue);
    }

    static HRESULT CreateDoubleArrayValue(UINT32 size, _In_opt_ DOUBLE *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateDoubleArray, size, pValues, ppValue);
    }

    static HRESULT CreateSingleArrayValue(UINT32 size, _In_opt_ FLOAT *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateSingleArray, size, pValues, ppValue);
    }

    static HRESULT CreateChar16ArrayValue(UINT32 size, _In_opt_ WCHAR *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateChar16Array, size, pValues, ppValue);
    }

    static HRESULT CreateStringArrayValue(UINT32 size, _In_opt_ HSTRING *pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateStringArray, size, pValues, ppValue);
    }

    static HRESULT CreateInspectableArrayValue(UINT32 size, _In_opt_ foundation::IInspectable **pValues, __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        return create_array_value_internal(&IPropertyValueStatics::CreateInspectableArray, size, pValues, ppValue);
    }

    static HRESULT GetUInt8Value(_In_ foundation::IInspectable *pInspectable, _Out_ BYTE *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetUInt8, value);
    }

    static HRESULT GetInt16Value(_In_ foundation::IInspectable *pInspectable, _Out_ INT16 *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetInt16, value);
    }

    static HRESULT GetUInt16Value(_In_ foundation::IInspectable *pInspectable, _Out_ UINT16 *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetUInt16, value);
    }

    static HRESULT GetInt32Value(_In_ foundation::IInspectable *pInspectable, _Out_ INT32 *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetInt32, value);
    }

    static HRESULT GetUInt32Value(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetUInt32, value);
    }

    static HRESULT GetInt64Value(_In_ foundation::IInspectable *pInspectable, _Out_ INT64 *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetInt64, value);
    }

    static HRESULT GetUInt64Value(_In_ foundation::IInspectable *pInspectable, _Out_ UINT64 *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetUInt64, value);
    }

    static HRESULT GetStringValue(_In_ foundation::IInspectable *pInspectable, _Outptr_result_maybenull_ HSTRING *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetString, value);
    }

    static HRESULT GetBooleanValue(_In_ foundation::IInspectable *pInspectable, _Out_ boolean *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetBoolean, value);
    }

    static HRESULT GetDoubleValue(_In_ foundation::IInspectable *pInspectable, _Out_ DOUBLE *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetDouble, value);
    }

    static HRESULT GetSingleValue(_In_ foundation::IInspectable *pInspectable, _Out_ FLOAT *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetSingle, value);
    }

    static HRESULT GetGuidValue(_In_ foundation::IInspectable *pInspectable, _Out_ IID *value)
    {
        foundation::PropertyType propertyType;
        GetType(pInspectable, &propertyType);
        if (propertyType == foundation::PropertyType_String)
        {
            foundation::HStringPtr hstr;
            IFR_ASSERT(GetStringValue(pInspectable, hstr.GetAddressOf()));
            return HStringToGuid(hstr, value);
        }
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetGuid, value);
    }

    static HRESULT GetDateTimeValue(_In_ foundation::IInspectable *pInspectable, _Out_ foundation::DateTime *value)
    {
        return GetValueInternal(pInspectable, &foundation::IPropertyValue::GetDateTime, value);
    }

    // Array Access support
    static HRESULT GetUInt8Array(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ BYTE **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetUInt8Array,
            foundation::PropertyType_UInt8Array,
            size,
            ppValues);
    }

    static HRESULT GetInt16Array(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ INT16 **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetInt16Array,
            foundation::PropertyType_Int16Array,
            size,
            ppValues);
    }

    static HRESULT GetUInt16Array(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ UINT16 **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetUInt16Array,
            foundation::PropertyType_UInt16Array,
            size, ppValues);
    }

    static HRESULT GetInt32Array(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ INT32 **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetInt32Array,
            foundation::PropertyType_Int32Array,
            size, ppValues);
    }

    static HRESULT GetUInt32Array(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ UINT32 **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetUInt32Array,
            foundation::PropertyType_UInt32Array,
            size, ppValues);
    }

    static HRESULT GetInt64Array(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ INT64 **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetInt64Array,
            foundation::PropertyType_Int64Array,
            size, ppValues);
    }

    static HRESULT GetUInt64Array(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ UINT64 **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetUInt64Array,
            foundation::PropertyType_UInt64Array,
            size, ppValues);
    }

    static HRESULT GetBooleanArray(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ boolean **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetBooleanArray,
            foundation::PropertyType_BooleanArray,
            size, ppValues);
    }

    static HRESULT GetDoubleArray(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ DOUBLE **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetDoubleArray,
            foundation::PropertyType_DoubleArray,
            size, ppValues);
    }

    static HRESULT GetSingleArray(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ FLOAT **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetSingleArray,
            foundation::PropertyType_SingleArray,
            size, ppValues);
    }

    static HRESULT GetGuidArray(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ GUID **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetGuidArray,
            foundation::PropertyType_GuidArray,
            size, ppValues);
    }

    static HRESULT GetDateTimeArray(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ foundation::DateTime **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetDateTimeArray,
            foundation::PropertyType_DateTimeArray,
            size, ppValues);
    }

    static HRESULT GetChar16Array(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ WCHAR **ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetChar16Array,
            foundation::PropertyType_Char16Array,
            size, ppValues);
    }

#pragma warning( push )
#pragma warning( disable : 6001 )

    static HRESULT GetStringArray(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ HSTRING **ppValues)
    {
        return GetArrayInternal<HSTRING>(
            pInspectable,
            &foundation::IPropertyValue::GetStringArray,
            foundation::PropertyType_StringArray,
            size, ppValues);
    }
#pragma warning( pop )

    static HRESULT GetInspectableArray(_In_ foundation::IInspectable *pInspectable, _Out_ UINT32 *size, _Outptr_ foundation::IInspectable ***ppValues)
    {
        return GetArrayInternal(
            pInspectable,
            &foundation::IPropertyValue::GetInspectableArray,
            foundation::PropertyType_InspectableArray,
            size, ppValues);
    }

    static foundation::DateTime Now()
    {
        foundation::SystemTime now;
        _pal_SystemTimeNow(&now);

        foundation::DateTime dt_now;
        _pal_SystemTimeToDateTime(now, &dt_now);
        return dt_now;
    }

    static HRESULT HStringToGuid(HSTRING hstring, _Out_ IID* piid)
    {
        IFCPTR_ASSERT(piid);

        InspectablePtr value = CreateValue(hstring);
        _IFR_(ChangeType(
            foundation::PropertyType_Guid,
            value.GetAddressOfPtr()));
        return GetValue(value, piid);
    }

private:
    static library::IPropertyValueInternalStatics *GetPropertyValueStaticsInternal()
    {
        return GetActivationFactoryWithCast<library::IPropertyValueInternalStatics>(U("foundation.PropertyValueStaticsInternal"));
    }

    template <typename T>
    static HRESULT GetValueInternal(
        _In_ foundation::IInspectable *pInspectable,
        _In_ HRESULT(STDMETHODCALLTYPE foundation::IPropertyValue::*pGetValue)(_Out_ T*),
        _Out_ T* value)
    {
        ComPtr<foundation::IPropertyValue> spPropertyValue;
        IFR_ASSERT(CastToPropertyValue(pInspectable, spPropertyValue.GetAddressOf()));

        return (spPropertyValue->*pGetValue)(value);
    }

    static HRESULT GetValueInternal(
        _In_ foundation::IInspectable *pInspectable,
        _In_ HRESULT(STDMETHODCALLTYPE foundation::IPropertyValue::*pGetValue)(_Outptr_result_maybenull_ HSTRING*),
        _Outptr_result_maybenull_ HSTRING* value)
    {
        ComPtr<foundation::IPropertyValue> spPropertyValue;
        IFR_ASSERT(CastToPropertyValue(pInspectable, spPropertyValue.GetAddressOf()));

        return (spPropertyValue->*pGetValue)(value);
    }

#pragma warning( push )
#pragma warning( disable : 6101 )
    template <typename T>
    static HRESULT GetArrayInternal(
        _In_ foundation::IInspectable *pInspectable,
        _In_ HRESULT(STDMETHODCALLTYPE foundation::IPropertyValue::*pGetArray)(_Out_ UINT32*, _Outptr_ T**),
        _In_ foundation::PropertyType targetType,
        _Out_ UINT32 *pSize,
        _Outptr_ T** ppArray)
    {
        foundation_assert(pSize);
        foundation_assert(ppArray);
        *ppArray = nullptr;

        ComPtr<foundation::IPropertyValue> spPropertyValue;
        IFR_ASSERT(CastToPropertyValue(pInspectable, spPropertyValue.GetAddressOf()));

        foundation::PropertyType propertyType;
        spPropertyValue->get_Type(&propertyType);
        if ((propertyType & 0x400) == 0)
        {
            return foundation::E_ARRAY_EXPECTED;
        }

        if (targetType != propertyType)
        {
            InspectablePtr spArray(pInspectable);
            _IFR_(ChangeType(targetType, spArray.GetAddressOfPtr()));
            IFR_ASSERT(CastToPropertyValue(spArray, spPropertyValue.ReleaseAndGetAddressOf()));
        }

        return (spPropertyValue->*pGetArray)(pSize, ppArray);
    }
#pragma warning( pop )

    template
    <
       typename T,
       HRESULT (*CreateValueFunc)(T, foundation::IInspectable **)
    >
    inline static InspectablePtr CreateValueInternal(_In_ T value)
    {
        InspectablePtr spValue;
        HRESULT hr = CreateValueFunc(value, spValue.GetAddressOf());
        foundation_assert(SUCCEEDED(hr));

        return spValue;
    }

    template <typename T>
    static HRESULT create_array_value_internal(
        _In_ HRESULT(STDMETHODCALLTYPE foundation::IPropertyValueStatics::*pCreateArray)(UINT32, T*, foundation::IInspectable **),
        UINT32 size,
        _In_opt_ T* pValues,
        __RPC__deref_out_opt foundation::IInspectable **ppValue)
    {
        static T empty;

        return (GetPropertyValueStatics()->*pCreateArray)(
            size,
            pValues != nullptr ? pValues : const_cast<T *>(&empty),
            ppValue
            );
    }

    template
    <
       typename T,
       HRESULT (*CreateValueArrayFunc)(_In_ UINT32 size, T *, foundation::IInspectable **)
    >
    inline static InspectablePtr CreateValueArrayInternal(_In_ UINT32 size, _In_opt_ T *values)
    {
        InspectablePtr valuePtr;
        HRESULT hr = CreateValueArrayFunc(size, values, valuePtr.GetAddressOf());
        foundation_assert(SUCCEEDED(hr));
        return valuePtr;
    }
};

}
