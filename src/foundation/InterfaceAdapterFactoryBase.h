/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InterfaceAdapterFactoryBase.h
****/
#pragma once
//+----------------------------------------------------------------------------
//
// Copyright (c) Microsoft Corporation
//
// File:        ModelAdapterFactoryBase.h
//
// Contents:    Base class for Hosting Model Adapters
//
// Classes:     CInterfaceAdapterFactoryBase
//
//
//------------------------------------------------------------------------------

#include <memory>
#include <vector>
#include <algorithm>

#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>
#include <foundation/macros.h>
#include <foundation/library/interfaces/disposable.h>
#include <foundation/library/interfaces/interface_adapter.h>

//------------------------------------------------------------------------------
// Class:   CInterfaceAdapterFactoryBase 
//
// Purpose: Define a base class for a Factory class that can register Model Adapters
//
//------------------------------------------------------------------------------
template <class TBASE,class TInterfaceFactoryHost>
class CInterfaceAdapterFactoryBase:
    public TBASE,
    public TInterfaceFactoryHost
{
public:
    // Interface IInterfaceAdapterFactoryHost
    // Attempt to create a Model Adapter for the 'pOuter' object
    STDMETHOD(QueryCreateInterfaceAdapter)(
        _In_ foundation::IInspectable *pOuter,
        _In_ foundation::IInspectable *pInner,
        _In_ REFIID riid_outer,
        _In_ REFIID riid,
        _Outptr_ foundation::IInspectable **ppModelAdapterInnerUnk) override
    {        
        return 
            QueryCreateAdapterInterfaceInternal
            (
                pOuter,
                pInner,
                riid_outer,
                riid,
                ppModelAdapterInnerUnk
            );
    }

    STDMETHOD(RegisterInterfaceAdapterFactory)(foundation::library::IInterfaceAdapterFactory *pModelAdapterFactory)
    {
        return RegisterInterfaceAdapterFactoryInternal(pModelAdapterFactory);
    }
    STDMETHOD(UnregisterInterfaceAdapterFactory)(foundation::library::IInterfaceAdapterFactory *pModelAdapterFactory)
    {
        return UnregisterInterfaceAdapterFactoryInternal(pModelAdapterFactory);
    }

    // ComBase Overrides
    void FinalRelease() override
    {
        m_InterfaceAdapterFactories.reset();
        TBASE::FinalRelease();
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IInterfaceAdapterFactoryHost::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IInterfaceAdapterFactoryHost *>(this);
        }
        else
        {
            return TBASE::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }

protected:
    void DisposeModelAdapterFactories()
    {
        if(m_InterfaceAdapterFactories.get() != nullptr)
        {
            for(InterfaceAdapterFactoryVector::iterator iter = m_InterfaceAdapterFactories->begin();
                iter != m_InterfaceAdapterFactories->end();
                ++iter)
            {
                foundation::library::IDisposable *pDisposable = nullptr;
                if(SUCCEEDED(foundation::QueryWeakReference(*iter,&pDisposable)))
                {
                    pDisposable->Dispose();
                }
            }
        }
    }

    typedef std::vector< foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> > InterfaceAdapterFactoryVector;

    // Attempt to create a Model Adapter for the 'pOuter' object
    virtual HRESULT QueryCreateAdapterInterfaceInternal(
        _In_ foundation::IInspectable *pOuter,
        _In_ foundation::IInspectable *pInner,
        _In_ REFIID riid_outer,
        _In_ REFIID riid,
        _Outptr_ foundation::IInspectable **ppModelAdapterInnerUnk)
    {
        if(m_InterfaceAdapterFactories.get() == nullptr)
        {
            return E_NOINTERFACE;
        }

        for(InterfaceAdapterFactoryVector::iterator iter = m_InterfaceAdapterFactories->begin();
            iter != m_InterfaceAdapterFactories->end();
            ++iter)
        {
            HRESULT hr = (*iter)->TryCreateInterfaceAdapter(
                riid_outer,
                riid,
                pOuter,
                pInner,
                ppModelAdapterInnerUnk);
            if(SUCCEEDED(hr))
            {
                // Adapter was found
                return S_OK;
            }
        }
        return E_NOINTERFACE;
    }

    virtual HRESULT RegisterInterfaceAdapterFactoryInternal(foundation::library::IInterfaceAdapterFactory *pInterfaceAdapterFactory)
    {
        IFCPTR_ASSERT(pInterfaceAdapterFactory);
        if(m_InterfaceAdapterFactories.get() == nullptr)
        {
            m_InterfaceAdapterFactories.reset(new InterfaceAdapterFactoryVector());
        }
        m_InterfaceAdapterFactories->push_back(pInterfaceAdapterFactory);
        return S_OK;
    }

    virtual HRESULT UnregisterInterfaceAdapterFactoryInternal(foundation::library::IInterfaceAdapterFactory *pInterfaceAdapterFactory)
    {
        IFCPTR_ASSERT(pInterfaceAdapterFactory);
        if(m_InterfaceAdapterFactories.get() == nullptr)
        {
            return E_UNEXPECTED;
        }

        InterfaceAdapterFactoryVector::iterator iter = std::find(
                m_InterfaceAdapterFactories->begin(),
                m_InterfaceAdapterFactories->end(),
                pInterfaceAdapterFactory);
        if(iter != m_InterfaceAdapterFactories->end())
        {
            m_InterfaceAdapterFactories->erase(iter);
        }
        return S_OK;
    }


private:
    // Model Adapter support
    std::auto_ptr<InterfaceAdapterFactoryVector> m_InterfaceAdapterFactories;

};
