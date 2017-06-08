/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCallbackAdapterBase.h
****/

#pragma once

#include <foundation/assert.h>

#include <foundation/ctl/com_library.h>

//------------------------------------------------------------------------------
// Class:   TMoCallbackAdapterBase
//
// Purpose: Define a template base class for a Delegate Adapter
//
//------------------------------------------------------------------------------

interface IDelegateAdapterInternal : public IUnknown
{
    virtual HRESULT GetModernDelegate(IUnknown **ppModernDelegate) = 0;
};

struct __declspec(uuid("B214F740-895C-4A31-B3D9-A622AA4A7BC8")) IDelegateAdapterInternal;

template <class TMoDelegate,class TDelegate,const IID* piidDelegate>
class TMoCallbackAdapterBase :
    public foundation::ctl::Implements<TDelegate,piidDelegate>,
    public IDelegateAdapterInternal
{
public:
    template <class T>
    static HRESULT CreateInstance(TMoDelegate *pMoDelegate,TDelegate **ppDelegate)
    {
        foundation_assert(pMoDelegate);
        foundation_assert(ppDelegate);

        T *pCallbackAdapter = nullptr;
        IFR(foundation::ctl::ComObject<T>::CreateInstance(&pCallbackAdapter));
        pCallbackAdapter->m_spMoDelegate = pMoDelegate;
        *ppDelegate = static_cast<TDelegate *>(pCallbackAdapter);
        return S_OK;
    }

    static HRESULT GetModernDelegateFromCore(TDelegate *pDelegate,TMoDelegate **ppModernDelegate)
    {
        foundation_assert(ppModernDelegate);
        if(pDelegate)
        {
            foundation::ComPtr<IDelegateAdapterInternal> spDelegateAdapterInternal;
            IFR(foundation::QueryInterface(pDelegate,spDelegateAdapterInternal.GetAddressOf()));
            IFR(spDelegateAdapterInternal->GetModernDelegate(reinterpret_cast<IUnknown **>(ppModernDelegate)));
        }
        else
        {
            *ppModernDelegate = nullptr;
        }
        return S_OK;
    }
protected:
    // Interface IDelegateAdapterInternal
    HRESULT GetModernDelegate(IUnknown **ppModernDelegate)
    {
        if(m_spMoDelegate)
        {
            *ppModernDelegate = m_spMoDelegate;
            (*ppModernDelegate)->AddRef();
        }
        return S_OK;
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == __uuidof(IDelegateAdapterInternal))
        {
            *ppInterface = static_cast<IDelegateAdapterInternal *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    TMoDelegate *GetMoDelegate() 
    {
        foundation_assert(m_spMoDelegate != nullptr);
        return m_spMoDelegate;
    }
private:
    foundation::ComPtr<TMoDelegate> m_spMoDelegate;
};

