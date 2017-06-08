/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:assert.h
****/
#pragma once

#include "foundation_api.h"

#ifdef  DEBUG

namespace foundation
{
	typedef void(*_FoundationAssertCallback)(HRESULT _hResult, LPCSTR_t _Message, LPCSTR_t _File, _In_ unsigned _Line);

	EXTERN_C FOUNDATION_API  void STDAPICALLTYPE FoundationAssert(HRESULT _hResult, LPCSTR_t _Message, LPCSTR_t _File, _In_ unsigned _Line);
	EXTERN_C FOUNDATION_API  void STDAPICALLTYPE SetFoundationAssertCallback(_FoundationAssertCallback pCallback);
}

#define foundation_assert(_Expression) (void)( (!!(_Expression)) || (foundation::FoundationAssert(E_UNEXPECTED,U(#_Expression), U(__FILE__), __LINE__), 0) )

#else
#define foundation_assert(_Expression)     ((void)0)

#endif
