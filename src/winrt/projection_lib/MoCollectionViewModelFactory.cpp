/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCollectionViewModelFactory.cpp
****/

#include "pch.h"

#include "MoCollectionViewModelFactory.h"
#include "MoObservableCollectionFactory.h"

#include <pmod/library/library_util.h>
#include <foundation/library/library_util.h>
#include <pmod/errors.h>

STDMETHODIMP CMoCollectionViewModelDelegateAdapter::OnSourceCollectionChanged(_In_ pmod::INotifyCollectionChangedEventArgs* pArgs)
{
    if(IsOverrideComposed())
    {
        ICollectionViewModelBaseOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));

        foundation::ComPtr<INotifyCollectionChangedEventArgs> spEventArgs;
        IFR(foundation::QueryInterface(pArgs,spEventArgs.GetAddressOf()));
        IFR(pOverrides->OnSourceCollectionChanged(spEventArgs));
    }
    return S_OK;    
}

    // When a Source Item Property has changed
STDMETHODIMP CMoCollectionViewModelDelegateAdapter::OnSourceItemPropertyChanged(
            _In_ pmod::IObservableObject *item,
            _In_ UINT32 propertyId,
            _In_ IInspectable *oldValue,
            _In_ IInspectable *newValue,
            _Outptr_ bool *pNeedRefresh)
{
    if(IsOverrideComposed())
    {
        ICollectionViewModelBaseOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));

        foundation::ComPtr<IObservableObject> spItem;
        IFR(foundation::QueryInterface(item,spItem.GetAddressOf()));

        boolean bNeedRefresh;
        IFR(pOverrides->OnSourceItemPropertyChanged(
            spItem,
            propertyId,oldValue,newValue,&bNeedRefresh));
        *pNeedRefresh = bNeedRefresh ? true:false;
    }
    return S_OK;
}

STDMETHODIMP CMoCollectionViewModelDelegateAdapter::FilterItem(_In_ IInspectable * item,_Outptr_ bool *pValue)
{
    HRESULT hr = S_FALSE;
    if(IsOverrideComposed())
    {
        ICollectionViewModelBaseOverrides *pOverrides;
        IFHR(QueryOverrides(pOverrides));

        boolean bFilter;
        IFHR(pOverrides->FilterItem(item,&bFilter));
        *pValue = bFilter ? true:false;
    }
    return hr;
}    

STDMETHODIMP CMoCollectionViewModelDelegateAdapter::CompareItem(_In_ IInspectable * item1,_In_ IInspectable * item2,_Outptr_ int *pResult)
{
    HRESULT hr = S_FALSE;
    if(IsOverrideComposed())
    {
        ICollectionViewModelBaseOverrides *pOverrides;
        IFHR(QueryOverrides(pOverrides));

        IFHR(pOverrides->CompareItem(item1,item2,pResult));
    }
    return hr;
}

STDMETHODIMP CMoCollectionViewModelDelegateAdapter::OnItemSourceAttached(_In_ pmod::IObservableObject *item)
{
    if(IsOverrideComposed())
    {
        ICollectionViewModelBaseOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));

        foundation::ComPtr<IObservableObject> spItem;
        IFR(foundation::QueryInterface(item,spItem.GetAddressOf()));

        IFR(pOverrides->OnItemSourceAttached(spItem));
    }
    return S_OK;
}

STDMETHODIMP CMoCollectionViewModelDelegateAdapter::OnItemSourceDetached(_In_ pmod::IObservableObject *item)
{
    if(IsOverrideComposed())
    {
        ICollectionViewModelBaseOverrides *pOverrides;
        IFR(QueryOverrides(pOverrides));

        foundation::ComPtr<IObservableObject> spItem;
        IFR(foundation::QueryInterface(item,spItem.GetAddressOf()));

        IFR(pOverrides->OnItemSourceDetached(spItem));
    }
    return S_OK;
}


STDMETHODIMP CMoCollectionViewModelDelegateAdapter::CreateModelAdapter(_In_ IInspectable *item,_Outptr_ pmod::ISourceModel **ppSourceModel)
{
    HRESULT hr = S_FALSE;
    if(IsOverrideComposed())
    {
        ICollectionViewModelBaseOverrides *pOverrides;
        IFHR(QueryOverrides(pOverrides));

        foundation::ComPtr<ISourceModel> spMoSourceModel;
        IFHR(pOverrides->CreateModelAdapter(item,spMoSourceModel.GetAddressOf()));
        if(spMoSourceModel != nullptr)
        {
            IFHR(foundation::QueryInterface(spMoSourceModel,ppSourceModel));
        }
    }
    return hr;
}



