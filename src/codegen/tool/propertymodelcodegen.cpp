/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:propertymodelcodegen.cpp
****/
#include <stdio.h>

#include "ToolUtil.h"
#include "CodeGenUtil.h"
// headers for supported codegen tools
#include "GenerateTypeInfo.h"
#include "GenerateModelAdapter.h"
#include "GenerateImplCppTemplate.h"
#include "GenerateFastCppTemplate.h"
#include "GenerateWindowsRuntimeAdapter.h"
#include "GenerateCSharpClasses.h"
#include "GenerateObjectiveC.h"
#include "GenerateJava.h"
#include "GenerateNetInterop.h"
#include "GenerateProxyJavascript.h"
#include "GenerateKoJavascript.h"
#include "GenerateCSharpTypeInfo.h"
#include "GeneratePtrClass.h"

#include <foundation/interfaces.h>
#include <foundation/com_ptr.h>
#include <foundation/library/json_statics_util.h>
#include <foundation/library/dictionary_class_ptr.h>
#include <foundation/hstring_wrapper.h>
#include <foundation/pv_util.h>
#include <foundation/exception.h>
#include <pal/pal_load_library.h>

using namespace foundation;
using namespace foundation::library;

template <class T>
void codegen(DictionaryClassPtr& parameters,bool isHelp)
{
    T tool;
    if(isHelp)
    {
        tool.help();
    }
    else
    {
        tool.start(parameters);
    }
}

// resolve dictionary values by replacing $(VAR_NAME) with the global scope key/value
void resolve_dictionary_values(
    DictionaryClassPtr& source,
    DictionaryClassPtr& target)
{
    DictionaryClassPtr::view kvp_view = target.getview();

    for (DictionaryClassPtr::view::const_iterator iter = kvp_view.begin();
        iter != kvp_view.end();
        ++iter)
    {
        PropertyType property_type;
        pv_util::GetType((*iter).second, &property_type);
        if (property_type != PropertyType_String)
        {
            continue;
        }

        HStringPtr value;
        pv_util::GetStringValue((*iter).second, value.GetAddressOf());
        string_t value_str = get_string_t(value);
        typedef std::vector<string_t> _vector_str_type;
        _vector_str_type vars;

        string_t::size_type posStart = 0;
        while ((posStart = value_str.find(U("$("), posStart)) != string_t::npos) {
            string_t::size_type posEnd = value_str.find(U(")"), posStart + 1);
            if (posEnd != string_t::npos)
            {
                vars.push_back(value_str.substr(posStart + 2, posEnd - posStart - 2));
                posStart = posEnd;
            }
            else
                break;
        }
        if (vars.size())
        {
            for (_vector_str_type::const_iterator iter2 = vars.begin();
                iter2 != vars.end();
                ++iter2)
            {
                value = source.Get<HStringPtr>((*iter2).c_str());

                string_t tokenSearch = U("$(") + *iter2 + U(")");
                string_util::find_and_replace(value_str, tokenSearch, get_string_t(value));

                target.Set(get_raw_buffer((*iter).first), value_str.c_str());
            }
        }

    }
}

// Invoke a code generation tool
void invoke_codegen(
    DictionaryClassPtr& parameters,
    const wchar_t *p_code_gen_type, 
    bool isHelp)
{
    wprintf(L"invoke_codegen -> %ls\n", p_code_gen_type);

    HStringPtr json_text;
    json_util::GetJsonStatics()->ToJson((IDictionary *)parameters, json_text.GetAddressOf());
    wprintf(L"%ls\n", to_wstring(json_text).c_str());


    if (wcscmp(p_code_gen_type, L"gentypeinfo") == 0)
    {
        codegen<CGenerateTypeInfo>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"genmodeladapter") == 0)
    {
        codegen<CGenerateModelAdapter>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"genmodelptr") == 0)
    {
        codegen<CGeneratePtrClass>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"genfastcpptemplate") == 0)
    {
        codegen<CGenerateFastCppTemplate>(parameters, isHelp);
    }
	else if (wcscmp(p_code_gen_type, L"genimplcpptemplate") == 0)
	{
		codegen<CGenerateImplCppTemplate>(parameters, isHelp);
	}
	else if (wcscmp(p_code_gen_type, L"genwinrtadapter") == 0)
    {
        codegen<CGenerateWindowsRuntimeAdapter>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"gencsharpclasses") == 0)
    {
        codegen<CGenerateCSharpClasses>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"gennsadapter") == 0)
    {
        codegen<CGenerateObjectiveC>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"genjava") == 0)
    {
        codegen<CGenerateJava>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"gennetinterop") == 0)
    {
        codegen<CGenerateNetInterop>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"genjsproxy") == 0)
    {
        codegen<CGenerateProxyJavascript>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"genkojsmodel") == 0)
    {
        codegen<CGenerateKoJavascript>(parameters, isHelp);
    }
    else if (wcscmp(p_code_gen_type, L"gencsharptypeinfo") == 0)
    {
        codegen<CGenerateCSharpTypeInfo>(parameters, isHelp);
    }
    else
    {
        _throwError(L"Unknown:'%ls' codegen selector", p_code_gen_type);
    }
}

