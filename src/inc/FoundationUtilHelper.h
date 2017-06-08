/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationUtilHelper.h
****/
#pragma once
#include <foundation/array_wrapper.h>
#include <pal/library/trace_util.h>

#ifdef __UTF16_STRINGS
#define _TO_UTF8_STR(hstring) foundation::to_utf8_string(hstring).c_str()
#define _TO_PAL_STR(str) foundation::to_pal_wstring(str).get()
#else
#define _TO_UTF8_STR(hstring) _pal_GetStringRawBuffer(hstring,nullptr)
#define _TO_PAL_STR(str) str
#endif

#define _traceMessage pal::library::TraceMessage

template <class TOptions, class TOptionMask>
inline bool IsOptionEnabled(TOptions options, TOptionMask maskOption)
{
	return (static_cast<int>(options) & static_cast<int>(maskOption)) ? true : false;
}

template <class TOptions, class TOptionMask>
inline void SetOptionEnabled(TOptions& options, TOptionMask maskOption, bool enabled)
{
	if (enabled)
	{
		options |= maskOption;
	}
	else
	{
		options &= ~maskOption;
	}
}

foundation::string_t FormatNumber(const CHAR_t * _Format, ...);
IID GetType(foundation::IInspectable *pInspectable);

__unused_attribute__
static HRESULT StringStreamToArray(const std::stringstream& os, UINT32 *size, UINT8 **data)
{
    std::string buffer = os.str();
    *size = (UINT32)buffer.size();
    *data = (UINT8 *)_pal_MemAlloc(*size);
    // copy data
    if (*data == nullptr)
    {
        return E_OUTOFMEMORY;
    }

    memcpy(*data, buffer.data(), buffer.size());
    return S_OK;
}

inline bool IsPropertyTypeInfoVoid(_In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo)
{
    foundation_assert(pPropertyTypeInfo);
    foundation::PropertyType propertyType;
    pPropertyTypeInfo->GetPropertyType(&propertyType);
    return propertyType == foundation::PropertyType_Empty;
}

inline HRESULT CreateStringHelper(LPCSTR_t source, HSTRING *hstring)
{
	return _pal_CreateString(
		source,
		(UINT32)_pal_strlen(source),
		hstring);
}


inline bool IsPropertyValue(_In_ foundation::IInspectable *pInspectable)
{
	return foundation::IsTypeOf(
		pInspectable,
		foundation_IID_IPropertyValue);
}

inline bool IsInspectableValue(_In_ foundation::IInspectable *pInspectable)
{
	return pInspectable != nullptr && !IsPropertyValue(pInspectable);
}

// get a value from an interface and return a QI on that value
template <class T, class Q, class R>
HRESULT GetAndQueryInterface(
    _In_ T *pT,
    _In_ HRESULT(T::*pGet)(Q**),
    _COM_Outptr_ R **ppOut
    )
{
    foundation::ComPtr<Q> spValue;
    IFR_ASSERT((pT->*pGet)(spValue.GetAddressOf()));
    IFR_ASSERT(foundation::QueryInterfaceIf(spValue, ppOut));
    return S_OK;
}

#if defined(_HAS_STDMETHODCALLTYPE)
// same as before but use the 'STDMETHODCALLTYPE' modifier
template <class T, class Q, class R>
HRESULT GetAndQueryInterface(
    _In_ T *pT,
    _In_ HRESULT(STDMETHODCALLTYPE T::*pGet)(Q**),
    _COM_Outptr_ R **ppOut
    )
{
    foundation::ComPtr<Q> spValue;
    IFR_ASSERT((pT->*pGet)(spValue.GetAddressOf()));
    IFR_ASSERT(foundation::QueryInterfaceIf(spValue, ppOut));
    return S_OK;
}
#endif

inline UINT64 GetUniversalId(_In_ foundation::IObject *pObject)
{
    foundation_assert(pObject);

    UINT32 uniqueId;
    IGNOREHR(pObject->GetUniqueId(&uniqueId));
    UINT32 processId;
    IGNOREHR(pObject->GetProcessId(&processId));

    return (((UINT64)processId) << 32) + uniqueId;
}

HRESULT IsObjectEqualsInternal(foundation::IObject *pItem1, foundation::IObject *pItem2, bool *pResult);
