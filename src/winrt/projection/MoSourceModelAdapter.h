/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoSourceModelAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

// Public Headers
#include <foundation/library/adapter_base.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoSourceModelBaseAdapter
//
// Purpose: Implementation of ISourceModel Interface
//
//------------------------------------------------------------------------------
class CMoSourceModelBaseAdapter :
    public foundation::AdapterBase
    <
        pmod::ISourceModelBase,
        ISourceModelBase,
        &__uuidof(ISourceModelBase)
    >
{
public:
    HRESULT STDMETHODCALLTYPE get_IsAttached( 
        boolean *value) override
    {
        bool bVal;
        IFR(m_pInner->GetIsAttached(&bVal));

        *value = bVal ? true:false;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE get_IsForceAttached( 
        boolean *value) override
    {
        bool bVal;
        IFR(m_pInner->GetForceAttached(&bVal));

        *value = bVal ? true:false;
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE put_IsForceAttached( 
        boolean value) override
    {
        return m_pInner->SetForceAttached(value ? true:false);
    }

    HRESULT STDMETHODCALLTYPE get_IsAttachEnabled( 
        boolean *value) override
    {
        bool bVal;
        IFR(m_pInner->GetAttachEnabled(&bVal));

        *value = bVal ? true:false;
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE put_IsAttachEnabled( 
        boolean value) override
    {
        return m_pInner->SetAttachEnabled(value ? true:false);
    }
};

//------------------------------------------------------------------------------
// Class:   CMoSourceModelAdapter
//
// Purpose: Implementation of ISourceModel Interface
//
//------------------------------------------------------------------------------
class CMoSourceModelAdapter :
    public foundation::AdapterBase
    <
        pmod::ISourceModel,
        ISourceModel,
        &__uuidof(ISourceModel)
    >
{
public:
    HRESULT STDMETHODCALLTYPE get_Source( 
        IObject **value) override
    {
        IFCPTR(value);
        *value = nullptr;

        foundation::ComPtr<foundation::IObject> spObject;
        IFR(m_pInner->GetSource(spObject.GetAddressOf()));
        if (spObject != nullptr)
        {
            return foundation::QueryInterface(spObject, value);
        }
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE put_Source( 
        IObject *value) override
    {
        foundation::ComPtr<foundation::IObject> spObject;
        if (value)
        {
            IFR(foundation::QueryInterface(value, spObject.GetAddressOf()));
        }
        return m_pInner->SetSource(spObject);
    }
};

//------------------------------------------------------------------------------
// Class:   CMoMultiSourceModelAdapter
//
// Purpose: Implementation of ISourceModel Interface
//
//------------------------------------------------------------------------------
class CMoMultiSourceModelAdapter :
    public foundation::AdapterBase
    <
        pmod::IMultiSourceModel,
        IMultiSourceModel,
        &__uuidof(IMultiSourceModel)
    >
{
public:

     HRESULT STDMETHODCALLTYPE AddSource( 
              IObject *value) override
     {
        IFCPTR(value);
        foundation::ComPtr<foundation::IObject> spObject;
        IFR(foundation::QueryInterface(value, spObject.GetAddressOf()));

        return m_pInner->AddSource(spObject);
     }
                        
     HRESULT STDMETHODCALLTYPE RemoveSource( 
                IObject *value) override
     {
        IFCPTR(value);
        foundation::ComPtr<foundation::IObject> spObject;
        IFR(foundation::QueryInterface(value, spObject.GetAddressOf()));

        return m_pInner->RemoveSource(spObject);
     }

};

//------------------------------------------------------------------------------
// Class:   CMoSourceModelBaseOverridesAdapter
//
// Purpose: Implementation of ISourceModelBaseOverrides Interface
//
//------------------------------------------------------------------------------
class CMoSourceModelBaseOverridesAdapter :
    public foundation::AdapterBase
    <
        pmod::ISourceModelBase,
        ISourceModelBaseOverrides,
        &__uuidof(ISourceModelBaseOverrides)
    >
{
public:                        
    HRESULT STDMETHODCALLTYPE OnSourceAttached( void) override
    {
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE OnSourceDetached( void) override
    {
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CMoSourceModelOverridesAdapter
//
// Purpose: Implementation of ISourceModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoSourceModelOverridesAdapter :
    public foundation::AdapterBase
    <
        pmod::ISourceModelBase,
        ISourceModelOverrides,
        &__uuidof(ISourceModelOverrides)
    >
{
public:
    HRESULT STDMETHODCALLTYPE OnSourceChanged( 
                            IObject *pSource)
    {
        UNREFERENCED_PARAMETER(pSource);
        return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   CMoMultiSourceModelOverridesAdapter
//
// Purpose: Implementation of IMultiSourceModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoMultiSourceModelOverridesAdapter :
    public foundation::AdapterBase
    <
        pmod::ISourceModelBase,
        IMultiSourceModelOverrides,
        &__uuidof(IMultiSourceModelOverrides)
    >
{
public:
};
