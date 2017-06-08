/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionProxy.cpp
****/
#include "pch.h"

#include "ObservableCollectionProxy.h"
#include "ProxyObjectFactory.h"
#include "ArrayHelper.h"

#include <pmod/library/interfaces/fast_items_container_parameters.h>
#include <pmod/library/notify_collection_changed_event_args.h>

#include <foundation/library/perf_counter_util.h>
#if defined(DEBUG)
#include <pmod/perf_type_info.h>
#endif

using namespace pmod;
using namespace pmod::library;
using namespace pmod::proxy;

// Private import
EXTERN_C PMOD_LIB_API HRESULT  STDAPICALLTYPE CreateFastItemsContainer(
    const pmod::library::FastItemsContainerParameters *pParameters,
    pmod::library::IFastItemsContainer **ppPropertiesContainer);

//------------------------------------------------------------------------------
// Class:   CProxyNotifyCollectionChangedEventArgs
// A specialized INotifyCollectionChangedEventArgs implemenation for a Collection Proxy
//------------------------------------------------------------------------------
class CProxyNotifyCollectionChangedEventArgs :
	public pmod::library::_NotifyCollectionChangedEventArgsBase
{
public:
    HRESULT _Initialize(
        _In_ CObservableCollectionProxy *pThis,
        foundation::NotifyCollectionModelChangedAction action,
        UINT32 oldStartingIndex,
        UINT32 newStartingIndex,
        _In_ INotifyCollectionChangedEventArgs* pSourceEventArgs
        )
    {
		pmod::library::_NotifyCollectionChangedEventArgsBase::_Initialize(action, oldStartingIndex, newStartingIndex);
        _pThis = pThis;
        foundation::ctl::inspectable::AddRefInterface(pThis);
        _spSourceEventArgs = pSourceEventArgs;
        return S_OK;
    }

    HRESULT _EnsureOldValues()
    {
        if (!_bOldValuesConverted)
        {
            foundation::InspectableArrayWrapper oldItems;
            IFR(_spSourceEventArgs->GetOldItems(oldItems.GetSizeAddressOf(), oldItems.GetBufferAddressOf()));
            if (oldItems.GetSize())
            {
                CopyArrayToVector(oldItems.GetSize(), oldItems.GetBuffer(), _proxyOldItemsArray);
                IFR_ASSERT(_pThis->ToProxyArray(_proxyOldItemsArray));
            }
            _bOldValuesConverted = true;
        }
        return S_OK;
    }
    HRESULT _EnsureNewValues()
    {
        if (!_bNewValuesConverted)
        {
            foundation::InspectableArrayWrapper newItems;
            IFR(_spSourceEventArgs->GetNewItems(newItems.GetSizeAddressOf(), newItems.GetBufferAddressOf()));
            if (newItems.GetSize())
            {
                CopyArrayToVector(newItems.GetSize(), newItems.GetBuffer(), _proxyNewItemsArray);
                IFR_ASSERT(_pThis->ToProxyArray(_proxyNewItemsArray));
            }
            _bNewValuesConverted = true;
        }
        return S_OK;
    }
    _Inspectable_ptr_vector_type& _GetOldItemsArray() { return _proxyOldItemsArray; }
    _Inspectable_ptr_vector_type& _GetNewItemsArray() { return _proxyNewItemsArray; }

protected:
    CProxyNotifyCollectionChangedEventArgs() :
        _pThis(nullptr), 
        _bNewValuesConverted(false),
        _bOldValuesConverted(false)
    {}

    void FinalRelease() override
    {
        foundation::ctl::inspectable::ReleaseInterface(_pThis);
		pmod::library::_NotifyCollectionChangedEventArgsBase::FinalRelease();
    }
    STDMETHOD(GetOldItems)(_Out_ UINT32 *size, _Outptr_ foundation::IInspectable*** oldItems) override
    {
        IFR(_EnsureOldValues());
        ContainerToArray(_proxyOldItemsArray, size, oldItems);
        return S_OK;
    }

    STDMETHOD(GetNewItems)(_Out_ UINT32 *size, _Outptr_ foundation::IInspectable*** newItems) override
    {
        IFR(_EnsureNewValues());
        ContainerToArray(_proxyNewItemsArray, size, newItems);
        return S_OK;
    }
private:
    CObservableCollectionProxy *_pThis;
    foundation::ComPtr<INotifyCollectionChangedEventArgs> _spSourceEventArgs;
    _Inspectable_ptr_vector_type _proxyOldItemsArray;
    _Inspectable_ptr_vector_type _proxyNewItemsArray;
    bool _bNewValuesConverted;
    bool _bOldValuesConverted;

};
//------------------------------------------------------------------------------
// Class:   CCollectionModelProxyIterator
// foundation::IIterator_impl<IInspectable*> implementation using the Items Container implementation
//------------------------------------------------------------------------------
class CCollectionModelProxyIterator :
    public foundation::ctl::ComInspectableBase,
    public foundation::IIterator_impl<foundation::IInspectable*>
{
public:
    static HRESULT CreateInstance(
        CObservableCollectionProxy *pCollectionModelProxy,
        foundation::IIterator_impl<IInspectable*> *pItemsContainerIterator,
        foundation::IIterator_impl<IInspectable*> **ppIterator)
    {
        foundation_assert(pCollectionModelProxy);
        foundation_assert(pItemsContainerIterator);

        CCollectionModelProxyIterator *pIterator = nullptr;
        foundation::ctl::ComInspectableObject<CCollectionModelProxyIterator>::CreateInstance(&pIterator);
        pIterator->_pCollectionModelProxy = pCollectionModelProxy;
        pIterator->_pItemsContainerIterator = pItemsContainerIterator;
        // add ref
        foundation::ctl::inspectable::AddRefInterface(pCollectionModelProxy);
        pItemsContainerIterator->AddRef();

        *ppIterator = pIterator;
        return S_OK;
    }
protected:
    CCollectionModelProxyIterator() :
        _pCollectionModelProxy(nullptr),
        _pItemsContainerIterator(nullptr)
    {}
    void FinalRelease()
    {
        // release used interafces
        RELEASE_INTERFACE(_pItemsContainerIterator);
        foundation::ctl::inspectable::ReleaseInterface(_pCollectionModelProxy);
    }

    STDMETHOD(GetCurrent)(_Outptr_ IInspectable* *current)
    {
        IFCPTR_ASSERT(current);
        foundation::InspectablePtr spItem;
        IFR_ASSERT(_pItemsContainerIterator->GetCurrent(spItem.GetAddressOf()));
        
        IFR_ASSERT(_pCollectionModelProxy->ToProxyValue(spItem));
        return spItem.CopyTo(current);
    }

    STDMETHOD(GetHasCurrent)(_Outptr_ bool *hasCurrent)
    {
        return _pItemsContainerIterator->GetHasCurrent(hasCurrent);
    }

    STDMETHOD(MoveNext)(_Outptr_ bool *hasCurrent)
    {
        return _pItemsContainerIterator->MoveNext(hasCurrent);
    }
private:
    CObservableCollectionProxy   *_pCollectionModelProxy;
    foundation::IIterator_impl<IInspectable*> *_pItemsContainerIterator;
};

