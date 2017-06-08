/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateTypeInfo.h
****/


#include "CodeGenToolBase.h"
#include <sstream>

class CGenerateTypeInfo : public CCodeGenToolBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CTypeInfo *pModelTypeInfo);
    void codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo) override;
	void codeGenerate(const schema::CObjectDispatchInfo *pPropertyDataInfo) override;
	void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) override;
    void codeGenerate(const schema::CCommandTypeInfo *pCCommandModelInfo) override;
    void codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo) override;

private:
    void emitModelTypeInfoRef(std::wostream& os,const schema::CTypeInfo *pModelInfoType);
    void emitPropertyTypeInfo(
        std::wostream& os,
        const wchar_t *propertyTypeInfoName,
        const schema::CTypeInfo *pModelTypeInfo,
        const schema::CPropertyTypeInfo *pPropertyTypeInfo);
    void emitPropertyTypeInfo(
        std::wostream& os,
        const wchar_t *prefix,
        const wchar_t *typeInfoName,
        const schema::CPropertyTypeInfo *pPropertyTypeInfo);

    void emitPropertiesInfo(
        const wchar_t *typeInfoName,
        const schema::CObjectDispatchInfo *pPropertyDataInfo);

    void emitMethodsInfo(
        const std::wstring& typeInfoName,
        const schema::CObjectDispatchInfo *pPropertyDataInfo);

private:
    std::wostringstream _osEnumsInc;
    std::wostringstream _osModelInc;
    std::wostringstream _osModelSrc;
    std::wostringstream _osModelTypeInfoEntries;
    std::wostringstream _osTypeInfoIIDsSrc;

    bool _isTypeInfoPrivate;
    bool _generateEnumPrefixValues;
    bool _generateEnumUnderscoreValues;
    bool _generateEnumUniqueValues;
	bool _generateEnumClass;
	bool _isEnumTypes;
    std::wstring _modelApi;
};


