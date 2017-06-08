/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCollectionViewModelFactory.h
****/

#pragma once

#include <pmod/library/model_library.h>

#include "MoModelFactory.h"
#include "MoSourceModelDelegateAdapter.h"

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoCollectionViewModelDelegateAdapter
//
// Purpose: Implementation of ICollectionViewModelDelegate Interface Adapter
//
//------------------------------------------------------------------------------
class CMoCollectionViewModelDelegateAdapter :
    public CMoSourceModelDelegateAdapter
    <
    pmod::library::ICollectionViewModelDelegate,
    pmod::INotifyCollectionChangedEventHandler,
    pmod::IObservableCollection
    >
{
protected:
     // ICollectionViewModelDelegate Implementation
    STDMETHOD(OnSourceCollectionChanged)(_In_ pmod::INotifyCollectionChangedEventArgs* pArgs);

    // When a Source Item Property has changed
    STDMETHOD( OnSourceItemPropertyChanged )(
            _In_ pmod::IObservableObject *item,
            _In_ UINT32 propertyId,
            _In_ IInspectable *oldValue,
            _In_ IInspectable *newValue,
            _Outptr_ bool *pNeedRefresh);
    STDMETHOD( FilterItem )(_In_ IInspectable * item,_Outptr_ bool *pValue);
    STDMETHOD( CompareItem )(_In_ IInspectable * item1,_In_ IInspectable * item2,_Outptr_ int *pResult);
    STDMETHOD( OnItemSourceAttached )(_In_ pmod::IObservableObject *item);

    STDMETHOD( OnItemSourceDetached )(_In_ pmod::IObservableObject *item);
    STDMETHOD( CreateModelAdapter )(_In_ IInspectable *item,_Outptr_ pmod::ISourceModel **ppSourceModel);
};
//------------------------------------------------------------------------------
// Class:   MoCollectionViewModelFactory
//
// Purpose: Factory class for a CollectionViewModel
//
//------------------------------------------------------------------------------
class MoCollectionViewModelFactory :
        public MoModelFactoryBase
		<
        ICollectionViewModelFactory
		>,
        public ICollectionViewModelStatics
{
public:
	// ICollectionViewModelFactory
    HRESULT STDMETHODCALLTYPE CreateInstanceWithSource( 
        IObservableCollection *pSource,
        ObservableCollectionOptions modelOptions,
        CollectionViewModelOptions viewModelOptions,
        IInspectable *outer,
        IInspectable **inner,
        ICollectionViewModel **instance) override;
                        
    HRESULT STDMETHODCALLTYPE CreateInstanceWithSourceAndType( 
        GUID iidType,
        IObservableCollection *pSource,
        ObservableCollectionOptions modelOptions,
        CollectionViewModelOptions viewModelOptions,
        IInspectable *outer,
        IInspectable **inner,
        ICollectionViewModel **instance)  override;
    // ICollectionViewModelStatics
    HRESULT STDMETHODCALLTYPE CreateCollectionViewModel(
        GUID iidType,
        IObservableCollection *pSource,
        UINT32 length_sortInfoExpressions,
        HSTRING *sortInfoExpressions,
        UINT32 length_sortInfoAscending,
        boolean *sortInfoAscending,
        HSTRING filter_expression,
        IObservableCollection **instance) override;

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if (iid == __uuidof(ICollectionViewModelStatics))
        {
            *ppInterface = static_cast<ICollectionViewModelStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};
