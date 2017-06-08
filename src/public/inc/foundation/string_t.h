/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:string_t.h
****/
#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "pal.h"
#include "pal_buffer.h"

namespace foundation
{

// the cross platform STL classes
#ifdef __UTF16_STRINGS
    typedef std::wstring string_t;
    typedef std::wostringstream ostringstream_t;
    typedef std::wistringstream istringstream_t;
#else
    typedef std::string string_t;
    typedef std::ostringstream ostringstream_t;
    typedef std::istringstream istringstream_t;
#endif

    typedef PalBuffer<wchar_t> pal_wstring;

    inline LPCSTR_t get_raw_buffer(HSTRING hstr)
    {
        return _pal_GetStringRawBuffer(hstr,nullptr);
    }

    inline string_t get_string_t(HSTRING hstr)
    {
        return _pal_GetStringRawBuffer(hstr,nullptr);
    }

    inline std::string to_utf8_string(const wchar_t* pString)
    {
        std::string buffer;
        size_t size = _pal_wcslen(pString);
        size_t size_buffer = _pal_wcstombs(nullptr, 0, pString, size);

        buffer.reserve(size_buffer + 1);

        _pal_wcstombs(&buffer[0], size_buffer + 1,pString, size);
        return std::string(buffer.data(), size_buffer);
    }

    inline std::string to_utf8_string(const string_t& s)
    {
    #ifdef __UTF16_STRINGS
        return to_utf8_string(s.c_str());
    #else
        return s;
    #endif
    }

    inline std::string to_utf8_string(HSTRING hstr)
    {
        LPCSTR_t pstr_t = get_raw_buffer(hstr);
    #ifdef __UTF16_STRINGS
        return to_utf8_string(pstr_t);
    #else
        return pstr_t;
    #endif
    }

    template <class _Container>
    std::vector<std::string>& to_utf8_string_values(
        const _Container& values,
        std::vector<std::string>& result)
    {
        result.clear();
        for(typename _Container::const_iterator iter = values.begin();
            iter != values.end();
            ++iter)
        {
            result.push_back(to_utf8_string(*iter));
        }
        return result;
    }

    inline pal_wstring to_pal_wstring(const char *pUtf8)
    {
        size_t size = strlen(pUtf8);
        size_t size_buffer = _pal_mbstowcs(nullptr, 0, pUtf8, size);

        pal_wstring buffer((UINT32)(size_buffer + 1));

        _pal_mbstowcs(buffer.get(), size_buffer + 1, pUtf8, size + 1);
        return pal_wstring(buffer.Detach());
    }


    inline std::wstring to_wstring(LPCSTR_t pStr)
    {
#ifdef __UTF16_STRINGS
        return pStr;
#else
        return to_pal_wstring(pStr).get();
#endif
    }

    inline std::wstring to_wstring(HSTRING hstr)
    {
        return to_wstring(get_raw_buffer(hstr));
    }

    inline string_t to_platform_string(const char *pUtf8)
    {
    #ifdef __UTF16_STRINGS
        return (LPCSTR_t)to_pal_wstring(pUtf8);
    #else
        return pUtf8;
    #endif
    }

    inline string_t to_platform_string(const wchar_t *p_wstr)
    {
#ifdef __UTF16_STRINGS
        return p_wstr;
#else
        return to_utf8_string(p_wstr);
#endif
    }

}
