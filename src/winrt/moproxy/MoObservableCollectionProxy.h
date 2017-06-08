/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionProxy.h
****/

#pragma once

#include <foundation/assert.h>

#include "Microsoft.PropertyModel.h"
#include "MoAdapterProxyInterface.h"
#include "MoModelProxy.h"
#include <pmod/library/model_library.h>
#include "MoEventSourceProxy.h"
#include "MoCoreProxyFactory.h"
#include "MoObservableObjectProxy.h"

//------------------------------------------------------------------------------
// Class:   CMoNotifyCollectionChangedEventArgsProxy
//
// Purpose: Implementation of pmod::INotifyCollectionChangedEventArgs
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoNotifyCollectionChangedEventArgsProxy :
    public TAdapterProxyInterface
    <
        pmod::INotifyCollectionChangedEventArgs,
        ABI::Microsoft::PropertyModel::INotifyCollectionChangedEventArgs
    >
{
protected:
    STDMETHOD (GetAction)(_Outptr_ foundation::NotifyCollectionModelChangedAction* action)
    {
        return this->m_pSourceInterface->get_Action((ABI::Microsoft::PropertyModel::NotifyCollectionModelChangedAction *)action);
    }
    STDMETHOD (GetNewItems)(_Outptr_ UINT32 *size,_Outptr_ IInspectable*** newItems)
    {
        foundation::ComPtr<__FIVectorView_1_IInspectable> spNewItems;
        IFR_ASSERT(this->m_pSourceInterface->get_NewItems(spNewItems.GetAddressOf()));
        return ToProxyArray(spNewItems,size,newItems);
    }

    STDMETHOD (GetNewStartingIndex)(_Outptr_ UINT32* index)
    {
        return this->m_pSourceInterface->get_NewStartingIndex( (int *)index);
    }

    STDMETHOD (GetOldStartingIndex)(_Outptr_ UINT32* index)
    {
        return this->m_pSourceInterface->get_OldStartingIndex( (int *)index);
    }

    STDMETHOD (GetOldItems)(_Outptr_ UINT32 *size,_Outptr_ IInspectable*** oldItems)
    {
        foundation::ComPtr<__FIVectorView_1_IInspectable> spOldItems;
        IFR_ASSERT(this->m_pSourceInterface->get_OldItems(spOldItems.GetAddressOf()));
        return ToProxyArray(spOldItems,size,oldItems);
    }
private:
    static HRESULT ToProxyArray(__FIVectorView_1_IInspectable *pVectorView,
                _Out_ UINT32* pSize,IInspectable ***ppInspectableArray)
    {
        if(pVectorView == nullptr)
        {
            * pSize = 0;
            *ppInspectableArray = nullptr;
        }
        unsigned int size = 0;
        pVectorView->get_Size(&size);

        std::vector< foundation::InspectablePtr > coreProxies;
        coreProxies.resize(size);
        for(UINT32 index = 0;index < size;++index)
        {
            foundation::InspectablePtr spItem;
            pVectorView->GetAt(index,spItem.GetAddressOf());
            IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spItem,coreProxies[index].GetAddressOf()));
        }
        ContainerRefCountToArray(coreProxies,pSize,ppInspectableArray);
        return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   CNotifyCollectionChangedEventHandlerProxy
