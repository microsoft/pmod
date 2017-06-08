/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CollectionViewModel.h
****/

#pragma once

#include "ObservableCollection.h"
#include "BaseModelSource.h"
#include "MultiSourceModelBase.h"


#include <pmod/event_connection.h>

typedef SingleSourceViewModelBase
    <
        CObservableCollection,
        pmod::library::ICollectionViewModelDelegate,
        pmod::IObservableCollection,
        pmod::INotifyCollectionChangedEventSource,
        pmod::INotifyCollectionChangedEventHandler,
        pmod::INotifyCollectionChangedEventArgs
    >
    _CCollectionViewModelBaseType;

class _CollectionViewModelPredicate;

class CCollectionViewModel :
    public _CCollectionViewModelBaseType,
    public pmod::library::ICollectionViewModelClass,
    protected CMultiSourcePropertyModelContainer
{
 public:
     HRESULT _Initialize(const pmod::library::collection_viewmodel_create_parameters *modelImpl);

    static HRESULT CreateInstance(
        _In_ const pmod::library::collection_viewmodel_create_parameters *pModelImpl,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance);

protected:

    CCollectionViewModel():
        m_NoSort(false),
        m_NoFilter(false),
        m_NoModelAdapter(false),
        m_AttachToItemsSource(true),
        m_BindingValueChangedDelegate(nullptr)
    {
    }
    virtual ~CCollectionViewModel()
    {
    }
    // Override from ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::library::ICollectionViewModelClass::GetIID())
        {
            *ppInterface = static_cast<pmod::library::ICollectionViewModelClass *>(this);
        }
        else
        {
            return _CCollectionViewModelBaseType::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
    // ComBase Override
    void OnFinalRelease() override;

    // Overrides from BaseModelSource
    HRESULT SetSourceInternal(pmod::IObservableCollection *pSource) override;
    bool NeedAttachToEventSource() override;
    bool NeedDetachFromEventSource() override;
    HRESULT AttachToSource() override;
    HRESULT DetachFromSource() override;
    HRESULT InvokeInternal(foundation::IUnknown* pSender,pmod::INotifyCollectionChangedEventArgs* pArgs) override;
    HRESULT GetSourceEventSource(pmod::INotifyCollectionChangedEventSource **pEventSource) override
    {
        foundation_assert(m_spObjectSource);
        return m_spObjectSource->GetNotifyCollectionChangedEventSource(pEventSource);
    }
    // Overrides from EventSourceBase
    foundation::IUnknown *GetControllerDelegate() override
    {
        return this->AsInspectable();
    }
    // Override from CollectionModel
    bool IsItemEqual(_In_ foundation::IInspectable *item1,_In_ foundation::IInspectable *item2) override;
    HRESULT ConvertItemValue(_In_ UINT32 nIndex, _In_ foundation::IInspectable *pValue, _In_ bool isConvertBack, _COM_Outptr_ foundation::IInspectable **ppConvertedValue) override;
    HRESULT ConvertItemValues(_Inspectable_ptr_vector_type& items, _In_ bool isConvertBack) override;

    // TMultiSourceModelContainer override
    HRESULT on_source_changed_internal(pmod::IObservableObject* pSender,pmod::IPropertyChangedEventArgs* pArgs) override;
    foundation::IUnknown *GetMultiSourceDelegateController()
    {
        return this->AsInspectable();
    }

    // ICollectionViewModelClass Interface
    STDMETHOD(AddSortInfo)(_In_ foundation::IInspectable *pSortInfoPath,bool isAscending);
    STDMETHOD(SetFilterInfo)(_In_ foundation::IExpression *pFilterInfoExpression);

    STDMETHOD(RefreshSourceItem)(foundation::IInspectable* item);

    STDMETHOD(GetFilterItemDelegate)(_COM_Outptr_ pmod::library::IFilterItemDelegate **ppFilterItemDelegate);
    STDMETHOD(SetFilterItemDelegate)(_In_ pmod::library::IFilterItemDelegate *pFilterItemDelegate);

    // Property 'CompareItemDelegate'
    STDMETHOD(GetCompareItemDelegate)(_COM_Outptr_ pmod::library::ICompareItemDelegate **ppCompareItemDelegate);
    STDMETHOD(SetCompareItemDelegate)(_In_ pmod::library::ICompareItemDelegate *pCompareItemDelegate);

    // Property 'CreateModelAdapterDelegate'
    STDMETHOD(GetCreateItemAdapterDelegate)(_COM_Outptr_ pmod::library::ICreateItemAdapterDelegate **ppCreateItemAdapterDelegate);
    STDMETHOD(SetCreateItemAdapterDelegate)(_In_ pmod::library::ICreateItemAdapterDelegate *pCreateItemAdapterDelegate);

private:
    friend class _CollectionViewModelPredicate;

    // compare two items using a a Delegate callback
    int  CompareItemsDelegate(_In_ foundation::IInspectable *item1,_In_ foundation::IInspectable *item2);

    // Add the Item if it is being Filtered
    HRESULT AddIfFiltered(UINT32 indexSource,foundation::IInspectable* item,bool fireNotify);

    // Return true/false depending if the items is being filtered
    bool FilterItem(foundation::IInspectable* item);

    // Add an Item to this Collection in a sorted order
    HRESULT AddSortedItem(UINT32 indexSource,foundation::IInspectable* item,bool fireNotify);
	
    // attach to a batch of Property Model Items
    HRESULT AttachToItems(const _Inspectable_ptr_vector_type& items);

    // attach/detach Event from a PropertyModel Instance
    HRESULT AttachIfPropertyModel(foundation::IInspectable* item);
    HRESULT DetachIfPropertyModel(foundation::IInspectable* item);

    // attach Sort Info bindings
    HRESULT AttachSortInfoBindings(pmod::IObservableObject* item);
    // attach Filter Info bindings
    HRESULT AttachFilterInfoBindings(pmod::IObservableObject* item);

    HRESULT AttachBinding(pmod::IObservableObject* item,pmod::IBindingBase* pBindingBase);
    HRESULT DetachBindings(pmod::IObservableObject* item);

    HRESULT ToAdapterItems(_Inspectable_ptr_vector_type& items);

    // Get the current Source Items
    HRESULT GetSourceItems(_Inspectable_ptr_vector_type& items);

    // Refresh the Source Items
    HRESULT RefreshSourceItems();
    HRESULT RefreshSourceItems(const _Inspectable_ptr_vector_type& items);
    HRESULT RefreshSourceItemInternal(foundation::IInspectable* item,bool fireNotify);
    HRESULT RefreshSourceItemsIf();
    HRESULT RefreshSourceItemsUsingContainer();

    // Add only filtered source items
    HRESULT AddFilteredSourceItemsInternal(const _Inspectable_ptr_vector_type& items);
    HRESULT AddFilteredSourceItems(const _Inspectable_ptr_vector_type& items);
    // Add every item from the Safe Array
    HRESULT AddSourceItems(const _Inspectable_ptr_vector_type& items);

    // Perform a 'Reset' action on my collection surrounded by
    // an invocation of pAddItemsMethod
    HRESULT RefreshSourceItems(
    _In_ const _Inspectable_ptr_vector_type& items,
    _In_ HRESULT (CCollectionViewModel::*pAddItemsMethod)(const _Inspectable_ptr_vector_type& ));

    // This method will attempt to Refresh the Source Items and verify if any 
    // change was made in the Source Collection Model
    HRESULT UpdateCurrentItems(const _Inspectable_ptr_vector_type& items);

    // get a collection of items with the most updated items coming from the source collection
    HRESULT GetCurrentItems(_In_ const _Inspectable_ptr_vector_type& sourceItems,_Out_ _Inspectable_ptr_vector_type& items);

    // Convert a 'raw' item into an Adapter Model
    HRESULT ToAdapterItem(foundation::IInspectable *item,foundation::IInspectable **itemAdapter);

    // Handlers from Source Events
    HRESULT OnItemAdded(UINT32 indexSource,foundation::IInspectable* item);
    HRESULT OnItemRemoved(UINT32 indexSource,foundation::IInspectable* item);
    HRESULT OnResetAction(pmod::INotifyCollectionChangedEventArgs* pArgs);
	HRESULT OnItemReplacedAction(
        _In_ UINT32 indexSource,
        _In_ const _Inspectable_ptr_vector_type& oldItems, 
        _In_ const _Inspectable_ptr_vector_type& newItems);


    static HRESULT GetItems(pmod::INotifyCollectionChangedEventArgs* pArgs,bool oldItems,_Inspectable_ptr_vector_type& items);
    static HRESULT GetItemsChanged(pmod::INotifyCollectionChangedEventArgs* pArgs,_Inspectable_ptr_vector_type& items);

    HRESULT CompareUsingSortInfo(
        pmod::IObservableObject* item1,
        pmod::IObservableObject* item2,
        int& result);
    HRESULT FilterUsingFilterInfo(foundation::IInspectable* item,bool& result);

    // Return true when No Filter and No Sort is desired
    bool IsNoSortAndFilter()
    {
        return m_NoSort && m_NoFilter;
    }

    // Return true when No Filter and No Sort and it has Adapter
    bool IsViewModelAdapterOnly()
    {
        return IsNoSortAndFilter() && !m_NoModelAdapter;
    }

    // Invoke when any of our SortInfo or FilterInfo bindings is being changed
    HRESULT OnBindingValueChanged(foundation::IUnknown *pSender, pmod::IBindingValueChangedEventArgs *pEventArgs);

private:
    typedef std::vector<foundation::ComPtr<pmod::IBindingBase> >         _BindingVectorType;
    typedef std::map<pmod::IObservableObject *, _BindingVectorType>   _ItemBindingsMapType;
    typedef pmod::BindingValueChangedConnection::_DelegateFactory<CCollectionViewModel> 
        _CollectionViewModelBindingValueChangedDelegateFactory;
    typedef std::vector<std::pair<foundation::ComPtr<pmod::IBindingBase>, bool> >  _SortInfoVectorType;

    bool m_UseResetForBatchNotify:1;
    bool m_NoSort:1;
    bool m_NoFilter:1;
    bool m_NoModelAdapter:1;
    bool m_AttachToItemsSource:1;

    _SortInfoVectorType                                                    m_SortInfo;
    _ItemBindingsMapType                                                   m_ItemsBindingsMap;
    foundation::ComPtr<foundation::IExpression>                               m_spFilterInfoExpression;

    foundation::ComPtr<pmod::library::IFilterItemDelegate>         m_FilterItemDelegate;
    foundation::ComPtr<pmod::library::ICompareItemDelegate>        m_CompareItemDelegate;
    foundation::ComPtr<pmod::library::ICreateItemAdapterDelegate>  m_CreateItemAdapterDelegate;

    _CollectionViewModelBindingValueChangedDelegateFactory::_type_eventhandler_delegate     *m_BindingValueChangedDelegate;
};


