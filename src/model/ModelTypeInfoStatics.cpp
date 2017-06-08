/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelTypeInfoStatics.cpp
****/
#include "pch.h"

#include "ModelTypeInfoStatics.h"
#include "CommandInfo.h"
#include "ObservableCollectionInfo.h"
#include "PropertyTypeInfo.h"

#include <foundation/library/com_ptr_singleton.h>

using namespace pmod;

EXTERN_C PMOD_API CTypeInfoStatics *STDAPICALLTYPE GetModelTypeInfoStatics()
{
    return static_cast<CTypeInfoStatics *>(CModelTypeInfoStatics::GetInstance());
}

foundation::ITypeInfoStatics * CModelTypeInfoStatics::GetInstance()
{
    return foundation::library::_GetFactoryInstance<CModelTypeInfoStatics, ITypeInfoStatics>();
}

HRESULT CModelTypeInfoStatics::IsTypeOfInternal(
    _In_ foundation::IObject *pObject,
    const IID& iidType,
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
    const GUID& typeInfoType,
    bool& result)
{
    if (typeInfoType == foundation::IEnumerableTypeInfo::GetIID())
    {
        CCollectionModelTypeInfo *pCollectionModelInfo = CastTypeInfoTypeClassInternal<CCollectionModelTypeInfo>(pPropertyTypeInfo);

        foundation::ComPtr<foundation::IEnumerableTypeInfo> spCollectionModelTypeInfo;
        IFR_ASSERT(CTypeInfoStatics::GetTypeInfo(pObject, spCollectionModelTypeInfo.GetAddressOf()));

        if (iidType == IID_IObservableCollection)
        {
            // the Model passed is a IObservableCollectionInfo so we are Ok
            result = true;
            return S_OK;
        }

        foundation::ComPtr<foundation::IPropertyTypeInfo> spVarTypeInfo;
        IFR_ASSERT(pCollectionModelInfo->GetItemTypeInfo(spVarTypeInfo.GetAddressOf()));

        foundation::ComPtr<foundation::IPropertyTypeInfo> spOtherVarTypeInfo;
        IFR_ASSERT(spCollectionModelTypeInfo->GetItemTypeInfo(spOtherVarTypeInfo.GetAddressOf()));

        _IFR_(CTypeInfoStatics::IsTypeOfInternal(spVarTypeInfo, spOtherVarTypeInfo, &result));
    }
    else if (typeInfoType == ICommandInfo::GetIID())
    {
        CCommandModelTypeInfo *pCommandModelInfo = CastTypeInfoTypeClassInternal<CCommandModelTypeInfo>(pPropertyTypeInfo);

        foundation::ComPtr<ICommandInfo> spOtherCommandModelTypeInfo;
        _IFR_(CTypeInfoStatics::GetTypeInfo(pObject, spOtherCommandModelTypeInfo.GetAddressOf()));

        if (iidType == IID_ICommand)
        {
            // the Model passed is a ICommandInfo so we are Ok
            result = true;
            return S_OK;
        }
        foundation::ComPtr<foundation::IPropertyTypeInfo> spVarTypeInfo;
        IFR_ASSERT(pCommandModelInfo->GetParameterTypeInfo(spVarTypeInfo.GetAddressOf()));

        foundation::ComPtr<foundation::IPropertyTypeInfo> spOtherVarTypeInfo;
        IFR_ASSERT(spOtherCommandModelTypeInfo->GetParameterTypeInfo(spOtherVarTypeInfo.GetAddressOf()));

        _IFR_(CTypeInfoStatics::IsTypeOfInternal(spVarTypeInfo, spOtherVarTypeInfo, &result));

        if (result)
        {
            spVarTypeInfo.Release();
            spOtherVarTypeInfo.Release();

            IFR_ASSERT(pCommandModelInfo->GetResultTypeInfo(spVarTypeInfo.GetAddressOf()));
            IFR_ASSERT(spOtherCommandModelTypeInfo->GetResultTypeInfo(spOtherVarTypeInfo.GetAddressOf()));
            _IFR_(CTypeInfoStatics::IsTypeOfInternal(spVarTypeInfo, spOtherVarTypeInfo, &result));
        }
    }
    else
    {
        _IFR_(CTypeInfoStatics::IsTypeOfInternal(pObject, iidType, &result));
    }
    return S_OK;
}
