/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollection.h
****/
#pragma once

#include <pmod/library/model_library.h>
#include "ObjectNode.h"
#include <pmod/library/notify_collection_changed_event_args.h>
#include "ItemsContainer.h"

#include <memory>

class CObservableCollectionLogFactory : public _ObjectBaseLogFactory<>
{
public:
    static const CLogCategory*GetLogCategory();
};
//------------------------------------------------------------------------------
// Class:   CObservableCollection
//
// Purpose: Define the CObservableCollection class
//
//------------------------------------------------------------------------------
typedef ObjectNode
<
	pmod::IObservableCollection,
	foundation::IEnumerableTypeInfo,
	pmod::library::IObservableCollectionClass,
	pmod::library::IObservableCollectionDelegate,
	pmod::library::ObservableCollectionCreateParametersBase,
	pmod::INotifyCollectionChangedEventSource,
	pmod::INotifyCollectionChangedEventHandler,
	foundation::IUnknown,
	pmod::INotifyCollectionChangedEventArgs,
	CObservableCollectionLogFactory
>
CObservableCollectionBase_Type;

class CObservableCollection :
    public CObservableCollectionBase_Type,
    public pmod::IObservableList
{
public:
    static HRESULT  CreateInstance(
        _In_ const pmod::library::ObservableCollectionCreateParameters *pModelImpl,
        _In_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance);

    HRESULT _Initialize(const pmod::library::ObservableCollectionCreateParametersBase *pCollectionModelParam);
    HRESULT _Initialize(const pmod::library::ObservableCollectionCreateParameters *pCollectionModelParamEx);

	// TModelChangesCallback Overrides
	HRESULT _OnFireNotifyInModelChangesChanged(bool newValue)
	{
		UNREFERENCED_PARAMETER(newValue);
		return S_OK;
	}

protected:
    friend class CObservableCollectionValueConverterCallback;
	friend class CObservableCollectionEventSourceAdapter;

#if defined(_WINDOWS_RUNTIME)
    INSPECTABLE_CLASS(U("Microsoft.PropertyModel.Library.ObservableCollection"), foundation::TrustLevel::BaseTrust);
#endif

    CObservableCollection() :
       m_IsReadOnly(false),
       m_IsCollectionSet(false),
       m_UseFastItemsContainer(false)
    {
    }
    ~CObservableCollection()
	{
	}

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)
    void DumpObject() override;
#endif

