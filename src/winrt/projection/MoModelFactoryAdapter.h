/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelFactoryAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoObjectFactoryAdapter
//
// Purpose: Implementation of IModelFactory Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoObjectFactoryAdapter :
    public foundation::AdapterBase
    <
    foundation::IObjectFactoryBase,
    IObjectFactory,
    &__uuidof(IObjectFactory)
    >
{
public:
    // IModelFactory
    virtual HRESULT STDMETHODCALLTYPE TryCreateObject( 
        GUID iidType,
        IInspectable *pContext,
        IObject **value) override
    {
        foundation::ComPtr<foundation::IObject> spCreatedObject;
        IFR(m_pInner->TryCreateObject(iidType, pContext, spCreatedObject.GetAddressOf()));
        if (spCreatedObject)
        {
            IFR(foundation::QueryInterface(spCreatedObject, value));
        }
        else
        {
            *value = nullptr;
        }
        return S_OK;
    }
};