CObservableCollectionProxy::CObservableCollectionProxy():
    _itemType(foundation::PropertyType_Empty)
{
}

HRESULT CObservableCollectionProxy::_Initialize(
    _In_ UINT32 proxyUniqueId,
    _In_ IObservableCollection *pSourceObject,
    _In_ IProxyObjectDelegate *pProxyModelDelegateBase,
    _In_ CProxyObjectFactory *pProxyObjectFactory,
    _In_ bool isFreeThreadedEnabled)
{
    IFR_ASSERT(_CollectionModelProxyBaseType::_Initialize(
        proxyUniqueId,
        pSourceObject,
        pProxyModelDelegateBase,
        pProxyObjectFactory,
        isFreeThreadedEnabled));
    // Note:
    // After initial construction if our 'Source' Collection
    // is Free threaded and caching is enabled we need to force attach to the underlying
    // source collection asap to offer a thread safe collection to the the consumer thread
    if (IsModelCacheEnabled() && IsSourceFreeThreaded())
    {
        // will cause the Proxy Collection to attach to event notification but also
        // get the initial snaphost of the current items in a safe way
        this->SetForceAttached(true);
    }
    return S_OK;
}

HRESULT CObservableCollectionProxy::on_invoke_internal(IUnknown* pSender,INotifyCollectionChangedEventArgs* pArgs)
{   
    if(IsInSourceConnectState())
    {
        return S_FALSE;
    }
    UNREFERENCED_PARAMETER(pSender);

    UINT32 oldStartingIndex = (UINT32)-1;
    UINT32 newStartingIndex = (UINT32)-1;

    foundation::NotifyCollectionModelChangedAction action;
    pArgs->GetAction(&action);

    if (action == foundation::NotifyCollectionModelChangedAction::ItemRemoved || action == foundation::NotifyCollectionModelChangedAction::Reset)
    {
        if (action == foundation::NotifyCollectionModelChangedAction::ItemRemoved)
        {
            IFR_ASSERT(pArgs->GetOldStartingIndex(&oldStartingIndex));
        }
    }
    else if (action == foundation::NotifyCollectionModelChangedAction::ItemAdded || action == foundation::NotifyCollectionModelChangedAction::ItemReplaced)
    {
        IFR_ASSERT(pArgs->GetNewStartingIndex(&newStartingIndex));
    }

    typedef foundation::ctl::ComInspectableObject<CProxyNotifyCollectionChangedEventArgs> _CProxyNotifyCollectionChangedEventArgs_Com_Object;

    foundation::ComPtr<_CProxyNotifyCollectionChangedEventArgs_Com_Object> spProxyNotifyCollectionChangedEventArgs;
    _CProxyNotifyCollectionChangedEventArgs_Com_Object::CreateInstance(spProxyNotifyCollectionChangedEventArgs.GetAddressOf());
    IFR_ASSERT(spProxyNotifyCollectionChangedEventArgs->_Initialize(
        this,
        action,
        oldStartingIndex,
        newStartingIndex,
        pArgs));

    if (IsProxyModelCacheEnabled())
    {
        foundation_assert(_spCachedFastItemsContainer != nullptr);
        foundation_lib::_FoundationLock csl(*this);

        // sync my cache items
        if (action == foundation::NotifyCollectionModelChangedAction::ItemRemoved || action == foundation::NotifyCollectionModelChangedAction::Reset)
        {
            if (action == foundation::NotifyCollectionModelChangedAction::Reset)
            {
                _IFR_(spProxyNotifyCollectionChangedEventArgs->_EnsureNewValues());
                _Inspectable_ptr_vector_type& newItemsArray = spProxyNotifyCollectionChangedEventArgs->_GetNewItemsArray();

                _spCachedFastItemsContainer->ResizeSlotItems((UINT32)newItemsArray.size());
                CacheItemsArray(0, newItemsArray);
            }

            if (action == foundation::NotifyCollectionModelChangedAction::ItemRemoved)
            {
                _IFR_(spProxyNotifyCollectionChangedEventArgs->_EnsureOldValues());
                _Inspectable_ptr_vector_type& oldItemsArray = spProxyNotifyCollectionChangedEventArgs->_GetOldItemsArray();

                if (oldStartingIndex != foundation::NINDEX)
                {
                    _spCachedFastItemsContainer->RemoveSlotItems(oldStartingIndex, (UINT32)oldItemsArray.size());
                }
                else
                {
                    // remove each item without using index
                    for (UINT32 index = 0; index < oldItemsArray.size(); ++index)
                    {
                        IFR_ASSERT(_spCachedFastItemsContainer->Remove(oldItemsArray[index]));
                    }
                }
            }
        }
        else if (action == foundation::NotifyCollectionModelChangedAction::ItemAdded || action == foundation::NotifyCollectionModelChangedAction::ItemReplaced)
        {
            _IFR_(spProxyNotifyCollectionChangedEventArgs->_EnsureNewValues());
            _Inspectable_ptr_vector_type& newItemsArray = spProxyNotifyCollectionChangedEventArgs->_GetNewItemsArray();

            if (action == foundation::NotifyCollectionModelChangedAction::ItemAdded)
            {
                _spCachedFastItemsContainer->InsertSlotItems(newStartingIndex, (UINT32)newItemsArray.size());
            }

            if (newStartingIndex == foundation::NINDEX && action == foundation::NotifyCollectionModelChangedAction::ItemAdded)
            {
                _spCachedFastItemsContainer->GetCount(&newStartingIndex);
                newStartingIndex -= (UINT32)newItemsArray.size();
            }
            if (newStartingIndex != foundation::NINDEX)
            {
                CacheItemsArray(newStartingIndex, newItemsArray);
            }
        }
    }

    return FireNotifyCollectionChangedEvent(spProxyNotifyCollectionChangedEventArgs);
}

