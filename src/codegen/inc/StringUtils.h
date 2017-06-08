/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:StringUtils.h
****/
#pragma once

#include <string>
#include <string.h>
#include <foundation/library/string_util.h>
#include "GuidHelper.h"

class WStringLessThan: public std::binary_function<std::wstring, std::wstring, bool>
{
public:
    bool operator () (const std::wstring& str1, const  std::wstring& str2) const
    {
        return wcscmp(str1.c_str(), str2.c_str()) < 0;
    }
};

class _StringLessThan: public std::binary_function<std::string, std::string, bool>
{
public:
    bool operator () (const std::string& str1, const  std::string& str2) const
    {
        return strcmp(str1.c_str(), str2.c_str()) < 0;
    }
};

inline void replace(std::wstring& str, const std::wstring& search, const std::wstring& replace)
{
	return foundation::library::string_util::find_and_replace(str,search,replace);
}

inline void replace(std::string& str, const std::string& search, const std::string& replace)
{
    return foundation::library::string_util::find_and_replace(str, search, replace);
}

inline std::wstring ToWideString(const char *p) 
{
	return foundation::to_pal_wstring(p).get();
}

inline std::wstring ToWideString(const IID& iid)
{
	return ToWideString(::ToString(iid).c_str());
}

inline std::string ToUtf8String(const wchar_t *pString)
{
	return foundation::to_utf8_string(pString);
}

inline std::string ToUtf8String(HSTRING hstr)
{
	return foundation::to_utf8_string(hstr);
}

inline std::wstring ToLowerCase(const wchar_t *pString)
{
	std::wstring temp(pString);
	foundation::library::string_util::to_lower(temp);
	return temp;
}

inline  std::wstring ToLowerCaseConvention(const wchar_t *pString)
{
	return foundation::library::string_util::to_lower_case_convention_w(pString);
}

inline  std::wstring ToPrivateLowerCaseConvention(const wchar_t *pString)
{
	std::wstring str = ToLowerCaseConvention(pString);
	str[0] = toupper(str[0]);
	return str;
}

inline std::wstring ToHexString(unsigned int value)
{
	std::wostringstream os;
	os << std::hex << value;
	return os.str();
}

#define FormatW	foundation::library::string_util::format_w
#define FindAndReplace	foundation::library::string_util::find_and_replace

#define SplitW foundation::library::string_util::split_w
