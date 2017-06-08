/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ToolUtil.cpp
****/

#include "ToolUtil.h"
#include "CodeGenUtil.h"
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <algorithm>

#if defined(_WINDOWS)
#include <io.h>
#define O_RDONLY    _O_RDONLY
#define open        _open
#define CLOSE_FILE       _close
#define stat             _stat
#define fstat            _fstat
#else
#define CLOSE_FILE       close
#include <unistd.h>
#endif

using namespace foundation::library;

// Parse an array of parameters looking for "-pProperty=Value
void CreateParameters(int argc, const string_tool_t::traits_type::char_type * argv[],_ToolParameterType& toolParameters)
{
    assert(argc > 0);
    
    bool printParameters = false;

    toolParameters[__TOOLSTR("exe")] = argv[0];
    
    for(int i = 1;i< argc; ++i)
    {
        string_tool_t next = argv[i];
        if(next == __TOOLSTR("-printParameters"))
        {
            printParameters = true;
            continue;
        }
        size_t pos;
        if(next.size() > 2 && next[0]== __TOOLSTR('-') && next[1]== __TOOLSTR('p') &&
            (pos=next.find(__TOOLSTR("="))) != std::wstring::npos)
        {
            string_tool_t parameterName = next.substr(2,pos-2);
            string_tool_t parameterValue = next.substr(pos + 1);
            if ((i + 1) == argc)
            {
                parameterValue.erase(std::remove(parameterValue.begin(), parameterValue.end(), L'\n'), parameterValue.end());
            }

			replace(parameterValue, __TOOLSTR("&lt;"), __TOOLSTR("<"));
            replace(parameterValue, __TOOLSTR("&gt;"), __TOOLSTR(">"));

            // add to the tool parameters
            toolParameters[parameterName] = parameterValue;
            
            if(printParameters)
            {
#ifdef _TOOL_UTIL_UTF8_
                printf("Parameter Name:%s Value:%s\n", parameterName.data(), parameterValue.data());
#else
                wprintf(L"Parameter Name:%ls Value:%ls\n",parameterName.data(),parameterValue.data());
#endif
            }
        }
    }
}

string_tool_t GetParameterValue(
                              const _ToolParameterType& toolParameters,
                              const string_tool_t::traits_type::char_type *parameterName,
                              const string_tool_t::traits_type::char_type *pDefaultValue)
{
    _ToolParameterType::const_iterator iter = toolParameters.find(parameterName);
    if(iter != toolParameters.end())
    {
        return (*iter).second;
    }
    if(!pDefaultValue)
    {
        string_tool_t err = 
#if defined(_TOOL_UTIL_UTF8_)
            string_util::format
#else
            string_util::format_w
#endif
            (__TOOLSTR("Parameter:'{0}' missing"), parameterName, nullptr);
        throw err;
    }
    return pDefaultValue;
}

void ReplaceProperties(const _ToolParameterType& toolParameters, string_tool_t& value)
{
    std::vector<string_tool_t> propertiesFound;

    string_tool_t::size_type posStart = 0;
    while ((posStart = value.find(__TOOLSTR("$("), posStart)) != string_tool_t::npos ) {
        string_tool_t::size_type posEnd = value.find(__TOOLSTR(")"), posStart + 1);
        if(posEnd != string_tool_t::npos)
        {
            propertiesFound.push_back(value.substr(posStart + 2,posEnd-posStart-2));
            posStart = posEnd;
        }
        else
            break;
    }
    if(propertiesFound.size())
    {
        for(std::vector<string_tool_t>::const_iterator iter = propertiesFound.begin();
            iter != propertiesFound.end();
            ++iter)
        {
            _ToolParameterType::const_iterator iter2 = toolParameters.find(*iter);
            if(iter2 != toolParameters.end())
            {
                string_tool_t tokenSearch = __TOOLSTR("$(") + *iter + __TOOLSTR(")");
                replace(value,tokenSearch,(*iter2).second);
            }
        }
    }
}


bool CheckFileDependencies(
    int _fh,
    const std::vector<std::string>& fileReferences)
{

    struct stat _fh_stat;
    fstat( _fh, &_fh_stat );

    for(std::vector<std::string>::const_iterator iter = fileReferences.begin();
        iter != fileReferences.end();
        ++iter)
    {
        int fh;
        std::string fileRelativPath = (*iter);

        if((fh = _pal_OpenReadOnlyFile(fileRelativPath.c_str())) ==  -1 )
        {
            _throwError(L"File:%s not found",fileRelativPath.c_str());
        }
        struct stat fh_stat;
        fstat( fh, &fh_stat );
        _pal_Close(fh);
        if(fh_stat.st_mtime > _fh_stat.st_mtime)
        {
            return false;
        }
    }
    return true;
}
int _pal_OpenReadOnlyFile(const char *filePath)
{
#if defined(_WINDOWS)
    int descr = -1;
    const int err = _sopen_s(&descr, filePath, O_RDONLY, _SH_DENYRW, 0 /* ignored */);
    if (err != 0)
    {
        return -1;
    }

    return descr;
#else
	return open(filePath, O_RDONLY);
#endif
}

void _pal_Close(int fh)
{
    CLOSE_FILE(fh);
}
