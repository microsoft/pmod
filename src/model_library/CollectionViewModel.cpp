/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CollectionViewModel.cpp
****/

#include "pch.h"

#include "CollectionViewModel.h"
#include "CreateFoundationHelper.h"
#include "ObservableObjectStatics.h"
#include "ExpressionBinding.h"

// Public Headers
#include <pmod/errors.h>
#include <foundation/pv_util.h>
#include <pmod/library/library_util.h>
#include <pmod/library/object_statics_util.h>

// {B5D0DCDA-1A5B-442B-B7F1-D8B2742166E9}
const GUID pmod::library::IID_ICollectionViewModelClassFactory =
{ 0xb5d0dcda, 0x1a5b, 0x442b,{ 0xb7, 0xf1, 0xd8, 0xb2, 0x74, 0x21, 0x66, 0xe9 } };

using namespace pmod;
using namespace foundation::library;
using namespace pmod::library;

// Note:
// The method will take into account if the left item can be a View Model item
// wrapping the 'real' model item
static bool IsViewModelEqual(
    _In_ foundation::IInspectable *modelItem,
    _In_ foundation::IInspectable *viewModelItem
    )
{    
    if (modelItem == nullptr || viewModelItem == nullptr)
    {
        return false;
    }

    // see if we can use a Model comparision technique
    foundation::ComPtr<foundation::IObject> spObject;
    if(SUCCEEDED(foundation::QueryInterface(modelItem, spObject.GetAddressOf())))
    {
        foundation::ComPtr<foundation::IObject> spViewModel;
        if(SUCCEEDED(foundation::QueryInterface(viewModelItem,spViewModel.GetAddressOf())) &&
			is_object_equals(spObject,spViewModel))
        {
            return true;
        }
        // if here let's try to use the Source Model
        foundation::ComPtr<ISourceModel> spSourceModel;
        if(SUCCEEDED(foundation::QueryInterface(viewModelItem,spSourceModel.GetAddressOf())))
        {
            IFRF(spSourceModel->GetSource(spViewModel.ReleaseAndGetAddressOf()));

            return is_object_equals(spObject, spViewModel);
        }
        return false;
    }
    return foundation::pv_util::CompareInspectableValues(viewModelItem,modelItem) == 0;
}

class _CollectionViewModelPredicate : 
    public CInspectablePtrItemsContainerBase::IContextComparer,
    std::binary_function<foundation::InspectablePtr,foundation::InspectablePtr, bool>
{
public:
    _CollectionViewModelPredicate(CCollectionViewModel *pCollectionViewModel):
        m_pCollectionViewModel(pCollectionViewModel)
    {
        foundation_assert(m_pCollectionViewModel);
    }

    int compare(const foundation::InspectablePtr& item1,const foundation::InspectablePtr& item2) const override
    {
        return m_pCollectionViewModel->CompareItemsDelegate(item1,item2);
    }

    static int __cdecl _ItemsComparerAdapter(foundation::IInspectable *elem1, foundation::IInspectable *elem2,void *Context)
    {
        return (reinterpret_cast<_CollectionViewModelPredicate *>(Context))->compare(elem1,elem2);
    }

    bool operator()(const foundation::InspectablePtr& item1, const foundation::InspectablePtr& item2) const
    {
        return compare(item1,item2) < 0;
    }
private:
    CCollectionViewModel *m_pCollectionViewModel;
};

struct _NoSortComparer : public CInspectablePtrItemsContainerBase::IContextComparer
{
public:
    _NoSortComparer(
            const _Inspectable_ptr_vector_type& sourceItems,_In_ foundation::IInspectable *item):
        m_SourceItems(sourceItems)
    {
        SetItem(item);
    }

    _NoSortComparer(const _Inspectable_ptr_vector_type& sourceItems):
        m_SourceItems(sourceItems)
    {
    }

    void SetItemIndex(_In_ UINT32 itemIndex)
    {
        m_ItemIndex = itemIndex;
        foundation_assert(m_ItemIndex != (UINT32)-1);
    }

    void SetItem(_In_ foundation::IInspectable *item)
    {
        m_ItemIndex = foundation::library::IndexOf(
            m_SourceItems,
            item);
        foundation_assert(m_ItemIndex != -1);
    }

    int compare(const foundation::InspectablePtr& item1,const foundation::InspectablePtr& item2) const override
    {
        UNREFERENCED_PARAMETER(item2);
        UINT32 index = foundation::library::IndexOf(m_SourceItems,item1);
        // if not found it has to be the Model Adapter
        if(index == -1)
        {
            foundation::ComPtr<foundation::IObject> spSource;
            CObservableObjectStatics::GetSourceInternal(item1,spSource.GetAddressOf());
            foundation_assert(spSource != nullptr);
            IFCPTR(spSource);

            index = foundation::library::IndexOf(m_SourceItems,spSource);
            foundation_assert(index != (UINT32)-1);
        }
        return index  - m_ItemIndex;
    }
    static int __cdecl _ItemsComparerAdapter(foundation::IInspectable *elem1, foundation::IInspectable *elem2,void *Context)
    {
        return (reinterpret_cast<_NoSortComparer *>(Context))->compare(elem1,elem2);
    }

private:
    const _Inspectable_ptr_vector_type&    m_SourceItems;
    UINT32                          m_ItemIndex;
};

static int __cdecl _ViewModelItemsComparerAdapter(foundation::IInspectable *elem1, foundation::IInspectable *elem2,void *Context)
{
        if(IsViewModelEqual(elem1,elem2))
            return 0;
        return -1;
}

HRESULT  CCollectionViewModel::CreateInstance(
    _In_ const collection_viewmodel_create_parameters *pModelImpl,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CCollectionViewModel>(pModelImpl,pOuter,ppNewInstance);
}

void CCollectionViewModel::OnFinalRelease()
{
    foundation::ctl::ReleaseInterface(m_BindingValueChangedDelegate);
    _CCollectionViewModelBaseType::OnFinalRelease();
}

