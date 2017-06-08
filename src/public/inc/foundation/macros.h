/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:macros.h
****/
#pragma once
#include "com_interfaces.h"

#include "assert.h"
#include "pal.h"

#if defined(DEBUG) && !defined(NDEBUG)
    static inline void failed_assert(HRESULT _hResult,LPCSTR_t _Message, LPCSTR_t _File, unsigned _Line)
    {
        foundation::FoundationAssert(_hResult,_Message, _File, _Line);
    }
#else
#   define failed_assert(_hResult,_Message,_File,_Line)
#endif

#define IFR_ASSERT(x) {HRESULT hr_IFR = (x); if(FAILED(hr_IFR)) { failed_assert(hr_IFR,U("IFR_ASSERT"), U(__FILE__), __LINE__); return hr_IFR; } }
#define IFHR_ASSERT(x) { hr = (x); if(FAILED(hr)) { failed_assert(hr, U("IFHR_ASSERT"), U(__FILE__), __LINE__); return hr; } }

#define IFCPTR_ASSERT(x) if ((x) == nullptr)  { failed_assert(E_POINTER, U("IFCPTR_ASSERT"), U(__FILE__), __LINE__);return E_POINTER; }
#define IFCPTREx_ASSERT(x, returnValue)     if ((x) == nullptr)  { failed_assert(E_POINTER, U("IFCPTREx_ASSERT"), U(__FILE__), __LINE__);return returnValue; }
#define IFRF_ASSERT(x) {HRESULT hr_IFR = (x); if(FAILED(hr_IFR)) { failed_assert(hr_IFR, U("IFRF_ASSERT"), U(__FILE__), __LINE__); return false; } }
#define IF_ASSERT(x) if(FAILED(x)) { failed_assert(E_FAIL, U("IF_ASSERT"), U(__FILE__), __LINE__); }

#define IFCEXPECT_ASSERT(x)  if (!(x))  { failed_assert(E_UNEXPECTED, U("IFCEXPECT_ASSERT"), U(__FILE__), __LINE__); return E_UNEXPECTED; }

#define RRETURN(hr) return (hr)
#define IGNOREHR(hr) (void) (hr)


#undef RELEASE_INTERFACE
#define RELEASE_INTERFACE(p)        if ((p)) {IGNOREHR((p)->Release()); (p) = nullptr;}
#undef ADDREF_INTERFACE
#define ADDREF_INTERFACE(p)         if ((p)) {(p)->AddRef();}

// define 'silent' macros
#define _IFR_(x) {HRESULT hr_IFR = (x); if(FAILED(hr_IFR)) { return hr_IFR; } }
#define _IFHR_(x) { hr = (x); if(FAILED(hr)) { return hr; } }

// define macros to help when '_COM_Outptr_' sal annotation is needed 
#define IFCPTR_COM_OUT_PTR_ASSERT(ppOut) IFCPTR_ASSERT(ppOut); *ppOut = nullptr

#define _IFR_COM_OUT_PTR_(x,ppOut) {HRESULT hr_IFR = (x); if(FAILED(hr_IFR)) { *ppOut = nullptr; return hr_IFR; } }
#define _IFHR_COM_OUT_PTR_(x,ppOut) { hr = (x); if(FAILED(hr)) { *ppOut = nullptr; return hr; } }