void copy_tool_parameters(_ToolParameterType toolParameters, DictionaryClassPtr& target)
{
    for (_ToolParameterType::const_iterator iter = toolParameters.begin();
        iter != toolParameters.end();
        ++iter)
    {
        string_t key = to_platform_string(((*iter).first).c_str());
        string_t value = to_platform_string(((*iter).second).c_str());
        if (value == U("true") || value == U("false"))
        {
            target.Set(key.c_str(), value == U("true") ? true:false);
        }
        else
        {
            // a string value
            target.Set(key.c_str(), value.c_str());
        }
    }
}


int _propertymodelcodegen(int argc, const wchar_t* argv[])
{
    wprintf(L"propertymodelcodegen->");
    for(int i = 1;i< argc; ++i)
    {
#if _WINDOWS
        wprintf(L" %s",argv[i]);
#else
        wprintf(L" %S",argv[i]);
#endif
    }
    wprintf(L"\n");
    try
    {
        _ToolParameterType parameters;
        CreateParameters(argc,argv,parameters);

        if(argc < 2)
        {
            _throwError(L"Missing tool type definition");
        }
        if(wcscmp(argv[1],L"/?")==0 || wcscmp(argv[1],L"help")==0)
        {
            wprintf(L"Syntax:\n\
                        'codegenselector'  -printParameters -pParameterName=ParameterValue ...\n\
                        config_file:json config file\n\
                        with 'codegenselector':\n\
                        -printParameters:if print parameters passed\n\
                        gentypeinfo:Generate Type Info for Schema\n\
                        genmodeladapter:Generate Model Type adapters\n\
                        gencpptemplate:Generate C++ implementation template & wrappers\n\
                        genfastcpptemplate:Generate fast C++ implementation template\n\
                        genwinrtadapter:Generate idl & winrt adapters for WinRT projections \n\
                        gencsharpclasses:Generate C# classes for WinRT projections \n\
                        gennsadapter:Generate Objective C adapter classes \n\
                        gennetinterop:Generate .NET interop classes \n\
                        gencsharptypeinfo:Generate C# type info suport \n\
                     ");
            return 0;
        }
        bool isHelp = false;
        if(argc > 2 && (wcscmp(argv[2],L"/?")==0 || wcscmp(argv[2],L"help")==0))
        {
            isHelp = true;
        }

        _ToolParameterType::const_iterator iter = parameters.find(L"config_file");
        if (iter != parameters.end())
        {
            std::string json_file_path = ToUtf8String(((*iter).second).c_str());
            foundation::InspectablePtr spJsonObject;

            foundation::ComPtr<IDictionary> spError;
            HRESULT hr = json_util::GetJsonStatics()->ParseFromFile(
                json_file_path.c_str(),
                spError.GetAddressOf(),
                spJsonObject.GetAddressOf());
            if (FAILED(hr))
            {
                DictionaryClassPtr error(spError);
                INT32 line = error.Get<INT32>(U("Line"));
                INT32 column = error.Get<INT32>(U("Column"));
                INT32 position = error.Get<INT32>(U("Position"));

                _throwError(L"Failed to parse json config file.Line:%d Column:%d Position:%d",
                    line, column, position);
            }

            DictionaryClassPtr code_gen_parameters(spJsonObject);
            copy_tool_parameters(parameters, code_gen_parameters);

            // resolve value at the root level
            resolve_dictionary_values(code_gen_parameters, code_gen_parameters);

            LPCSTR_t codegen_key = U("codegen");
            if (!code_gen_parameters.HasKey(codegen_key))
            {
                _throwError(L"missing codegen section");
            }
            DictionaryClassPtr code_gen_tool = code_gen_parameters.Get<DictionaryClassPtr>(codegen_key);
            DictionaryClassPtr::view kvp_code_gen = code_gen_tool.getview();

            for (DictionaryClassPtr::view::const_iterator iter = kvp_code_gen.begin();
                iter != kvp_code_gen.end();
                ++iter)
            {
                string_t code_gen_tool_selector = get_string_t((*iter).first);

                string_t code_gen_tool_selector_run_key = code_gen_tool_selector + U(".run");

                if (code_gen_parameters.HasKey(code_gen_tool_selector_run_key.c_str()))
                {
                    bool will_run = code_gen_parameters.Get<bool>(code_gen_tool_selector_run_key.c_str());
                    if (!will_run)
                    {
                        continue;
                    }
                }
                DictionaryClassPtr code_gen_params((*iter).second);
                resolve_dictionary_values(code_gen_parameters, code_gen_params);
                invoke_codegen(code_gen_params,to_wstring((*iter).first).c_str(), false);
            }
        }
        else
        {
            DictionaryClassPtr code_gen_params;
            copy_tool_parameters(parameters, code_gen_params);
            invoke_codegen(code_gen_params, argv[1], isHelp);
        }
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
    return _propertymodelcodegen(argc, (const wchar_t* *)argv);
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
    return _propertymodelcodegen(argc, _argv.data());
}

#endif
