/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelTypeInfoStatics.h
****/
#pragma once
#include "TypeInfoStatics.h"

#include <pmod/pmod_api.h>

EXTERN_C PMOD_API CTypeInfoStatics *STDAPICALLTYPE GetModelTypeInfoStatics();

class CModelTypeInfoStatics :
    public CTypeInfoStatics
{
public:
    inline static HRESULT _VerifyPropertyTypeInfo(
        _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        _Inout_ foundation::IInspectable **ppValue)
    {
        return GetModelTypeInfoStatics()->VerifyPropertyTypeInfoInternal(pPropertyTypeInfo, ppValue);
    }

    static foundation::ITypeInfoStatics * GetInstance();

protected:
    HRESULT IsTypeOfInternal(
        _In_ foundation::IObject *pObject,
        const IID& iidType,
        _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
        const GUID& typeInfoType,
        bool& result) override;
};

