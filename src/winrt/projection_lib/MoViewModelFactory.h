/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModelFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   MoPropertyViewModelFactory
//
// Purpose: Factory class for a PropertyViewModel
//
//------------------------------------------------------------------------------
class MoPropertyViewModelFactory :
        public MoModelFactoryBase
        <
            ABI::Microsoft::PropertyModel::Library::IViewModelFactory
        >
{
public:
    // IPropertyViewModelFactory
    HRESULT STDMETHODCALLTYPE CreateInstanceWithSource( 
        IObservableObject *pSource,
        ObservableObjectOptions modelOptions,
        ViewModelOptions viewModelOptions,
        IInspectable *outer,
        IInspectable **inner,
        IViewModel **instance) override;
                        
    HRESULT STDMETHODCALLTYPE CreateInstanceWithSourceAndType( 
        GUID iidType,
        IObservableObject *pSource,
        ObservableObjectOptions modelOptions,
        ViewModelOptions viewModelOptions,
        IInspectable *outer,
        IInspectable **inner,
        IViewModel **instance) override;
    
};
