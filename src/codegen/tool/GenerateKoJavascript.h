/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateKoJavascript.h
****/


#include "GenerateProxyJavascript.h"

class CGenerateKoJavascript : public CGenerateProxyJavascript
{
protected:
    void codeGenerateCtor(const schema::CObservableObjectInfo *pPropertyModelInfo,std::wostringstream& osCtor) override;
    std::wstring toJsTypeName(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
};