HRESULT CObservableCollectionProxy::PerformFireNotifyCollectionChangedEventArgs(
	_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
	_In_ INotifyCollectionChangedEventArgs *pEventArgs)
{
    foundation_assert(pEventArgs);

    HRESULT hr;
    hr = FireEventInternal(pEventHandlerContainer,pEventArgs);
    return hr;
}

HRESULT CObservableCollectionProxy::FireNotifyCollectionChangedEvent(_In_ INotifyCollectionChangedEventArgs *pEventArgs)
{
    IFR(FireEventSupportInternal(
        &CObservableCollectionProxy::PerformFireNotifyCollectionChangedEventArgs,
        pEventArgs));
    return S_OK;

}

STDMETHODIMP CObservableCollectionProxy::GetCount(UINT32* pCount)
{
    PERF_CONTEXT(ProxyFactory_PerfId_CollectionModel_GetCount_Id)
    
    IFR(VerifyModelSource());
    if (IsProxyModelCacheEnabled())
    {
        return _spCachedFastItemsContainer->GetCount(pCount);
    }
    else
    {
        return m_spObjectSource->GetCount(pCount);
    }
}

STDMETHODIMP CObservableCollectionProxy::GetItem(UINT32 index,IInspectable* *ppItemValue)
{
    PERF_CONTEXT(ProxyFactory_PerfId_CollectionModel_GetItem_Id)
    
    IFR(VerifyModelSource());

    foundation::InspectablePtr value;
    if(IsProxyModelCacheEnabled())
    {
        IFR(_spCachedFastItemsContainer->Get(index, value.GetAddressOf()));
    }
    else
    {
        IFR(m_spObjectSource->GetItem(index, value.GetAddressOf()));
        IFR_ASSERT(ToProxyValue(value));
    }
    return value.CopyTo(ppItemValue);
}

