/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationAssert.cpp
****/
#include "pch.h"

#include <assert.h>
#include <foundation/assert.h>
#include "DebugUtil.h"

#if defined( DEBUG)
#include <pal/library/trace_util.h>

static foundation::_FoundationAssertCallback _sAssertCallback;

EXTERN_C FOUNDATION_API  void STDAPICALLTYPE SetFoundationAssertCallback(foundation::_FoundationAssertCallback pCallback)
{
    _sAssertCallback = pCallback;
}

EXTERN_C FOUNDATION_API  void STDAPICALLTYPE FoundationAssert(
    HRESULT _hResult,
    LPCSTR_t _Message, 
    LPCSTR_t _File, 
    _In_ unsigned _Line)
{
    // platform trace log
    pal::library::TraceMessage(U("FoundationAssert->Hr:%08x msg:%s file:%s line:%d\n"), _hResult, _Message, _File, _Line);

    if (_sAssertCallback != nullptr)
    {
        // invoke custom callback
        (*_sAssertCallback)(_hResult, _Message, _File, _Line);
    }

#if defined(DEBUG)

    if ((CDebugUtil::GetDebugUtilOptionsFlagsInternal() & foundation::library::_OptionIsAssertDisabled) == 0)
    {
#ifdef _WINDOWS
        _wassert(_Message, _File, _Line);
#else
        // TODO: figure the right CRT to call
        assert(false);
#endif
    }
#endif
}



#endif
