/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoDispatcherAdapter.h
****/

#pragma once

#include <pmod/interfaces.h>
#include "Microsoft.PropertyModel.h"
#include <foundation/ctl/com_library.h>
#include <foundation/library/logger_macros.h>
#include "FtmBase.h"

#include <foundation/com_ptr.h>

using namespace ABI::Microsoft::PropertyModel;

//+-----------------------------------------------------------------------------
// Class:   _TDelegateDispatchableAdapter
//
// Purpose: Template to translate <-> IDelegateDispatchable from Core/WinRT
//------------------------------------------------------------------------------
template<class TDispatchable,class TInterface,class TBase,const IID *pIIDInterface>
class _TDelegateDispatchableAdapter : 
    public foundation::ctl::ImplementsBase<TInterface, TBase, pIIDInterface>
{
public:
    static HRESULT CreateInstance(
        _In_ TDispatchable *pDispatchable,
        _COM_Outptr_ TInterface** ppDelegateDispatchable
        )
    {
        foundation::ComPtr<foundation::ctl::ComObject<_TDelegateDispatchableAdapter>> spDelegateDispatchableAdapter;
        IFR(foundation::ctl::ComInspectableObject<_TDelegateDispatchableAdapter>::CreateInstance(spDelegateDispatchableAdapter.GetAddressOf()));

        spDelegateDispatchableAdapter->m_spDispatchable = pDispatchable;
        *ppDelegateDispatchable = spDelegateDispatchableAdapter.Detach();
        return S_OK;
    }
protected:
    HRESULT STDMETHODCALLTYPE Invoke( void)
    {
        return m_spDispatchable->Invoke();
    }
private:
    foundation::ComPtr<TDispatchable> m_spDispatchable;
};

typedef _TDelegateDispatchableAdapter<
    IDispatchable,
    foundation::library::IDelegateDispatchable,
    foundation::ctl::ComBase,
    &foundation::library::IID_IDelegateDispatchable> CDelegateDispatchableAdapter;

typedef _TDelegateDispatchableAdapter<
    foundation::library::IDelegateDispatchable,
    IDispatchable,
    _FtmInspectableBase,
    &IID_IDispatchable> MoDelegateDispatchableAdapter;

//+-----------------------------------------------------------------------------
// Class:   MoDispatcherAdapterBase
//
// Purpose: Base Adapter for winRT for the IDispatcher interface
//------------------------------------------------------------------------------
template <class T>
class MoDispatcherAdapterBase : 
    public foundation::ctl::ImplementsInspectable<IDispatcher,&IID_IDispatcher>
{
public:
protected:   
    HRESULT STDMETHODCALLTYPE get_HasThreadAccess(boolean *value)
    {
        IFCPTR(value);

        bool bValue;
        IFR(_GetCoreDispatcher()->HasThreadAccess(&bValue));
        *value = bValue ? true:false;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE RunAsync( 
        IDispatchable *pDispatchable,
        IAsyncOperation **ppAsyncAction) override
    {
        foundation::ComPtr<foundation::library::IDelegateDispatchable> spDelegateDispatchable;
        IFR(CDelegateDispatchableAdapter::CreateInstance(pDispatchable,spDelegateDispatchable.GetAddressOf()));

        foundation::ComPtr<foundation::IAsyncOperation> spAsyncOperationModel;
        IFR(_GetCoreDispatcher()->RunAsync(spDelegateDispatchable,spAsyncOperationModel.GetAddressOf()));
        if(spAsyncOperationModel != nullptr)
        {
            IFR(foundation::QueryInterface(spAsyncOperationModel,ppAsyncAction));
        }
        else
        {
            *ppAsyncAction = nullptr;
        }
        return S_OK;
    }
private:   
    foundation::library::IDispatcher *_GetCoreDispatcher()
    {
        return (static_cast<T*>(this))->GetCoreDispatcher();
    }
};
//+-----------------------------------------------------------------------------
// Class:   MoDispatcherAdapter
//
// Purpose: Adapter for winRT for the IDispatcher interface
//------------------------------------------------------------------------------
class MoDispatcherAdapter : 
    public MoDispatcherAdapterBase<MoDispatcherAdapter>
{
public:
    static HRESULT 
        CreateInstance
        (
        _In_ foundation::library::IDispatcher *pDispatcher,
        _COM_Outptr_ IDispatcher** ppDispatcher
        )
    {
        foundation_assert(pDispatcher);
        foundation_assert(ppDispatcher);

        foundation::ComPtr<foundation::ctl::ComInspectableObject<MoDispatcherAdapter>> spMoDispatcherAdapter;
        IFR(foundation::ctl::ComInspectableObject<MoDispatcherAdapter>::CreateInstance(spMoDispatcherAdapter.GetAddressOf()));

        spMoDispatcherAdapter->m_spDispatcher = pDispatcher;
        *ppDispatcher = spMoDispatcherAdapter.Detach();
        return S_OK;
    }
    foundation::library::IDispatcher *GetCoreDispatcher()
    {
        return m_spDispatcher;
    }
private:
    foundation::ComPtr<foundation::library::IDispatcher> m_spDispatcher;
};

//+-----------------------------------------------------------------------------
// Class:   CDispatcherAdapter
//
// Purpose: Adapter for winRT for the IDispatcher interface
//------------------------------------------------------------------------------
class CDispatcherAdapter : 
    public foundation::ctl::Implements<foundation::library::IDispatcher,&foundation::library::IID_IDispatcher>
{
public:
    static HRESULT 
        CreateInstance
        (
        _In_ ABI::Microsoft::PropertyModel::IDispatcher *pDispatcher,
        _COM_Outptr_ foundation::library::IDispatcher** ppDispatcher
        )
    {
        foundation_assert(pDispatcher);
        foundation_assert(ppDispatcher);

        foundation::ComPtr<foundation::ctl::ComInspectableObject<CDispatcherAdapter>> spDispatcherAdapter;
        IFR(foundation::ctl::ComInspectableObject<CDispatcherAdapter>::CreateInstance(spDispatcherAdapter.GetAddressOf()));

        spDispatcherAdapter->m_spDispatcher = pDispatcher;
        *ppDispatcher = spDispatcherAdapter.Detach();
        return S_OK;
    }
protected:   
    HRESULT STDMETHODCALLTYPE HasThreadAccess(bool *pValue)
    {
        IFCPTR(pValue);

        boolean value;
        IFR(m_spDispatcher->get_HasThreadAccess(&value));
        *pValue = value ? true:false;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE RunAsync( 
        foundation::library::IDelegateDispatchable *pDispatchable,
        foundation::IAsyncOperation **ppAsyncAction) override
    {
        foundation::ComPtr<IDispatchable> spDispatchable;
        IFR(MoDelegateDispatchableAdapter::CreateInstance(pDispatchable,spDispatchable.GetAddressOf()));

        foundation::ComPtr<IAsyncOperation> spAsyncOperationModel;
        IFR(m_spDispatcher->RunAsync(spDispatchable,spAsyncOperationModel.GetAddressOf()));
        if(spAsyncOperationModel != nullptr)
        {
            IFR(foundation::QueryInterface(spAsyncOperationModel,ppAsyncAction));
        }
        else
        {
            *ppAsyncAction = nullptr;
        }
        return S_OK;
    }
private:
    foundation::ComPtr<ABI::Microsoft::PropertyModel::IDispatcher> m_spDispatcher;
};