HRESULT CCollectionViewModel::_Initialize(const collection_viewmodel_create_parameters *pCollectionViewModelParam)
{
    bool isNotCollectionSet = !IsOptionEnabled(pCollectionViewModelParam->m_options,ObservableCollectionOptions::IsSet);
    IFCEXPECT_ASSERT(isNotCollectionSet);

    IFR(InitializeModelImpl(pCollectionViewModelParam));
    m_UseResetForBatchNotify = IsOptionEnabled(pCollectionViewModelParam->m_view_model_options,CollectionViewModelOptions::UseResetOnBatchNotify);
    m_NoSort = IsOptionEnabled(pCollectionViewModelParam->m_view_model_options,CollectionViewModelOptions::NoSort);
    m_NoFilter = IsOptionEnabled(pCollectionViewModelParam->m_view_model_options,CollectionViewModelOptions::NoFilter);
    m_NoModelAdapter = IsOptionEnabled(pCollectionViewModelParam->m_view_model_options,CollectionViewModelOptions::NoModelAdapter);

    bool forceAttachSourceItems = IsOptionEnabled(pCollectionViewModelParam->m_view_model_options,CollectionViewModelOptions::ForceAttachSourceItems);

    // If attach/detach from Source Items 
    m_AttachToItemsSource = forceAttachSourceItems || !(m_NoSort && m_NoFilter);

    m_FilterItemDelegate = pCollectionViewModelParam->m_p_filter_item_delegate;
    m_CompareItemDelegate = pCollectionViewModelParam->m_p_compare_item_delegate;
    m_CreateItemAdapterDelegate = pCollectionViewModelParam->m_p_create_item_adapter_delegate;

    foundation_assert(IsReadOnly());

    return S_OK;
}

HRESULT CCollectionViewModel::SetSourceInternal(::IObservableCollection *pSource)
{
    // When calling this if we are 'Attached' the 'AttachSource'
    // method will call 'RefreshSourceItems'
    IFR(_CCollectionViewModelBaseType::SetSourceInternal(pSource));

    // next line will use 'QuirksMode' mode behavior from V1 engine
    if(IsUseQuirksMode() && !IsAttached() )
    {
        RefreshSourceItems();
    }

    return S_OK;
}

HRESULT CCollectionViewModel::RefreshSourceItems()
{
    _Inspectable_ptr_vector_type items;
    IFR(GetSourceItems(items));
    return RefreshSourceItems(items);
}

HRESULT CCollectionViewModel::RefreshSourceItemsIf()
{
    if (IsAttached())
    {
        _Inspectable_ptr_vector_type items;
        IFR(GetSourceItems(items));
        IFR(RefreshSourceItems(items, &CCollectionViewModel::AddFilteredSourceItemsInternal));
    }
    return S_OK;
}

HRESULT CCollectionViewModel::RefreshSourceItemsUsingContainer()
{
    IFR_ASSERT(EnsureItemsContainer());

    this->GetItemsContainer()->Clear();
    if (m_spObjectSource != nullptr)
    {
        UINT32 count;
        m_spObjectSource->GetCount(&count);
        // TODO: we may need a better API in ItemsContainer to add multiple item slots
        for (UINT32 index = 0; index < count; ++index)
        {
            this->GetItemsContainer()->Append(nullptr);
        }
    }

    return FireResetNotifyCollectionChangedWithCallback();
}

HRESULT CCollectionViewModel::ConvertItemValue(
    _In_ UINT32 nIndex, 
    _In_ foundation::IInspectable *pValue, 
    _In_ bool isConvertBack, 
    _COM_Outptr_ foundation::IInspectable **ppConvertedValue)
{
    foundation::InspectablePtr spItem(pValue);

    // Note: when we enable virtualization of the container when (NoSort | NoFilter)
    // we fill the container with null values and get the model adapter 'on the fly'
    if (!isConvertBack && IsViewModelAdapterOnly())
    {
        if (pValue == nullptr)
        {
            foundation_assert(m_spObjectSource != nullptr);
            foundation::InspectablePtr spSourceItem;
            IFR(m_spObjectSource->GetItem(nIndex, spSourceItem.GetAddressOf()));
            if (spSourceItem != nullptr)
            {
                // create the adapter item 
                IFR(ToAdapterItem(spSourceItem, spItem.GetAddressOf()));
                // store directly into the container
                IFR_ASSERT(this->GetItemsContainer()->Set(nIndex, spItem));
            }
        }
    }
    // invoke base class Item Converter
    return _CCollectionViewModelBaseType::ConvertItemValue(nIndex, spItem, isConvertBack, ppConvertedValue);
}

HRESULT CCollectionViewModel::ConvertItemValues(_Inspectable_ptr_vector_type& items, _In_ bool isConvertBack)
{
    // Note: we want to enforce calling 'ConvertItemValue' even if we don't have an Item Converter
    return ConvertItemValuesInternal(items, isConvertBack);
}

HRESULT CCollectionViewModel::GetCurrentItems(
    _In_ const _Inspectable_ptr_vector_type& sourceItems,
    _Out_ _Inspectable_ptr_vector_type& items)
{
    for(_Inspectable_ptr_vector_type::const_iterator iter = sourceItems.begin();
        iter != sourceItems.end();
        ++iter)
    {
        foundation::InspectablePtr item = (*iter);
        if(FilterItem(item))
        {
            struct _ViewModelItemsComparer : public CInspectablePtrItemsContainerBase::IContextComparer
            {
            protected:
                int compare(const foundation::InspectablePtr& item1,const foundation::InspectablePtr& item2) const override
                {
                    if(IsViewModelEqual(item1,item2))
                        return 0;
                    return -1;
                }
            };
            foundation::InspectablePtr itemViewModelValue;
            // we will attempt to reuse the already created 
            // Model Adapters for items that we already have
            _ViewModelItemsComparer comparer;
            UINT32 index = -1;
            GetItemsContainer()->Find(item,_ViewModelItemsComparerAdapter,nullptr,&index);

            if(index == -1)
            {
                // Item is not mapped
                IFR(ToAdapterItem(item,itemViewModelValue.GetAddressOf()));
            }
            else
            {
                // Item was already mapped in our collection items
                GetItemsContainer()->Get(index,itemViewModelValue.ReleaseAndGetAddressOf());
            }

            if(m_NoSort)
            {
                items.push_back(itemViewModelValue);
            }
            else
            {
                _Inspectable_ptr_vector_type::iterator iter = std::lower_bound(
                    items.begin(),
                    items.end(),
                    itemViewModelValue,
                    _CollectionViewModelPredicate(this));

                items.insert(iter,itemViewModelValue);
            }
        }
    }
    return S_OK;
}

