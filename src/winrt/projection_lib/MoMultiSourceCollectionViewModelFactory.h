/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceCollectionViewModelFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   MoMultiSourceCollectionViewModelFactory
//
// Purpose: Factory class for a CollectionViewModel
//
//------------------------------------------------------------------------------
class MoMultiSourceCollectionViewModelFactory :
        public MoModelFactoryBase
        <
        IMultiSourceCollectionViewModelFactory
        >
{
public:
    // IMultiSourceCollectionViewModelFactory
    HRESULT STDMETHODCALLTYPE CreateInstance( 
        ObservableCollectionOptions modelOptions,
        CollectionViewModelOptions viewModelOptions,
        IInspectable *outer,
        IInspectable **inner,
        IMultiSourceCollectionViewModel **instance) override;
                        
    HRESULT STDMETHODCALLTYPE CreateInstanceWithType( 
        GUID iidType,
        ObservableCollectionOptions modelOptions,
        CollectionViewModelOptions viewModelOptions,
        IInspectable *outer,
        IInspectable **inner,
        IMultiSourceCollectionViewModel **instance)  override;
    
};
