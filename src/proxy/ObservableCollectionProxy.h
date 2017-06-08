/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionProxy.h
****/

#pragma once

#include "TBaseModelProxy.h"
#include <pmod/library/interfaces/observable_collection_class.h>

//------------------------------------------------------------------------------
// Class:   CObservableCollectionProxy
//
// Purpose: Collection Model Proxy implementation
//
//------------------------------------------------------------------------------
typedef 
TBaseModelProxy
    <
    pmod::IObservableCollection,
    pmod::INotifyCollectionChangedEventSource,
    pmod::INotifyCollectionChangedEventHandler,
    pmod::INotifyCollectionChangedEventArgs,
    pmod::proxy::IProxyObjectDelegate
    >
_CollectionModelProxyBaseType;

class CObservableCollectionProxy:
    public _CollectionModelProxyBaseType,
    public pmod::IObservableList,
    public pmod::library::IFastItemsContainerDelegate
{
public:
    HRESULT _Initialize(
        _In_ UINT32 proxyUniqueId,
        _In_ pmod::IObservableCollection *pSourceObject,
        _In_ pmod::proxy::IProxyObjectDelegate *pProxyModelDelegateBase,
        _In_ CProxyObjectFactory *pProxyFactory,
        _In_ bool IsFreeThreadedEnabled);

    // IObservableCollection
    STDMETHOD(GetCount)(UINT32* pCount) override;
    STDMETHOD(GetItem)(UINT32 index, IInspectable* *ppItemValue) override;
    STDMETHOD(GetItems)(_Outptr_ UINT32* size, _Outptr_ IInspectable*** items) override;
    STDMETHOD(IndexOf)(_In_ IInspectable *item, _Outptr_ UINT32* pnIndex) override;
    STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable *> **first) override;

    STDMETHOD(GetNotifyCollectionChangedEventSource)(_COM_Outptr_ pmod::INotifyCollectionChangedEventSource** ppEventSource)
    {
        * ppEventSource =  m_spEventHandler;
        (*ppEventSource)->AddRef();
        return S_OK;
    }

    // IObjectNode Interface for IListModel
    STDMETHOD (GetUniqueId)(UINT32 *uniqueId) 
    {
        return BaseObjectProxy::GetUniqueId(uniqueId);
    }
    STDMETHOD (GetProcessId)(UINT32 *processId)
    {
        return BaseObjectProxy::GetProcessId(processId);
    }
     STDMETHOD (GetTypeInfo)(foundation::IObjectTypeInfo** ppModelTypeInfo)
    {
        return BaseObjectProxy::GetTypeInfo(ppModelTypeInfo);
    }

    // IListModel
    STDMETHOD (SetItem)(_In_ UINT32 nIndex,_In_ IInspectable *pItem);
    STDMETHOD (AppendItem)(_In_ IInspectable *pItem);
    STDMETHOD (InsertItem)(_In_ UINT32 nIndex,_In_ IInspectable *pItem);
    STDMETHOD (RemoveItem)(_In_ IInspectable *pItem);
    STDMETHOD (RemoveAt)(_In_ UINT32 nIndex);
    STDMETHOD (RemoveAll)();

    // Override from TBaseObjectProxy<>
    HRESULT GetEventSource(
        _COM_Outptr_ pmod::INotifyCollectionChangedEventSource **ppEventSource) override
    {
        return this->GetNotifyCollectionChangedEventSource(ppEventSource);
    }

    // Override from BaseModelSource
    HRESULT on_invoke_internal(IUnknown* pSender,pmod::INotifyCollectionChangedEventArgs* pArgs) override;

    // Overrides from BaseModelProxy
    foundation::ILogCategory *GetLogCategory() override;
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
    UINT32 GetTrackRefCountOption() override { return _OptionTrackRefCountCollectionModelProxies; }
#endif

    // Pure Virtual Methods
   HRESULT GetSourceEventSource(pmod::INotifyCollectionChangedEventSource **ppEventSource) override
   {
        IFR(VerifyModelSource());
        return m_spObjectSource->GetNotifyCollectionChangedEventSource(ppEventSource);
   }

   // Override from ComBase
   HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override;