HRESULT CCollectionViewModel::RefreshSourceItems(
    _In_ const _Inspectable_ptr_vector_type& newItems,
    _In_ HRESULT (CCollectionViewModel::*pAddItemsMethod)(const _Inspectable_ptr_vector_type& ))
{
    _FoundationBaseLock csl(*this);
    IFR_ASSERT(EnsureItemsContainer());

    // Copy current Items
    _Inspectable_ptr_vector_type oldItems;
    this->CopyContainerItems(oldItems);

    // blank my Array
    GetItemsContainer()->Clear();

    HRESULT hr = (this->*pAddItemsMethod)(newItems);
    IFHR(hr);
    if(!this->IsInModelChanges())
    {
        // Fire my Reset Notification
        _Inspectable_ptr_vector_type newItems;
        this->CopyContainerItems(newItems);
        IFR(FireResetNotifyCollectionChanged(oldItems,newItems));
    }
    return S_OK;
}

HRESULT CCollectionViewModel::AddFilteredSourceItemsInternal(const _Inspectable_ptr_vector_type& newItems)
{
    // Add the Filtered Items
    for (_Inspectable_ptr_vector_type::const_iterator iter = newItems.begin();
        iter != newItems.end();
        ++iter)
    {
        IFR_ASSERT(AddIfFiltered((UINT32)(iter - newItems.begin()), *iter, false));
    }
    return S_OK;
}

HRESULT CCollectionViewModel::AddFilteredSourceItems(const _Inspectable_ptr_vector_type& newItems)
{
    IFR_ASSERT(AddFilteredSourceItemsInternal(newItems));

    // attach to new Items
    if(_BaseModelSource::IsAttached() && m_AttachToItemsSource)
    {
        IFR_ASSERT(AttachToItems(newItems));
    }
    return S_OK;
}

HRESULT CCollectionViewModel::AddSourceItems(const _Inspectable_ptr_vector_type& items)
{
    for(_Inspectable_ptr_vector_type::const_iterator iter = items.begin();
        iter != items.end();
        ++iter)
    {
        GetItemsContainer()->Append(*iter);
    }
    return S_OK;
}

HRESULT CCollectionViewModel::RefreshSourceItems(const _Inspectable_ptr_vector_type& newItems)
{
    return RefreshSourceItems(newItems,&CCollectionViewModel::AddFilteredSourceItems);
}

HRESULT CCollectionViewModel::OnItemAdded(UINT32 indexSource,foundation::IInspectable* item)
{
    // Note:
    // On a multi thread scenario items could have been already populated in the AttachToSource
    // method and then some ItemAdded events being posted in the Queue
    // So we will first verify that the item does not exist before

    if (this->FindItemInternal(item) == (UINT32)-1)
    {
        IFR(AddIfFiltered(indexSource,item,true));
        if(_BaseModelSource::IsAttached() && m_AttachToItemsSource)
        {
            IFR(AttachIfPropertyModel(item));
        }
    }
    return S_OK;
}

HRESULT CCollectionViewModel::OnItemRemoved(
    UINT32 indexSource,
    foundation::IInspectable* item)
{
    UNREFERENCED_PARAMETER(indexSource);
    HRESULT hr;

    if (IsViewModelAdapterOnly())
    {
        // get existing Item in our container
        foundation::InspectablePtr view_model_item_ptr;
        this->GetItemsContainer()->Get(indexSource, view_model_item_ptr.GetAddressOf());
        // remove it
        this->GetItemsContainer()->RemoveAt(indexSource);

        if (view_model_item_ptr == nullptr)
        {
            IFHR(ToAdapterItem(item, view_model_item_ptr.GetAddressOf()));
        }
        _Inspectable_ptr_vector_type oldItems;
        oldItems.push_back(view_model_item_ptr);
        _FoundationBaseLock csl(*this);
        return FireNotifyCollectionChanged(
            csl,
            foundation::NotifyCollectionModelChangedAction::ItemRemoved,
            oldItems,
            indexSource);
    }

    hr = RemoveItemInternal(item,true);
    // don't fail if the Item was not found
    if(hr != (HRESULT)foundation::Errors::E_PROPERTY_NOT_FOUND && FAILED(hr))
        return hr;
    if(_BaseModelSource::IsAttached() && m_AttachToItemsSource)
    {
        IFHR(DetachIfPropertyModel(item));
    }
    return S_OK;
}

HRESULT CCollectionViewModel::UpdateCurrentItems(const _Inspectable_ptr_vector_type& sourceItems)
{
    IFR_ASSERT(EnsureItemsContainer());

    // this will optimize to detect if changes on the items were made compared with
    // the existing items we preserve
    _Inspectable_ptr_vector_type newItems;
    IFR(GetCurrentItems(sourceItems,newItems));

    _Inspectable_ptr_vector_type currentItems;
    this->CopyContainerItems(currentItems);

    // Only if array is changed we will continue
    // Note the use of the Standard 'IsArrayEqual' which will use a regular comparission function
    if(!foundation::library::IsEqual(currentItems,newItems))
    {
        IFR(RefreshSourceItems(newItems,&CCollectionViewModel::AddSourceItems));
    }
    if(m_AttachToItemsSource)
    {
        // always attach to every item
        IFR(AttachToItems(sourceItems));
    }
    return S_OK;
}