//
// Purpose: Implementation of Modern INotifyCollectionChangedEventHandler
//          Interface using the Core Interface callback
//------------------------------------------------------------------------------
class CNotifyCollectionChangedEventHandlerProxy :
    public TFreeThreadedAdapterProxyInterface
    <
        ABI::Microsoft::PropertyModel::INotifyCollectionChangedEventHandler,
        pmod::INotifyCollectionChangedEventHandler
    >,
    public ABI::Microsoft::PropertyModel::IDispatchable
{
protected:
    // Interface ABI::Microsoft::PropertyModel::INotifyCollectionChangedEventHandler
    HRESULT STDMETHODCALLTYPE Invoke(
        IInspectable *sender,
        ABI::Microsoft::PropertyModel::INotifyCollectionChangedEventArgs *e) override
    {
        foundation::ComPtr<pmod::INotifyCollectionChangedEventArgs> spNotifyCollectionChangedEventArgs;
        HRESULT hr = CMoNotifyCollectionChangedEventArgsProxy::CreateInstance<CMoNotifyCollectionChangedEventArgsProxy>
           (e,spNotifyCollectionChangedEventArgs.GetAddressOf());
        IFR_ASSERT(hr);
        return this->m_pSourceInterface->Invoke(sender,spNotifyCollectionChangedEventArgs);
    }

    // Interface ABI::Microsoft::PropertyModel::IDispatchable
    HRESULT STDMETHODCALLTYPE Invoke()
    {
        return S_OK;
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == __uuidof(ABI::Microsoft::PropertyModel::IDispatchable))
        {
            *ppInterface = static_cast<ABI::Microsoft::PropertyModel::IDispatchable *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};

typedef 
    TMoEventSourceProxy<
        pmod::INotifyCollectionChangedEventSource,
        pmod::INotifyCollectionChangedEventHandler,
        ABI::Microsoft::PropertyModel::INotifyCollectionChangedEventHandler,
        ABI::Microsoft::PropertyModel::IObservableCollection,
        &ABI::Microsoft::PropertyModel::IObservableCollection::add_NotifyCollectionChanged,
        &ABI::Microsoft::PropertyModel::IObservableCollection::remove_NotifyCollectionChanged,
        CNotifyCollectionChangedEventHandlerProxy>
    CMoNotifyCollectionChangedEventSourceProxy;

//------------------------------------------------------------------------------
// Class:   CMoObservableCollectionProxyIterator
// foundation::IIterator_impl<IInspectable*> implementation using interafce:ABI::Windows::Foundation::Collections::Iterator<IInspectable *>
//------------------------------------------------------------------------------
class CMoObservableCollectionProxyIterator :
    public foundation::ctl::ComInspectableBase,
    public foundation::IIterator_impl<IInspectable*>
{
public:
    static HRESULT CreateInstance(
        ABI::Windows::Foundation::Collections::IIterator<IInspectable *> *pMoIterator,
        foundation::IIterator_impl<IInspectable*> **ppIterator)
    {
        foundation_assert(ppIterator);
        foundation_assert(pMoIterator);

        CMoObservableCollectionProxyIterator *pIterator = nullptr;
        foundation::ctl::ComInspectableObject<CMoObservableCollectionProxyIterator>::CreateInstance(&pIterator);
        pIterator->_pMoIterator = pMoIterator;
        // add ref
        pMoIterator->AddRef();

        *ppIterator = pIterator;
        return S_OK;
    }
protected:
    CMoObservableCollectionProxyIterator() :
        _pMoIterator(nullptr)
    {}
    void FinalRelease()
    {
        // release used interafces
        RELEASE_INTERFACE(_pMoIterator);
    }
    STDMETHOD(GetCurrent)(_Outptr_ IInspectable* *current)
    {
        IFCPTR_ASSERT(current);
        foundation::InspectablePtr spItem;
        IFR_ASSERT(_pMoIterator->get_Current(spItem.GetAddressOf()));

        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spItem, current));
        return S_OK;
    }
    STDMETHOD(GetHasCurrent)(_Outptr_ bool *hasCurrent)
    {
        boolean bHasCurrent;
        IFR_ASSERT(_pMoIterator->get_HasCurrent(&bHasCurrent));
        *hasCurrent = bHasCurrent ? true : false;
        return S_OK;
    }
    STDMETHOD(MoveNext)(_Outptr_ bool *hasCurrent)
    {
        boolean bHasCurrent;
        IFR_ASSERT(_pMoIterator->MoveNext(&bHasCurrent));
        *hasCurrent = bHasCurrent ? true : false;
        return S_OK;
    }
