/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:gccxml2chema.cpp
****/
#include <stdio.h>

#include "ToolUtil.h"
#include "GccXmlParser.h"

#include "StringUtils.h"

int _gccxml2schema(int argc, const wchar_t* argv[])
{
    if(argc > 1 && (wcscmp(argv[1],L"/?")==0 || wcscmp(argv[1],L"help")==0))
    {
        wprintf(L"Syntax:\n\
                 -printParameters\n\
                 -pxml=path to gccxml to parse\n\
                 -pnamespace=C++ namespace to look for\n\
                 -pinterfaces=list of interfaces to process\n\
                 -poutput=unified schema binary file output\n\
                    ");
        return 0;
    }

    try
    {
        _ToolParameterType parameters;
        CreateParameters(argc,argv,parameters);
        CGccXmlParser gccXmlParser;
        gccXmlParser.execute(parameters);
    }
    catch(std::wstring& error)
    {
        wprintf(error.data());
        return -1;
    }    
    return 0;
}

