/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateObjectiveC.h
****/


#include "GenerateCppBase.h"
#include <sstream>

class CGenerateObjectiveC : public CGenerateCppBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo) override;
    void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) override;
private:
    static std::wstring toObjectiveCAsyncOperation();
    static std::wstring toObjectiveCType(const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    static const wchar_t *toObjectiveCType(foundation::PropertyType propertyType);
    static const wchar_t *toNSNumberValue(foundation::PropertyType propertyType);
    static const wchar_t *toNSNumberWithValue(foundation::PropertyType propertyType);
    void emitNSReturn(
                       std::wostream& os,
                       const schema::CPropertyTypeInfo* pPropertyTypeInfo,
                       const wchar_t *pVarName);
    void emitFoundationVar(
        std::wostream& os,
        const schema::CPropertyTypeInfo* pPropertyTypeInfo,
        const wchar_t *pObjectiveCVar,
        const wchar_t *pFoundationVar);
private:
    bool            _useInterfaceAdapter;
    std::wstring _adapterPrefix;
    std::wostringstream _osEnumsInc;
    std::wostringstream _osForwardsInc;
    std::wostringstream _osEnumsPropertiesInc;
    std::wostringstream _osEnumsEventsInc;
    std::wostringstream _osProtocolsInc;
    std::wostringstream _osProtocolsSrc;
    std::wostringstream _osFactoryEntries;
    std::wostringstream _osFactoryTypesInc;
    std::wostringstream _osFactoryTypesSrc;
};