#if defined(DEBUG) && defined(TRACK_REF_COUNT)
   void FinalRelease() override
   {
       if (foundation::debug_util::IsOptionsFlag(_OptionTrackRefCountAllProxies | _OptionTrackRefCountCollectionModelProxies))
       {
           pal::library::TraceMessage(U("CObservableCollectionProxy::FinalRelease unique_id:%d\n"),this->m_UniqueId);
       }
       _CollectionModelProxyBaseType::FinalRelease();
   }
#endif

   friend class CCollectionModelProxyIterator;
protected:
    CObservableCollectionProxy();
    friend class CProxyNotifyCollectionChangedEventArgs;

    // Override from IBaseModelProxyInternal
   STDMETHOD(Dispose)() override;
   STDMETHOD(DisconnectFromSource)(foundation::IInspectable **ppSourceInfo) override;
   STDMETHOD(ConnectToSource)(foundation::IInspectable *pSourceInfo, _SyncSourceInfo_Vector_Type& syncSourceInfo) override;
   STDMETHOD(ResyncSource)(foundation::IInspectable *pSource, _SyncSourceInfo_Vector_Type& syncSourceInfo) override;
   STDMETHOD(FireResyncEvent)(foundation::IUnknown *pEventArg) override;

    // Overrides from BaseModelSource
   void SetIsAttached(bool isAttached) override;
    
   HRESULT AttachToEventSource() override;
   HRESULT DetachFromEventSource() override;
   HRESULT DetachFromSource() override;

    void InvalidateItemsCache();
    HRESULT FireNotifyCollectionChangedEvent(_In_ pmod::INotifyCollectionChangedEventArgs *pEventArgs);
    HRESULT PerformFireNotifyCollectionChangedEventArgs(
		_In_ foundation::library::IFireEventHandlerContainer *pEventHandlerContainer,
		_In_ pmod::INotifyCollectionChangedEventArgs *pEventArgs);


    HRESULT VerifyListModelSource(pmod::IObservableList **ppListModelSource)
    {
        foundation_assert(ppListModelSource);
        IFR(VerifyModelSource());
        return foundation::QueryInterface(m_spObjectSource,ppListModelSource);
    }

private:
    HRESULT EnsureItemType();
    HRESULT GetCurrentSourceItems(_Inspectable_ptr_vector_type& items);

    struct _ItemSlotPrefix_Type
    {
        inline void _Initialize(_In_ LPVOID pItemSlotPtr)
        {
            memcpy((void *)this, pItemSlotPtr, sizeof(_ItemSlotPrefix_Type));
        }
        inline void _Commit(_In_ LPVOID pItemSlotPtr)
        {
            memcpy(pItemSlotPtr,(void *)this, sizeof(_ItemSlotPrefix_Type));
        }
        UINT32  _indexSource;
        boolean _isCached;
        foundation::IWeakReference *_pCacheWeakRef;
    };
    // callback internals for Fast Items Container
    STDMETHOD(OnClearSlotItem)(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr) override;
    STDMETHOD(OnGetSlotItem)(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr, _COM_Outptr_result_maybenull_ foundation::IInspectable **value) override;
    STDMETHOD(OnSetSlotItem)(_In_ UINT32 index, _In_ LPVOID pItemSlotPtr, _In_opt_ foundation::IInspectable *value) override;

    void CommitCache(
        _ItemSlotPrefix_Type& customItemData,
        _In_ LPVOID pItemSlotPtr,
        _In_ foundation::IInspectable *itemValue);

    HRESULT CreateCachedFastItemsContainer(UINT32 initialSize);

    HRESULT CacheItemsArray(
        _In_ UINT32 nIndex,
        const _Inspectable_ptr_vector_type& itemsArray);
    bool IsAllItemsCached();
private:
    foundation::PropertyType                    _itemType;
    foundation::ComPtr<pmod::library::IFastItemsContainer> _spCachedFastItemsContainer;
    foundation::ComPtr<pmod::IObservableCollection> _spCachedFastItems;
};

  
