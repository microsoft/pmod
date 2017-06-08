/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionAdapter.h
****/

#pragma once
#include "Microsoft.PropertyModel.h"
#include "MoBaseAdapter.h"

#include <pmod\interfaces.h>

using namespace ABI::Microsoft::PropertyModel;


//------------------------------------------------------------------------------
// Class:   CMoObservableCollectionAdapter
//
// Purpose: Implementation of IObservableCollection Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoObservableCollectionAdapter :
    public CMoBaseAdapter
        <
        pmod::IObservableCollection,
        IObservableCollection,
        INotifyCollectionChangedEventHandler,
        INotifyCollectionChangedEventArgs
        >
{
public:

    // Interface IObservableCollection

    HRESULT STDMETHODCALLTYPE GetItem( 
        unsigned int nIndex,
        IInspectable **value) override;
    HRESULT STDMETHODCALLTYPE SetItem( 
        unsigned int nIndex,
        IInspectable *value) override;
    HRESULT STDMETHODCALLTYPE AddItem(IInspectable *item) override;
    HRESULT STDMETHODCALLTYPE InsertItem(int nIndex,IInspectable *item) override;
    HRESULT STDMETHODCALLTYPE RemoveItem(IInspectable *item) override;
    HRESULT STDMETHODCALLTYPE RemoveItemAt(int nIndex) override;

    HRESULT STDMETHODCALLTYPE RemoveAll() override;
    HRESULT STDMETHODCALLTYPE ToArray( 
                            UINT32 *size,
                            IInspectable ***items) override;
    HRESULT STDMETHODCALLTYPE add_NotifyCollectionChanged( 
        INotifyCollectionChangedEventHandler *value,
        EventRegistrationToken *token) override
    {
        return this->add_Handler(value,token);
    }
    HRESULT STDMETHODCALLTYPE remove_NotifyCollectionChanged( 
        EventRegistrationToken token) override
    {
        return this->remove_Handler(token);
    }
};

//------------------------------------------------------------------------------
// Class:   CMoNotifyCollectionChangedEventArgsAdapter
//
// Purpose: Implementation of INotifyCollectionChangedEventArgs Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoNotifyCollectionChangedEventArgsAdapter :
    public foundation::AdapterBase
    <
        pmod::INotifyCollectionChangedEventArgs,
        INotifyCollectionChangedEventArgs,
        &__uuidof(INotifyCollectionChangedEventArgs)
    >
{
public:

    HRESULT STDMETHODCALLTYPE get_Action(NotifyCollectionModelChangedAction *action) override;
                    
    HRESULT STDMETHODCALLTYPE get_NewItems(ABI::Windows::Foundation::Collections::IVectorView<IInspectable *> **value) override;
                    
    HRESULT STDMETHODCALLTYPE get_NewStartingIndex( 
        int *index) override;
                    
    HRESULT STDMETHODCALLTYPE get_OldItems(ABI::Windows::Foundation::Collections::IVectorView<IInspectable *> **value) override;
                    
    HRESULT STDMETHODCALLTYPE get_OldStartingIndex( 
        int *index) override;

};

