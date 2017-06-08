/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_core.h
****/
#pragma once

// to facilitate _WINDOWS consuming
#if !defined(_WINDOWS) && defined(_MSC_VER)
#define _WINDOWS
#endif

// define if Windows Runtime is present
#if defined(_WINDOWS) && !defined(_WINDOWS_CLASSIC)
#define _WINDOWS_RUNTIME
#endif

// check c++0x support
#ifndef _HAS_CPP0X

// check __cplusplus macro for c++0x support
#if defined(_MSC_VER) || (__cplusplus > 199711L)
#define _HAS_CPP0X 1
#endif

#endif

// check variadic support support
#if defined(_NO_VARIADIC_SUPPORT)
#undef _VARIADIC_SUPPORT
#elif !defined(_VARIADIC_SUPPORT)

#if (_MSC_VER > 1700) || !defined(_MSC_VER)
#define _VARIADIC_SUPPORT
#endif

#if defined(_MSC_VER)
#define __unused_attribute__
#endif

#endif

// for Windows use Unicode characters
#if defined(_WINDOWS) && !defined(_MBCS)
#define __UTF16_STRINGS
#endif

// define STRING encoding depending on the platforms
#ifdef __UTF16_STRINGS
typedef wchar_t CHAR_t;
#define __XPLATSTR(x) L ## x
#else
typedef char CHAR_t;
#define __XPLATSTR(x) x
#endif

typedef const CHAR_t *LPCSTR_t;

#if !defined(U)
#define U(x) __XPLATSTR(x)
#endif

#if defined(_WINDOWS) && !defined(_M_ARM) && !defined(_WIN64)
#define _HAS_STDMETHODCALLTYPE
#endif


#if !defined(_WINDOWS)

// Note: we force this header to be included before our types
// so SAL macros can be processed before
#include <string.h>

#define EXTERN_C               extern "C"

#if defined(__GNUC__)

#define __unused_attribute__ __attribute__((unused))

#ifndef __stdcall

#ifdef _ATTRIBUTE_STDCALL_SUPPORTED_
#define __stdcall __attribute__((__stdcall))
#else
#define __stdcall
#endif

#endif

#ifndef __cdecl

#ifdef _ATTRIBUTE_CDECL_SUPPORTED_
#define __cdecl __attribute__((__cdecl__))
#else
#define __cdecl
#endif

#endif

#endif

#define STDMETHODCALLTYPE       __stdcall
#define STDAPICALLTYPE          __stdcall
#define STDAPIVCALLTYPE         __cdecl

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef int            HRESULT;
#endif

#define STDMETHOD(method)        virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method)  virtual type STDMETHODCALLTYPE method
#define STDMETHODV(method)       virtual HRESULT STDMETHODVCALLTYPE method
#define STDMETHODV_(type,method) virtual type STDMETHODVCALLTYPE method

#define PURE   = 0
#define THIS_
#define THIS   void

#ifndef DECLSPEC_NOVTABLE
#if (_MSC_VER >= 1100) && defined(__cplusplus)
#define DECLSPEC_NOVTABLE   __declspec(novtable)
#else
#define DECLSPEC_NOVTABLE
#endif
#endif


#define STDAPI                  EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)           EXTERN_C type STDAPICALLTYPE
#define STDMETHODIMP            HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)     type STDMETHODCALLTYPE

// compatibility with COM base SAL
#define __RPC_FAR
#define __RPC__out
#define __RPC__deref_out
#define __RPC__deref_out_ecount_full_opt(x)
#define __RPC__deref_out_opt
#define __RPC__out
#define __RPC__in

// Standard C/C++ SAL

#ifndef _In_
#define _In_
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _In_opt_
#define _In_opt_
#endif

#ifndef _Inout_
#define _Inout_
#endif

#ifndef _Outptr_
#define _Outptr_
#endif

#ifndef _Outref_
#define _Outref_
#endif

#ifndef _Outptr_result_maybenull_
#define _Outptr_result_maybenull_
#endif

#ifndef _COM_Outptr_
#define _COM_Outptr_
#endif

#ifndef _COM_Outptr_result_maybenull_
#define _COM_Outptr_result_maybenull_
#endif

#ifndef _COM_Outptr_opt_result_maybenull_
#define _COM_Outptr_opt_result_maybenull_
#endif

#ifndef _Inout_opt_
#define _Inout_opt_
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#ifndef _Always_
#define _Always_(a)
#endif

#ifndef _In_reads_
#define _In_reads_(a)
#endif

#ifndef _In_reads_opt_
#define _In_reads_opt_(a)
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P)
#endif

#if defined(__GNUC__)
// This will prevent compiler erross due to GNU compilers
#define nullptr __null
#define override
#endif

#endif


