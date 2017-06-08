/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCSharpTypeInfo.h
****/


#include "GenerateCSharpBase.h"
#include <sstream>

class CGenerateCSharpTypeInfo : public CGenerateCSharpBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CTypeInfo *pModelTypeInfo);
    void codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo) override;
    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) override;
    void codeGenerate(const schema::CCommandTypeInfo *pCCommandModelInfo) override;
    void codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo) override;

    std::wstring toIIdType(const schema::CTypeInfo *pModelTypeInfo);
    std::wstring toPropertyModelInfoRef(const schema::CTypeInfo *pModelTypeInfo);
private:
    std::wstring getPropertyTypeInfoCreator(const schema::CPropertyTypeInfo*pPropertyTypeInfo);

private:
    std::wostringstream _osModelTypes;
    std::wostringstream _osRegisterTypes;
    std::wstring        _typeInfoPostfix;
    bool                _exportModelMetadata;
};