protected:
    // IObservableCollection Interface
    STDMETHOD (GetCount)(_Outptr_ UINT32 *pCount) override;
    STDMETHOD (GetItem)(_In_ UINT32 nIndex,_Outptr_ foundation::IInspectable* *ppItemValue) override;
	STDMETHOD (GetItems)(_Outptr_ UINT32* size,_Outptr_ foundation::IInspectable*** items) override;
    STDMETHOD (IndexOf)(_In_ foundation::IInspectable *item, _Outptr_ UINT32* pnIndex) override;
    STDMETHOD (First)(_COM_Outptr_ foundation::IIterator_impl<foundation::IInspectable *> **first) override;

    // IObject Interface for IObservableList
    STDMETHOD (GetUniqueId)(UINT32 *uniqueId) 
    {
        return CObservableCollectionBase_Type::GetUniqueId(uniqueId);
    }
    STDMETHOD (GetProcessId)(UINT32 *processId)
    {
        return CObservableCollectionBase_Type::GetProcessId(processId);
    }
    STDMETHOD (GetTypeInfo)(foundation::IObjectTypeInfo** ppModelTypeInfo)
    {
        return CObservableCollectionBase_Type::GetTypeInfo(ppModelTypeInfo);
    }

    // IObservableList Interface
    STDMETHOD (SetItem)(_In_ UINT32 nIndex,_In_ foundation::IInspectable *pItem) override;
    STDMETHOD (AppendItem)(_In_ foundation::IInspectable *pItem) override;
    STDMETHOD (InsertItem)(_In_ UINT32 nIndex,_In_ foundation::IInspectable *pItem) override;
    STDMETHOD (RemoveItem)(_In_ foundation::IInspectable *pItem) override;
    STDMETHOD (RemoveAt)(_In_ UINT32 nIndex) override;
    STDMETHOD (RemoveAll)() override;

    STDMETHOD(GetNotifyCollectionChangedEventSource)(_Outptr_ pmod::INotifyCollectionChangedEventSource** ppEventSource);

    // Override from _BaseNode

    // IObservableCollectionClass Interface
    STDMETHOD(GetItemsContainer)(_COM_Outptr_ pmod::library::IItemsContainer **ppItemsContainer) override;
    STDMETHOD(GetItemConverter)(_COM_Outptr_ pmod::library::IValueConverter **ppValueConverter) override;
    STDMETHOD(SetItemConverter)(_In_ pmod::library::IValueConverter *pValueConverter) override;
    STDMETHOD(Sort)(_In_ pmod::library::CompareItemValueFunc compare,void *Context) override;
    STDMETHOD(InsertInOrder)(_In_ foundation::IInspectable *newValue, _In_ pmod::library::CompareItemValueFunc compare,_In_ void *Context,_Outptr_ UINT32 *index) override;
    // Internal Add/Insert/Replace Item
    STDMETHOD(AppendItemInternal)(_In_ foundation::IInspectable *item) override;
    STDMETHOD(InsertItemInternal)(_In_ UINT32 nIndex,_In_ foundation::IInspectable *item) override;
    STDMETHOD(ReplaceItemInternal)(_In_ UINT32 nIndex,_In_ foundation::IInspectable *item) override;
    STDMETHOD(RemoveItemInternal)(_In_ foundation::IInspectable *item);

    STDMETHOD(RemoveAllInternal)() override;
    STDMETHOD(IndexOfInternal)(_In_ foundation::IInspectable *item, _Outptr_ UINT32* pnIndex) override;

   // batch method support
    STDMETHOD(InsertItems)(_In_ UINT32 nIndex,_In_ UINT32 size,_In_ foundation::IInspectable **items) override;
    STDMETHOD(ReplaceItems)(_In_ UINT32 nIndex,_In_ UINT32 size,_In_ foundation::IInspectable **items) override;
    STDMETHOD(RemoveItems)(_In_ UINT32 nIndex,_In_ UINT32 count) override;
    STDMETHOD(FireCollectionChanged)(
        _In_ foundation::NotifyCollectionModelChangedAction action,
        _In_ UINT32 oldStartingIndex,
        _In_ UINT32 newStartingIndex,
        _In_ UINT32 sizeOldItems,
        _In_opt_ foundation::IInspectable **ppOldItems,
        _In_ UINT32 sizeNewItems,
        _In_opt_ foundation::IInspectable **ppNewItems
        );

    // ComBase Override
    void OnFinalRelease() override;
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;

    // ObjectNode Overrides
	HRESULT OnBeginModelChangesInternal(pmod::library::BatchModelChangesOptions options);
	HRESULT OnEndModelChangesInternal(pmod::library::EndModelChangesOptions options);
	HRESULT DisposeInternal() override;

    // FoundationBase overrides
    HRESULT GetEventSource(
        _COM_Outptr_ pmod::INotifyCollectionChangedEventSource **ppEventSource) override;

    HRESULT _ReturnAddHandlerWithOptions(
        _In_ foundation::IInspectable *pOptions,
        _COM_Outptr_ foundation::IInspectable **ppResult
        );

protected:
    HRESULT InsertInOrderInternal(
        _In_ foundation::IInspectable *newValue, 
        _In_ pmod::library::CompareItemValueFunc comparer,
        _In_ void *Context,
        _Outptr_ UINT32 *index,
        bool fireNotify=true);

    UINT32 FindItemInternal(_In_ foundation::IInspectable *item);


    bool IsNotifyCollectionChanged();
private:
    friend struct _ItemsComparer;
    friend class CCollectionModelIterator;
    HRESULT CheckArrayBounds(_In_ UINT32 nIndex);
    HRESULT VerifyItemTypeInfo(_Inout_ foundation::IInspectable **item);
    HRESULT VerifyItemsTypeInfo(_In_ _Inspectable_ptr_vector_type& items);
    HRESULT InitializeIfValueConverterCallback(pmod::INotifyCollectionChangedEventArgs *pNotifyCollectionChangedEventArgs);
    void LogGetItem(UINT32 index);
    void LogSetItem(UINT32 index);
    void LogGetItems();

    static int __cdecl _ItemsComparerAdapter(foundation::IInspectable *elem1, foundation::IInspectable *elem2,void *Context);