STDMETHODIMP CObservableCollectionProxy::SetItem(_In_ UINT32 nIndex,_In_ IInspectable *pItem)
{

    foundation::ComPtr<IObservableList> spListModelSource;
    IFR(VerifyListModelSource(spListModelSource.GetAddressOf()));

    foundation::InspectablePtr itemValue(pItem);
    IFR(FromProxyValue(itemValue));

    IFR(spListModelSource->SetItem(nIndex,itemValue));
    if(IsProxyModelCacheEnabled())
    {
        foundation_lib::_FoundationLock csl(*this);
        IFR_ASSERT(_spCachedFastItemsContainer->Set(nIndex, itemValue));
    }
    return S_OK;
}

STDMETHODIMP CObservableCollectionProxy::GetItems(_Outptr_ UINT32* size,_Outptr_ IInspectable*** items)
{
    IFR(VerifyModelSource());

    if (IsProxyModelCacheEnabled())
    {
        foundation_lib::_FoundationLock csl(*this);
        return _spCachedFastItemsContainer->GetItems(size, items);
    }
    _Inspectable_ptr_vector_type itemsArray;
    IFR(GetCurrentSourceItems(itemsArray));
    ContainerRefCountToArray(itemsArray, size, items);
    return S_OK;
}

STDMETHODIMP CObservableCollectionProxy::IndexOf(_In_ IInspectable *pItem, _Outptr_ UINT32* pnIndex)
{
    IFR(VerifyModelSource());

    foundation::InspectablePtr itemValue(pItem);
    IFR(FromProxyValue(itemValue));
    if (IsProxyModelCacheEnabled())
    {
        foundation_lib::_FoundationLock csl(*this);
        if (IsAllItemsCached())
        {
            return _spCachedFastItemsContainer->Find(itemValue, nullptr, nullptr, pnIndex);
        }
    }
    return m_spObjectSource->IndexOf(itemValue, pnIndex);
}

STDMETHODIMP CObservableCollectionProxy::First(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable *> **first)
{
    IFR(VerifyModelSource());

    if (IsProxyModelCacheEnabled())
    {
        foundation_lib::_FoundationLock csl(*this);
        return _spCachedFastItemsContainer->First(first);
    }
    else
    {
        foundation::ComPtr<foundation::IIterator_impl<foundation::IInspectable *>> spIterator;
        IFR_ASSERT(m_spObjectSource->First(spIterator.GetAddressOf()));
        return CCollectionModelProxyIterator::CreateInstance(this, spIterator, first);
    }
}

STDMETHODIMP CObservableCollectionProxy::AppendItem(_In_ IInspectable *pItem)
{
    foundation::ComPtr<IObservableList> spListModelSource;
    IFR(VerifyListModelSource(spListModelSource.GetAddressOf()));

    foundation::InspectablePtr itemValue(pItem);
    IFR(FromProxyValue(itemValue));
    return spListModelSource->AppendItem(itemValue);
}

