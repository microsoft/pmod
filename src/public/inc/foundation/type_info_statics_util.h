/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_info_statics_util.h
****/
#pragma once

#include "macros.h"
#include "com_ptr.h"

#include "interfaces/enum_value.h"
#include "interfaces/type_info_statics.h"
#include "activation_factory.h"

namespace foundation {

    inline ITypeInfoStatics *GetTypeInfoStatics()
    {
        return GetActivationFactoryWithCast<ITypeInfoStatics>(U("foundation.TypeInfoStatics"));
    }

    inline HRESULT GetTypeInfos(
        _In_opt_ _TypeInfoFilter *pTypeInfoFilter, 
        _Out_ UINT32 *size,
        _Outptr_ IObjectTypeInfo ***ppTypeInfos)
    {
        return GetTypeInfoStatics()->GetTypeInfos(pTypeInfoFilter, size, ppTypeInfos);
    }

    inline HRESULT GetCustomTypeInfos(
        _Out_ UINT32 *size,
        _Outptr_ IObjectTypeInfo ***ppTypeInfos)
    {
        _TypeInfoFilter typeInfoFilter;
        return GetTypeInfos(&typeInfoFilter, size, ppTypeInfos);
    }

    inline HRESULT GetTypeInfo(GUID iidType, _COM_Outptr_result_maybenull_ IObjectTypeInfo **ppTypeInfos)
    {
        return GetTypeInfoStatics()->GetTypeInfo(iidType, ppTypeInfos);
    }

    template <class T>
    HRESULT GetTypeInfoAs(GUID iidType, _COM_Outptr_result_maybenull_ T **ppTypeInfo)
    {
        IFCPTR_ASSERT(ppTypeInfo);
        *ppTypeInfo = nullptr;

        ComPtr<IObjectTypeInfo> spTypeInfo;
        _IFR_(GetTypeInfo(iidType, spTypeInfo.GetAddressOf()));
        foundation_assert(spTypeInfo != nullptr);
        return spTypeInfo->QueryInterface(T::GetIID(), (void **)ppTypeInfo);
    }

    inline bool IsTypeInfoRegistered(GUID iidType)
    {
        ComPtr<IObjectTypeInfo> spTypeInfo;
        return SUCCEEDED(GetTypeInfo(iidType, spTypeInfo.GetAddressOf()));
    }

    template <class T>
    HRESULT GetObjectTypeInfo(_In_ IObject *pObject, _COM_Outptr_result_maybenull_ T** ppT)
    {
        IFCPTR_ASSERT(pObject);
        IFCPTR_ASSERT(ppT);
        *ppT = nullptr;

        ComPtr<IObjectTypeInfo> spTypeInfo;
        _IFR_(pObject->GetTypeInfo(spTypeInfo.GetAddressOf()));
        if (spTypeInfo != nullptr)
        {
            return foundation::QueryInterface(spTypeInfo, ppT);
        }

        return S_FALSE;
    }

    inline HRESULT GetObjectType(_In_ IObject *pObject, IID& iidType)
    {
        IFCPTR_ASSERT(pObject);
        ComPtr<IObjectTypeInfo> spTypeInfo;
        _IFR_(GetObjectTypeInfo(pObject, spTypeInfo.GetAddressOf()));
        return spTypeInfo->GetType(&iidType);
    }

    inline HRESULT GetEnumTypeInfo(UINT32 enumTypeId, _COM_Outptr_result_maybenull_ IEnumTypeInfo **ppEnumTypeInfo)
    {
        _IFR_(GetTypeInfoStatics()->GetEnumTypeInfo(enumTypeId, ppEnumTypeInfo));
        return S_OK;
    }

    inline HRESULT GetEnumTypeInfoByNamespaceName(HSTRING Namespace,HSTRING Name, _COM_Outptr_result_maybenull_ IEnumTypeInfo **ppEnumTypeInfo)
    {
        _IFR_(GetTypeInfoStatics()->GetEnumTypeInfoByNamespaceAndName(Namespace, Name,ppEnumTypeInfo));
        return S_OK;
    }

