/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateKoJavascript.cpp
****/

#include "GenerateKoJavascript.h"

// Format template for Read Only property declared in a javascript object
// {0} = Property Name
const wchar_t *_koDefineReadOnlyPropertyObservable_Template = 
L"        this.{0} = ko.observable();\n\
";

void CGenerateKoJavascript::codeGenerateCtor(const schema::CObservableObjectInfo *pPropertyModelInfo,std::wostringstream& osCtor)
{
    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        const schema::CTypeInfo *pPropertyModelTypeInfo = (*iter)->getModelType();

        osCtor << FormatW(_koDefineReadOnlyPropertyObservable_Template,
            (*iter)->getName().data(),
            nullptr);
    }
}

std::wstring CGenerateKoJavascript::toJsTypeName(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    if(pPropertyModelInfo == nullptr)
    {
        return L"KoModelProxy";
    }
    return L"Ko" + toTypeInfoName(pPropertyModelInfo->getName()) + L"Proxy";
}