HRESULT CCollectionViewModel::OnResetAction(INotifyCollectionChangedEventArgs* pArgs)
{
    HRESULT hr;

    // Make Sure we detach to every item in the Collection Source
    RemoveAllSources(_BaseModelSource::IsAttached());

    if (IsViewModelAdapterOnly())
    {
        IFR(RefreshSourceItemsUsingContainer());
    }
    else
    {
        _Inspectable_ptr_vector_type newItems;
        IFHR(GetItems(pArgs, false, newItems));
        // Update existing items
        IFHR(UpdateCurrentItems(newItems));
    }

    return S_OK;
}

HRESULT CCollectionViewModel::OnItemReplacedAction(
    _In_ UINT32 indexSource,
    _In_ const _Inspectable_ptr_vector_type& oldItems,
    _In_ const _Inspectable_ptr_vector_type& newItems)
{
    HRESULT hr;

    if (IsViewModelAdapterOnly())
    {
        for (_Inspectable_ptr_vector_type::const_iterator iter = newItems.begin();
            iter != newItems.end();
            ++iter)
        {
            // null our internal container
            this->GetItemsContainer()->Set(indexSource + (UINT32)(iter - newItems.begin()), nullptr);
        }
        return FireItemNotifyCollectionChangedWithCallback(
            indexSource,
            foundation::NotifyCollectionModelChangedAction::ItemReplaced);
    }

    
    for(_Inspectable_ptr_vector_type::const_iterator iter = oldItems.begin();
        iter != oldItems.end();
        ++iter)
    {
        IFHR(OnItemRemoved(indexSource + (UINT32)(iter - oldItems.begin()),*iter));
    }

    for(_Inspectable_ptr_vector_type::const_iterator iter = newItems.begin();
        iter != newItems.end();
        ++iter)
    {
        IFHR(RefreshSourceItem(*iter));
    }

    return S_OK;
}


HRESULT CCollectionViewModel::InvokeInternal(foundation::IUnknown* pSender,INotifyCollectionChangedEventArgs* pArgs)
{
    HRESULT hr; 

    UNREFERENCED_PARAMETER(pSender);
    foundation::NotifyCollectionModelChangedAction action;
    IFHR_ASSERT(pArgs->GetAction(&action));

    // Notify Delegate
    if(m_pViewModelDelegate)
    {
        hr = m_pViewModelDelegate->OnSourceCollectionChanged(pArgs);
        IFHR(hr);
    }

    UINT32 oldIndex;
    IFHR_ASSERT(pArgs->GetOldStartingIndex(&oldIndex));
    UINT32 newIndex;
    IFHR_ASSERT(pArgs->GetNewStartingIndex(&newIndex));

    _Inspectable_ptr_vector_type items;
    if (action == foundation::NotifyCollectionModelChangedAction::ItemAdded ||
        action == foundation::NotifyCollectionModelChangedAction::ItemRemoved ||
        action == foundation::NotifyCollectionModelChangedAction::ItemReplaced)
    {
        IFHR_ASSERT(GetItemsChanged(pArgs, items));
    }

    _Inspectable_ptr_vector_type oldItems;
    if (action == foundation::NotifyCollectionModelChangedAction::ItemReplaced)
    {
        //This is needed to GetOld items for the replace scenario.
        IFHR_ASSERT(GetItems(pArgs, true, oldItems));
    }

    switch (action)
    {
    case foundation::NotifyCollectionModelChangedAction::ItemAdded:
        for (_Inspectable_ptr_vector_type::const_iterator iter = items.begin();
            iter != items.end();
            ++iter)
        {
            UINT32 nIndex = newIndex == foundation::NINDEX ? foundation::NINDEX : newIndex + (UINT32)(iter - items.begin());
            IFHR(OnItemAdded(nIndex, *iter));
        }
        break;
    case foundation::NotifyCollectionModelChangedAction::ItemRemoved:
        for (_Inspectable_ptr_vector_type::const_iterator iter = items.begin();
            iter != items.end();
            ++iter)
        {
            UINT32 nIndex = oldIndex == foundation::NINDEX ? foundation::NINDEX : oldIndex + (UINT32)(iter - items.begin());
            IFHR(OnItemRemoved(nIndex, *iter));
        }
        break;
    case foundation::NotifyCollectionModelChangedAction::ItemReplaced:
        IFHR(OnItemReplacedAction(newIndex, oldItems, items));
        break;
    case foundation::NotifyCollectionModelChangedAction::Reset:
        IFHR(OnResetAction(pArgs));
        break;
    }

    return S_OK;
}

HRESULT CCollectionViewModel::on_source_changed_internal(IObservableObject* pItem,IPropertyChangedEventArgs* pArgs)
{
    UINT32 propertyId;
    IFR_ASSERT(pArgs->GetProperty(&propertyId));

    // By default we don't want to trigger an item refresh if it's the parent property that is changing.
    // Since every item's parent property is changed (to nullptr) when a collection is cleared, this would
    // generate a significant amount of churn within the collection as each item is refreshed, when the
    // intent is to simply remove all elements.
    if(propertyId == Property_Parent || propertyId == Property_Root)
    {
        return S_FALSE;
    }

    foundation::InspectablePtr oldValue,newValue;
    IFR_ASSERT(pArgs->GetOldValue(oldValue.GetAddressOf()));
    IFR_ASSERT(pArgs->GetNewValue(newValue.GetAddressOf()));
    bool needRefresh = true;
    if(m_pViewModelDelegate)
    {
        IFR(m_pViewModelDelegate->OnSourceItemPropertyChanged(
            pItem,
            propertyId,
            oldValue,
            newValue,
            &needRefresh));
    }
    
    if(needRefresh)
    {
        return RefreshSourceItem(pItem);
    }
    return S_OK;
}

