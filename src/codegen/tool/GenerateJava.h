/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateJava.h
****/


#include "CodeGenToolBase.h"
#include <sstream>

class CGenerateJava : public CCodeGenToolBase
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo) override;
    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;

    void generateTypeInfo();

private:
    void generateTypeInfo() const;
    std::string toJavaSourceFile(const wchar_t *name);
    std::wstring toJavaModelTypeName(const schema::CTypeInfo*pModelTypeInfo);
    std::wstring toJavaPropertyTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo, bool isObject = false);
    std::wstring toJavaAsyncOperationTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo);
    std::wstring toCastFunc(const schema::CPropertyTypeInfo * pPropertyTypeInfo, bool& isFoundationArray);
    static bool isEnumArrayProperty(const schema::CPropertyTypeInfo * pPropertyTypeInfo);
    static std::wstring toJavaType(foundation::PropertyType propertyType,bool isObject);

private:
    std::wstring _javaPackageName;
    std::wstring _javaSourceDir;
    std::wostringstream _osAddFactoryTypes;
    std::wostringstream _osTypeInfoEntries;
};
