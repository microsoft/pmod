/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModelFactory.cpp
****/

#include "pch.h"

#include "MoViewModelFactory.h"
#include "MoObservableObjectFactory.h"

#include "MoSourceModelDelegateAdapter.h"

#include <pmod/library/library_util.h>

//------------------------------------------------------------------------------
// Class:   CMoViewModelDelegateAdapter
//
// Purpose: Implementation of IViewModelDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoViewModelDelegateAdapter :
    public CMoSourceModelDelegateAdapter
    <
    pmod::library::IViewModelDelegate,
    pmod::IPropertyChangedEventHandler,
    pmod::IObservableObject
    >
{
protected:
     // IViewModelDelegate Implementation
    STDMETHOD( OnSourcePropertyChanged )(
        _In_ UINT32 propertyId,_In_ IInspectable *oldValue,_In_ IInspectable *newValue) override
    {
        if(IsOverrideComposed())
        {
            IViewModelOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));

            IFR(pOverrides->OnSourcePropertyChanged(propertyId,oldValue,newValue));
        }
        return S_OK;
    }
    STDMETHOD( OnSourceEventModel )(_In_ UINT32 eventId,_In_ IInspectable *pEventArgs)
    {
        if(IsOverrideComposed())
        {
            IViewModelOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));

            IFR(pOverrides->OnSourceEventModel(eventId,pEventArgs));
        }
        return S_OK;
    }
};

STDMETHODIMP MoPropertyViewModelFactory::CreateInstanceWithSource( 
        IObservableObject *pSource,
        ObservableObjectOptions modelOptions,
        ViewModelOptions viewModelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        IViewModel **ppInstance)
{
    return CreateInstanceWithSourceAndType( 
        GUID_NULL,
        pSource,
        modelOptions,
        viewModelOptions,
        pOuter,
        ppInner,
        ppInstance);
}

STDMETHODIMP MoPropertyViewModelFactory::CreateInstanceWithSourceAndType( 
        GUID iidType,
        IObservableObject *pSource,
        ObservableObjectOptions modelOptions,
        ViewModelOptions viewModelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        IViewModel **ppInstance)
{
    // Find the Type...
    if (!::IsEqualIID(iidType, GUID_NULL) && !foundation::IsTypeInfoRegistered(iidType))
    {
        return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
    }
    HRESULT hr;

    foundation::ComPtr<CMoObservableObjectDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    foundation::ComPtr<CMoViewModelDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spViewModelDelegate;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spViewModelDelegate.GetAddressOf()));

    foundation::ComPtr<pmod::IObservableObject> spSource;
    if(pSource != nullptr)
    {
        IFHR(foundation::QueryInterface(pSource,spSource.GetAddressOf()));
    }


    pmod::library::ViewModelCreateParameters param;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_p_viewmodel_delegate = spViewModelDelegate;
    param.m_p_source = spSource;

    param.m_options = (pmod::library::ObservableObjectOptions)modelOptions;
    param.m_view_model_options = (pmod::library::ViewModelOptions)viewModelOptions;

    foundation::ComPtr<IInspectable> spInner;
    hr = pmod::library::GetViewModelClassFactory()->
            CreateViewModelInstance(&param,pOuter,spInner.GetAddressOf());
    IFHR(hr);
    IFHR(foundation::QueryInterface(spInner,ppInstance));
    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}
