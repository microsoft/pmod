/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:gencsharpschema.cpp
****/
#include <stdio.h>

#include "SchemaParser.h"
#include "SchemaSerializer.h"
#include "ModelSchema.h"
#include "ToolUtil.h"
#include "StringUtils.h"

#include <foundation/interfaces.h>
#include <foundation/library/json_statics_util.h>
#include <foundation/com_ptr.h>
#include <foundation/library/dictionary_class_ptr.h>
#include <foundation/exception.h>
#include <pal/pal_load_library.h>

#include <fstream>

using namespace foundation;
using namespace foundation::library;

int _gencsharpschema(int argc, const wchar_t* argv[])
{
    if(argc > 1 && (wcscmp(argv[1],L"/?")==0 || wcscmp(argv[1],L"help")==0))
    {
        wprintf(L"Syntax:\n\
                 -printParameters\n\
                 -pschema=path to schema definition to parse\n\
                 -pgrammars=grammar directory\n\
                 -poutput=unified schema binary file output\n\
                 -pjson=true/false\n\
                 -pcheckdependencies=if file dependencies are checked\n\
                 -pschema_attributes=separate the schema definition from the shema files.\n\
                  ");
        return 0;
    }

    try
    {
        _ToolParameterType parameters;
        CreateParameters(argc,argv,parameters);

        DictionaryClassPtr other_attributes;
        _ToolParameterType::const_iterator iter = parameters.find(L"schema_attributes");
        // if found and path is not empty
        if (iter != parameters.end() && (*iter).second.length())
        {
            std::string json_file_path = ToUtf8String(((*iter).second).c_str());
            foundation::InspectablePtr spJsonObject;

            foundation::ComPtr<IDictionary> spError;
            HRESULT hr = foundation::json_util::GetJsonStatics()->ParseFromFile(
                json_file_path.c_str(),
                spError.GetAddressOf(),
                spJsonObject.GetAddressOf());
            if (FAILED(hr))
            {
                DictionaryClassPtr error(spError);
                INT32 line = error.Get<INT32>(U("Line"));
                INT32 column = error.Get<INT32>(U("Column"));
                INT32 position = error.Get<INT32>(U("Position"));

                _throwError(L"Failed to parse json attribute file.Line:%d Column:%d Position:%d",
                    line, column, position);
            }

            other_attributes = DictionaryClassPtr(spJsonObject);
        }

        std::string modelSchemaPath = ToUtf8String(GetParameterValue(parameters,L"schema",nullptr).c_str());
        std::string outputFile = ToUtf8String(GetParameterValue(parameters,L"output",nullptr).c_str());

        bool checkDependencies = GetParameterValue(parameters,L"checkdependencies",L"false") == L"true";

        if(checkDependencies)
        {
            int fh = _pal_OpenReadOnlyFile(outputFile.c_str());
            if(fh !=  -1 )
            {
                if(CSchemaParser::checkFileDependencies(fh,modelSchemaPath.data(),parameters))
                {
                    wprintf(L"Unified Scheme:%hs up to date\n",outputFile.c_str());
                    _pal_Close(fh);
                    return 1;
                }
                _pal_Close(fh);
            }
        }

        // parse the C# into a Unified Model Schema
        schema::CModelSchema *pModelSchema = CSchemaParser::parse(
            modelSchemaPath.data(),
            parameters,
            other_attributes);
        wprintf(L"generating unified shema ->%hs\n",outputFile.data());

        std::ios_base::openmode mode = std::ios_base::out;
        std::fstream fos(outputFile.data(),mode);
        // Serialize the parsed schema into a file
        CSchemaSerializer::serializeJson(*pModelSchema,fos);
        fos.close();
        // delete the created schema
        delete pModelSchema;
    }
    catch(std::wstring& error)
    {
        wprintf(string_util::format_w(L"Error:{0}\n", error.c_str(), nullptr).c_str());
        return -1;
    }    
    catch(foundation::com_exception& runtime_error)
    {
        wprintf(L"runtime error -> com_exception hr:%08x\n",runtime_error.result());
        return -1;
    }
    return 0;
}

#ifdef _WINDOWS
#include <tchar.h>
int _tmain(int argc, _TCHAR* argv[])
{
    return _gencsharpschema(argc, (const wchar_t* *)argv);
}

#else

int main(int argc, const char * argv[])
{
    // next block will convert to Utf-8 array
    std::vector<const wchar_t *> _argv;
    for (int i = 0; i < argc; ++i)
    {
        std::wstring arg = ToWideString(argv[i]);
        wchar_t *pWideStr = new wchar_t[arg.size() + 1];
        wcscpy(pWideStr, arg.data());
        _argv.push_back(pWideStr);
    }
    return _gencsharpschema(argc, _argv.data());
}

#endif

