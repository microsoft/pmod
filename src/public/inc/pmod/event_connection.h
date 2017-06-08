/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:event_connection.h
****/
#pragma once

#include<foundation/macros.h>

#include "interfaces.h"
#include <foundation/library/library_interfaces.h>

#include "eventhandler_util.h"

namespace pmod
{
//------------------------------------------------------------------------------
// Declare Strong Reference typedef for our 'Core' models
typedef 
    foundation::SingleSourceEventHandlerConnection
    <
    IObservableObject,
    IPropertyChangedEventHandler,
    IPropertyChangedEventArgs,
    ObservableObjectListenerBase
    > PropertyChangedConnection;

typedef 
    foundation::SingleSourceEventHandlerConnection
    <
    IObservableObject,
    IEventModelEventHandler,
    IEventModelEventArgs,
    EventModelListenerBase
    > EventModelConnection;

typedef
    foundation::SingleSourceEventHandlerConnection
    <
    ICommand,
    ICanExecuteChangedEventHandler,
    ICanExecuteChangedEventArgs,
    CommandListenerBase
    > CanExecuteConnection;

typedef foundation::SingleSourceEventHandlerConnection<
    IObservableCollection,
    INotifyCollectionChangedEventHandler,
    INotifyCollectionChangedEventArgs,
    ObservableCollectionListenerBase
> NotifyCollectionChangedConnection;

typedef
foundation::SingleSourceEventHandlerConnection
    <
    IBindingBase,
    IBindingValueChangedEventHandler,
    IBindingValueChangedEventArgs,
    PropertyBindingListener
    > BindingValueChangedConnection;

//------------------------------------------------------------------------------
// Declare Weak Reference typedef for our 'Core' models
typedef 
    foundation::SingleSourceEventHandlerConnection
    <
    IObservableObject,
    IPropertyChangedEventHandler,
    IPropertyChangedEventArgs,
    ObservableObjectListenerBase,
    foundation::WeakReferenceHolder<IObservableObject>
    > WeakRefPropertyChangedConnection;

//------------------------------------------------------------------------------
// Declare MultiSource Reference typedef for our 'Core' models
typedef 
    foundation::MultiSourceEventHandlerConnection
    <
    IObservableObject,
    IPropertyChangedEventHandler,
    IPropertyChangedEventArgs,
    ObservableObjectListenerBase
    > MultiSourcePropertyChangedConnection;

typedef 
    foundation::MultiSourceEventHandlerConnection
    <
    IObservableObject,
    IEventModelEventHandler,
    IEventModelEventArgs,
    EventModelListenerBase
    > MultiSourceEventModelConnection;

typedef
    foundation::MultiSourceEventHandlerConnection
    <
    ICommand,
    ICanExecuteChangedEventHandler,
    ICanExecuteChangedEventArgs,
    CommandListenerBase
    > MultiSourceCanExecuteConnection;

typedef foundation::MultiSourceEventHandlerConnection<
    IObservableCollection,
    INotifyCollectionChangedEventHandler,
    INotifyCollectionChangedEventArgs,
    ObservableCollectionListenerBase
    > MultiSourceNotifyCollectionChangedConnection;


typedef foundation::MultiSourceEventHandlerConnection
    <
    IBindingBase,
    IBindingValueChangedEventHandler,
    IBindingValueChangedEventArgs,
    PropertyBindingListener
    > MultiSourceBindingValueChangedConnection;

//------------------------------------------------------------------------------
// Declare Sink Strong Reference templates for our 'Core' models
template <typename TSink>
class SinkPropertyChangedConnection :
    public foundation::SinkEventHandlerConnection<PropertyChangedConnection, TSink>
{
public:
    SinkPropertyChangedConnection(
        TSink *pSink,
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, IPropertyChangedEventArgs *)
        ) :
        foundation::SinkEventHandlerConnection<PropertyChangedConnection, TSink>(pSink, pMethod)
    {
    }
};

template <typename TSink>
class SinkEventModelConnection :
    public foundation::SinkEventHandlerConnection<EventModelConnection, TSink>
{
public:
    SinkEventModelConnection(
        TSink *pSink,
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, IEventModelEventArgs *)
        ) :
        foundation::SinkEventHandlerConnection<EventModelConnection, TSink>(pSink, pMethod)
    {
    }
};

template <typename TSink>
class SinkNotifyCollectionChangedConnection :
    public foundation::SinkEventHandlerConnection<NotifyCollectionChangedConnection, TSink>
{
public:
    SinkNotifyCollectionChangedConnection(
        TSink *pSink,
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, INotifyCollectionChangedEventArgs *)
        ) :
        foundation::SinkEventHandlerConnection<NotifyCollectionChangedConnection, TSink>(pSink, pMethod)
    {
    }
};

template <typename TSink>
class SinkCanExecuteConnection :
    public foundation::SinkEventHandlerConnection<CanExecuteConnection, TSink>
{
public:
    SinkCanExecuteConnection(
        TSink *pSink,
        HRESULT(TSink::*pMethod)(foundation::IUnknown *, ICanExecuteChangedEventArgs *)
        ) :
        foundation::SinkEventHandlerConnection<CanExecuteConnection, TSink>(pSink, pMethod)
    {
    }
};

