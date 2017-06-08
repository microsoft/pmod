/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeInfoStatics.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>
#include <foundation/macros.h>
#include <foundation/interfaces.h>


BEGIN_DECLARE_INTERFACE(ITypeInfoInternalSupport, foundation::IUnknown,FOUNDATION_API)
    STDMETHOD(IsInstanceOfType)(REFIID iidInstanceType,foundation::InstanceTypeOptions options,bool *pResult) = 0;
END_DECLARE_INTERFACE()

class CTypeInfoStatics :
    public foundation::ctl::ComInspectableBase,
    public foundation::ITypeInfoStatics
{
 public:
    static foundation::ITypeInfoStatics * GetInstance();
    static HRESULT GetOrCreateInstance(foundation::ITypeInfoStatics **ppModelTypeInfoStatics);
//Interface ITypeInfoStatics
    STDMETHOD(GetTypeInfos)(
        _In_opt_ foundation::_TypeInfoFilter *pTypeInfoFilter,
        _Out_ UINT32 *size,
        _Outptr_ foundation::IObjectTypeInfo ***ppTypeInfos);
    STDMETHOD(GetTypeInfo)(
            GUID iidType,
            foundation::IObjectTypeInfo **ppTypeInfo);
    STDMETHOD(GetEnumTypeInfo)(
        UINT32 enumTypeId,
        foundation::IEnumTypeInfo **ppEnumTypeInfo);
    STDMETHOD(GetEnumTypeInfoByName)(
        HSTRING enumTypeName,
        foundation::IEnumTypeInfo **ppEnumTypeInfo);
    STDMETHOD(GetEnumTypeInfoByNamespaceAndName)(
        HSTRING Namespace,
        HSTRING Name,
        foundation::IEnumTypeInfo **ppEnumTypeInfo);
    STDMETHOD(GetEnumValueByName)(
        HSTRING enumValueName,
        foundation::IEnumValue **ppEnumValue);
    STDMETHOD(GetEnumValueByValue)(
        UINT32 enumValue,
        foundation::IEnumValue **ppEnumValue);

    STDMETHOD(GetAsyncOperationTypeInfo)(
        foundation::IPropertyTypeInfo *pResultTypeInfo,
        foundation::IAsyncOperationTypeInfo **ppAsyncOperationTypeInfo);

    STDMETHOD(IsInstanceOfType)(foundation::IObject *pObject, REFIID iidInstanceType, bool *pResult) override;
    STDMETHOD(VerifyType)(foundation::IObject *pObject, REFIID iidExpectedType) override;
    STDMETHOD(IsInstanceOfTypeWithOptions)(
        foundation::IObject *pObject,
        REFIID iidInstanceType,
        foundation::InstanceTypeOptions options,
        bool *pResult) override;
    STDMETHOD(VerifyPropertyTypeInfo)(
        _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        _Inout_ foundation::IInspectable **ppValue) override;

    virtual HRESULT VerifyPropertyTypeInfoInternal(
        _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        _Inout_ foundation::IInspectable **ppValue);

    static HRESULT IsTypeOfInternal(
        foundation::IPropertyTypeInfo *pVarTypeInfo,
        foundation::IPropertyTypeInfo *pOtherVarTypeInfo,
        bool *pResult);
    static HRESULT IsTypeOfInternal(
        foundation::IObjectTypeInfo *pModelTypeInfo,
        REFIID iidInstanceType,
        bool *pResult);
    static HRESULT IsTypeEqual(
        foundation::IObjectTypeInfo *pTypeInfo,
        REFIID iidInstanceType, bool *pResult);

    static HRESULT GetTypeInternal(
        _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        _Out_ IID* pIIdType);
    static HRESULT GetTypeInternal(
        _In_ foundation::IObject *pObject,
        _Out_ IID* pIIdType);

    static HRESULT VerifyPropertyTypeInfoInternal(
        foundation::IPropertyTypeInfo *pVarTypeInfo,
        foundation::IPropertyTypeInfo *pOtherVarTypeInfo);

    static HRESULT VerifyEnumTypeVarInternal(foundation::IEnumTypeInfo *pEnumTypeInfo, _In_ foundation::IInspectable **ppValue);
    static HRESULT VerifyArrayEnumTypeVarInternal(foundation::IEnumTypeInfo *pEnumTypeInfo, _In_ foundation::IInspectable **ppValue);

    static HRESULT VerifyTypeObjectInternal(_In_ foundation::IPropertyTypeInfo *pVarTypeInfo, _In_ foundation::IUnknown *pValueUnknown);
    static HRESULT VerifyTypeVarInternal(_In_ foundation::IPropertyTypeInfo *pVarTypeInfo, _Inout_ foundation::IInspectable **value);
    static HRESULT IsTypeOfInternal(foundation::IObject *pObject, REFIID iidInstanceType, bool *pResult);
    static HRESULT IsTypeOfInternalWithOptions(foundation::IObject *pObject, REFIID iidInstanceType, foundation::InstanceTypeOptions options, bool *pResult);

    static HRESULT _VerifyPropertyTypeInfo(
        _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        _Inout_ foundation::IInspectable **ppValue);

    template <class T>
    static HRESULT GetTypeInfo(
        foundation::IObject *pObject,
        T **ppModelTypeInfo)
    {
        foundation_assert(pObject);

        foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
        IFR_ASSERT(pObject->GetTypeInfo(spTypeInfo.GetAddressOf()));
        IFCEXPECT_ASSERT(spTypeInfo.Get());

        IFR_ASSERT(foundation::QueryInterface(spTypeInfo, ppModelTypeInfo));
        return S_OK;
    }

protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::ITypeInfoStatics::GetIID())
        {
            *ppInterface = static_cast<foundation::ITypeInfoStatics *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    virtual HRESULT VerifyPropertyTypeInfoInternal(
        _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        const GUID& typeInfoType,
        _In_ foundation::IInspectable **ppValue);

    virtual HRESULT IsTypeOfInternal(
        _In_ foundation::IObject *pObject,
        const IID& iidType,
        _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        const GUID& typeInfoType,
        bool& result);
};
