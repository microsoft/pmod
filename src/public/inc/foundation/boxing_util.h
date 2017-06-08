/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:boxing_util.h
****/
#pragma once

#include "type_traits_util.h"

#include <vector>
#include "pv_util.h"

namespace foundation
{

    template <class T>
    HRESULT GetValue(
        IInspectable *p_value,
        T& value
#if _HAS_CPP0X
        , typename std::enable_if<
        (is_foundation_type<T>::value || std::is_same<T, HStringPtr>::value) &&
        !std::is_base_of<_ComPtrBaseType, T>::value &&
        !std::is_base_of<foundation::_ArrayWrapperBase, T>::value>::type* = nullptr
#endif
        )
    {
        return pv_util::GetValue(p_value, &value);
    }

    __unused_attribute__
    static HRESULT GetValue(
        IInspectable *p_value,
        foundation::string_t& value
        )
    {
        foundation::HStringPtr hstr;
        IFR_ASSERT(pv_util::GetValue(p_value, hstr.GetAddressOf()));
        value = get_string_t(hstr);
        return S_OK;
    }
    __unused_attribute__
	static HRESULT GetValue(
		IInspectable *p_value,
		foundation::HStringPtr& value
		)
	{
		return pv_util::GetValue(p_value, &value);
	}

#ifdef __UTF16_STRINGS
    static HRESULT GetValue(
        IInspectable *p_value,
        std::string& value
        )
    {
        foundation::HStringPtr hstr;
        IFR_ASSERT(pv_util::GetValue(p_value, hstr.GetAddressOf()));
        value = foundation::to_utf8_string(hstr);
        return S_OK;
    }
#endif

#if _HAS_CPP0X
    template <class com_ptr_t>
    HRESULT GetValue(
        IInspectable *p_value,
        com_ptr_t& ptr,
        typename std::enable_if<std::is_base_of<_ComPtrBaseType, com_ptr_t>::value>::type* = nullptr)
    {
        return pv_util::GetValue(p_value, ptr.ReleaseAndGetAddressOf());
    }

    template <class array_wrapper_t>
    HRESULT GetValue(
        IInspectable *p_value,
        array_wrapper_t& arrayWrapper,
        typename std::enable_if<
        std::is_base_of<foundation::_ArrayWrapperBase, array_wrapper_t>::value &&
        is_foundation_type<typename array_wrapper_t::_Item_Type>::value
        >::type* = nullptr)
    {
        return pv_util::GetValue(
            p_value,
            arrayWrapper.GetSizeAddressOf(),
            arrayWrapper.GetBufferAddressOf());
    }

    template <class array_wrapper_t>
    HRESULT GetValue(
        IInspectable *p_value,
        array_wrapper_t& arrayWrapper,
        typename std::enable_if<
        std::is_base_of<foundation::_ArrayWrapperBase, array_wrapper_t>::value &&
        !is_foundation_type<typename array_wrapper_t::_Item_Type>::value
        >::type* = nullptr)
    {
        arrayWrapper.Clear();
        return pv_util::GetValue(
            p_value,
            arrayWrapper.GetSizeAddressOf(),
            arrayWrapper.GetBufferAddressOf());
    }

    template <class ContainerT>
    HRESULT GetValue(
        IInspectable *p_value,
        ContainerT& values,
        ENABLE_IF_VECTOR_VALUE
        )
    {
        ArrayWrapper<typename ContainerT::value_type> arrayValues;
        IFR_ASSERT(pv_util::GetValue(
            p_value,
            arrayValues.GetSizeAddressOf(),
            arrayValues.GetBufferAddressOf()));

        values.resize(arrayValues.GetSize());
        for (UINT32 index = 0; index < arrayValues.GetSize(); ++index)
        {
            values[index] = arrayValues[index];
        }
        return S_OK;
    }

