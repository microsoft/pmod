/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelTypeInfoStatics.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include <winrt/com_activation.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoObjectTypeInfoStatics
//
// Purpose: Factory class for a ModelTypeInfo
//
//------------------------------------------------------------------------------
class MoObjectTypeInfoStatics :
    public ctlwinrt::AbstractActivationFactory,
    public IObjectTypeInfoStatics
{
public:
    HRESULT STDMETHODCALLTYPE get_CommandModelType( 
        IObjectTypeInfo **value) override;
                    
    HRESULT STDMETHODCALLTYPE get_CollectionModelType( 
        IObjectTypeInfo **value) override;
                    
    HRESULT STDMETHODCALLTYPE get_PropertyModelType( 
       IObjectTypeInfo **value) override;
                    
    HRESULT STDMETHODCALLTYPE get_AsyncOperationType( 
        IObjectTypeInfo **value) override;

    HRESULT STDMETHODCALLTYPE GetObjectTypeInfo(
            GUID iidType,
            IObjectTypeInfo **value) override;

    HRESULT STDMETHODCALLTYPE GetEnumTypeInfo( 
        UINT32 typeId,
        IEnumTypeInfo **value) override;

    HRESULT STDMETHODCALLTYPE GetEnumTypeInfoByName( 
        HSTRING typeName,
        IEnumTypeInfo **value) override;

    HRESULT STDMETHODCALLTYPE GetEnumValue(UINT32 uniqueEnumValue, IEnumValue** value) override;

    HRESULT STDMETHODCALLTYPE GetEnumTypeInfoFromReference(
        IInspectable *pObject,
        IEnumTypeInfo **value) override;

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if (iid == __uuidof(IObjectTypeInfoStatics))
        {
            *ppInterface = static_cast<IObjectTypeInfoStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};