STDMETHODIMP CObservableCollectionProxy::InsertItem(_In_ UINT32 nIndex,_In_ IInspectable *pItem)
{
    foundation::ComPtr<IObservableList> spListModelSource;
    IFR(VerifyListModelSource(spListModelSource.GetAddressOf()));

    foundation::InspectablePtr itemValue(pItem);
    IFR(FromProxyValue(itemValue));
    return spListModelSource->InsertItem(nIndex,itemValue);
}

STDMETHODIMP CObservableCollectionProxy::RemoveItem(_In_ IInspectable *pItem)
{
    foundation::ComPtr<IObservableList> spListModelSource;
    IFR(VerifyListModelSource(spListModelSource.GetAddressOf()));

    foundation::InspectablePtr itemValue(pItem);
    IFR(FromProxyValue(itemValue));
    IFR(spListModelSource->RemoveItem(itemValue));
    return S_OK;
}

STDMETHODIMP CObservableCollectionProxy::RemoveAt(_In_ UINT32 nIndex)
{
    foundation::ComPtr<IObservableList> spListModelSource;
    IFR(VerifyListModelSource(spListModelSource.GetAddressOf()));
    return spListModelSource->RemoveAt(nIndex);
}

STDMETHODIMP CObservableCollectionProxy::RemoveAll()
{
    foundation::ComPtr<IObservableList> spListModelSource;
    IFR(VerifyListModelSource(spListModelSource.GetAddressOf()));
    return spListModelSource->RemoveAll();
}


void CObservableCollectionProxy::SetIsAttached(bool isAttached)
{
    InvalidateItemsCache();
    _CollectionModelProxyBaseType::SetIsAttached(isAttached);
}
    
STDMETHODIMP CObservableCollectionProxy::Dispose()
{
    InvalidateItemsCache();
    return _CollectionModelProxyBaseType::Dispose();
}

void CObservableCollectionProxy::InvalidateItemsCache()
{
    //m_CachedItems.clear();
}

HRESULT CObservableCollectionProxy::QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface)
{
    HRESULT hr = S_OK;
    if (iid == IObservableList::GetIID())
    {
       *ppInterface = static_cast<IObservableList *>(this);
    }
    else
    {
        hr = _CollectionModelProxyBaseType::QueryInterfaceImpl(iid,ppInterface);
    }
    return hr;
}

foundation::ILogCategory *CObservableCollectionProxy::GetLogCategory()
{
    return this->m_pProxyObjectFactory->GetCollectionProxyCategory();
}

STDMETHODIMP CObservableCollectionProxy::ConnectToSource(
    foundation::IInspectable *pSourceInfo, 
    _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    InvalidateItemsCache();

    if(IsAttachedNeeded())
    {
        HRESULT hr = S_OK;
         _Inspectable_ptr_vector_type newItems;
        IFHR(GetCurrentSourceItems(newItems));

        foundation::InspectableArrayWrapper disconnectItemsArray;
        IFR_ASSERT(foundation::pv_util::GetInspectableArray(
            pSourceInfo,
            disconnectItemsArray.GetSizeAddressOf(),
            disconnectItemsArray.GetBufferAddressOf()
            ));
        _Inspectable_ptr_vector_type disconnectItems;
        CopyArrayToVector(disconnectItemsArray.GetSize(), disconnectItemsArray.GetBuffer(), disconnectItems);

        foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        if (!foundation::library::IsEqual(newItems, disconnectItems))
        {
            hr = pmod::library::NotifyCollectionChangedEventArgsImpl::CreateInstance(
                foundation::NotifyCollectionModelChangedAction::Reset,
                (UINT32)-1,
                (UINT32)-1,
                disconnectItems,
                newItems,
                spNotifyCollectionChangedEventArgs.GetAddressOf());
        }
        // Destroy Safe Arrays
        disconnectItems.clear();
        newItems.clear();
        IFHR(hr);
        // Only notify if the collection where changed
        if(spNotifyCollectionChangedEventArgs != nullptr)
        {
            syncSourceInfo.push_back(std::make_pair(this,spNotifyCollectionChangedEventArgs.Get()));
        }
        return S_OK;
    }
    return _CollectionModelProxyBaseType::ConnectToSource(pSourceInfo, syncSourceInfo);
}

STDMETHODIMP CObservableCollectionProxy::DisconnectFromSource(foundation::IInspectable **ppSourceInfo)
{
    if(IsAttachedNeeded())
    {
        // Save Current items before we stop receiving any Events from our Source
        _Inspectable_ptr_vector_type sourceItems;
        GetCurrentSourceItems(sourceItems);

        IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
            (UINT32)sourceItems.size(),
            sourceItems.size() ? sourceItems.front().GetAddressOfPtr() : nullptr,
            ppSourceInfo
            ));
        return S_OK;
    }
    return _CollectionModelProxyBaseType::DisconnectFromSource(ppSourceInfo);
}

