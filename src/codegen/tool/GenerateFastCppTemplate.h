/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateFastCppTemplate.h
****/


#include "GenerateCppLibrary.h"
#include <sstream>

class CGenerateFastCppTemplate : public CGenerateCppLibrary
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;

    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;
    void emitGetSetValuePropertyCases(
        std::wostream& getValueCases,
        std::wostream& setValueCases,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitCommandInitializer(
        std::wostream& createinstance,
        std::wostream& typedefs,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitPropertyContainer(
        std::wostream& os,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitPropertyInitializer(
        std::wostream& os,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitWrapperMethod(
        std::wostream& os,
        const schema::CMethodInfo*pMethodInfo);

    void emitWrapperResultTypeInfo(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isAsync);

    void emitReturnResult(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pResultPropertyTypeInfo,
        bool isAsync);

    void emitPropertyBaseTypeDelegate(
        std::wostream& os,
        std::wostream& osQualifications,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitMethodBaseTypeDelegate(
        std::wostream& os,
        std::wostream& osQualifications,
        const schema::CMethodInfo* pMethodInfo);

    void emitDelegateTypeVarAddress(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        const wchar_t *pVarName);

    void emitActivationSupport(
        std::wostream& osOngetPropertyCases,
        std::wostream& osEnsureActivation,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitPropertyAccessMethods(
        std::wostream& osPublicAccess,
        std::wostream& osInternalAccess,
        std::wostream& osPrivateAccess,
        std::wostream& osPrivateAccessDeclare,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitInternalGetBodyMethods(
        std::wostream& osInternalAccess,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitPropertyCommandHandler(
        std::wostream& osAbstractMethods,
        const schema::CPropertyInfo* pPropertyInfo);

    void emitAncestorContainerFriendDecl(
        std::wostream& os,
        const schema::CObservableObjectInfo* pModelType);

    void emitContainerReleaseStatement(
        std::wostream& os,
        const schema::CPropertyInfo* pPropertyInfo);

    std::wstring toContainerPropertyName(const wchar_t *pString);
    std::wstring toAncestorContainerName(const wchar_t *pString);
    void emitPropertyTypeInitializer(std::wostream& os, foundation::PropertyType propertyType);
    std::wstring toIdlIIDType(GUID iidType);
    IID createUniqueIId(const schema::CTypeInfo*pModelTypeInfo, std::wstring postFix = L"");
    std::wstring toIdlIIDType(const schema::CTypeInfo*pModelTypeInfo);

    std::wstring getPropertyCaseInvoke(const schema::CPropertyInfo*pPropertyInfo, bool isGet)override;
	std::wstring getPropertyCaseObjectInvokeImpl(
		const schema::CPropertyInfo*pPropertyInfo,
		const std::wstring& propertyModelTypeName,
		bool isGet) override;

    std::wstring getMethodCallback(const schema::CMethodInfo*pMethodInfo, std::wstring& invokeInstance) override;

private:
    bool _generateMethodInternal;
    std::wostringstream _osFastModelContainersTemplates;
    std::wostringstream _osFastModelBaseClassTemplates;
    std::wostringstream _osFastModelPrivateInterfaceDeclares;
    std::wostringstream _osFastModelPrivateInterfaceIIDs;
    std::wostringstream _osFastModelPrivateAdapters;
    std::wostringstream _osFastModelPrivateAdapterRegistrationEntries;
};
