/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCSharpClasses.h
****/


#include "GenerateCSharpBase.h"
#include <foundation/library/util_stl.h>

#include <sstream>

class CGenerateCSharpClasses : public CGenerateCSharpBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo) override;
    void codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo) override;
    void codeGenerate(const schema::CCommandTypeInfo *pCommandModelInfo) override;

    std::wstring getNamespaceLookup(const std::wstring& namespaceStr);

private:
    std::wstring toCSharpModelTypeName(const schema::CTypeInfo*pModelTypeInfo);
    std::wstring toCSharpPropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    std::wstring toCSharpInterfaceModelTypeName(const schema::CTypeInfo*pModelTypeInfo);
    std::wstring toCSharpInterfacePropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    static std::wstring toCSharpType(foundation::PropertyType propertyType);
private:
    std::wostringstream _osCSharpClassesSrc;
    std::wostringstream _osEnums;
    bool _useClassInterfaceCallback;
    typedef std::map<GUID, std::wstring,foundation::library::GuidLessThan> _TypeInfoClasses_MapType;

    _TypeInfoClasses_MapType _typeInfoClasses;
};