HRESULT CObservableCollectionProxy::GetCurrentSourceItems(_Inspectable_ptr_vector_type& items)
{
    foundation_assert(m_spObjectSource);

    foundation::InspectableArrayWrapper itemsWrapper;
    IFR(m_spObjectSource->GetItems(itemsWrapper.GetSizeAddressOf(),itemsWrapper.GetBufferAddressOf()));

    CopyArrayToVector(itemsWrapper.GetSize(),itemsWrapper.GetBuffer(),items);

    IFR_ASSERT(ToProxyArray(items));

    return S_OK;
}

bool CObservableCollectionProxy::IsAllItemsCached()
{
    foundation_assert(_spCachedFastItemsContainer != nullptr);

    UINT32 itemsCount;
    _spCachedFastItemsContainer->GetCount(&itemsCount);

    for (UINT32 index = 0; index < itemsCount; ++index)
    {
        _ItemSlotPrefix_Type customItemData;
        _spCachedFastItemsContainer->ReadCustomData(index,&customItemData);
        if (!customItemData._isCached)
        {
            return false;
        }
    }
    // all items in our container are cached
    return true;
}

HRESULT CObservableCollectionProxy::CacheItemsArray(
    _In_ UINT32 nIndex,
    const _Inspectable_ptr_vector_type& itemsArray)
{
    foundation_assert(_spCachedFastItemsContainer != nullptr);
    for (_Inspectable_ptr_vector_type::const_iterator iter = itemsArray.begin();
        iter != itemsArray.end();
        ++iter)
    {
        foundation::InspectablePtr spItem = *iter;
        // we want to store the 'source' reference in our cached items container
        IFR_ASSERT(FromProxyValue(spItem));
        IFR_ASSERT(_spCachedFastItemsContainer->Set(nIndex + (UINT32)(iter - itemsArray.begin()), spItem));
    }
    return S_OK;
}

HRESULT CObservableCollectionProxy::EnsureItemType()
{
    if (_itemType == foundation::PropertyType_Empty)
    {
        foundation::ComPtr<foundation::IEnumerableTypeInfo> spEnumerableTypeInfo;
        foundation::GetObjectTypeInfo(m_spObjectSource, spEnumerableTypeInfo.GetAddressOf());
        foundation::ComPtr<foundation::IPropertyTypeInfo> spItemTypeInfo;
        IFR_ASSERT(spEnumerableTypeInfo->GetItemTypeInfo(spItemTypeInfo.GetAddressOf()));
        IFR_ASSERT(spItemTypeInfo->GetPropertyType(&_itemType));
    }
    return S_OK;
}

STDMETHODIMP CObservableCollectionProxy::OnClearSlotItem(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr)
{
    _ItemSlotPrefix_Type customItemData;
    customItemData._Initialize(pItemSlotPtr);
    customItemData._isCached = false;
    customItemData._indexSource = foundation::NINDEX;
    RELEASE_INTERFACE(customItemData._pCacheWeakRef);

    customItemData._Commit(pItemSlotPtr);
    return S_OK;
}

STDMETHODIMP CObservableCollectionProxy::OnGetSlotItem(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr, foundation::IInspectable **value)
{
    _ItemSlotPrefix_Type customItemData;
    customItemData._Initialize(pItemSlotPtr);

    foundation::InspectablePtr spItemValue;
    if (!customItemData._isCached)
    {
        if (IsSourceFreeThreaded())
        {
            foundation_assert(_spCachedFastItems != nullptr);
            IFR_ASSERT(_spCachedFastItems->GetItem(customItemData._indexSource, spItemValue.GetAddressOf()));
        }
        else
        {
            IFR(m_spObjectSource->GetItem(index, spItemValue.GetAddressOf()));
        }
        IFR_ASSERT(_spCachedFastItemsContainer->SetSlotItem(index, spItemValue));
    }
    else
    {
        // if weak ref is alive return the resolved strong ref
        if (customItemData._pCacheWeakRef != nullptr &&
            SUCCEEDED(foundation::ResolveWeakRef(customItemData._pCacheWeakRef, *value)) &&
            *value != nullptr)
        {
            return S_OK;
        }
        // item was cached in our container but no weak ref was found
        IFR_ASSERT(_spCachedFastItemsContainer->GetSlotItem(index, spItemValue.GetAddressOf()));
    }
    IFR_ASSERT(ToProxyValue(spItemValue));

    CommitCache(customItemData, pItemSlotPtr, spItemValue);
    return spItemValue.CopyTo(value);
}