private:
    ABI::Windows::Foundation::Collections::IIterator<IInspectable *> *_pMoIterator;
};
//------------------------------------------------------------------------------
// Class:   CMoObservableCollectionProxy
//
// Purpose: Implementation of pmod::IObservableCollection
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoObservableCollectionProxy :
    public TBaseObservableObjectAggregationProxy
    <
        TMoObjectProxy
        <
            pmod::IObservableList,
            ABI::Microsoft::PropertyModel::IObservableCollection,
            pmod::IObservableCollection
        >
    >
{
public:
    HRESULT SetSourceInterface(ABI::Microsoft::PropertyModel::IObservableCollection *pMoInterface)
    {
        IFR_ASSERT(__super::SetSourceInterface(pMoInterface));
        IFR_ASSERT(foundation::QueryInterface(pMoInterface,&this->_pInspectableVector));
        return S_OK;
    }
protected:
    CMoObservableCollectionProxy():
        _pInspectableVector(nullptr)
    {
    }
        
    void FinalRelease() override
    {
        RELEASE_INTERFACE(_pInspectableVector);
        __super::FinalRelease();
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == pmod::IObservableCollection::GetIID() )
        {
            *ppInterface = static_cast<pmod::IObservableCollection *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    HRESULT GetMoEventHandler(
        _In_ GUID eventSourceType,
        foundation::IUnknown *pEventHandler,
        ABI::Microsoft::PropertyModel::IDispatchable **ppMoDispatchable) override
    {
        foundation_assert(pEventHandler != nullptr);
        foundation_assert(ppMoDispatchable != nullptr);

        if (eventSourceType == pmod::INotifyCollectionChangedEventSource::GetIID())
        {
            foundation::ComPtr<pmod::INotifyCollectionChangedEventHandler> spNotifyCollectionChangedEventHandler;
            IFR_ASSERT(foundation::QueryInterface(
                pEventHandler,
                spNotifyCollectionChangedEventHandler.GetAddressOf()));

            foundation::ComPtr<ABI::Microsoft::PropertyModel::INotifyCollectionChangedEventHandler> spMoNotifyCollectionChangedEventHandler;
            IFR_ASSERT(CNotifyCollectionChangedEventHandlerProxy::CreateInstance<CNotifyCollectionChangedEventHandlerProxy>(
                spNotifyCollectionChangedEventHandler,
                spMoNotifyCollectionChangedEventHandler.GetAddressOf()
                ));
            IFR_ASSERT(foundation::QueryInterface(
                spMoNotifyCollectionChangedEventHandler,
                ppMoDispatchable));
            return S_OK;
        }
        return __super::GetMoEventHandler(eventSourceType, pEventHandler, ppMoDispatchable);
    }

    // Interface IObservableCollection
    STDMETHOD (GetCount)(UINT32* pCount)
    {
        PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_CollectionModel_GetCount_Id)

        foundation_assert(_pInspectableVector);
        return _pInspectableVector->get_Size(pCount);
    }

    STDMETHOD (GetItem)(UINT32 index,IInspectable* *ppItemValue)
    {
        PERF_CONTEXT(pmod::CoreProxyFactory_PerfId_CollectionModel_GetItem_Id)

        foundation::InspectablePtr spItem;
        IFR_ASSERT(this->m_pSourceInterface->GetItem(index,spItem.GetAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxy(spItem,ppItemValue));
        return S_OK;

    }
    STDMETHOD (GetItems)(_Outptr_ UINT32* size,_Outptr_ IInspectable*** items)
    {
        foundation::InspectableArrayWrapper itemValues;
        IFR_ASSERT(this->m_pSourceInterface->ToArray(itemValues.GetSizeAddressOf(),itemValues.GetBufferAddressOf()));
        IFR_ASSERT(CMoCoreProxyFactory::ToProxyArray(
            itemValues.GetSize(),(IInspectable**)itemValues.GetBuffer(),
            size,items));
        return S_OK;
    }

    STDMETHOD (IndexOf)(_In_ IInspectable *item, _Outptr_ UINT32* pnIndex)
    {
        boolean found;
        return _pInspectableVector->IndexOf(item,pnIndex,&found);
    }

    STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<IInspectable *> **first)
    {
        foundation::ComPtr<ABI::Windows::Foundation::Collections::IIterable<IInspectable *>> spMoIterable;
        IFR_ASSERT(foundation::QueryInterface(this->m_pSourceInterface, spMoIterable.GetAddressOf()));

        foundation::ComPtr<ABI::Windows::Foundation::Collections::IIterator<IInspectable *>> spMoIterator;
        IFR_ASSERT(spMoIterable->First(spMoIterator.GetAddressOf()));
        return CMoObservableCollectionProxyIterator::CreateInstance(spMoIterator, first);
    }

    // Event methods.
    STDMETHOD(GetNotifyCollectionChangedEventSource)(pmod::INotifyCollectionChangedEventSource** ppEventSource)
    {
        if(_spNotifyCollectionChangedEventSource == nullptr)
        {
            HRESULT hr =  CMoNotifyCollectionChangedEventSourceProxy::CreateInstance<CMoNotifyCollectionChangedEventSourceProxy>
                (this->m_pSourceInterface,_spNotifyCollectionChangedEventSource.GetAddressOf());
        }
        return _spNotifyCollectionChangedEventSource.CopyTo(ppEventSource);
    }

    // Interface IObservableList
    STDMETHOD (SetItem)(_In_ UINT32 nIndex,_In_ IInspectable *pItem)
    {
        foundation::InspectablePtr spItem;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxy(pItem,spItem.GetAddressOf()));
        return this->m_pSourceInterface->SetItem(nIndex,spItem);
    }

    STDMETHOD (AppendItem)(_In_ IInspectable *pItem)
    {
        foundation::InspectablePtr spItem;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxy(pItem,spItem.GetAddressOf()));
        return this->m_pSourceInterface->AddItem(spItem);
    }

    STDMETHOD (InsertItem)(_In_ UINT32 nIndex,_In_ IInspectable *pItem)
    {
        foundation::InspectablePtr spItem;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxy(pItem,spItem.GetAddressOf()));
        return this->m_pSourceInterface->InsertItem(nIndex,spItem);
    }

    STDMETHOD (RemoveItem)(_In_ IInspectable *pItem)
    {
        foundation::InspectablePtr spItem;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxy(pItem,spItem.GetAddressOf()));
        return this->m_pSourceInterface->RemoveItem(spItem);
    }

    STDMETHOD (RemoveAt)(_In_ UINT32 nIndex)
    {
        return this->m_pSourceInterface->RemoveItemAt(nIndex);
    }

    STDMETHOD (RemoveAll)()
    {
        return this->m_pSourceInterface->RemoveAll();
    }
private:
    ABI::Windows::Foundation::Collections::IVector<IInspectable *>       *_pInspectableVector;
    foundation::ComPtr<pmod::INotifyCollectionChangedEventSource> _spNotifyCollectionChangedEventSource;
};
