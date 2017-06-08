/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CodeGenUtil.cpp
****/

#include "CodeGenUtil.h"
#include "StringUtils.h"

#include <stdarg.h>

extern "C"
{
    #include "../md5/md5.h"
}

#if defined(_WINDOWS)
#include "Shlwapi.h"

#define _VSWPRINTF _vsnwprintf_s
#define _SWPRINTF _snwprintf_s
#define _TRUNCATE_PARAMETER _TRUNCATE,
#else
#define _VSWPRINTF vswprintf
#define _SWPRINTF swprintf
#define _TRUNCATE_PARAMETER
#endif

void _throwError(const wchar_t * _Format,...)
{
    static wchar_t buffer[2048];

    va_list argList;
    va_start( argList, _Format );     /* Initialize variable arguments. */
    _VSWPRINTF(
        buffer, 
        2048, 
        _TRUNCATE_PARAMETER
        _Format, 
        argList);

    throw std::wstring(buffer);
}


void GetNamespaceAndType(const wchar_t *type,std::wstring& _namespace,std::wstring& typeName)
{
    std::wstring str(type);

     size_t pos = str.find_last_of(L".");
     if(std::wstring::npos != pos)
     {
         _namespace = str.substr(0, pos);
         typeName = str.substr(pos + 1);
     }
     else
     {
         _namespace = L"";
        typeName = str;
     }
}

bool isAbsolutePath(const wchar_t *filepath)
{
#if defined(_WINDOWS) && defined(__UTF16_STRINGS)
    return PathIsRelative(filepath) ? false:true;
#else
    return filepath[0] == L'/';
#endif
}
