/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:adapter_base.h
****/

#pragma once

#include <foundation/assert.h>

#include <foundation/ctl/com_library.h>
#include <foundation/macros.h>
#include <foundation/interfaces.h>
#include "interfaces/fire_event.h"
#include "interfaces/interface_adapter.h"

namespace foundation
{
    // Error returned from adapters when the Interface is not supported
    const   HRESULT E_ADAPTER_INTERFACE_NOT_SUPPORTED = 0x80f60400;

//+-----------------------------------------------------------------------------
// Function:   CreateAdapter
//
// Purpose: create a model adapter instance 
//
//------------------------------------------------------------------------------
template <class T>
static HRESULT STDMETHODCALLTYPE CreateAdapter(
    _In_ foundation::IInspectable *pOuter,
    _In_ foundation::IInspectable *pInner,
    _COM_Outptr_ foundation::IInspectable **ppUnknown)
{
    foundation_assert(ppUnknown);
    *ppUnknown = nullptr;

    foundation_assert(pOuter);

    HRESULT hr = S_OK;

    ctl::AggregableComInspectableObject<T> *pNewInstance;
    IFHR_ASSERT(ctl::AggregableComInspectableObject<T>::CreateInstance(pOuter, &pNewInstance));
    IFR_ASSERT(pNewInstance->_Initialize(pOuter, pInner));
    *ppUnknown = reinterpret_cast<foundation::IInspectable*>(static_cast<ctl::INonDelegatingInspectable *>(pNewInstance));
    return S_OK;
}

template <class T>
static HRESULT STDMETHODCALLTYPE CreateAdapterWithOuter(
    _In_ foundation::IInspectable *pOuter,
    _In_ foundation::IInspectable *pInner,
    _COM_Outptr_ foundation::IInspectable **ppUnknown)
{
    UNREFERENCED_PARAMETER(pInner);
    return CreateAdapter<T>(pOuter, pOuter, ppUnknown);
}

//------------------------------------------------------------------------------
// Class:   AdapterBase
//
// Purpose: Define a template base class for an Adapter class
//
//------------------------------------------------------------------------------
template <class TInnerInterface,class TInterface,const IID* piid,class TBASE=ctl::ComInspectableBase>
class AdapterBase :
    public TBASE,
    public TInterface,
    public library::IInterfaceAdapter
{
public:
    HRESULT _Initialize(
        _In_ foundation::IInspectable *pOuter,
        _In_ foundation::IInspectable *pInner
        )
    {
        foundation_assert(pOuter);
        foundation_assert(pInner);

        m_pOuter = pOuter;
        IFR_ASSERT(foundation::QueryWeakReference(pInner,&m_pInner));
        return S_OK;
    }

protected:
    AdapterBase():
        m_pOuter(nullptr),
        m_pInner(nullptr)
    {
    }

    // override TBASE
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        HRESULT hr = QueryAdapterInterface(iid,ppInterface);
        if(hr == E_NOINTERFACE)
        {
            hr = foundation::QueryWeakReference(m_pOuter,iid,ppInterface);
        }
        return hr;
    }

    // Interface foundation::IInterfaceAdapter
	STDMETHOD_(IUnknown *, _GetInnerInterface)()
	{
		return this->GetInnerInterface();
	}

    STDMETHOD (QueryAdapterInterface)(REFIID iid, foundation::IUnknown **ppInterface) 
    {
        if (iid == *piid)
        {
            *ppInterface = static_cast<TInterface *>(this);
        }
        else if (iid == library::IInterfaceAdapter::GetIID())
        {
            *ppInterface = static_cast<library::IInterfaceAdapter *>(this);
        }
        else
        {
            if (iid == foundation_IID_IInspectable || iid == foundation_IID_IUnknown)
            {
                return E_NOINTERFACE;
            }
            else
            {
                return TBASE::QueryInterfaceImpl(iid,ppInterface);
            }
        }
        return S_OK;
    }

