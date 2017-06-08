/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoBaseDelegateAdapter.h
****/

#pragma once

#include <foundation/ctl/com_library.h>
#include <pmod/interfaces.h>
#include "Microsoft.PropertyModel.h"

//------------------------------------------------------------------------------
// Class:   CMoComposed
//
// Purpose: Base Class for a Composed Adapter
//
//------------------------------------------------------------------------------
class CMoComposed
{
public:
    CMoComposed():
        m_pOuter(nullptr)
    {
    }

    void SetOuter(IInspectable *pOuter)
    {
        m_pOuter = pOuter;
    }
protected:

    template <class Q>
    HRESULT QueryOverrides(_COM_Outptr_ Q*& pOut)
    {
        foundation_assert(IsOverrideComposed());
        return foundation::QueryWeakReference(m_pOuter,__uuidof(Q),&pOut);
    }

    bool IsOverrideComposed() { return m_pOuter != nullptr; }
private:
    IInspectable *m_pOuter;
};

//------------------------------------------------------------------------------
// Class:   _CMoDelegeteBaseAdapter
//
// Purpose: Base Class for a Delegate Adapter
//
//------------------------------------------------------------------------------
template <class TDelegate>
class _CMoDelegeteBaseAdapter:
    public foundation::ctl::ComBase,
    public CMoComposed,
    public TDelegate
{
public:

    _CMoDelegeteBaseAdapter()
    {
    }

protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == TDelegate::GetIID() )
        {
            *ppInterface = static_cast<TDelegate *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CMoDelegeteBaseAdapter
//
// Purpose: Base Class for a Delegate Adapter
//
//------------------------------------------------------------------------------
template <class TDelegate,class TEventHandler>
class CMoDelegeteBaseAdapter:
    public _CMoDelegeteBaseAdapter<TDelegate>
{
public:

    CMoDelegeteBaseAdapter()
    {
    }

protected:
    // Interface IModelDelegateBase<TEventHandler>
    STDMETHOD(OnRuntimeInitialize)()
    {
        if(IsOverrideComposed())
        {
            IObjectOverridesBase *pOverrides;
            IFR(QueryOverrides(pOverrides));
            IFR(pOverrides->OnRuntimeInitialize());
        }
        return S_OK;
    }

    STDMETHOD (OnAddEventHandler)(_In_ TEventHandler *pEventHandler)
    {
        UNREFERENCED_PARAMETER(pEventHandler);
        return S_OK;
    }

    STDMETHOD (OnRemoveEventHandler)(_In_ TEventHandler *pEventHandler)
    {
        UNREFERENCED_PARAMETER(pEventHandler);
        return S_OK;
    }

    STDMETHOD (OnStartObserve)()
    {
        if(IsOverrideComposed())
        {
            IObjectOverridesBase *pOverrides;
            IFR(QueryOverrides(pOverrides));
            IFR(pOverrides->OnStartObserve());
        }
        return S_OK;
    }

    STDMETHOD (OnStopObserve)()
    {
        if(IsOverrideComposed())
        {
            IObjectOverridesBase *pOverrides;
            IFR(QueryOverrides(pOverrides));
            IFR(pOverrides->OnStopObserve());
        }
        return S_OK;
    }
};

template <class TBASE>
static HRESULT CreateMoDelegateAdapter(IInspectable *pOuter,TBASE **ppNewInstance)
{
    IFR_ASSERT(foundation::ctl::ComObject<TBASE>::CreateInstance(ppNewInstance));
    (*ppNewInstance)->SetOuter(pOuter);
    return S_OK;
}

