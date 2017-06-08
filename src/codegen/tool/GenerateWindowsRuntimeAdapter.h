/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateWindowsRuntimeAdapter.h
****/


#include "CodeGenToolBase.h"
#include <sstream>
#include <stdexcept>

class CGenerateWindowsRuntimeAdapter : public CCodeGenToolBase
{
public:
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo) override;
    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) override;
    void codeGenerate(const schema::CCommandTypeInfo *pCCommandModelInfo) override;
    void codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo) override;

private:
    std::wstring  toIdlModelTypeInfo(
        const schema::CTypeInfo*pModelTypeInfo);
    std::wstring  toIdlPropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    std::wstring  toNativePropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    std::wstring toNativeResultTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isAsync);
    std::wstring toNativeModelTypeName(const schema::CTypeInfo* pModelTypeInfo);
    std::wstring toNativeCollectionModelTypeName(const schema::CEnumerableTypeInfo* pCollectionModelInfo);
    std::wstring toNativeCommandModelTypeName(const schema::CCommandTypeInfo* pCommandModelInfo);

    std::wstring toIdlResultTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isAsync);

    std::wstring toIdlAsyncOperation(const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    std::wstring toNativeAsyncOperation(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    std::wstring toNativeFoundationAsyncOperation(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    std::wstring toNativeFoundationAsyncOperation(
        const char *prefix,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);

    std::wstring toIdlModelTypeName(const schema::CTypeInfo*pModelTypeInfo);
    std::wstring toNativeModelTypeName(const wchar_t *_namespace, const wchar_t * name);
    std::wstring getWindowsRuntimeNamespace();

    static std::wstring toIdlType(foundation::PropertyType propertyType);
    static std::wstring toIdlIIDType(GUID iidType);
    static std::wstring toIdlIIDType(const schema::CTypeInfo*pModelTypeInfo);
    static std::wstring toWindowsRuntimeCoreType(foundation::PropertyType propertyType);

    static IID createUniqueIId(const schema::CTypeInfo*pModelTypeInfo);
private:
    std::wostringstream _osInterfaceForwardIdl;
    std::wostringstream _osGenericVectorForwardIdl;
    std::wostringstream _osGenericEnumForwardIdl;
    std::wostringstream _osGenericAsyncOperationForwardIdl;
    std::wostringstream _osEnumsIdl;
    std::wostringstream _osInterfacesIdl;
	std::wostringstream _osRuntimeClassesIdl;
	std::wostringstream _osModelAdapterSrc;
    std::wostringstream _osModelAdapterFactoryEntries;
    foundation::library::DictionaryClassPtr _winrtNamespaceLookup;

    bool _isGenerateGenericVector;
    bool _isGenerateCommand;
    bool _isGenerateGenericAsyncOperation;
};


