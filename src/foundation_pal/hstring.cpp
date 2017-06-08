/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:hstring.cpp
****/
#include "hstring.h"

#include "foundation/pal.h"
#include "pal_str.h"

#include <foundation/macros.h>
#include <foundation/assert.h>
#include <wchar.h>

// Note: uncomment this next line if you want to use standard C allocation
// for CHAR_t allocation
//#define _USE_NON_PAL_ALLOC

#if defined(_USE_NON_PAL_ALLOC)
#define _HSTRING_ALLOC malloc
#define _HSTRING_FREE free
#else
#define _HSTRING_ALLOC _pal_MemAlloc
#define _HSTRING_FREE _pal_MemFree

#endif

class _hstring
{
public:
    static _hstring *CastTo(_In_ HSTRING string)
    {
        foundation_assert(string);
        _hstring *hstr = reinterpret_cast<_hstring *>(string);
        if(hstr->_nullId == 0)
        {
            return hstr;
        }
        // it is a String ref created
        return nullptr;
    }

    _hstring():
		_nullId(0),
		_ref(1),
		_length(0),
		_buffer(nullptr)
    {
    }

	HRESULT _Initialize(LPCSTR_t sourceString, UINT32 length)
	{
		ULONG size = sizeof(CHAR_t)*(length + 1);
		CHAR_t *pBuffer = (CHAR_t *)_HSTRING_ALLOC(size);
		IFCPTR_ASSERT(pBuffer);
		_pal_strcpy_s(pBuffer, length + 1, sourceString);
		pBuffer[length] = 0;
		_length = length;
		_buffer = (LPCSTR_t)pBuffer;
		return S_OK;
	}

    LPCSTR_t data()
    {
        return _buffer;
    }

    UINT32 length()
    {
        return _length;
    }
    _pal_AtomicIntegerType addRef()
    {
        return _pal_InterlockedIncrement(&_ref);
    }

    _pal_AtomicIntegerType Release()
    {
        _pal_AtomicIntegerType ref = _pal_InterlockedDecrement(&_ref);
        if(ref == 0)
        {
            _HSTRING_FREE((LPVOID)_buffer);
            delete this;
        }
        return ref;
    }
private:
    CHAR_t                      _nullId;
    _pal_AtomicIntegerType      _ref;
    LPCSTR_t                    _buffer;
    UINT32                      _length;
};


HRESULT CreateString(LPCSTR_t sourceString,UINT32 length,HSTRING *string)
{
    if(sourceString && length)
    {
		_hstring *phstr = new _hstring();
		_IFR_(phstr->_Initialize(sourceString, length));
        *string = reinterpret_cast<HSTRING>(phstr);
    }
    else
    {
        *string = nullptr;
    }
    return S_OK;
}

HRESULT DeleteString(HSTRING string)
{
    if(string != nullptr)
    {
        _hstring *hstr = _hstring::CastTo(string);
        if(hstr)
        {
            hstr->Release();
        }
    }
    return S_OK;
}

HRESULT DuplicateString(HSTRING string,HSTRING *newString)
{
    IFCPTR_ASSERT(newString);
    if(string != nullptr)
    {
        _hstring *hstr = _hstring::CastTo(string);
        if(hstr)
        {
            hstr->addRef();
        }
        else
        {
            // string passed is a String Ref
            LPCSTR_t sourceString = reinterpret_cast<LPCSTR_t>(string);
            return CreateString(
                sourceString,
                (UINT32)_pal_strlen(sourceString),
                newString);
        }
    }
    *newString = string;
    return S_OK;
}

UINT32 GetStringLen(HSTRING string)
{
    if(string != nullptr)
    {
        _hstring *hstr = _hstring::CastTo(string);
        if(hstr)
        {
            return hstr->length();
        }
        else
        {
            return (UINT32)_pal_strlen(reinterpret_cast<LPCSTR_t>(string));
        }
    }
    return 0;
}

LPCSTR_t GetStringRawBuffer(HSTRING string,UINT32 *length)
{
    if(string != nullptr)
    {
        _hstring *hstr = _hstring::CastTo(string);
        if(length)
        {
            *length = hstr ?  hstr->length():(UINT32)_pal_strlen(reinterpret_cast<LPCSTR_t>(string));
        }
        return hstr ?  hstr->data() : reinterpret_cast<LPCSTR_t>(string);
    }
    if(length)
    {
        *length = 0;
    }
    static LPCSTR_t emptyStr = U("");
    return emptyStr;
}

bool IsStringEmpty(HSTRING string)
{
    return string == nullptr || GetStringLen(string) == 0;
}

HRESULT CompareStringOrdinal(HSTRING string1, HSTRING string2, INT32 *result)
{
    *result = _pal_strcmp(GetStringRawBuffer(string1,nullptr),GetStringRawBuffer(string2,nullptr));
    return S_OK;
}

HRESULT CreateStringReference(LPCSTR_t sourceString,UINT32 length,HSTRING_HEADER * hstringHeader,HSTRING * string)
{
    if(sourceString && length)
    {
        foundation_assert(*sourceString != 0);
        *string = reinterpret_cast<HSTRING>((CHAR_t *)sourceString);
    }
    else
    {
        *string = nullptr;
    }
    return S_OK;
}