STDMETHODIMP CCollectionViewModel::RefreshSourceItem(foundation::IInspectable *pItem)
{
    return RefreshSourceItemInternal(pItem,true);
}

// Refresh a Source Item
// Note: Item will be always coming from the Source Type without any Model Adapter applied
HRESULT CCollectionViewModel::RefreshSourceItemInternal(foundation::IInspectable *pItem,bool fireNotify)
{
    UINT32 currentIndex = FindItemInternal(pItem);
    bool isFiltered = FilterItem(pItem);

    // Current Items was not contained and now it is being added
    if(currentIndex == -1 && isFiltered)
    {
        return AddSortedItem((UINT32)-1,pItem,fireNotify);
    }
    // Current item was contained but is being filtered out
    else if(currentIndex != -1  && !isFiltered)
    {
        return RemoveAtInternal(currentIndex,fireNotify,true);
    }
    // Was contained and is Filtered ok
    // Note: the next piece of code is intented to evaluate if the Sort
    // position has changed when the Source Item changed one of its properties
    // if 'NoSort' is enable dthere is not need for this code
    else if(!m_NoSort && currentIndex != -1 && isFiltered)
    {
        // Find the current Item (which can have the Model Adapter wrapped)
        foundation::InspectablePtr item;
        this->GetItemsContainer()->Get(currentIndex,item.GetAddressOf());

        // let's figure out if the position was changed
        // Remove temporaly
        this->GetItemsContainer()->RemoveAt(currentIndex);

        // Calculate add position again
        _CollectionViewModelPredicate comparer(this);
        UINT32 newIndex;
        this->GetItemsContainer()->InsertPositionOf(
                            item,
                            &_CollectionViewModelPredicate::_ItemsComparerAdapter,
                            &comparer,
                            &newIndex);
        // put the item back
        this->GetItemsContainer()->Insert(currentIndex,item);

        // if item change position
        if(newIndex != currentIndex)
        {
            IFR(MoveItemInternal(currentIndex,newIndex,fireNotify,true));
        }
    }
    return S_OK;
}

bool CCollectionViewModel::NeedAttachToEventSource()
{
    return IsReferenceAttached() && CObservableCollection::HasEventHandlers();
}

bool CCollectionViewModel::NeedDetachFromEventSource()
{
    return !CObservableCollection::HasEventHandlers();
}

HRESULT CCollectionViewModel::AttachToSource()
{
    IFR(_CCollectionViewModelBaseType::AttachToSource());

    if (IsViewModelAdapterOnly())
    {
        IFR(RefreshSourceItemsUsingContainer());
    }
    else
    {
        // Collect source items
        _Inspectable_ptr_vector_type sourceItems;
        IFR(GetSourceItems(sourceItems));

        // Since our original attachment things could have changed so
        // we will enforce a refresh of every item
        IFR(RefreshSourceItems(sourceItems));
    }

    return S_OK;
}

HRESULT CCollectionViewModel::DetachFromSource()
{
    IFR(_CCollectionViewModelBaseType::DetachFromSource());

    if (m_AttachToItemsSource && !IsViewModelAdapterOnly())
    {
        // Make Sure we detach to every item in the Collection Source
        RemoveAllSources(true);
    }

    // when View Adapter Only virtualization we dnt' want to retrieve the 
    // the 'Source' items for Fire notification
    if (IsViewModelAdapterOnly())
    {
        this->GetItemsContainer()->Clear();
        return 
            this->IsNotifyCollectionChanged() ? 
            FireResetNotifyCollectionChangedWithCallback():
            S_OK;
    }
    // Note: since we are disconnected from the Original Source
    // it will be better to Remove all the items from the ViewModel
    // This scenario helps the moment we enforce the use of ReadOnly
    // collections and developers want o RemoveAll the items as part of the final cleanup
    IFR(RemoveAllInternal());

    return S_OK;
}

HRESULT CCollectionViewModel::AddIfFiltered(UINT32 indexSource,foundation::IInspectable *pItem,bool fireNotify)
{
    if(FilterItem(pItem))
    {
        return AddSortedItem(indexSource,pItem,fireNotify);
    }
    return S_FALSE;
}

HRESULT CCollectionViewModel::FilterUsingFilterInfo(foundation::IInspectable* item,bool& result)
{
    if(m_spFilterInfoExpression != nullptr)
    {
        foundation::ComPtr<IObservableObject> spItem;
        IFR(foundation::QueryInterface(item,spItem.GetAddressOf()));

        foundation::ComPtr<IBindingBase> spBinding;
        IFR(CreateExpressionBinding(
            spItem,
            m_spFilterInfoExpression,
            PropertyBindingOptions::None,
            spBinding.GetAddressOf()));
        
        foundation::InspectablePtr value;
        IFR(spBinding->GetValue(value.GetAddressOf()));
        boolean _result;
        IFR(foundation::pv_util::GetBooleanValue(value, &_result));
        result = _result ? true:false;
    }
    return S_OK;
}

bool CCollectionViewModel::FilterItem(foundation::IInspectable *pItem)
{
    if(m_NoFilter)
    {
        // No Filter is desired
        return true;
    }

    bool retVal = true;
    HRESULT hr = S_FALSE;
    if(m_pViewModelDelegate)
    {
        hr = m_pViewModelDelegate->FilterItem(pItem,&retVal);
        if(hr == S_OK)
        {
            return retVal;
        }
    }
    if(m_FilterItemDelegate)
    {
        hr = m_FilterItemDelegate->Invoke(pItem,&retVal);
    }
    if(hr == S_FALSE)
    {
        FilterUsingFilterInfo(pItem,retVal);
    }
    return retVal;
}

