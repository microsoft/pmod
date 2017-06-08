/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CodeGenToolBase.h
****/
#ifndef _CODE_GEN_TOOL_BASE_DEFINED
#define _CODE_GEN_TOOL_BASE_DEFINED

#include "ToolUtil.h"
#include "ModelSchema.h"
#include <foundation/library/dictionary_class_ptr.h>

#include <iostream>
#include <fstream>

class CCodeGenToolBase
{
public:
    void start(foundation::library::DictionaryClassPtr& parameters);
    void help(void);
protected:
	CCodeGenToolBase():
		_pModelSchema(nullptr)
	{}
    virtual ~CCodeGenToolBase();

    virtual void helpInternal(std::wostream& os);
    virtual void initialize(foundation::library::DictionaryClassPtr& parameters);
    virtual void codeGenerateStart(foundation::library::DictionaryClassPtr& parameters);
    virtual void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters);

    virtual void codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo) { UNREFERENCED_PARAMETER(pEnumTypeInfo); }

    virtual void codeGenerate(const schema::CTypeInfo *pModelTypeInfo);

	virtual void codeGenerate(const schema::CObjectDispatchInfo *pPropertyDataInfo) { UNREFERENCED_PARAMETER(pPropertyDataInfo); }
	virtual void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) { UNREFERENCED_PARAMETER(pPropertyModelInfo); }
    virtual void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) { UNREFERENCED_PARAMETER(pCollectionModelInfo); }
    virtual void codeGenerate(const schema::CCommandTypeInfo *pCCommandModelInfo) { UNREFERENCED_PARAMETER(pCCommandModelInfo); }
    virtual void codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo) { UNREFERENCED_PARAMETER(pAsyncOperationModelInfo); }
    virtual void codeGenerate(const schema::CPropertyViewModelInfo *pPropertyViewModelInfo) 
    { 
        this->codeGenerate( (schema::CObservableObjectInfo *)pPropertyViewModelInfo);
    }

    const schema::CModelSchema *getModelSchema() { return _pModelSchema; }

    std::wstring getNamespaceLookup(const std::wstring& namespaceStr);
    std::wstring getNamespaceLookup(const schema::CTypeInfo *pModelTypeInfo);
    std::wstring getNativeNamespaceLookup(const std::wstring& namespaceStr);
    std::wstring getNamespace();
    std::wstring getNativeNamespace();

    static std::wstring toNativeNamespace(const std::wstring& modelNamespace);
    static std::wstring toNativeNamespaceDeclare(const std::wstring& nativeNamespace);
    static std::wstring toNativeNamespaceEnd(const std::wstring& nativeNamespace);
    static std::wstring toNamespaceFullName(const std::wstring& nativeNamespace, const std::wstring& name);

    void emitModelTypeInfo(
        std::wostream& os,
        const schema::CTypeInfo*pModelTypeInfo);
    void emitPropertyTypeInfo(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    void emitPropertyTypeName(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    void emitDeclarePropertyTypeInfo(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isOutput,
        int parameterIndex = 0);
    void emitSalOutputTypeInfo(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isAsync);
    std::wstring toDeclarePropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isOutput,
        int parameterIndex = 0);
    void emitFullName(std::wostream& os, const schema::CTypeInfo*pModelTypeInfo);
    std::wstring toModelTypeInfo(const schema::CTypeInfo*pModelTypeInfo);
    std::wstring toFullName(const schema::CTypeInfo*pModelTypeInfo);
    std::wstring toPropertyTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    std::wstring toPropertyTypeName(const schema::CPropertyTypeInfo*pPropertyTypeInfo);

    bool isModelInSchema(const schema::CTypeInfo *pModelTypeInfo);

    // Static helpers
    static void formatHexBytes(std::wostream& os,UINT8 *&pIIdType,unsigned int nbytes);
    static std::wstring getFilePath(const std::wstring& path);
    static void formatIIDType(std::wostream& os,GUID iidType);
    static std::wstring toTypeInfoName(const std::wstring& typeInfoDeclare);
    static std::wstring toTypeInfoName(const schema::CEnumTypeInfo *pEnumTypeInfo);
    static const wchar_t *getCoreNamespace();
    static const wchar_t *getFoundationNamespace();

    static void emitIncludeReferences(
        std::wostream& os,
        foundation::library::DictionaryClassPtr& parameters,
        const wchar_t *parameterName,
        const wchar_t *includeKeyword=L"include");
    static std::wstring toPropertyTypeName(foundation::PropertyType propertyType);
    static std::wstring toCoreType(foundation::PropertyType propertyType);
    static void emitCoreModelTypeInfo(
        std::wostream& os,
        schema::ModelTypeInfoType modelTypeInfoType);
    static bool isInspectableType(const schema::CPropertyTypeInfo *pTypeInfo);

    static std::wstring getModelTypeInfoIncName(const schema::CTypeInfo *pModelTypeInfo);
    static std::wstring toTypeDesc(foundation::PropertyType propertyType);
    static std::wstring toPropertyTypeDesc(foundation::PropertyType propertyType);
    
    static std::wstring toPropertyType(
        foundation::PropertyType propertyType,
        const wchar_t *typeName,
        const wchar_t *arrayPostfix);
    static bool getBoolParamater(
        foundation::library::DictionaryClassPtr& parameters,
        const wchar_t *parameterName,
        bool defaultValue);
    static bool isSimpleCommand(const schema::CCommandTypeInfo *pCommandModelInfo);
    static bool isCommandResult(const schema::CCommandTypeInfo *pCommandModelInfo);
    static bool isCommandParameter(const schema::CCommandTypeInfo *pCommandModelInfo);
    static bool isObjectObjectCommand(const schema::CCommandTypeInfo *pCommandModelInfo);
    static bool isCoreCommand(const schema::CCommandTypeInfo *pCommandModelInfo);

    std::wstring getModelTypeInfoIIdType(const schema::CTypeInfo *pModelTypeInfo);
    std::wstring replaceNamespace(const std::wstring& value);

    static std::wstring getStringParameter(
        foundation::library::DictionaryClassPtr& parameters,
        const wchar_t *key,
        const wchar_t *default_value = nullptr);

    static bool getBoolParameter(
        foundation::library::DictionaryClassPtr& parameters,
        const wchar_t *key,
        const bool default_value = false);

    static const wchar_t * _simpleCommandTypeId;
    static const wchar_t * _simpleAsyncCommandTypeId;
    static const wchar_t * _objectObjectCommandTypeId;
    static const wchar_t * _objectObjectAsyncCommandTypeId;

    static bool isPropertyTypeInfoArray(const schema::CPropertyTypeInfo* pPropertyTypeInfo)
    {
        return (pPropertyTypeInfo->getPropertyType() & 0x400) != 0;
    }
    
    static foundation::PropertyType getPropertyTypeInfoArrayItemType(const schema::CPropertyTypeInfo* pPropertyTypeInfo)
    {
        return (foundation::PropertyType)(pPropertyTypeInfo->getPropertyType() & 0xff);
    }

    static bool hasPropertyTypeInfoModel(const schema::CPropertyTypeInfo* pPropertyTypeInfo)
    {
        return pPropertyTypeInfo->getModelType() != nullptr;
    }
    
    static bool isPropertyTypeInfoNumber(const schema::CPropertyTypeInfo* pPropertyTypeInfo)
    {
        return pPropertyTypeInfo->getPropertyType() <= foundation::PropertyType::PropertyType_Boolean;
    }
    
    static bool isPropertyTypeInfoObject(const schema::CPropertyTypeInfo* pPropertyTypeInfo)
    {
        return pPropertyTypeInfo->getPropertyType() == foundation::PropertyType::PropertyType_Inspectable;
    }

    static bool isPropertyTypeInfoObjectNoType(const schema::CPropertyTypeInfo* pPropertyTypeInfo)
    {
        return 
            isPropertyTypeInfoObject(pPropertyTypeInfo) && 
            !hasPropertyTypeInfoModel(pPropertyTypeInfo);
    }
    static bool isPropertyTypeInfoEnum(const schema::CPropertyTypeInfo* pPropertyTypeInfo)
    {
        return pPropertyTypeInfo->getModelType() != nullptr &&
            pPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum;
    }
    static void initializeNamespaceLookup(
        foundation::library::DictionaryClassPtr& parameters,
        LPCSTR_t key,
        foundation::library::DictionaryClassPtr& namespaceLookup,
        LPCSTR_t namespaceReplace);

    static std::wstring getNamespaceLookup(
        foundation::library::DictionaryClassPtr& namespaceLookup,
        const std::wstring& namespaceStr);

    static const UINT32 PropertyFlagType_CanRead = 1;
    static const UINT32 PropertyFlagType_CanWrite = 2;
    static const UINT32 PropertyFlagType_NoCache = 0x10;
    static const UINT32 PropertyFlagType_IsReserved = 0x20;
    static const UINT32 PropertyFlagType_IsAncestor = 0x40;
    static const UINT32 PropertyFlagType_IsWeakReference = 0x80;
    static const UINT32 PropertyFlagType_IsEnum = 0x1000;

private:
    typedef std::map<std::wstring,std::wstring, WStringLessThan> _NamespacelookupType;

    const schema::CModelSchema *_pModelSchema;
    foundation::library::DictionaryClassPtr _namespaceLookup;
};

#endif
