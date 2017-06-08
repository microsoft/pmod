/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCppBase.h
****/
#pragma once

#include "CodeGenToolBase.h"
#include <sstream>

class CGenerateCppBase : public CCodeGenToolBase
{
protected:
    CGenerateCppBase():
        _useLowerCaseConvention(false)
    {}

    void emitDeclareArrayWrapperType(
        std::wostream& os,
        const schema::CPropertyTypeInfo*
        pPropertyTypeInfo);

    void emitArrayWrapperType(std::wostream& os,
        const schema::CPropertyTypeInfo* pPropertyTypeInfo,
        const wchar_t *varName,
        bool useJustType = false);

    void emitTypeVarDeclare(
        std::wostream& os,
        const schema::CPropertyTypeInfo* pPropertyTypeInfo,
        const wchar_t *pVarName);

    void emitModelTypeVarDeclare(
        std::wostream& os,
        const schema::CPropertyTypeInfo* pPropertyTypeInfo,
        const wchar_t *pVarName);

    void emitInputPtrVar(
        std::wostream& os,
        const schema::CPropertyTypeInfo* pPropertyTypeInfo,
        const wchar_t *pVarName);

    std::wstring toPropertyTypeInfoAsync(
        const schema::CPropertyTypeInfo* pResultPropertyTypeInfo,
        bool isAsync);

    std::wstring toCodeGenTypeName(const std::wstring& typeName);
    std::wstring toCodeGenVarName(const std::wstring& typeName);

    static void emitOutputPtrVar(
        std::wostream& os,
        const schema::CPropertyTypeInfo* pPropertyTypeInfo,
        const wchar_t *pVarName);

    bool _useLowerCaseConvention;
};