    STDMETHOD (HasEventHandlers)(_Out_ bool *pValue)
    {
        *pValue = false;
        return S_OK;
    }

    STDMETHOD(FireAdapterEvent)(
		_In_ library::IFireEventHandlerContainer *pFireEventHandlerContainer,
		_In_ foundation::IUnknown* pEventArgs)
    {
		UNREFERENCED_PARAMETER(pFireEventHandlerContainer);
		UNREFERENCED_PARAMETER(pEventArgs);
        return S_OK;
    }

    STDMETHOD(OnHostObservedChanged)(_In_ bool isObserved)
    {
        UNREFERENCED_PARAMETER(isObserved);
        return S_OK;
    }

    HRESULT _OnAdapterAddEventHandler(foundation::IUnknown *pEventHandler)
    {
        foundation::IUnknown *pThisUnk = static_cast<foundation::ctl::ComInspectableBase *>(this);
        foundation::library::IInterfaceAdapterOverrides *pInterfaceAdapterOverrides = nullptr;
        IFR_ASSERT(foundation::QueryWeakReference(pThisUnk, &pInterfaceAdapterOverrides));
        return pInterfaceAdapterOverrides->OnAdapterAddHandler(pThisUnk, pEventHandler);
    }

    HRESULT _OnAdapterRemoveEventHandler(foundation::IUnknown *pEventHandler)
    {
        foundation::IUnknown *pThisUnk = static_cast<foundation::ctl::ComInspectableBase *>(this);
        foundation::library::IInterfaceAdapterOverrides *pInterfaceAdapterOverrides = nullptr;
        IFR_ASSERT(foundation::QueryWeakReference(pThisUnk, &pInterfaceAdapterOverrides));
        return pInterfaceAdapterOverrides->OnAdapterRemoveHandler(pThisUnk, pEventHandler);
    }

    TInnerInterface *GetInner()
    {
        foundation_assert(m_pInner);
        return m_pInner;
    }

protected:
    foundation::IInspectable        *m_pOuter;
    TInnerInterface					*m_pInner;
};

//------------------------------------------------------------------------------
// Class:   AdapterPrivateBase
//
// Purpose: Define a template base class for an Adapter class that has both a public and
// private interfaces to be used
//
//------------------------------------------------------------------------------
template <class TInnerPrivateModel,class TBASE>
class AdapterPrivateBase :
    public TBASE
{
public:

    HRESULT _Initialize(
        _In_ foundation::IInspectable *pOuter,
        _In_ foundation::IInspectable *pInner
        )
    {
        IFR_ASSERT(TBASE::_Initialize(pOuter, pInner));
        IFR_ASSERT(foundation::QueryWeakReference(pOuter, &m_pInnerPrivate));
        return S_OK;
    }
protected:
    AdapterPrivateBase() :
        m_pInnerPrivate(nullptr)
    {
    }

    TInnerPrivateModel *GetInnerPrivate()
    {
        foundation_assert(m_pInnerPrivate);
        return m_pInnerPrivate;
    }

protected:
    TInnerPrivateModel           *m_pInnerPrivate;
};

//------------------------------------------------------------------------------
// Class:   NotSupportedAdapter
//
// Purpose: Define a template base class for an Adapter class that has both a public and
//
//------------------------------------------------------------------------------
struct _INotSupportedAdapterInterface : IUnknown
{};

template <const IID* piid>
class NotSupportedAdapter :
    public AdapterBase<IInspectable, _INotSupportedAdapterInterface, piid>
{
protected:
    STDMETHOD(QueryAdapterInterface)(REFIID iid, foundation::IUnknown **ppInterface)
    {
        if (iid == *piid)
        {
            return E_ADAPTER_INTERFACE_NOT_SUPPORTED;
        }
        else
        {
            return AdapterBase<IInspectable, _INotSupportedAdapterInterface, piid>::QueryAdapterInterface(iid, ppInterface);
        }
    }
};

}
