/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelTypeInfoFactory.h
****/
#pragma once

#include <map>

#include <foundation/ctl/com_library.h>
#include <pmod/library/model_library.h>

#include "TypeInfoFactory.h"

class CModelTypeInfoFactory :
    public CTypeInfoFactory,
    public pmod::library::IModelTypeInfoFactory
{
 public:
     static pmod::library::IModelTypeInfoFactory *GetInstance();
     static HRESULT GetOrCreateInstance(pmod::library::IModelTypeInfoFactory **ppPropertyModelInfoFactory);

     //Interface IModelTypeInfoFactory

    STDMETHOD(CreateCommandInfo)(
        _In_ UINT32 typeId,
        _In_ GUID iidType,
        _In_ HSTRING name,
        _In_ foundation::IPropertyTypeInfo *pParameterTypeInfo,
        _In_ foundation::IPropertyTypeInfo *pResultTypeInfo,
        _In_ bool isAsync,
        _Outptr_ pmod::ICommandInfo **ppCommandModelInfo);

    STDMETHOD(CreateObservableObjectInfo)(
        _In_ UINT32 typeId,
        _In_ GUID iidType,
        _In_ HSTRING name,
        _In_opt_ pmod::IObservableObjectInfo *pBaseType,
        _Outptr_ pmod::IObservableObjectInfo **ppPropertyModelInfo);

    STDMETHOD(CreateViewModelInfo)(
        _In_ UINT32 typeId,
        _In_ GUID iidType,
        _In_ HSTRING name,
        _In_opt_ pmod::IObservableObjectInfo *pBaseType,
        _In_ pmod::IObservableObjectInfo *pSourceType,
        _In_ bool isInherited,
        _Outptr_ pmod::IViewModelInfo **ppPropertyViewModelInfo);

    STDMETHOD(RegisterObjectTypeInfo)(_In_ pmod::IObservableObjectInfo *pPropertyModelInfo);
    STDMETHOD(RegisterCommandTypeInfo)(_In_ pmod::ICommandInfo *pCommandModelInfo);
    STDMETHOD(RegisterCollectionTypeInfo)(_In_ foundation::IEnumerableTypeInfo *pCollectionModelInfo);
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::library::IModelTypeInfoFactory::GetIID())
        {
            *ppInterface = static_cast<pmod::library::IModelTypeInfoFactory *>(this);
        }
        else
        {
            return CTypeInfoFactory::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }


};

