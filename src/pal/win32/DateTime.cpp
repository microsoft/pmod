/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DateTime.cpp
****/
#pragma once

#include <pal/pal_foundation.h>
#include <foundation/macros.h>
#include <foundation/com_interfaces.h>

#include <intsafe.h>

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

// Convert a DateTime to a SYSTEMTIME with loss of fidelity for times with precision greater than the millisecond.
// Returns success if SYSTEMTIME was created, or E_BOUNDS if dateTime is
// outside the bounds of a representable SYSTEMTIME.
EXTERN_C PAL_API HRESULT _pal_DateTimeToSystemTime(
    foundation::DateTime dateTime,    // Time between the year 1601 and 30827
	_Out_ foundation::SystemTime *systemTime)                   // The outputted time 
{
    IFCPTR_ASSERT(systemTime);

    FILETIME fileTime;
    HRESULT hr = RoDateTimeToFileTime(dateTime, &fileTime);
    if (SUCCEEDED(hr))
    {
        BOOL result = FileTimeToSystemTime(&fileTime, systemTime);

        if (!result || systemTime->wYear > 30827)
        {
            hr = E_BOUNDS;
        }
    }

    return hr;
}

// Converts a SYSTEMTIME to a DateTime without loss of fidelity for wellformed SYSTEMTIMEs,
// i.e. times where all SYSTEMTIME fields mutually correspond to a valid calendar date.
// Returns success if DateTime was created, or E_BOUNDS if systemTime is
// outside the bounds of a valid SYSTEMTIME.
EXTERN_C PAL_API HRESULT _pal_SystemTimeToDateTime(
    foundation::SystemTime systemTime,                     // Time satisfying, 1601 <= systemTime.wYear <= 30827
	_Out_ foundation::DateTime *dateTime)  // The outputted time 
{
    IFCPTR_ASSERT(dateTime);

    HRESULT hr = S_OK;

    FILETIME fileTime;
    BOOL result = SystemTimeToFileTime(&systemTime, &fileTime);
    if (!result)
    {
        hr = E_BOUNDS;
    }

    if (SUCCEEDED(hr))
    {
        hr = RoFileTimeToDateTime(fileTime, dateTime);
    }

    return hr;
}

EXTERN_C PAL_API HRESULT _pal_DateTimeAdd(
    foundation::DateTime dateTime,
    INT32 milliSecs,
	_Out_ foundation::DateTime *value)
{
    IFCPTR_ASSERT(value);

    value->UniversalTime = dateTime.UniversalTime + ((INT64)(milliSecs) * 10000);
    return S_OK;
}

EXTERN_C PAL_API HRESULT _pal_SystemTimeNow(_Out_ foundation::SystemTime *pSystemTime)
{
    ::GetSystemTime(pSystemTime);
    return S_OK;
}

EXTERN_C PAL_API HRESULT _pal_SystemTimeToLocalTime(
    foundation::SystemTime systemTime,
	_Out_ foundation::SystemTime *pLocalSystemTime)
{
    return ::SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, pLocalSystemTime) ? S_OK : E_FAIL;
}
