/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObjectClassAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

#include "MoDispatcherAdapter.h"

//------------------------------------------------------------------------------
// Class:   CMoObjectNodeClassAdapter
//
// Purpose: Implementation of IObjectNodeClass Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoObjectClassAdapter :
    public foundation::AdapterBase
        <
        foundation::library::IFoundationClassBase,
        IObjectClass,
        &__uuidof(IObjectClass)
        >
{
    // Interface IModelProtected support
    HRESULT STDMETHODCALLTYPE get_IsObserved( 
                        boolean *value) override
    {
        bool isObserved;
        IFR_ASSERT(m_pInner->GetIsObserved(&isObserved));
        *value = isObserved ? true:false;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE get_FoundationOptions(
        unsigned short *value) override
    {
        return m_pInner->GetFoundationOptions(value);
    }

    HRESULT STDMETHODCALLTYPE get_EventDispatcher( 
        IDispatcher **ppDispatcher) override
    {
        IFCPTR(ppDispatcher);

        foundation::ComPtr<foundation::library::IDispatcher> spDispatcher;
        IFR(m_pInner->GetEventDispatcher(spDispatcher.GetAddressOf()));
        if(spDispatcher != nullptr)
        {
            return MoDispatcherAdapter::CreateInstance(spDispatcher,ppDispatcher);
        }
        else
        {
            *ppDispatcher = nullptr;
            return S_OK;
        }
    }

    HRESULT STDMETHODCALLTYPE put_EventDispatcher( 
        IDispatcher *pDispatcher) override
    {
        foundation::ComPtr<foundation::library::IDispatcher> spDispatcher;
        if(pDispatcher != nullptr)
        {
            IFR_ASSERT(CDispatcherAdapter::CreateInstance(pDispatcher, spDispatcher.GetAddressOf()));
        }
        IFR_ASSERT(m_pInner->SetEventDispatcher(spDispatcher));
        return S_OK;
    }
};