HRESULT CCollectionViewModel::CompareUsingSortInfo(
    IObservableObject* item1,
    IObservableObject* item2,
    int& result)
{
    foundation_assert(item1);
    foundation_assert(item2);

    for (_SortInfoVectorType::const_iterator iter = m_SortInfo.begin();
        iter != m_SortInfo.end();
        ++iter)
    {
        foundation::ComPtr<ISourceModel> spSourceModel;
        IFR_ASSERT(foundation::QueryInterface((*iter).first,spSourceModel.GetAddressOf()));

        foundation::InspectablePtr value1;
        foundation::InspectablePtr value2;

        // resolve value for Item1
        IFR_ASSERT(spSourceModel->SetSource(item1));
        IFR_ASSERT((*iter).first->GetValue(value1.GetAddressOf()));
        // resolve value for Item2
        IFR_ASSERT(spSourceModel->SetSource(item2));
        IFR_ASSERT((*iter).first->GetValue(value2.GetAddressOf()));

        result = foundation::pv_util::CompareInspectableValues(value1, value2);
        if(result != 0)
        {
            if(!(*iter).second)
            {
                result = -result;
            }
            break;
        }
    }
    return S_OK;
}

int  CCollectionViewModel::CompareItemsDelegate(_In_ foundation::IInspectable *item1,_In_ foundation::IInspectable *item2)
{
    int result = 0;
    HRESULT hr = S_FALSE;
    if(m_pViewModelDelegate)
    {
        hr = m_pViewModelDelegate->CompareItem(item1,item2,&result);
        if(hr == S_OK)
        {
            return result;
        }
    }
    if(m_CompareItemDelegate)
    {
        hr = m_CompareItemDelegate->Invoke(item1,item2,&result);
    }
    if(hr == S_FALSE)
    {
        foundation::ComPtr<IObservableObject> spItem1;
        foundation::QueryInterface(item1, spItem1.GetAddressOf());
        foundation::ComPtr<IObservableObject> spItem2;
        foundation::QueryInterface(item2, spItem2.GetAddressOf());

        if (spItem1 != nullptr && spItem2 != nullptr)
        {
            CompareUsingSortInfo(spItem1, spItem2, result);
        }
        else
        {
            // fallback will be to use default comparission
            result = foundation::pv_util::CompareInspectableValues(item1, item2);
        }
    }
    return result;
}

HRESULT CCollectionViewModel::ToAdapterItem(foundation::IInspectable *item,foundation::IInspectable **itemAdapter)
{
    foundation_assert(item);
    foundation_assert(itemAdapter);

    foundation::ComPtr<ISourceModel> spSourceModel;
    // If Model Adapter is desired
    if(!m_NoModelAdapter)
    {
        HRESULT hr = S_FALSE;
        if(m_pViewModelDelegate)
        {
            hr = m_pViewModelDelegate->CreateModelAdapter(item,spSourceModel.GetAddressOf());
            IFHR(hr);
        }
        if(hr == S_FALSE && m_CreateItemAdapterDelegate)
        {
            hr = m_CreateItemAdapterDelegate->Invoke(item,spSourceModel.ReleaseAndGetAddressOf());
            IFHR(hr);
        }
    }

    if(spSourceModel != nullptr)
    {
        // Use the Adapter
        *itemAdapter = spSourceModel.Detach();
    }
    else
    {
        // no model adapter was defined for this item
        *itemAdapter = item;
        (*itemAdapter)->AddRef();
    }
    return S_OK;
}

HRESULT CCollectionViewModel::ToAdapterItems(_Inspectable_ptr_vector_type& items)
{
    for (_Inspectable_ptr_vector_type::iterator iter = items.begin();
        iter != items.end();
        ++iter)
    {
        if (*iter != nullptr)
        {
            foundation::InspectablePtr spAdapterItem;
            IFR(ToAdapterItem(*iter, spAdapterItem.GetAddressOf()));
            // Put back into the array
            (*iter) = spAdapterItem;
        }
    }
    return S_OK;
}

HRESULT CCollectionViewModel::AddSortedItem(UINT32 indexSource,foundation::IInspectable *item,bool fireNotify)
{
    if (IsViewModelAdapterOnly() && indexSource != foundation::NINDEX)
    {
        // insert nullptr slot item into our vitualized container
        // also we do not want the standard notification
        IFR_ASSERT(this->InsertItemInternal(indexSource, nullptr, false, true));

        // notify with callback
        return FireItemNotifyCollectionChangedWithCallback(
            indexSource,
            foundation::NotifyCollectionModelChangedAction::ItemAdded);
    }

    foundation::InspectablePtr itemAdapter;
    IFR(ToAdapterItem(item,itemAdapter.GetAddressOf()));

    foundation_assert(itemAdapter != nullptr);

    if(m_NoSort)
    {
        if(m_NoFilter && indexSource != foundation::NINDEX)
        {
            return InsertItemInternal(indexSource,itemAdapter,fireNotify,true);
        }
        else
        {
            _Inspectable_ptr_vector_type items;
            IFR(GetSourceItems(items));

            _NoSortComparer comparer(items);
            if(indexSource != foundation::NINDEX)
            {
                comparer.SetItemIndex(indexSource);
            }
            else
            {
                comparer.SetItem(item);
            }
            UINT32 index;
            return InsertInOrderInternal(
                    itemAdapter,
                    &_NoSortComparer::_ItemsComparerAdapter,
                    &comparer,
                    &index,
                    fireNotify);
        }
    }
    else
    {
        UINT32 index;
        _CollectionViewModelPredicate comparer(this);
        return InsertInOrderInternal(
                itemAdapter,
                &_CollectionViewModelPredicate::_ItemsComparerAdapter,
                &comparer,
                &index,
                fireNotify);
    }
}

