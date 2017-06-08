/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCppLibrary.h
****/
#pragma once

#include "GenerateCppBase.h"
#include <sstream>

class CGenerateCppLibrary : public CGenerateCppBase
{
protected:
    CGenerateCppLibrary();
    void emitPropertyCommandHandler(
        std::wostream& os,
        const schema::CPropertyInfo*pPropertyInfo,
        const wchar_t *base_class_name,
        const wchar_t *prefix_class_name);

    static void emitTypeVarAddress(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        const wchar_t *pVarName);

    void emitTypeVarDeclare(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        const wchar_t *pVarName);

    virtual void emitDelegateTypeVarDeclare(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        const wchar_t *pVarName);

    virtual  void emitDelegateTypeVarAddress(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        const wchar_t *pVarName);

    virtual void emitDelegateTypeVarAssign(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        const wchar_t *pVarName,
        const wchar_t *pVarNameIn);

    virtual void emitDelegateTypeVarReturn(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        const wchar_t *pVarName,
        const wchar_t *pVarNameIn);

    void emitGetSetValuePropertyCases(
        std::wostream* pGetValueCases,
        std::wostream* pSetValueCases,
        const schema::CPropertyInfo*pPropertyInfo);

    virtual std::wstring getPropertyCaseInvoke(const schema::CPropertyInfo*pPropertyInfo,bool isGet);
	virtual std::wstring getPropertyCaseObjectInvokeImpl(
		const schema::CPropertyInfo*pPropertyInfo,
		const std::wstring& propertyModelTypeName, 
		bool isGet);

    std::wstring toNativePropertyType(const schema::CPropertyInfo *pPropertyInfo);

	void emitDeclarePropertyTypeInfo(
		std::wostream& os,
		const schema::CPropertyTypeInfo*pPropertyTypeInfo,
		bool isOutput,
		int parameterIndex = 0);
	void emitWrapperPropertyTypeInfo(
		std::wostream& os,
		const schema::CPropertyTypeInfo*pPropertyTypeInfo,
		bool isOutput = false);

    void emitModelPropertyTypeInfo(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isOutput,
        int parameterIndex = 0);
    void emitAsyncOperationClassDeclare(std::wostream& os);

    void emitMethodHandler(
        std::wostream& osMethodHandler,
        std::wostream& osAbstractMethods,
        const schema::CMethodInfo*pMethodInfo,
        bool isPublic);

    virtual std::wstring getMethodCallback(
        const schema::CMethodInfo*pMethodInfo,
        std::wstring& invokeInstance)
    {
        invokeInstance = L"this";
        return pMethodInfo->getName();
    }

    static std::wstring emitPropertyUtilCreateMethod(const schema::CPropertyInfo* pPropertyInfo);
    static std::wstring toNativePropertyType(
        foundation::PropertyType propertyType);
    static bool isUseWrapperDelegatePropertyTypeInfo(const schema::CPropertyTypeInfo* pPropertyTypeInfo);

protected:
    bool _useAsyncOperationClassPtr;

};


