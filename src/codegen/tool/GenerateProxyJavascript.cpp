/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateProxyJavascript.cpp
****/

#include "GenerateProxyJavascript.h"
#include "guidhelper.h"

// Format template for prototype based object
// {0} = Type
// {1} = Base Type
// {2} = ctor initialize
const wchar_t *_definePrototype_Template = 
L"\n\
 function {0}() {\n\
    // define properties on this model\n\
{2}\n\
        {1}.call(this);\n\
    }\n\
\n\
    {0}.prototype = new {1};\n\
    {0}.prototype.constructor = {0};\n\
";


// Format template for Model creator entry
// {0} = IID type as text
// {1} = Javascript type
const wchar_t *_modelCreatorType_Template = 
L"\n\
    this['{0}'] = function() { return new {1}(); }\n\
";

// Format template ModelFactory creator for a namespace
// {0} = namespace
// {1} = factory type entries
const wchar_t *_modelFactoryCreator_Template = 
L"\n\
 function {0}_ModelFactoryCreator() {\n\
 {1}\n\
 }";

// Format template of a set property
// {0} = Type
// {1} = property name
// {2} = property id
const wchar_t *_setPropertyPrototype_Template = 
L"\n\
    {0}.prototype.set{1} = function (value,success, fail) {\n\
        this.setProperty({2}, value, success, fail);\n\
    }\n";

// Format template of a method invocation
// {0} = Type
// {1} = method name
// {2} = method id
// {3} = parameters
// {4} = comma sep or nothing
const wchar_t *_invokeMethodPrototype_Template = 
L"\n\
    {0}.prototype.{1} = function ({3}{4}success, fail,serializeOptions) {\n\
        this.invokeMethod(\n\
           {2},\n\
           [{3}],\n\
           success,\n\
           fail,\n\
           serializeOptions);\n\
    }\n";

void CGenerateProxyJavascript::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
}

void CGenerateProxyJavascript::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "jsProxyFile: jsvascript proxy file generated" << std::endl;
}

void CGenerateProxyJavascript::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring jsProxPathyFile = getStringParameter(parameters, L"jsProxyFile", nullptr);

    // generatefile
    std::wofstream fileJavascript(ToUtf8String(jsProxPathyFile.data()),std::ios_base::out);
    fileJavascript << _osTypes.str();
    std::wstring jsNamespace = this->getNamespace();
    replace(jsNamespace,L".",L"_");

    fileJavascript << FormatW(_modelFactoryCreator_Template,
        jsNamespace.c_str(),
        _osCreatorTypeFactories.str().c_str(),
        nullptr);
    fileJavascript.close();
}

std::wstring CGenerateProxyJavascript::toJsTypeName(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    if(pPropertyModelInfo == nullptr)
    {
        return L"PropertyModelProxy";
    }
    return toTypeInfoName(pPropertyModelInfo->getName()) + L"Proxy";
}

void CGenerateProxyJavascript::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    bool isViewModel = pPropertyModelInfo->getModelType() == schema::ModelTypeInfoType_ViewModel;

    std::wstring typeInfoName = toJsTypeName(pPropertyModelInfo);

    std::wostringstream osCtor;
    codeGenerateCtor(pPropertyModelInfo,osCtor);

    std::wstring baseType;
    if(pPropertyModelInfo->getBaseType())
    {
        baseType = toJsTypeName((const schema::CObservableObjectInfo *)pPropertyModelInfo->getBaseType());
    }
    else
    {
        baseType = toJsTypeName(nullptr);
    }

    _osTypes << FormatW(_definePrototype_Template,
                typeInfoName.data(),
                baseType.data(),
                osCtor.str().data(),
                nullptr);

    // Iterate Properties
    for (schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        std::wostringstream osPropertyId;
        osPropertyId << (*iter)->getId();

        UINT32 flags = (*iter)->getFlags();
        if ((flags & PropertyFlagType_CanWrite) != 0)
        {
            _osTypes << FormatW(_setPropertyPrototype_Template,
                typeInfoName.data(),
                (*iter)->getName().data(),
                osPropertyId.str().data(),
                nullptr);
        }
    }
    // Iterate Methods
    for(schema::_MethodIteratorType::const_iterator iter = pPropertyModelInfo->getMethods().begin();
        iter != pPropertyModelInfo->getMethods().end();
        ++iter)
    {
        std::wostringstream osParameters;
        for(schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            if(iterParam != (*iter)->getParameters().begin())
            {
                osParameters << ",";
            }
            std::wstring parameterName = (*iterParam)->getParameterName();
            if(parameterName == L"var") {
                parameterName = L"_var";
            }
            osParameters << parameterName;
        }
        std::wostringstream osMethodId;
        osMethodId << (*iter)->getId();

        _osTypes << FormatW(_invokeMethodPrototype_Template,
                typeInfoName.data(),
                (*iter)->getName().data(),
                osMethodId.str().data(),
                osParameters.str().data(),
                (*iter)->getParameters().size() ? L",":L"",
                nullptr);
    }

    std::wstring iidTypeStr = ToWideString(pPropertyModelInfo->getType());

    _osCreatorTypeFactories << FormatW(_modelCreatorType_Template,
        iidTypeStr.data(),
        typeInfoName.data(),
        nullptr);
}