    inline HRESULT GetEnumValueByName(HSTRING enumValueName, _COM_Outptr_result_maybenull_ IEnumValue **ppEnumValue)
    {
        _IFR_(GetTypeInfoStatics()->GetEnumValueByName(enumValueName, ppEnumValue));
        return S_OK;
    }

    inline HRESULT GetEnumValue(UINT32 enumTypeId, UINT32 enumValue, _COM_Outptr_result_maybenull_ IEnumValue **ppEnumValue)
    {
        ComPtr<IEnumTypeInfo> spEnumTypeInfo;
        IFR_ASSERT(GetEnumTypeInfo(enumTypeId, spEnumTypeInfo.GetAddressOf()));
        IFR_ASSERT(spEnumTypeInfo->GetEnumValue(enumValue, ppEnumValue));
        return S_OK;
    }

    inline HRESULT GetEnumValueName(UINT32 enumTypeId, UINT32 enumValue, _Outptr_result_maybenull_ HSTRING *pName)
    {
        IFCPTR_ASSERT(pName);

        ComPtr<IEnumValue> spEnumValue;
        IFR_ASSERT(GetEnumValue(enumTypeId, enumValue, spEnumValue.GetAddressOf()));
        IFR_ASSERT(spEnumValue->ToString(pName));
        return S_OK;
    }

    inline HRESULT GetEnumValueByName(UINT32 enumTypeId, HSTRING enumValueName, _COM_Outptr_result_maybenull_ IEnumValue **ppEnumValue)
    {
        ComPtr<IEnumTypeInfo> spEnumTypeInfo;
        IFR_ASSERT(GetEnumTypeInfo(enumTypeId, spEnumTypeInfo.GetAddressOf()));
        IFR_ASSERT(spEnumTypeInfo->GetEnumValueByName(enumValueName, ppEnumValue));
        return S_OK;
    }

    inline HRESULT GetEnumValue(UINT32 uniqueEnumValue, _COM_Outptr_result_maybenull_ IEnumValue **ppEnumValue)
    {
        IFR_ASSERT(GetTypeInfoStatics()->GetEnumValueByValue(uniqueEnumValue, ppEnumValue));
        return S_OK;
    }

    inline HRESULT GetAsyncOperationTypeInfo(
        _In_ IPropertyTypeInfo *pResultTypeInfo,
        _Out_ IID& iidType)
    {
        ComPtr<IAsyncOperationTypeInfo> spAsyncOperationTypeInfo;
        IFR_ASSERT(GetTypeInfoStatics()->GetAsyncOperationTypeInfo(
            pResultTypeInfo,
            spAsyncOperationTypeInfo.GetAddressOf()));

        return spAsyncOperationTypeInfo->GetType(&iidType);
    }

    // Return true if an instance descends from a Type (Does not fail on verification)
    inline HRESULT IsInstanceOfType(_In_ IObject *pObject, const IID& iidInstanceType, _Out_ bool& result)
    {
        IFR_ASSERT(GetTypeInfoStatics()->IsInstanceOfType(pObject, iidInstanceType, &result));
        return S_OK;
    }

    inline HRESULT IsInstanceOfTypeWithOptions(
        _In_ IObject *pObject, 
        const IID& iidInstanceType, 
        InstanceTypeOptions options,
        _Out_ bool *pResult)
    {
        IFR_ASSERT(GetTypeInfoStatics()->IsInstanceOfTypeWithOptions(pObject, iidInstanceType, options, pResult));
        return S_OK;
    }

    inline HRESULT VerifyType(IObject *pObject, REFIID iidExpectedType)
    {
        IFR_ASSERT(GetTypeInfoStatics()->VerifyType(pObject, iidExpectedType));
        return S_OK;
    }
    inline HRESULT VerifyPropertyTypeInfo(_In_ IPropertyTypeInfo *pPropertyTypeInfo, _Inout_ foundation::IInspectable **ppValue)
    {
        return GetTypeInfoStatics()->VerifyPropertyTypeInfo(pPropertyTypeInfo, ppValue);
    }
}
