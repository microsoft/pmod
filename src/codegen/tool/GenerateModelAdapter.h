/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateModelAdapter.h
****/


#include "GenerateCppBase.h"
#include <sstream>

class CGenerateModelAdapter : public CGenerateCppBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) override;
    void codeGenerate(const schema::CCommandTypeInfo *pCommmandModelInfo) override;
    void codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo) override;
    void codeGenerate(const schema::CObjectDispatchInfo *pObjectDispatchInfo) override;

private:
    void emitMethods(
        const std::wstring& typeInfoName,
        const schema::CObjectDispatchInfo *pPropertyModelInfo);

    void emitPropertyInfo(
        const std::wstring& typeInfoName,
        const schema::CPropertyTypeInfo *pTypeInfo,
        const std::wstring& name,
        UINT32 flags);
    void emitMethodDeclare(std::wostream& os,const schema::CMethodInfo*pMethodInfo);
    void emitResultTypeInfo(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isAsync);

    std::wstring toTypeInfoConverter(const schema::CPropertyTypeInfo *pPropertyTypeInfo);
    std::wstring toTypeInfoConverter(const schema::CPropertyTypeInfo *pPropertyTypeInfo,bool isAsync);
private:
    std::wostringstream _osModelAdapterForwardInc;
    std::wostringstream _osModelAdapterCommandForwardInc;
    std::wostringstream _osModelAdapterInc;
    std::wostringstream _osModelAdapterSrc;
    std::wostringstream _osModelAdapterFactoryEntries;

    std::wstring _modelAdapterApi;
};