//------------------------------------------------------------------------------
// Class:   NotifyCollectionChangedHelper
//
// Purpose: Helper class to process Collection Model Events
//
//------------------------------------------------------------------------------
class NotifyCollectionChangedHelper
{
protected:
    // Typically invoked directly from an Event Handler Callback
    HRESULT InvokeInternal(_In_ foundation::IUnknown* pSender, _In_ INotifyCollectionChangedEventArgs* pArgs)
    {
        foundation::ComPtr<IObservableCollection>  spCollectionModel;

        IFCPTREx_ASSERT(pSender, E_INVALIDARG);
        IFCPTREx_ASSERT(pArgs, E_INVALIDARG);

        IFR_ASSERT(foundation::QueryInterface(pSender,spCollectionModel.GetAddressOf()));
        return HandleCollectionChanged(spCollectionModel,pArgs);
    }

    HRESULT HandleCollectionChanged(
        _In_ IObservableCollection* pObservableCollection,
        _In_ INotifyCollectionChangedEventArgs* pArgs)
    {
        foundation_assert(pObservableCollection);
        foundation_assert(pArgs);

        foundation::NotifyCollectionModelChangedAction   action;
        IFR_ASSERT(pArgs->GetAction(&action));

        if (action == foundation::NotifyCollectionModelChangedAction::ItemAdded)
        {
            foundation::InspectableArrayWrapper newItems;

            UINT32 nOldStartIndex = -1;
            UINT32 nNewStartIndex = -1;

            IFR_ASSERT(pArgs->GetNewItems(newItems.GetSizeAddressOf(),newItems.GetBufferAddressOf()));

            IFR_ASSERT(pArgs->GetOldStartingIndex(&nOldStartIndex));
            IFR_ASSERT(pArgs->GetNewStartingIndex(&nNewStartIndex));

            bool fItemMove = (nOldStartIndex != (UINT32)-1);
            UINT32 nCount = newItems.GetSize();
            for (UINT32 index = 0; index < nCount; index++)
            {
                IFR_ASSERT(OnCollectionItemAdd(
                    pObservableCollection,
                    newItems[index],
                    nNewStartIndex + index,
                    fItemMove));
            }
        }
        else if (action == foundation::NotifyCollectionModelChangedAction::ItemRemoved)
        {
            foundation::InspectableArrayWrapper oldItems;
            UINT32 nOldStartIndex = -1;
            UINT32 nNewStartIndex = -1;

            IFR_ASSERT(pArgs->GetOldItems(oldItems.GetSizeAddressOf(),oldItems.GetBufferAddressOf()));

            IFR_ASSERT(pArgs->GetOldStartingIndex(&nOldStartIndex));
            IFR_ASSERT(pArgs->GetNewStartingIndex(&nNewStartIndex));

            bool fItemMove = (nNewStartIndex != (UINT32)-1);
            UINT32 nCount = oldItems.GetSize();
            for (UINT32 index = 0; index < nCount; index++)
            {
                IFR_ASSERT(OnCollectionItemRemove(
                    pObservableCollection,
                    oldItems[index],
                    nOldStartIndex + index,
                    fItemMove));
            }
        }
        else if (action == foundation::NotifyCollectionModelChangedAction::ItemReplaced)
        {
            foundation::InspectableArrayWrapper oldItems;
            foundation::InspectableArrayWrapper newItems;
            UINT32 nStartIndex = -1;

            IFR_ASSERT(pArgs->GetOldItems(oldItems.GetSizeAddressOf(),oldItems.GetBufferAddressOf()));
            IFR_ASSERT(pArgs->GetNewItems(newItems.GetSizeAddressOf(),newItems.GetBufferAddressOf()));

            IFR_ASSERT(pArgs->GetNewStartingIndex(&nStartIndex));


            UINT32 nCount = newItems.GetSize();
            foundation_assert(oldItems.GetSize() == nCount);
            for (UINT32 index = 0; index < nCount; index++)
            {
                IFR_ASSERT(OnCollectionItemReplaced(
                        pObservableCollection,
                        oldItems[index],
                        newItems[index],
                        nStartIndex + index));
            }
        }
        else if (action == foundation::NotifyCollectionModelChangedAction::Reset)
        {
            foundation::InspectableArrayWrapper oldItems;
            foundation::InspectableArrayWrapper newItems;

            IFR_ASSERT(pArgs->GetOldItems(oldItems.GetSizeAddressOf(),oldItems.GetBufferAddressOf()));
            IFR_ASSERT(pArgs->GetNewItems(newItems.GetSizeAddressOf(),newItems.GetBufferAddressOf()));

            IFR_ASSERT(OnCollectionReset(pObservableCollection, oldItems,newItems));
        }

        return S_OK;
    }

protected:
    virtual HRESULT OnCollectionItemAdd(
            _In_ IObservableCollection* pSender,
            _In_ foundation::IInspectable *item,
            _In_ UINT32 nNewStartIndex,
            _In_ bool fItemMove) = 0;

    virtual HRESULT OnCollectionItemRemove(
            _In_ IObservableCollection* pSender,
            _In_ foundation::IInspectable *item,
            _In_ UINT32 nOldStartIndex,
            _In_ bool fItemMove) = 0;

    virtual HRESULT OnCollectionReset(
        _In_ IObservableCollection* pSender,
        _In_ const foundation::InspectableArrayWrapper& oldItems,
        _In_ const foundation::InspectableArrayWrapper& newItems) = 0;

    virtual HRESULT OnCollectionItemReplaced(
        _In_ IObservableCollection* pSender,
        _In_ foundation::IInspectable *oldItem,
        _In_ foundation::IInspectable *newItem,
        _In_ UINT32 nStartIndex) = 0;
};

}