HRESULT CCollectionViewModel::AttachIfPropertyModel(foundation::IInspectable* item)
{
    foundation_assert(m_AttachToItemsSource);
    foundation::ComPtr<IObservableObject> spPropertyModel;
    if(SUCCEEDED(foundation::QueryInterface(item,spPropertyModel.GetAddressOf())))
    {
        IFR(AddSourceModel(spPropertyModel,true));
        IFR(AttachSortInfoBindings(spPropertyModel));
        IFR(AttachFilterInfoBindings(spPropertyModel));

        if(m_pViewModelDelegate)
        {
            IFR(m_pViewModelDelegate->OnItemSourceAttached(spPropertyModel));
        }
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CCollectionViewModel::DetachIfPropertyModel(foundation::IInspectable* item)
{
    foundation_assert(m_AttachToItemsSource);
    foundation::ComPtr<IObservableObject> spPropertyModel;
    if(SUCCEEDED(foundation::QueryInterface(item,spPropertyModel.GetAddressOf())))
    {
        IFR(RemoveSourceModel(spPropertyModel,true));
        IFR(DetachBindings(spPropertyModel));

        if(m_pViewModelDelegate)
        {
            IFR(m_pViewModelDelegate->OnItemSourceDetached(spPropertyModel));
        }
        return S_OK;
    }
    return S_FALSE;
}

HRESULT CCollectionViewModel::AttachToItems(const _Inspectable_ptr_vector_type& items)
{
    for(_Inspectable_ptr_vector_type::const_iterator iter = items.begin();
        iter != items.end();
        ++iter)
    {
        IFR(AttachIfPropertyModel(*iter));
    }
    return S_OK;
}

HRESULT CCollectionViewModel::GetItems(
    INotifyCollectionChangedEventArgs* pArgs,
    bool oldItems,
    _Inspectable_ptr_vector_type& items)
{
    foundation::InspectableArrayWrapper itemsWrapper;
    if(oldItems)
    {
        IFR(pArgs->GetOldItems(itemsWrapper.GetSizeAddressOf(),itemsWrapper.GetBufferAddressOf()));
    }
    else
    {
        IFR(pArgs->GetNewItems(itemsWrapper.GetSizeAddressOf(),itemsWrapper.GetBufferAddressOf()));
    }
    CopyArrayToVector(itemsWrapper.GetSize(),itemsWrapper.GetBuffer(),items);
    return S_OK;
}

HRESULT CCollectionViewModel::GetItemsChanged(
        INotifyCollectionChangedEventArgs* pArgs,
        _Inspectable_ptr_vector_type& items)
{
    foundation::NotifyCollectionModelChangedAction action;
    IFR(pArgs->GetAction(&action));

    foundation_assert(action != foundation::NotifyCollectionModelChangedAction::Reset);
    switch(action)
    {
    case foundation::NotifyCollectionModelChangedAction::ItemReplaced:
    case foundation::NotifyCollectionModelChangedAction::ItemAdded:
        IFR(GetItems(pArgs,false,items));
        break;
    case foundation::NotifyCollectionModelChangedAction::ItemRemoved:
        IFR(GetItems(pArgs,true,items));
            break;
    case foundation::NotifyCollectionModelChangedAction::Reset:
            foundation_assert(0);
            break;
    }
    foundation_assert(items.size()>0);
    return S_OK;
}

HRESULT CCollectionViewModel::GetSourceItems(_Inspectable_ptr_vector_type& items)
{
    if(m_spObjectSource)
    {
        foundation::InspectableArrayWrapper itemsWrapper;
        IFR(m_spObjectSource->GetItems(itemsWrapper.GetSizeAddressOf(),itemsWrapper.GetBufferAddressOf()));
        CopyArrayToVector(itemsWrapper.GetSize(),itemsWrapper.GetBuffer(),items);
    }
    else
    {
        items.clear();
    }
    return S_OK;
}

bool CCollectionViewModel::IsItemEqual(_In_ foundation::IInspectable *item1,_In_ foundation::IInspectable *item2)
{
    return IsViewModelEqual(item1,item2);
}

STDMETHODIMP CCollectionViewModel::GetFilterItemDelegate(_COM_Outptr_ IFilterItemDelegate **ppFilterItemDelegate)
{
    return m_FilterItemDelegate.CopyTo(ppFilterItemDelegate);
}

STDMETHODIMP CCollectionViewModel::SetFilterItemDelegate(_In_ IFilterItemDelegate *pFilterItemDelegate)
{
    m_FilterItemDelegate = pFilterItemDelegate;
    // refresh if needed
    IFR(RefreshSourceItemsIf())
    return S_OK;
}

STDMETHODIMP CCollectionViewModel::GetCompareItemDelegate(_COM_Outptr_ ICompareItemDelegate **ppCompareItemDelegate)
{
    return m_CompareItemDelegate.CopyTo(ppCompareItemDelegate);
}

STDMETHODIMP CCollectionViewModel::SetCompareItemDelegate(_In_ ICompareItemDelegate *pCompareItemDelegate)
{
    m_CompareItemDelegate = pCompareItemDelegate;
    // refresh if needed
    IFR(RefreshSourceItemsIf())
    return S_OK;
}
STDMETHODIMP CCollectionViewModel::GetCreateItemAdapterDelegate(_COM_Outptr_ ICreateItemAdapterDelegate **ppCreateItemAdapterDelegate)
{
    return m_CreateItemAdapterDelegate.CopyTo(ppCreateItemAdapterDelegate);
}

STDMETHODIMP CCollectionViewModel::SetCreateItemAdapterDelegate(_In_ ICreateItemAdapterDelegate *pCreateItemAdapterDelegate)
{
    m_CreateItemAdapterDelegate = pCreateItemAdapterDelegate;
    // refresh if needed
    IFR(RefreshSourceItemsIf())
    return S_OK;
}

HRESULT CCollectionViewModel::AttachBinding(pmod::IObservableObject* item,pmod::IBindingBase* pBindingBase)
{
    foundation_assert(item);
    foundation_assert(pBindingBase);

    if(!m_BindingValueChangedDelegate)
    {
        IFR_ASSERT(_CollectionViewModelBindingValueChangedDelegateFactory::CreateInstance(
            this,
            &CCollectionViewModel::OnBindingValueChanged,
            GetControllerDelegate(),
            &m_BindingValueChangedDelegate));
    }

    IFR_ASSERT(PropertyBindingListener::AttachEventHandler(pBindingBase, m_BindingValueChangedDelegate));

    _ItemBindingsMapType::iterator iter = m_ItemsBindingsMap.find(item);
    if(iter == m_ItemsBindingsMap.end())
    {
        _BindingVectorType bindings;
        bindings.push_back(pBindingBase);
        m_ItemsBindingsMap.insert(std::pair<IObservableObject *,_BindingVectorType>(item,bindings));
    }
    else
    {
        (*iter).second.push_back(pBindingBase);
    }
    return S_OK;
}

HRESULT CCollectionViewModel::DetachBindings(pmod::IObservableObject* item)
{
    _ItemBindingsMapType::iterator iter = m_ItemsBindingsMap.find(item);
    if(iter != m_ItemsBindingsMap.end())
    {
        for (_BindingVectorType::const_iterator iterBinding = (*iter).second.begin();
            iterBinding != (*iter).second.end();
            ++iterBinding)
        {
            IFR(PropertyBindingListener::DetachEventHandler(*iterBinding,m_BindingValueChangedDelegate));
        }
        (*iter).second.clear();
        m_ItemsBindingsMap.erase(iter);
    }
    return S_OK;
}

HRESULT CCollectionViewModel::AttachSortInfoBindings(IObservableObject* item)
{
    foundation_assert(item);

    for (_SortInfoVectorType::const_iterator iter = m_SortInfo.begin();
        iter != m_SortInfo.end();
        ++iter)
    {
        // the only possible static cast is to QI for ISourceModel and convert back
        // to a CExpressionBinding class
        foundation::ComPtr<ISourceModel> spAsSourceModel;
        IFR_ASSERT(foundation::QueryInterface((*iter).first, spAsSourceModel.GetAddressOf()));

        CExpressionBinding *pExpressionBinding = static_cast<CExpressionBinding *>(spAsSourceModel.Get());
        foundation::ComPtr<IBindingBase> spBinding;
        IFR_ASSERT(CExpressionBinding::CreateInstance(
            item,
            pExpressionBinding->GetExpressionBindingInfo(),
            PropertyBindingOptions::None,
            spBinding.GetAddressOf()
            ));

        IFR(AttachBinding(item,spBinding))
    }
    return S_OK;
}

HRESULT CCollectionViewModel::AttachFilterInfoBindings(pmod::IObservableObject* item)
{
    if(m_spFilterInfoExpression != nullptr)
    {
        foundation::ComPtr<IBindingBase> spBinding;
        IFR_ASSERT(CreateExpressionBinding(
           item,
           m_spFilterInfoExpression,
           PropertyBindingOptions::None,
           spBinding.GetAddressOf()));
        IFR(AttachBinding(item,spBinding));
    }
    return S_OK;
}

STDMETHODIMP CCollectionViewModel::AddSortInfo(_In_ foundation::IInspectable *pSortInfoPath,bool isAscending)
{
    IFCPTR(pSortInfoPath);
    if (m_NoSort)
    {
        // collection is marked as No Sort
        IFR(E_UNEXPECTED);
    }

    foundation::ComPtr<IBindingBase> spBinding;

    foundation::ComPtr<foundation::IExpression> spExpression;
    if (SUCCEEDED(foundation::QueryInterface(pSortInfoPath, spExpression.GetAddressOf())))
    {
        IFR_ASSERT(CreateExpressionBinding(
            nullptr,
            spExpression,
            PropertyBindingOptions::None,
            spBinding.GetAddressOf()
            ));
    }
    else
    {
        foundation::HStringPtr hSortInfoPtr;
        IFR(foundation::pv_util::GetStringValue(pSortInfoPath, hSortInfoPtr.GetAddressOf()));
        IFR_ASSERT(CreateExpressionBinding(
            nullptr,
            hSortInfoPtr.GetRawBuffer(),
            PropertyBindingOptions::None,
            spBinding.GetAddressOf()
            ));
    }
    m_SortInfo.push_back(std::pair<foundation::ComPtr<IBindingBase>, bool>(spBinding, isAscending));

    // adding sort info will cause the list to be refreshed
    IFR(RefreshSourceItemsIf())
    return S_OK;
}

STDMETHODIMP CCollectionViewModel::SetFilterInfo(_In_ foundation::IExpression *pFilterInfoExpression)
{
    m_spFilterInfoExpression = pFilterInfoExpression;
    // refresh if needed
    IFR(RefreshSourceItemsIf())
    return S_OK;
}

HRESULT CCollectionViewModel::OnBindingValueChanged(foundation::IUnknown *pSender, pmod::IBindingValueChangedEventArgs *pEventArgs)
{
    foundation::ComPtr<ISourceModel> spSourceModel;
    IFR(foundation::QueryInterface(pSender,spSourceModel.GetAddressOf()));

    foundation::ComPtr<foundation::IObject> spObject;
    IFR(spSourceModel->GetSource(spObject.GetAddressOf()));

    IFCPTR(spObject);

    foundation::ComPtr<IObservableObject> spPropertyModel;
    IFR(foundation::QueryInterface(spObject, spPropertyModel.GetAddressOf()));

    IFR(RefreshSourceItem(spPropertyModel));

    return S_OK;
}

class CCollectionViewModelClassFactory :
    public foundation::ctl::Implements<ICollectionViewModelClassFactory, &IID_ICollectionViewModelClassFactory>
{
public:
    static ICollectionViewModelClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CCollectionViewModelClassFactory, ICollectionViewModelClassFactory>();
        }
    }

    STDMETHOD(CreateCollectionViewModelInstance)(
        _In_ const collection_viewmodel_create_parameters *pViewModelFactoryParam,
        _In_opt_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CCollectionViewModel::CreateInstance(pViewModelFactoryParam, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.CollectionViewModelClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CCollectionViewModelClassFactory::GetInstance)
);
