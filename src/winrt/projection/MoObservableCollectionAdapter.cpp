/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionAdapter.cpp
****/

#include "pch.h"

#include "MoObservableCollectionAdapter.h"
#include "VectorView.h"
#include <foundation/array_wrapper.h>

static HRESULT CreateVectorView(UINT32 size,IInspectable **pInspectableArray,ABI::Windows::Foundation::Collections::IVectorView<IInspectable *> **value)
{
    utils::View<IInspectable *> *pViewInstance = nullptr;

    IFR(foundation::ctl::ComInspectableObject<utils::View<IInspectable *>>::CreateInstance(&pViewInstance));
    IFR(pViewInstance->SetView(pInspectableArray,size));

    *value = pViewInstance;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMoObservableCollectionAdapter::GetItem( 
    unsigned int nIndex,
    IInspectable **value)
{
    return m_pInner->GetItem((UINT32)nIndex,value);
}
                    
HRESULT STDMETHODCALLTYPE CMoObservableCollectionAdapter::SetItem( 
    unsigned int nIndex,
    IInspectable *value)
{
    foundation::ComPtr<pmod::IObservableList> spListModel;
    IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));
    return spListModel->SetItem((UINT32)nIndex,value);
}

HRESULT STDMETHODCALLTYPE CMoObservableCollectionAdapter::AddItem(IInspectable *item)
{
    foundation::ComPtr<pmod::IObservableList> spListModel;
    IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));
    return spListModel->AppendItem(item);
}

HRESULT STDMETHODCALLTYPE CMoObservableCollectionAdapter::InsertItem(int nIndex,IInspectable *item)
{
    foundation::ComPtr<pmod::IObservableList> spListModel;
    IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));
    return spListModel->InsertItem((UINT32)nIndex,item);
}

HRESULT STDMETHODCALLTYPE CMoObservableCollectionAdapter::RemoveItem(IInspectable *item)
{
    foundation::ComPtr<pmod::IObservableList> spListModel;
    IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));
    return spListModel->RemoveItem(item);
}

HRESULT STDMETHODCALLTYPE CMoObservableCollectionAdapter::RemoveItemAt(int nIndex)
{
    foundation::ComPtr<pmod::IObservableList> spListModel;
    IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));
    return spListModel->RemoveAt((UINT32)nIndex);
}

HRESULT STDMETHODCALLTYPE CMoObservableCollectionAdapter::RemoveAll()
{
    foundation::ComPtr<pmod::IObservableList> spListModel;
    IFR(foundation::QueryInterface(m_pInner,spListModel.GetAddressOf()));
    return spListModel->RemoveAll();
}

HRESULT STDMETHODCALLTYPE CMoObservableCollectionAdapter::ToArray( 
                        UINT32 *size,
                        IInspectable ***items)
{
    return m_pInner->GetItems(size,items);
}

HRESULT STDMETHODCALLTYPE CMoNotifyCollectionChangedEventArgsAdapter::get_Action( 
    NotifyCollectionModelChangedAction *action)
{
    foundation::NotifyCollectionModelChangedAction _action;
    IFR(m_pInner->GetAction(&_action));
    *action = (NotifyCollectionModelChangedAction)_action;
    return S_OK;
}
                    
HRESULT STDMETHODCALLTYPE CMoNotifyCollectionChangedEventArgsAdapter::get_NewItems(ABI::Windows::Foundation::Collections::IVectorView<IInspectable *> **value)
{
    foundation::InspectableArrayWrapper newItems;
    IFR(m_pInner->GetNewItems(newItems.GetSizeAddressOf(),newItems.GetBufferAddressOf()));
    return CreateVectorView(newItems.GetSize(),(IInspectable **) newItems.GetBuffer(),value);
}
                    
HRESULT STDMETHODCALLTYPE CMoNotifyCollectionChangedEventArgsAdapter::get_NewStartingIndex( 
    int *index)
{
    UINT32 _index;
    m_pInner->GetNewStartingIndex(&_index);
     *index = (int)_index;
     return S_OK;
}
                    
HRESULT STDMETHODCALLTYPE CMoNotifyCollectionChangedEventArgsAdapter::get_OldItems(ABI::Windows::Foundation::Collections::IVectorView<IInspectable *> **value)
{
    foundation::InspectableArrayWrapper oldItems;
    IFR(m_pInner->GetOldItems(oldItems.GetSizeAddressOf(),oldItems.GetBufferAddressOf()));
    return CreateVectorView(oldItems.GetSize(),(IInspectable **)oldItems.GetBuffer(),value);
}
                    
HRESULT STDMETHODCALLTYPE CMoNotifyCollectionChangedEventArgsAdapter::get_OldStartingIndex( 
    int *index)
{
    UINT32 _index;
    m_pInner->GetOldStartingIndex(&_index);
    *index = (int)_index;
    return S_OK;
}

