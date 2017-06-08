/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateNetInterop.h
****/


#include "GenerateCSharpBase.h"
#include <sstream>

class CGenerateNetInterop : public CGenerateCSharpBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo)override;
    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) override;
    void codeGenerate(const schema::CCommandTypeInfo *pCCommandModelInfo) override;
    void codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo) override;

private:
    std::wstring toCSharpModelTypeName(const schema::CTypeInfo*pModelTypeInfo,bool isTypeOf);
    std::wstring toCSharpPropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,bool isTypeOf);
    std::wstring toAsyncOperationTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo,bool isTypeOf);
    std::wstring toTaskTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo, bool isTypeOf);
    static std::wstring toCSharpType(foundation::PropertyType propertyType);
private:
    std::wostringstream _osCSharpInteropEnum;
    std::wostringstream _osCSharpInteropSrc;
    std::wostringstream _osFactoryTypeEntrySrc;

};


