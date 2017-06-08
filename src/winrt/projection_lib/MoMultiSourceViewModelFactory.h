/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceViewModelFactory.h
****/

#pragma once

#include "MoModelFactory.h"

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   MoMultiSourcePropertyViewModelFactory
//
// Purpose: Factory class for a MultiSourcePropertyViewModel
//
//------------------------------------------------------------------------------
class MoMultiSourcePropertyViewModelFactory :
        public MoModelFactoryBase
        <
        IMultiSourceViewModelFactory
        >
{
public:
    // IMultiSourcePropertyViewModelFactory
   HRESULT STDMETHODCALLTYPE CreateInstance( 
        ObservableObjectOptions modelOptions,
        IInspectable *outer,
        IInspectable **inner,
        IMultiSourceViewModel **instance) override;
                        
    HRESULT STDMETHODCALLTYPE CreateInstanceWithType( 
        GUID iidType,
        ObservableObjectOptions modelOptions,
        IInspectable *outer,
        IInspectable **inner,
        IMultiSourceViewModel **instance)  override;
    
};
