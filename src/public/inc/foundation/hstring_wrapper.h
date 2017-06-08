/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:hstring_wrapper.h
****/
#pragma once

#include <foundation/assert.h>
#include <wchar.h>

#include "foundation/pal.h"
#include "foundation/string_t.h"

namespace foundation
{

/*---------------------------------------------------------------------------
    class HStringWrapper

    Common initializations, typecasts and operators for HString wrapper
    classes.  It takes a helper class which needs to implement:
        // init from another string
        void InitFromRef(const HStringWrapper<HstringRefHelper>& from) throw()

        // init from a string and count, supports null as input
        void SetToWz(_In_z_ _In_count_(cch) const wchar_t *wz, _In_range_(>,0) int cch) throw()

        // steal the contents of the other class
        void StealString(HStringWrapper<HstringRefHelper>&& fromTemp) throw()

        // free the string and set it to null
        void FreeHstring()
---------------------------------------------------------------------------*/
template <class T>
class HStringWrapper : public T
{
public:
    // CONSTRUCTORS ---------------------------------------------------------
    HStringWrapper() throw()
    {
        this->m_hs = nullptr;
    }

    explicit HStringWrapper(HSTRING hs)
    {
        _pal_DuplicateString(hs, &this->m_hs);
    }

    explicit HStringWrapper(LPCSTR_t sz) throw()
    {
        this->m_hs = nullptr;
        InitFromString(sz, -1);
    }

#ifdef __UTF16_STRINGS
    explicit HStringWrapper(const char *pUtf8) throw()
    {
        this->m_hs = nullptr;
        this->init_from_utf8(pUtf8);
    }
#endif

    explicit HStringWrapper(LPCSTR_t sz,int cch) throw()
    {
        this->m_hs = nullptr;
        InitFromString(sz, cch);
    }

    HStringWrapper(const HStringWrapper<T>& from) throw()
    {
        this->m_hs = nullptr;
        T::InitFromRef(from);
    }

#if 0
    HStringWrapper(HStringWrapper<T>&& fromTemp) throw()
    {
        StealString(fromTemp);
    }
#endif

    // DESTRUCTOR ------------------------------------------------------------
    ~HStringWrapper() throw()
    {
        this->FreeHstring();
    }

    // ASSIGNMENT OPERATORS --------------------------------------------------
    HStringWrapper<T>& operator=(HSTRING from) throw()
    {
        this->FreeHstring();
        _pal_DuplicateString(from, &this->m_hs);
        return *this;
    }

    HStringWrapper<T>& operator=(const HStringWrapper<T>& from) throw()
    {
        if (this != &from)
            T::InitFromRef(from);
        return *this;
    }

    HStringWrapper<T>& operator=(LPCSTR_t sz) throw()
    {
        InitFromString(sz, -1);
        return *this;
    }

#ifdef __UTF16_STRINGS
    HStringWrapper<T>& operator=(const char *pUtf8) throw()
    {
        init_from_utf8(pUtf8);
        return *this;
    }
#endif

    // CASTS/CONVERSIONS -----------------------------------------------------

    operator const HSTRING() const throw()
    {
        return this->m_hs;
    }

    HSTRING const * GetAddressOf() const throw()
    {
        foundation_assert(this->m_hs == nullptr);
        return &this->m_hs;
    }

    HSTRING* GetAddressOf() throw()
    {
        foundation_assert(this->m_hs == nullptr);
        return &this->m_hs;
    }
    
    HSTRING const * GetAddressOfPtr() const throw()
    {
        return &this->m_hs;
    }

    HSTRING* GetAddressOfPtr() throw()
    {
        return &this->m_hs;
    }

    HSTRING* ReleaseAndGetAddressOf() throw()
    {
        this->FreeHstring();
        return &this->m_hs;
    }

    LPCSTR_t GetRawBuffer() const throw()
    {
        return _pal_GetStringRawBuffer(this->m_hs, nullptr);
    }

    // HELPERS ---------------------------------------------------------------

    UINT32 GetLength() const throw()
    {
        return (this->m_hs != nullptr)
            ? _pal_GetStringLen(this->m_hs)
            : 0;
    }

    void Clear() throw()
    {
        this->FreeHstring();
    }

    bool Empty() const throw()
    {
        return (this->m_hs == nullptr || _pal_IsStringEmpty(this->m_hs));
    }

