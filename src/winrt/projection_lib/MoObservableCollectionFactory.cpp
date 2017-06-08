/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionFactory.cpp
****/

#include "pch.h"

#include "MoObservableCollectionFactory.h"

#include <pmod/library/library_util.h>
#include <pmod/errors.h>

//------------------------------------------------------------------------------
// Class:   CMoObservableCollectionDelegateAdapter
//
// Purpose: Implementation of IObservableObjectDelegate Interface Adapter
//
//------------------------------------------------------------------------------
STDMETHODIMP CMoObservableCollectionDelegateAdapter::OnCreateItemsContainer(_COM_Outptr_ pmod::library::IItemsContainer **ppValue)
{
    UNREFERENCED_PARAMETER(ppValue);
    return S_OK;
}

STDMETHODIMP CMoObservableCollectionDelegateAdapter::OnCollectionChanged(_In_ pmod::INotifyCollectionChangedEventArgs *pEventArgs)
{
    if(IsOverrideComposed())
    {
        IObservableCollectionBaseOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));

        foundation::ComPtr<INotifyCollectionChangedEventArgs> spMoNotifyCollectionChangedEventArgs;
        IFR(foundation::QueryInterface(pEventArgs,spMoNotifyCollectionChangedEventArgs.GetAddressOf()));
        IFR(pOverrides->OnCollectionChanged(spMoNotifyCollectionChangedEventArgs));
        return S_OK;
    }
    return S_OK;
}

STDMETHODIMP MoCollectionModelFactory::CreateInstance(
        IInspectable* pOuter,
        IInspectable** ppInner,
        IObservableCollection** ppInstance)
{
    return CreateInstanceWithOptions(
        ObservableCollectionOptions_None,
        pOuter,
        ppInner,
        ppInstance);
}

STDMETHODIMP MoCollectionModelFactory::CreateInstanceWithOptions(
        ObservableCollectionOptions modelOptions,
        IInspectable* pOuter,
        IInspectable** ppInner,
        IObservableCollection** ppInstance)
{
    return CreateInstanceWithTypeAndOptions(
        GUID_NULL,
        modelOptions,
        pOuter,
        ppInner,
        ppInstance);
}

STDMETHODIMP MoCollectionModelFactory::CreateInstanceWithTypeAndOptions(
        GUID iidType,
        ObservableCollectionOptions modelOptions,
        IInspectable* pOuter,
        IInspectable** ppInner,
        IObservableCollection** ppInstance)
{
    // Find the Type...
    if (!::IsEqualIID(iidType, GUID_NULL) && !foundation::IsTypeInfoRegistered(iidType))
    {
        return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
    }
    HRESULT hr;

    foundation::ComPtr<CMoObservableCollectionDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    pmod::library::ObservableCollectionCreateParameters param;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_options = (pmod::library::ObservableCollectionOptions)modelOptions;

    foundation::InspectablePtr spInner;
    hr = pmod::library::GetObservableCollectionClassFactory()->
        CreateObservableCollectionInstance(&param,pOuter,spInner.GetAddressOf());
    IFHR(hr);

    IFHR(foundation::QueryInterface(spInner,ppInstance));
    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}
