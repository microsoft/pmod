/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ToolUtil.h
****/
#ifndef _TOOL_UTIL_DEFINED
#define _TOOL_UTIL_DEFINED

#include "StringUtils.h"

#include <vector>
#include <map>
#include <string>

#ifdef _TOOL_UTIL_UTF8_
typedef std::string string_tool_t;
#define __TOOLSTR(x) x
#else
typedef std::wstring string_tool_t;
#define __TOOLSTR(x) L ## x
#endif

#if !defined(U)
#define U(x) __XPLATSTR(x)
#endif


typedef std::map<string_tool_t, string_tool_t,
#ifdef _TOOL_UTIL_UTF8_
    _StringLessThan
#else
    WStringLessThan
#endif
> _ToolParameterType;

void CreateParameters(int argc, const string_tool_t::traits_type::char_type * argv[],_ToolParameterType& toolParameters);

string_tool_t GetParameterValue(
                              const _ToolParameterType& toolParameters,
                              const string_tool_t::traits_type::char_type *parameterName,
                              const string_tool_t::traits_type::char_type *pDefaultValue);
void ReplaceProperties(const _ToolParameterType& toolParameters, string_tool_t& value);

bool CheckFileDependencies(
    int _fh,
    const std::vector<std::string>& fileReferences);

// open/clsoe abstraction for Unix/Windows
int _pal_OpenReadOnlyFile(const char *filePath);
void _pal_Close(int fh);

#endif