STDMETHODIMP MoCollectionViewModelFactory::CreateInstanceWithSource( 
        IObservableCollection *pSource,
        ObservableCollectionOptions modelOptions,
        CollectionViewModelOptions viewModelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        ICollectionViewModel **ppInstance)

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

STDMETHODIMP MoCollectionViewModelFactory::CreateInstanceWithSourceAndType( 
        GUID iidType,
        IObservableCollection *pSource,
        ObservableCollectionOptions modelOptions,
        CollectionViewModelOptions viewModelOptions,
        IInspectable *pOuter,
        IInspectable **ppInner,
        ICollectionViewModel **ppInstance)
{
    // Find the Type...
    if (!::IsEqualIID(iidType, GUID_NULL) && !foundation::IsTypeInfoRegistered(iidType))
    {
        return foundation::Errors::E_TYPE_INFO_NOT_REGISTERED;
    }
        HRESULT hr;

    foundation::ComPtr<CMoObservableCollectionDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spDelegateAdapter;
    if (pOuter != nullptr)
    {
        IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spDelegateAdapter.GetAddressOf()));
    }

    foundation::ComPtr<CMoCollectionViewModelDelegateAdapter, foundation::ctl::_ComBasePtrImpl> spViewModelDelegate;
    if (pOuter != nullptr)
    {
        IFHR_ASSERT(::CreateMoDelegateAdapter(pOuter, spViewModelDelegate.GetAddressOf()));
    }

    foundation::ComPtr<pmod::IObservableCollection> spSource;
    if(pSource != nullptr)
    {
         IFHR(foundation::QueryInterface(pSource,spSource.GetAddressOf()));
    }
    pmod::library::collection_viewmodel_create_parameters param;
    param.m_options = (pmod::library::ObservableCollectionOptions)modelOptions;
    param.m_view_model_options = (pmod::library::CollectionViewModelOptions)viewModelOptions;
    param.m_iid_type = iidType;
    param.m_p_delegate = spDelegateAdapter;
    param.m_p_viewmodel_delegate = spViewModelDelegate;
    param.m_p_source = spSource;
    foundation::InspectablePtr spInner;

    hr = pmod::library::GetCollectionViewModelClassFactory()->
        CreateCollectionViewModelInstance(&param,pOuter,spInner.GetAddressOf());

    IFHR(hr);
    IFHR(foundation::QueryInterface(spInner,ppInstance));
    // Return ppInner
    if(ppInner)
    {
        *ppInner = spInner.Detach();
    }
    return S_OK;
}

STDMETHODIMP MoCollectionViewModelFactory::CreateCollectionViewModel(
    GUID iidType,
    IObservableCollection *pSource,
    UINT32 length_sortInfoExpressions,
    HSTRING *sortInfoExpressions,
    UINT32 length_sortInfoAscending,
    boolean *sortInfoAscending,
    HSTRING filter_expression,
    IObservableCollection **instance)
{
    foundation::ComPtr<ICollectionViewModel> spCollectionViewModel;
    foundation::InspectablePtr spInner;

    IFR(CreateInstanceWithSourceAndType(
        iidType,
        pSource,
        (ObservableCollectionOptions)((UINT32)ObservableCollectionOptions_IsReadOnly | foundation::library::FoundationClassOptions_IsFreeThreadApartment),
        CollectionViewModelOptions_None,
        nullptr,
        spInner.GetAddressOf(),
        spCollectionViewModel.GetAddressOf()));

    foundation::ComPtr<ICollectionViewModelClass> spCollectionViewModelClass;
    IFR_ASSERT(foundation::QueryInterface(spCollectionViewModel,
        __uuidof(ICollectionViewModelClass),
        spCollectionViewModelClass.GetAddressOf()));

    for (UINT32 index = 0; index < length_sortInfoExpressions; ++index)
    {
        IFR_ASSERT(spCollectionViewModelClass->AddSortInfo(
            foundation::CreateValue(sortInfoExpressions[index]),
            sortInfoAscending[index]));
    }
    if (filter_expression != nullptr && _pal_GetStringLen(filter_expression) > 0)
    {
        foundation::ComPtr<foundation::IExpression> spExpression;
        IFR_ASSERT(foundation::library::CreateExpression(
            _pal_GetStringRawBuffer(filter_expression, nullptr),
            spExpression.GetAddressOf()));

        foundation::ComPtr<IExpression> spMoExpression;
        IFR_ASSERT(foundation::QueryInterface(spExpression, spMoExpression.GetAddressOf()));
        IFR_ASSERT(spCollectionViewModelClass->SetFilterInfo(spMoExpression));
    }
    IFR_ASSERT(foundation::QueryInterface(
        spCollectionViewModel,
        __uuidof(IObservableCollection),
        instance));
    return S_OK;
}
