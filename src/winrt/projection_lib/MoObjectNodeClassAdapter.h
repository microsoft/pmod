/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObjectNodeClassAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;


//------------------------------------------------------------------------------
// Class:   CMoObjectNodeClassAdapter
//
// Purpose: Implementation of IObjectNodeClass Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoObjectNodeClassAdapter :
    public foundation::AdapterBase
        <
        pmod::library::IObjectNodeClass,
        IObjectNodeClass,
        &__uuidof(IObjectNodeClass)
        >
{
    // Interface IObjectNodeClass support
    HRESULT STDMETHODCALLTYPE get_ParentInternal( 
                            IObject **ppValue)  override
    {
		foundation::ComPtr<foundation::IObject> spThisObject;
		IFR_ASSERT(foundation::QueryInterface(m_pInner, spThisObject.GetAddressOf()));

		foundation::ComPtr<foundation::IObject> spParentObject;
		IFR(foundation::GetObjectParent(spThisObject, spParentObject.GetAddressOf()));

		IFR_ASSERT(foundation::QueryInterfaceIf(spParentObject, ppValue));
        return S_OK;    
    }

    HRESULT STDMETHODCALLTYPE put_ParentInternal( 
                            IObject *pValue)  override
    {
        foundation::ComPtr<foundation::IObject> spObject;
        IFR(foundation::QueryInterfaceIf(pValue, spObject.GetAddressOf()));
        IFR(m_pInner->SetParentInternal(spObject));
        return S_OK;
    }
    
    HRESULT STDMETHODCALLTYPE get_LocalWorkingDispatcher(
        IDispatcher **ppDispatcher) override
    {
        IFCPTR(ppDispatcher);

        foundation::ComPtr<foundation::library::IDispatcher> spDispatcher;
        IFR(m_pInner->GetLocalWorkingDispatcher(spDispatcher.GetAddressOf()));
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
    
    HRESULT STDMETHODCALLTYPE put_LocalWorkingDispatcher( 
        IDispatcher *pDispatcher) override
    {
        foundation::ComPtr<foundation::library::IDispatcher> spDispatcher;
        if(pDispatcher != nullptr)
        {
            IFR(CDispatcherAdapter::CreateInstance(pDispatcher,spDispatcher.GetAddressOf()));
        }
        IFR(m_pInner->SetLocalWorkingDispatcher(spDispatcher));
        return S_OK;
   }

    HRESULT STDMETHODCALLTYPE get_WorkingDispatcher(
        IDispatcher **ppDispatcher) override
    {
        IFCPTR(ppDispatcher);

        foundation::ComPtr<foundation::library::IDispatcher> spDispatcher;
        IFR(m_pInner->GetWorkingDispatcher(spDispatcher.GetAddressOf()));
        if (spDispatcher != nullptr)
        {
            return MoDispatcherAdapter::CreateInstance(spDispatcher, ppDispatcher);
        }
        else
        {
            *ppDispatcher = nullptr;
            return S_OK;
        }
    }

    HRESULT STDMETHODCALLTYPE BeginModelChanges(BatchModelChangesOptions options) override
    {
        return m_pInner->BeginModelChanges((pmod::library::BatchModelChangesOptions)options);
    }
                        
    HRESULT STDMETHODCALLTYPE EndModelChanges(EndModelChangesOptions options) override
    {
        return m_pInner->EndModelChanges((pmod::library::EndModelChangesOptions)options);
    }
};

