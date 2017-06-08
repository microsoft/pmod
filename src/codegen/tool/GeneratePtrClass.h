/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GeneratePtrClass.h
****/
#pragma once

#include "GenerateCppBase.h"
#include <sstream>

class CGeneratePtrClass : public CGenerateCppBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) override;
    void codeGenerate(const schema::CCommandTypeInfo *pCommmandModelInfo) override;
    void codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo) override;

private:
    void emitComPtrClass(std::wostream& os, const schema::CTypeInfo *pModelTypeInfo);
    std::wstring toComPtrClass(const schema::CTypeInfo *pModelTypeInfo);
    std::wstring getCommandClassPtrName(const schema::CCommandTypeInfo *pCommmandModelInfo);
    static bool tryGetStandardCommandClassPtr(const schema::CCommandTypeInfo *pCommmandModelInfo, std::wstring& commandClassPtrName);

private:
    std::wostringstream _osPtrTypedefInc;
    std::wostringstream _osPtrForwardInc;
    std::wostringstream _osPtrClassInc;
    std::wostringstream _osPtrClassTypedefInc;
    std::wostringstream _osPtrClassInlineInc;
};


