/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokePropertyValue.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <foundation/pv_util.h>

using namespace foundation;

#if defined(_WINDOWS)


#define LODWORD(_qw)    ((DWORD)(_qw))
#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))

// Converts a DateTime to a FILETIME without loss of fidelity.
// Returns success if the FILETIME was created, or E_BOUNDS if dateTime was invalid.
inline HRESULT RoDateTimeToFileTime(
	__in foundation::DateTime dateTime,    // Time satisfying, 0 <= dateTime.UniversalTime < 2^63
	__out FILETIME* fileTime)                       // The outputted time
{
	if (dateTime.UniversalTime < 0)
	{
		return E_BOUNDS;
	}

	fileTime->dwLowDateTime = LODWORD(dateTime.UniversalTime & 0xFFFFFFFFi64);
	fileTime->dwHighDateTime = HIDWORD(dateTime.UniversalTime);
	return S_OK;
}

// Convert a FILETIME to a DateTime without loss of fidelity.
// Returns success if DateTime was created, or E_BOUNDS if fileTime is
// outside the bounds of a representable DateTime.
inline HRESULT RoFileTimeToDateTime(
	__in FILETIME fileTime,                         // Time satisfying, 0 <= time < 2^63
	__out foundation::DateTime *dateTime)  // The outputted time
{
	// FILETIME is unsigned 64 and DateTime is signed 64, so
	// return out of bounds failure if high-bit is set
	if (static_cast<INT32>(fileTime.dwHighDateTime) < 0)
	{
		return E_BOUNDS;
	}

	dateTime->UniversalTime = (ULONGLONG)fileTime.dwLowDateTime |
		(((ULONGLONG)fileTime.dwHighDateTime) << 32);
	return S_OK;
}

#endif

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetType(foundation::IInspectable *pPropertyValue, int *pType)
{
	PropertyType propertyType;
	_IFR_(pv_util::GetType(pPropertyValue, &propertyType));
	*pType = (int)propertyType;
	return S_OK;
}

// --------------------------- ------------- UInt8 --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateByte(UINT8 byte, foundation::IInspectable **ppValue)
{
	return pv_util::CreateUInt8Value(byte, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateBytes(UINT32 size, UINT8* pBytes,foundation::IInspectable **ppValue)
{
	return pv_util::CreateUInt8ArrayValue(size, pBytes, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetByte(foundation::IInspectable *pValue,UINT8 *value)
{
	return pv_util::GetUInt8Value(pValue, value);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetBytes(foundation::IInspectable *pValue, UINT32 *size, UINT8 **values)
{
	return pv_util::GetUInt8Array(pValue, size,values);
}
// --------------------------- ------------- Boolean --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateBoolean(bool value, foundation::IInspectable **ppValue)
{
    return pv_util::CreateBooleanValue(value, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetBoolean(foundation::IInspectable *pValue, bool *value)
{
    boolean bVal;
    _IFR_(pv_util::GetBooleanValue(pValue, &bVal));
    *value = bVal ? true : false;
    return S_OK;
}
// --------------------------- ------------- Int32 --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateInt32(INT32 value, foundation::IInspectable **ppValue)
{
	return pv_util::CreateInt32Value(value, ppValue);
}


EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetInt32(foundation::IInspectable *pValue, INT32 *value)
{
	return pv_util::GetInt32Value(pValue, value);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateInt32Array(UINT32 size, INT32* pArray, foundation::IInspectable **ppValue)
{
    return pv_util::CreateInt32ArrayValue(size, pArray, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetInt32Array(foundation::IInspectable *pValue, UINT32 *size, INT32 **values)
{
    return pv_util::GetInt32Array(pValue, size, values);
}
// --------------------------- ------------- Int16 --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateInt16(INT16 value, foundation::IInspectable **ppValue)
{
    return pv_util::CreateInt16Value(value, ppValue);
}


EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetInt16(foundation::IInspectable *pValue, INT16 *value)
{
    return pv_util::GetInt16Value(pValue, value);
}
// --------------------------- ------------- UInt16 --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateUInt16(UINT16 value, foundation::IInspectable **ppValue)
{
    return pv_util::CreateUInt16Value(value, ppValue);
}


EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetUInt16(foundation::IInspectable *pValue, UINT16 *value)
{
    return pv_util::GetUInt16Value(pValue, value);
}

// --------------------------- ------------- UInt32 --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateUInt32(UINT32 value, foundation::IInspectable **ppValue)
{
	return pv_util::CreateUInt32Value(value, ppValue);
}


EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetUInt32(foundation::IInspectable *pValue, UINT32 *value)
{
	return pv_util::GetUInt32Value(pValue, value);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateUInt32Array(UINT32 size, UINT32* pArray, foundation::IInspectable **ppValue)
{
    return pv_util::CreateUInt32ArrayValue(size, pArray, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetUInt32Array(foundation::IInspectable *pValue, UINT32 *size, UINT32 **values)
{
    return pv_util::GetUInt32Array(pValue, size, values);
}
// --------------------------- ------------- UInt64 --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateUInt64(UINT64 value, foundation::IInspectable **ppValue)
{
	return pv_util::CreateUInt64Value(value, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetUInt64(foundation::IInspectable *pValue, UINT64 *value)
{
	return pv_util::GetUInt64Value(pValue, value);
}

// --------------------------- ------------- HSTRING --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateString(LPCSTR_t pStr, foundation::IInspectable **ppValue)
{
	HStringPtr hstr(pStr);
	return pv_util::CreateStringValue(hstr, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetString(foundation::IInspectable *pValue, CHAR_t **values)
{
	HStringPtr hstr;
	_IFR_(pv_util::GetStringValue(pValue, hstr.GetAddressOf()));
	return ReturnString(hstr,values);
}

// --------------------------- ------------- DateTime --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateDateTime(UINT64 fileTime, foundation::IInspectable **ppValue)
{
    DateTime dt;
#if defined(_WINDOWS)
    _IFR_(RoFileTimeToDateTime(*(reinterpret_cast<FILETIME*>(&fileTime)), &dt));
#else
    dt.UniversalTime = (INT64)fileTime;
#endif
	_IFR_(pv_util::CreateDateTimeValue(dt,ppValue));
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetDateTime(foundation::IInspectable *pValue, UINT64 *pFileTime)
{
	DateTime dt;
	_IFR_(pv_util::GetDateTimeValue(pValue, &dt));
#if defined(_WINDOWS)
    _IFR_(RoDateTimeToFileTime(dt, reinterpret_cast<FILETIME*>(pFileTime)));
#else
    *pFileTime = (UINT64)dt.UniversalTime;
#endif
	return S_OK;
}
// --------------------------- ------------- Inspectable[] --------------------------- -------------
EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_CreateInspectableArray(UINT32 size, foundation::IInspectable **values, foundation::IInspectable **ppValue)
{
	return pv_util::CreateInspectableArrayValue(size, values, ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE PropertyValue_GetInspectableArray(foundation::IInspectable *pValue, UINT32 *size, foundation::IInspectable ***values)
{
	return pv_util::GetInspectableArray(pValue, size, values);
}
