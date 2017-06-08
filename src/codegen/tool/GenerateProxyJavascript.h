/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateProxyJavascript.h
****/
#pragma once

#include "CodeGenToolBase.h"
#include <sstream>

class CGenerateProxyJavascript : public CCodeGenToolBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;

    // Overridables methods
    virtual void codeGenerateCtor(const schema::CObservableObjectInfo *pPropertyModelInfo,std::wostringstream& osCtor) {};
    virtual std::wstring toJsTypeName(const schema::CObservableObjectInfo *pPropertyModelInfo);
private:
    std::wostringstream _osTypes;
    std::wostringstream _osCreatorTypeFactories;
};