protected:
    HRESULT EnsureItemsContainer();
    void CopyContainerItems(_Inspectable_ptr_vector_type& items);

    HRESULT FireNotifyCollectionChanged(
        _FoundationBaseLock& csl,
        pmod::INotifyCollectionChangedEventArgs *pEventArgs);

    HRESULT FireNotifyCollectionChanged(
        _FoundationBaseLock& csl,
        foundation::NotifyCollectionModelChangedAction action,
        _Inspectable_ptr_vector_type& items,
        UINT32 index);
    HRESULT FireReplaceNotifyCollectionChanged(
        _FoundationBaseLock& csl,
        _Inspectable_ptr_vector_type& oldItems,
        _Inspectable_ptr_vector_type& newItems,
        UINT32 index);
    HRESULT FireResetNotifyCollectionChanged(
        _FoundationBaseLock& csl,
        _Inspectable_ptr_vector_type& oldItems,
        _Inspectable_ptr_vector_type& newItems);
    inline HRESULT FireResetNotifyCollectionChanged(
        _Inspectable_ptr_vector_type& oldItems,
        _Inspectable_ptr_vector_type& newItems)
    {
        _FoundationBaseLock csl(*this);
        return FireResetNotifyCollectionChanged(
            csl,
            oldItems,
            newItems);
    }

    HRESULT FireMoveNotifyCollectionChanged(
        _FoundationBaseLock& csl,
        foundation::NotifyCollectionModelChangedAction action,
        _Inspectable_ptr_vector_type& items,
        UINT32 nOldIndex,
        UINT32 nNewIndex);

    HRESULT InsertItemInternal(
        _In_ UINT32 nIndex,_In_ foundation::IInspectable *item,bool fireNotify,bool isProtected);
    HRESULT RemoveAtInternal(_In_ UINT32 nIndex, bool fireNotify, bool isProtected);
    HRESULT RemoveAllInternalHelper(bool isProtected);
    HRESULT MoveItemInternal(_In_ UINT32 nOldIndex,_In_ UINT32 nNewIndex,_In_ bool fireNotify,_In_ bool isProtected);

    HRESULT FireResetNotifyCollectionChangedWithCallback();
    HRESULT FireItemNotifyCollectionChangedWithCallback(
        _In_ UINT32 nNewIndex,
        foundation::NotifyCollectionModelChangedAction action
        );
    HRESULT DisposeItems();
    // Remove Internal by Item
    HRESULT RemoveItemInternal(_In_ foundation::IInspectable *item,bool isProtected);

    // batch internal operations
    HRESULT ReplaceItemsInternal(_In_ UINT32 nIndex,_In_ _Inspectable_ptr_vector_type& items,bool fireNotify,bool isProtected);
    HRESULT InsertItemsInternal(_In_ UINT32 nIndex,_In_ _Inspectable_ptr_vector_type& items,bool fireNotify,bool isProtected);
    HRESULT RemoveAtInternal(_In_ UINT32 nIndex,_In_ UINT32 nCount,bool fireNotify,bool isProtected);
    HRESULT MoveItemsInternal(_In_ UINT32 nOldIndex,_In_ UINT32 nNewIndex,_In_ UINT32 nCount,_In_ bool fireNotify,_In_ bool isProtected);

    HRESULT ConvertItemValuesInternal(_Inspectable_ptr_vector_type& items, _In_ bool isConvertBack);
    HRESULT GetContainerItem(_In_ UINT32 nIndex, _In_ bool bUseItemConverter, _COM_Outptr_ foundation::IInspectable **ppItemValue);
    bool IsReadOnly() { return m_IsReadOnly; }
    bool IsCollectionSet() { return m_IsCollectionSet; }
    pmod::library::IItemsContainer *GetItemsContainer() { return m_spItemsContainer; }

    // overridable methods
    virtual bool IsItemEqual(_In_ foundation::IInspectable *item1, _In_ foundation::IInspectable *item2);
    virtual HRESULT ConvertItemValue(_In_ UINT32 nIndex, _In_ foundation::IInspectable *pValue, _In_ bool isConvertBack, _COM_Outptr_ foundation::IInspectable **ppConvertedValue);
    virtual HRESULT ConvertItemValues(_Inspectable_ptr_vector_type& items, _In_ bool isConvertBack);
private:
    HRESULT RemoveAtInternal(
        _FoundationBaseLock& csl,
        UINT32 nIndex,
        UINT32 nCount, 
        bool fireNotify, 
        bool isProtected);

    HRESULT MoveItemsInternal(
        _FoundationBaseLock& csl,
        UINT32 nOldIndex,
        UINT32 nNewIndex, 
        UINT32 nCount, 
        bool fireNotify,
        bool isProtected);

private:
    struct _CollectionModelChangesType
    {
        struct _CollectionModelChangedActionType
        {
            foundation::NotifyCollectionModelChangedAction  _action;
            UINT32                                          _index;
            _Inspectable_ptr_vector_type                           _items;
        };

        bool CanFireModelChanges(CObservableCollection *pThis);
        HRESULT FireModelChanges(CObservableCollection *pThis);
        void AddCollectionModelChangeAction(
            CObservableCollection *pThis,
            foundation::NotifyCollectionModelChangedAction  action,
            UINT32 index,
            const _Inspectable_ptr_vector_type& items
            );
        void AddResetAction(CObservableCollection *pThis);

        _Inspectable_ptr_vector_type _oldValues;
        std::vector<_CollectionModelChangedActionType> _modelChanges;

    };
    // Our Items Container Ptr
    foundation::ComPtr<pmod::library::IItemsContainer>   m_spItemsContainer;
    // Value Converter storage
    foundation::ComPtr<pmod::library::IValueConverter>   m_spItemConverter;
    std::unique_ptr<_CollectionModelChangesType>                      m_CollectionModelChanges;
    bool m_IsReadOnly:1;
    bool m_IsCollectionSet:1;
    bool m_UseFastItemsContainer : 1;
};