void CObservableCollectionProxy::CommitCache(
    _ItemSlotPrefix_Type& customItemData,
    _In_ LPVOID pItemSlotPtr,
    _In_ foundation::IInspectable *itemValue)
{
    // next code will write our custom data section including update to 
    // the cached state and possible weak ref caching
    customItemData._isCached = true;
    RELEASE_INTERFACE(customItemData._pCacheWeakRef);

    // see if we can cache a weak ref of the returned value
    if (itemValue != nullptr && _itemType == foundation::PropertyType_Inspectable)
    {
        foundation::AsWeakRef(customItemData._pCacheWeakRef, itemValue);
    }
    customItemData._Commit(pItemSlotPtr);
}

STDMETHODIMP CObservableCollectionProxy::OnSetSlotItem(
    _In_ UINT32 index, 
    _In_ LPVOID pItemSlotPtr,
    foundation::IInspectable *value)
{
    IFR_ASSERT(_spCachedFastItemsContainer->SetSlotItem(index, value));

    _ItemSlotPrefix_Type customItemData;
    customItemData._Initialize(pItemSlotPtr);

    CommitCache(customItemData, pItemSlotPtr, value);
    return S_OK;
}

HRESULT CObservableCollectionProxy::CreateCachedFastItemsContainer(UINT32 initialSize)
{
    foundation_assert(_spCachedFastItemsContainer == nullptr);

    IFR_ASSERT(EnsureItemType());

    FastItemsContainerParameters parameters;
    parameters.m_p_delegate = this;
    parameters.m_options = FastItemsContainerOptions::IsDelegateWeakRef;
    parameters._initialSize = initialSize;
    parameters._propertyType = _itemType;
    parameters._slotPrefixSize = sizeof(_ItemSlotPrefix_Type);

    IFR_ASSERT(CreateFastItemsContainer(&parameters, _spCachedFastItemsContainer.GetAddressOf()));
    for (UINT32 index = 0; index < initialSize; ++index)
    {
        _ItemSlotPrefix_Type customItemData;
        customItemData._isCached = false;
        customItemData._indexSource = index;
        customItemData._pCacheWeakRef = nullptr;
        IFR_ASSERT(_spCachedFastItemsContainer->WriteCustomData(index, &customItemData));
    }
    return S_OK;
}

HRESULT CObservableCollectionProxy::AttachToEventSource()
{
    if (IsProxyModelCacheEnabled())
    {
        UINT32 initialSize;
        if (IsSourceFreeThreaded())
        {
            foundation::ComPtr<foundation::library::IEventSourceClass> spEventSourceClass;
            IFR_ASSERT(foundation::QueryInterface(m_spObjectSource, spEventSourceClass.GetAddressOf()));

            foundation::InspectablePtr spResult;
            IFR_ASSERT(spEventSourceClass->AddEventHandler(
                pmod::INotifyCollectionChangedEventSource::GetIID(),
                this->m_pEventHandlerDelegate,
                nullptr,
                spResult.GetAddressOf()));

            IFCPTR_ASSERT(spResult);
            IFR_ASSERT(foundation::QueryInterface(spResult, _spCachedFastItems.ReleaseAndGetAddressOf()));

            // Initialize our Cache Fast Container
            _spCachedFastItems->GetCount(&initialSize);
        }
        else
        {
            IFR_ASSERT(_CollectionModelProxyBaseType::AttachToEventSource());
            this->m_spObjectSource->GetCount(&initialSize);
        }
        IFR_ASSERT(CreateCachedFastItemsContainer(initialSize));
        return S_OK;
    }
    else
    {
        return _CollectionModelProxyBaseType::AttachToEventSource();
    }
}

HRESULT CObservableCollectionProxy::DetachFromEventSource()
{
    if (IsModelCacheEnabled() && IsSourceFreeThreaded())
    {
        foundation::ComPtr<foundation::library::IEventSourceClass> spEventSourceClass;
        IFR_ASSERT(foundation::QueryInterface(m_spObjectSource, spEventSourceClass.GetAddressOf()));
        IFR_ASSERT(spEventSourceClass->RemoveEventHandler(
            pmod::INotifyCollectionChangedEventSource::GetIID(),
            this->m_pEventHandlerDelegate
            ));
        return S_OK;
    }
    else
    {
        return _CollectionModelProxyBaseType::DetachFromEventSource();
    }
}