    HRESULT CopyFrom(LPCSTR_t sz) throw()
    {
        return InitFromString(sz, -1) ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT CopyFrom(const HSTRING) throw();

    HRESULT CopyTo(HSTRING *hstr) throw();

protected:
    bool InitFromString(LPCSTR_t sz, int cch) throw()
    {
        this->FreeHstring();

        if (sz != nullptr)
        {
            if (cch < 0)
                cch = static_cast<int>(_pal_strlen(sz));

            return this->CreateString(sz, cch);
        }

        return true;
    }
#ifdef __UTF16_STRINGS
    bool init_from_utf8(const char *pUtf8) throw()
    {
        this->FreeHstring();
        if (pUtf8 == nullptr)
        {
            return true;
        }
        LPCSTR_t sz;
        bool free_buffer = this->FromUtf8(pUtf8, sz);
        bool result = this->InitFromString(sz, -1);
        if (free_buffer)
        {
            _pal_MemFree((LPVOID)sz);
        }
        return result;
    }
#endif
};

/*---------------------------------------------------------------------------
    class HstringPtrHelper

    helper to work with HSTRINGS that duplicate memory into the string.
---------------------------------------------------------------------------*/
class HstringPtrHelper
{
protected:
    HstringPtrHelper() throw() : m_hs(nullptr) { }
    ~HstringPtrHelper() throw() { }

    // PROTECTED INITIALIZATION HELPERS --------------------------------------

    void InitFromRef(const HStringWrapper<HstringPtrHelper>& from) throw()
    {
        FreeHstring();
        if (from.m_hs != nullptr)
            _pal_DuplicateString(from.m_hs, &m_hs);
    }

#ifdef __UTF16_STRINGS
    bool FromUtf8(const char *pUtf8, LPCSTR_t& sz) throw()
    {
        sz = to_pal_wstring(pUtf8).Detach();
        return true;
    }
#endif

#if 0
    void StealString(HStringWrapper<HstringPtrHelper>&& fromTemp) throw()
    {
        FreeHstring();
        m_hs = fromTemp.m_hs;
        fromTemp.m_hs = nullptr;
    }
#endif

    bool CreateString(LPCSTR_t sz, int cch) throw()
    {
        return SUCCEEDED(_pal_CreateString(sz, cch, &m_hs));
    }

    void FreeHstring() throw()
    {
        if (m_hs != nullptr)
        {
            _pal_DeleteString(m_hs);
            m_hs = nullptr;
        }
    }

    // PROTECTED MEMBERS -----------------------------------------------------

    HSTRING			m_hs;
};
/*---------------------------------------------------------------------------
    class HstringRefHelper

    helper to work with HSTRINGs that are references to existing strings.
    No additional allocations should be done when using this class
---------------------------------------------------------------------------*/
class HstringRefHelper
{
protected:
    HstringRefHelper() throw() : m_hs(nullptr) { }
    ~HstringRefHelper() throw() { }

    // PROTECTED INITIALIZATION HELPERS --------------------------------------
    
    void InitFromRef(const HStringWrapper<HstringRefHelper>& from) throw()
    {
        m_hs = from.m_hs;
        m_hsHeader = from.m_hsHeader;
    }

#ifdef __UTF16_STRINGS
    bool FromUtf8(const char *pUtf8, LPCSTR_t& sz) throw()
    {
        _buffer.attach(to_pal_wstring(pUtf8).Detach());
        sz = _buffer.get();
        return false;
    }
#endif

    bool CreateString(LPCSTR_t sz,int cch) throw()
    {
        return SUCCEEDED(_pal_CreateStringReference(sz, cch, &m_hsHeader, &m_hs));
    }
    
    void FreeHstring() throw()
    {
        // nothing needs to be freed in this case
        m_hs = nullptr;
    }
    // PROTECTED MEMBERS -----------------------------------------------------
    
    HSTRING			m_hs;
    HSTRING_HEADER	m_hsHeader;

#ifdef __UTF16_STRINGS
private:
    pal_wstring _buffer; // this will hold a temporary buffer to backup the UTF-8 conversion
#endif
};
 
template <>
inline HRESULT HStringWrapper<HstringPtrHelper>::CopyFrom(const HSTRING hs) throw()
{
    this->FreeHstring();
    return _pal_DuplicateString(hs, &this->m_hs);
}
template <>
inline HRESULT HStringWrapper<HstringPtrHelper>::CopyTo(HSTRING *hstr) throw()
{
    return _pal_DuplicateString(this->m_hs,hstr);
}

// smart pointer for _pal_CreateStringReference
typedef HStringWrapper<HstringRefHelper> HStringRef;

// smart pointer for _pal_CreateString
typedef HStringWrapper<HstringPtrHelper> HStringPtr;

inline HStringPtr CreateAttachedHstring(HSTRING hs)
    {
        HStringPtr hstr;
        *hstr.GetAddressOfPtr() = hs;
        return hstr;
    }
}
