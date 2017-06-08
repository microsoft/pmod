/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionClassAdapter.h
****/

#pragma once


#include "Microsoft.PropertyModel.Library.h"

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;




//------------------------------------------------------------------------------
// Class:   CMoObservableCollectionClassAdapter
//
// Purpose: Implementation of IObservableCollectionClass Interface
//
//------------------------------------------------------------------------------
class CMoObservableCollectionClassAdapter :
    public foundation::AdapterBase<
        pmod::library::IObservableCollectionClass,
        IObservableCollectionClass,
        &__uuidof(IObservableCollectionClass)
        >
{
public:
    // Interface IObservableCollectionClass
    HRESULT STDMETHODCALLTYPE AddItemInternal( 
        IInspectable *item) override;

    HRESULT STDMETHODCALLTYPE InsertItemInternal( 
        int nIndex,
        IInspectable *item) override;
                        
    HRESULT STDMETHODCALLTYPE ReplaceItemInternal( 
        int nIndex,
        IInspectable *item) override;

    HRESULT STDMETHODCALLTYPE InsertItems( 
        int nIndex,
        UINT32 size,
        IInspectable **items) override;
                        
    HRESULT STDMETHODCALLTYPE ReplaceItems( 
        int nIndex,
        UINT32 size,
        IInspectable **items) override;
                       
    HRESULT STDMETHODCALLTYPE RemoveItems( 
        int nIndex,
        UINT32 count) override;

};
//------------------------------------------------------------------------------
// Class:   CMoObservableCollectionProtectedAdapter
//
// Purpose: Implementation of ICollectionModelProtected Interface
//
//------------------------------------------------------------------------------
class CMoObservableCollectionProtectedAdapter :
    public foundation::AdapterBase<
    pmod::library::IObservableCollectionClass,
    IObservableCollectionProtected,
    &__uuidof(IObservableCollectionProtected)
    >
{
public:
    // Interface ICollectionModelProtected
};
//------------------------------------------------------------------------------
// Class:   CMoObservableCollectionBaseOverridesAdapter
//
// Purpose: Implementation of ICollectionModelBaseOverrides Interface
//
//------------------------------------------------------------------------------
class CMoObservableCollectionBaseOverridesAdapter :
    public foundation::AdapterBase<
        pmod::library::IObservableCollectionClass,
        IObservableCollectionBaseOverrides,
        &__uuidof(IObservableCollectionBaseOverrides)
        >
{
public:                        
    HRESULT STDMETHODCALLTYPE OnCollectionChanged(INotifyCollectionChangedEventArgs *pEventArgs) override
    {
        UNREFERENCED_PARAMETER(pEventArgs);
        return S_OK;
    }
};
//------------------------------------------------------------------------------
// Class:   CMoCollectionModelOverridesAdapter
//
// Purpose: Implementation of CollectionModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoCollectionModelOverridesAdapter :
    public foundation::AdapterBase<
        pmod::library::IObservableCollectionClass,
        IObservableCollectionOverrides,
        &__uuidof(IObservableCollectionOverrides)
        >
{
public:
    // Interface ICollectionModelOverrides

};


