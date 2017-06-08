/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceCollectionViewModelAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;


//------------------------------------------------------------------------------
// Class:   CMoMultiSourceCollectionViewModelAdapter
//
// Purpose: Implementation of IMultiSourceCollectionViewModel Interface
//
//------------------------------------------------------------------------------
class CMoMultiSourceCollectionViewModelAdapter :
    public foundation::AdapterBase
    <
        pmod::library::IObservableCollectionClass,
        IMultiSourceCollectionViewModel,
        &__uuidof(IMultiSourceCollectionViewModel)
    >
{
public:
};
//------------------------------------------------------------------------------
// Class:   CMoMultiSourceCollectionViewModelOverridesAdapter
//
// Purpose: Implementation of IMultiSourceCollectionViewModelOverrides Interface
//
//------------------------------------------------------------------------------
class CMoMultiSourceCollectionViewModelOverridesAdapter :
    public foundation::AdapterBase
        <
        pmod::library::IObservableCollectionClass,
        IMultiSourceCollectionViewModelOverrides,
        &__uuidof(IMultiSourceCollectionViewModelOverrides)
        >
{
public:
    HRESULT STDMETHODCALLTYPE OnMultiSourceCollectionChanged( 
         IObservableCollection *pSource,
         INotifyCollectionChangedEventArgs *pEventArgs)
    {
        UNREFERENCED_PARAMETER(pSource);
        UNREFERENCED_PARAMETER(pEventArgs);
        return S_OK;
    }
};