    template <class ContainerT>
    HRESULT GetValue(
        IInspectable *p_value,
        ContainerT& values,
        ENABLE_IF_VECTOR_PTR)
    {
        InspectableArrayWrapper arrayValues;
        IFR_ASSERT(pv_util::GetValue(
            p_value,
            arrayValues.GetSizeAddressOf(),
            arrayValues.GetBufferAddressOf()));

        values.resize(arrayValues.GetSize());
        for (UINT32 index = 0; index < arrayValues.GetSize(); ++index)
        {
            if (arrayValues[index] != nullptr)
            {
                IFR_ASSERT(foundation::QueryInterface(
                    arrayValues[index], 
                    values[index].GetAddressOf()));
            }
        }
        return S_OK;
    }

    __unused_attribute__
    static HRESULT GetValue(
        IInspectable *p_value,
        std::vector<HStringPtr>& values)
    {
        HStringArrayWrapper arrayValues;
        IFR_ASSERT(pv_util::GetValue(
            p_value,
            arrayValues.GetSizeAddressOf(),
            arrayValues.GetBufferAddressOf()));

        values.resize(arrayValues.GetSize());
        for (UINT32 index = 0; index < arrayValues.GetSize(); ++index)
        {
            values[index] = HStringPtr(arrayValues[index]);
        }
        return S_OK;
    }

#else
    // when Cx11 is not enabled we need to provide template specialization for arrays
    template <class T>
    inline HRESULT GetValue(
        IInspectable *p_value,
        ArrayRefCountWrapper<T>& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        UInt8ArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        Int16ArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        UInt16ArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        Int32ArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        UInt32ArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        Int64ArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        UInt64ArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        SingleArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        DoubleArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        Char16ArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
        IInspectable *p_value,
        HStringArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

    template <>
    inline HRESULT GetValue(
         IInspectable *p_value,
         InspectableArrayWrapper& value)
    {
        return pv_util::GetValue(
            p_value,
            value.GetSizeAddressOf(),
            value.GetBufferAddressOf());
    }

#endif

    template <class T>
    HRESULT GetValueAs(
        IInspectable *p_value,
        T& value)
    {
        InspectablePtr ptr(p_value);
        _IFR_(pv_util::ChangeType(
            foundation::GetPropertyType<T>(),
            ptr.GetAddressOfPtr()));
        return GetValue(ptr, value);
    }

    template <class T>
    inline static InspectablePtr CreateDefaultValue()
    {
        InspectablePtr spValue;
        pv_util::CreateDefaultValue(
            GetPropertyType<T>(),
            spValue.GetAddressOf());
        return spValue;
    }

#if _HAS_CPP0X
    template <class T>
    inline InspectablePtr CreateValue(
        T value,
        typename std::enable_if<
        (
            is_foundation_type<T>::value || 
            std::is_same<T, LPCSTR_t>::value ||
            std::is_same<T, HStringPtr>::value
            ) &&
        !std::is_base_of<foundation::_ArrayWrapperBase, T>::value>::type* = nullptr
        )
    {
        return pv_util::CreateValue(value);
    }

    template <class com_ptr_t>
    inline InspectablePtr CreateValue(
        const com_ptr_t& value,
        typename std::enable_if
        <
            std::is_base_of<_ComPtrBaseType, com_ptr_t>::value &&
            !std::is_base_of<_ComPtrImplBase, typename com_ptr_t::_PtrClass>::value
        >::type* = nullptr)
    {
        return InspectablePtr(value.Get());
    }

    template <class com_ptr_t>
    inline InspectablePtr CreateValue(
        const com_ptr_t& value,
        typename std::enable_if
        <
            std::is_base_of<_ComPtrBaseType, com_ptr_t>::value &&
            std::is_base_of<_ComPtrImplBase, typename com_ptr_t::_PtrClass>::value
        >::type* = nullptr)
    {
        return InspectablePtr(value.Get()->CastToInspectable());
    }

    template <class com_base_class>
    inline InspectablePtr CreateValue(
        com_base_class *pValue,
        typename std::enable_if
        <
        std::is_base_of<_ComPtrImplBase, com_base_class>::value
        >::type* = nullptr)
    {
        return InspectablePtr(pValue != nullptr ? pValue->CastToInspectable() : nullptr);
    }

#else
    template <class T>
    inline InspectablePtr CreateValue(T value)
    {
        return pv_util::CreateValue(value);
    }
#endif

    inline static InspectablePtr CreateValue(const string_t& str)
    {
        return CreateValue(str.c_str());
    }

#ifdef __UTF16_STRINGS
    inline static InspectablePtr CreateValue(const char *pUtf8)
    {
        string_t str = foundation::to_platform_string(pUtf8);
        return pv_util::CreateValue(str.c_str());
    }

    inline static InspectablePtr CreateValue(const std::string& str)
    {
        return CreateValue(str.c_str());
    }

#endif

    template <class T>
    inline InspectablePtr CreateValue(UINT32 size, T* pValues)
    {
        return pv_util::CreateValue(size, pValues);
    }

#if _HAS_CPP0X
	template <class T>
	inline InspectablePtr CreateValue(
		T value,
		typename std::enable_if<
		std::is_enum<T>::value>::type* = nullptr
		)
	{
		return pv_util::CreateValue((UINT32)value);
	}
#endif

#if _HAS_CPP0X
    template <class T>
    inline InspectablePtr CreateValue(
        T value,
        typename std::enable_if<
            std::is_base_of<foundation::_ArrayWrapperBase, T>::value &&
            is_foundation_type<typename T::_Item_Type>::value >::type* = nullptr
        )
    {
        return pv_util::CreateValue(value.GetSize(), (typename T::_Item_Type *)value.GetBuffer());
    }
    template <class T>
    inline InspectablePtr CreateValue(
        T value,
        typename std::enable_if<
        std::is_base_of<foundation::_ArrayWrapperBase, T>::value &&
        !is_foundation_type<typename T::_Item_Type>::value >::type* = nullptr
        )
    {
        return pv_util::CreateValue(value.GetSize(), value.GetBuffer());
    }
#else
    template <class T>
    inline InspectablePtr CreateValue(const ArrayRefCountWrapper<T>& value)
    {
        return pv_util::CreateValue(
                                             value.GetSize(),
                                             (IInspectable **)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const UInt8ArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (BYTE *) value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const Int16ArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (INT16 *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const UInt16ArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (UINT16 *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const Int32ArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (INT32 *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const UInt32ArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (UINT32 *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const Int64ArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (INT64 *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const UInt64ArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (UINT64 *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const SingleArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (FLOAT *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const DoubleArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (DOUBLE *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const BoolArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (bool *)value.GetBuffer());
    }

    template <>
    inline InspectablePtr CreateValue(const Char16ArrayWrapper& value)
    {
        return pv_util::CreateValue(value.GetSize(), (WCHAR *)value.GetBuffer());
    }
#endif

    template <
        class ContainerT
    >
    inline InspectablePtr CreateValueFromVector(const ContainerT& values)
    {
        typename ContainerT::value_type defaultValue;
#pragma warning(suppress: 6001)
        return pv_util::CreateValue(
            (UINT32)values.size(),
            values.size() > 0 ? (typename ContainerT::value_type *)&values.front() : &defaultValue);
    }

    inline InspectablePtr CreateValueFromVector(const std::vector<bool>& values)
    {
        PalBuffer<bool> bool_buffer((UINT32)values.size());
        for (std::vector<bool>::const_iterator it = values.begin(); it != values.end(); ++it)
        {
            *(bool_buffer.get() + (it - values.begin())) = *it;
        }
        return pv_util::CreateValue(
            (UINT32)values.size(),
            bool_buffer.get());
    }

    template <
        class ContainerT
    >
    inline InspectablePtr CreateValueFromVectorPtr(
        const ContainerT& values)
    {
        foundation::IInspectable* defaultValue = nullptr;
        return pv_util::CreateValue(
            (UINT32)values.size(),
            values.size() > 0 ? (foundation::IInspectable **)(&values.front()) : &defaultValue);
    }

    template <
        class ContainerT
    >
    inline InspectablePtr CreateValueFromContainer(const ContainerT& values)
    {
        const size_t size = values.size();

        if (size == 0)
        {
            typename ContainerT::value_type defaultValue;
            return pv_util::CreateValue(0, &defaultValue);
        }

        std::vector<typename ContainerT::value_type> vec_values(size);
        std::copy(values.cbegin(), values.cend(), vec_values.begin());
        return pv_util::CreateValue(
            (UINT32)size, 
            (typename ContainerT::value_type *)(&vec_values.front()));
    }

    template <
        class ContainerT
    >
    inline InspectablePtr CreateValueFromContainerPtr(const ContainerT& values)
    {
        const size_t size = values.size();

        if (size == 0)
        {
            foundation::IInspectable* pNull = nullptr;
            return pv_util::CreateValue(0, &pNull);
        }

        std::vector<foundation::IInspectable*> vec_values(size);
        std::copy(values.cbegin(), values.cend(), vec_values.begin());
        return pv_util::CreateValue((UINT32)size, (foundation::IInspectable**)(&vec_values.front()));
    }

    inline InspectablePtr CreateValue(LPCSTR_t *p_values)
    {
        foundation_assert(p_values != nullptr);
        std::vector<foundation::HStringPtr> values;
        while (*p_values != nullptr)
        {
            values.push_back(foundation::HStringPtr(*p_values));
            ++p_values;
        }
        return CreateValueFromVector(values);
    }

    template <class _String_type>
    InspectablePtr CreateStringValues(const std::vector<_String_type>& values)
    {
        std::vector<foundation::HStringPtr> hstring_values;
        for (typename std::vector<_String_type>::const_iterator 
            it = values.begin(); 
            it != values.end(); 
            ++it)
        {
            hstring_values.push_back(HStringPtr((*it).c_str()));
        }
        return CreateValueFromVector(hstring_values);
    }

    inline InspectablePtr CreateValue(const std::vector<string_t>& values)
    {
        return CreateStringValues(values);
    }

#ifdef __UTF16_STRINGS
    inline InspectablePtr CreateValue(const std::vector<std::string>& values)
    {
        return CreateStringValues(values);
    }

#endif

#if _HAS_CPP0X
    /// <summary>
    /// create a property value array from any container but vector
    /// </summary>
    template <
        class ContainerT
        >
    inline InspectablePtr CreateValue(
        const ContainerT& values,
        ENABLE_IF_CONTAINER_VALUE)
    {
        return CreateValueFromContainer(values);
    }

    template <
        class ContainerT
        >
    inline InspectablePtr CreateValue(
        const ContainerT& values,
        ENABLE_IF_CONTAINER_PTR)
    {
        return CreateValueFromContainerPtr(values);
    }

    /// <summary>
    /// create a property value array from a vector
    /// </summary>
    template <
        class ContainerT
        >
    inline InspectablePtr CreateValue(
        const ContainerT& values,
        ENABLE_IF_VECTOR_VALUE)
    {
            return CreateValueFromVector(values);
    }

    template <
        class ContainerT
        >
    inline InspectablePtr CreateValue(
        const ContainerT& values,
        ENABLE_IF_VECTOR_PTR)
    {
        return CreateValueFromVectorPtr(values);
    }
#endif

#if defined(_VARIADIC_SUPPORT)
    template<typename... Args> static inline void _PassValues(Args&&...) {}

    template <class T>
    int _ExpandValues(std::vector<InspectablePtr>& values, T value)
    {
        values.insert(
#if defined(_WINDOWS) || (defined __GNUC__ && !defined(__APPLE__))
            values.begin(),
#else
            values.end(),
#endif
            pv_util::CreateValue(value));
        return 0;
    }

    template<typename... Args>
    static InspectablePtr CreateArrayWithValues(Args&&... args)
    {
        std::vector<InspectablePtr> values;
		_PassValues(_ExpandValues(values, args)...);
        return CreateValueFromVectorPtr(values);
    }
#endif

}
