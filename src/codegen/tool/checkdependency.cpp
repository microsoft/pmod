/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:checkdependency.cpp
****/
#include <stdio.h>

#include "ToolUtil.h"
#include "CodeGenUtil.h"
#include "StringUtils.h"

static void getFiles(_ToolParameterType& parameters,
              const wchar_t *parameterName,
              std::vector<std::string>& files)
{
    std::wstring filesParam = GetParameterValue(parameters,parameterName,nullptr);

    std::vector<std::wstring> wfiles = split(filesParam, L',');
    for(std::vector<std::wstring>::const_iterator iter = wfiles.begin();
        iter != wfiles.end();
        ++iter)
    {
        files.push_back(ToUtf8String((*iter).c_str()));
    }
}

int _checkdependency(int argc, const wchar_t* argv[])
{
    wprintf(L"check dependency tool\n");
    try
    {
        _ToolParameterType parameters;
        CreateParameters(argc,argv,parameters);

        std::vector<std::string> outputFiles,dependencyFiles;

        getFiles(parameters,L"outputfiles",outputFiles);
        getFiles(parameters,L"dependencyfiles",dependencyFiles);

        for(std::vector<std::string>::const_iterator iter = outputFiles.begin();
            iter != outputFiles.end();
            ++iter)
        {
            int fh = _pal_OpenReadOnlyFile((*iter).c_str());
            if(fh !=  -1 )
            {
                bool result = CheckFileDependencies(fh,dependencyFiles);
                _pal_Close(fh);
                if(!result)
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        // indication that is up to date
        return 1;
    }
    catch(std::wstring& error)
    {
        wprintf(error.data());
        return -1;
    }    
    return 0;
}