HRESULT CObservableCollectionProxy::DetachFromSource()
{
    if (_spCachedFastItemsContainer != nullptr)
    {
        // clear all our cache 
        _spCachedFastItemsContainer->Clear();

        _spCachedFastItemsContainer.Release();
        _spCachedFastItems.Release();
    }
    _IFR_(_CollectionModelProxyBaseType::DetachFromSource());
    return S_OK;
}

STDMETHODIMP CObservableCollectionProxy::ResyncSource(
    IInspectable *pSource,
    _SyncSourceInfo_Vector_Type& syncSourceInfo)
{
    IFR_ASSERT(_CollectionModelProxyBaseType::ResyncSource(pSource, syncSourceInfo));

    if (_spCachedFastItemsContainer == nullptr)
    {
        // no caching was enabled on this (means not observed !)
        return S_OK;
    }

    // re create cache with current source collection size
    UINT32 sourceCountSize;
    m_spObjectSource->GetCount(&sourceCountSize);
    _spCachedFastItemsContainer->ResizeSlotItems(sourceCountSize);

    class CResyncCollectionChangedEventArgs :
        public foundation::library::_EventArgsBase<pmod::INotifyCollectionChangedEventArgs>
    {
    public:
        static HRESULT CreateInstance(
        CObservableCollectionProxy *pCollectionModelProxy,
        pmod::INotifyCollectionChangedEventArgs **ppNotifyCollectionChangedEventArgs)
        {
            foundation_assert(pCollectionModelProxy);

            CResyncCollectionChangedEventArgs *pResyncCollectionChangedEventArgs = nullptr;
            foundation::ctl::ComInspectableObject<CResyncCollectionChangedEventArgs>::CreateInstance(&pResyncCollectionChangedEventArgs);
            pResyncCollectionChangedEventArgs->_pCollectionModelProxy = pCollectionModelProxy;
            // add ref
            foundation::ctl::inspectable::AddRefInterface(pCollectionModelProxy);

            *ppNotifyCollectionChangedEventArgs = pResyncCollectionChangedEventArgs;
            return S_OK;
        }
    protected:
        CResyncCollectionChangedEventArgs() :
            _pCollectionModelProxy(nullptr)
        {}
        void FinalRelease()
        {
            // release used interafces
            foundation::ctl::inspectable::ReleaseInterface(_pCollectionModelProxy);
        } 
        STDMETHOD(GetAction)(_Outptr_ foundation::NotifyCollectionModelChangedAction* action) override
        {
            *action = foundation::NotifyCollectionModelChangedAction::Reset;
            return S_OK;
        }

        STDMETHOD(GetNewItems)(_Outptr_ UINT32 *size, _Outptr_ foundation::IInspectable*** newItems) override
        {
            return _pCollectionModelProxy->GetItems(size, newItems);
        }

        STDMETHOD(GetNewStartingIndex)(_Outptr_ UINT32* index) override
        {
            *index = foundation::NINDEX;
            return S_OK;
        }

        STDMETHOD(GetOldStartingIndex)(_Outptr_ UINT32* index)  override
        {
            *index = foundation::NINDEX;
            return S_OK;
        }

        STDMETHOD(GetOldItems)(_Outptr_ UINT32 *size, _Outptr_ foundation::IInspectable*** newItems) override
        {
            *size = 0;
            *newItems = nullptr;
            return S_OK;
        }
    private:
        CObservableCollectionProxy   *_pCollectionModelProxy;
    };

    foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
    IFR_ASSERT(CResyncCollectionChangedEventArgs::CreateInstance(
        this,
        spNotifyCollectionChangedEventArgs.GetAddressOf()));

    syncSourceInfo.push_back(std::make_pair(
        static_cast<IBaseObjectProxyInternal *>(this),
        spNotifyCollectionChangedEventArgs.Get()));

    return S_OK;
}

STDMETHODIMP CObservableCollectionProxy::FireResyncEvent(foundation::IUnknown *pEventArg)
{
    foundation::ComPtr<INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
    IFR_ASSERT(foundation::QueryInterface(pEventArg, spNotifyCollectionChangedEventArgs.GetAddressOf()));
    LogSourceEvent(
        foundation::LoggingLevel::Info,
        (UINT32)proxy::BaseProxy_Category::ResyncEvent,
        spNotifyCollectionChangedEventArgs.Get());
    return this->FireNotifyCollectionChangedEvent(spNotifyCollectionChangedEventArgs);
}
