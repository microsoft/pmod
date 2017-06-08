/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceCollectionViewModelFactory.cpp
****/

#include "pch.h"

#include "MoMultiSourceCollectionViewModelFactory.h"
#include "MoObservableCollectionFactory.h"
#include "MoCollectionViewModelFactory.h"

#include "MoSourceModelDelegateAdapter.h"

#include <pmod/library/library_util.h>
#include <pmod/errors.h>

//------------------------------------------------------------------------------
// Class:   CMoMultiSourceCollectionViewModelDelegateAdapter
//
// Purpose: Implementation of ICollectionViewModelDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoMultiSourceCollectionViewModelDelegateAdapter :
    public CMoSourceModelBaseDelegateAdapter
    <
    pmod::library::IMultiSourceCollectionViewModelDelegate,
    pmod::INotifyCollectionChangedEventHandler,
    pmod::IObservableCollection
    >
{
protected:
     // IMultiSourceCollectionViewModelDelegate Implementation
    STDMETHOD( OnSourceCollectionChanged )(_In_ pmod::IObservableCollection *pSource,_In_ pmod::INotifyCollectionChangedEventArgs* pArgs)
    {
        if(IsOverrideComposed())
        {
            IMultiSourceCollectionViewModelOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));

            foundation::ComPtr<IObservableCollection> spMoCollectionModel;
            IFR(foundation::QueryInterface(pSource,spMoCollectionModel.GetAddressOf()));

            foundation::ComPtr<INotifyCollectionChangedEventArgs> spEventArgs;
            IFR(foundation::QueryInterface(pArgs,spEventArgs.GetAddressOf()));
            IFR(pOverrides->OnMultiSourceCollectionChanged(spMoCollectionModel,spEventArgs));
        }
        return S_OK;    
    }

};
STDMETHODIMP MoMultiSourceCollectionViewModelFactory::CreateInstance( 
        ObservableCollectionOptions modelOptions,
        CollectionViewModelOptions viewModelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        IMultiSourceCollectionViewModel **ppInstance)

{
    return CreateInstanceWithType( 
        GUID_NULL,
        modelOptions,
        viewModelOptions,
        pOuter,
        ppInner,
        ppInstance);
}

STDMETHODIMP MoMultiSourceCollectionViewModelFactory::CreateInstanceWithType( 
        GUID iidType,
        ObservableCollectionOptions modelOptions,
        CollectionViewModelOptions viewModelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        IMultiSourceCollectionViewModel **ppInstance)
{
    // Find the Type...
    if (!::IsEqualIID(iidType, GUID_NULL) && !foundation::IsTypeInfoRegistered(iidType))
    {
        return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
    }
        HRESULT hr;

    foundation::ComPtr<CMoObservableCollectionDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));

    foundation::ComPtr<CMoCollectionViewModelDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spViewModelDelegate;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spViewModelDelegate.GetAddressOf()));

    foundation::ComPtr<CMoMultiSourceCollectionViewModelDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spMultiSourceCollectionViewModelDelegateAdapter;
    IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spMultiSourceCollectionViewModelDelegateAdapter.GetAddressOf()));

    pmod::library::MultiSourceCollectionViewModelCreateParameters param;
    param.m_options = (pmod::library::ObservableCollectionOptions)modelOptions;
    param.m_view_model_options = (pmod::library::CollectionViewModelOptions)viewModelOptions;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_p_viewmodel_delegate = spViewModelDelegate;
    param.m_p_multi_source_CollectionViewModelDelegate = spMultiSourceCollectionViewModelDelegateAdapter;

    foundation::ComPtr<IInspectable> spInner;
    hr = pmod::library::GetMultiSourceCollectionViewModelClassFactory()->
        CreateMultiSourceCollectionViewModelInstance(&param,pOuter,spInner.GetAddressOf());
    IFHR(hr);
    IFHR(foundation::QueryInterface(spInner,ppInstance));
    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}
