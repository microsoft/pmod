/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandViewModelFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   MoCommandViewModelFactory
//
// Purpose: Factory class for a CollectionViewModel
//
//------------------------------------------------------------------------------
class MoCommandViewModelFactory :
        public MoModelFactoryBase
        <
        ICommandViewModelFactory
        >
{
public:
    // ICommandViewModelFactory
    HRESULT STDMETHODCALLTYPE CreateInstanceWithSource( 
        ICommandBase *pSource,
        CommandOptions modelOptions,
        CommandViewModelOptions viewModelOptions,
        IInspectable *outer,
        IInspectable **inner,
        ICommandViewModel **instance)override;
                        
    HRESULT STDMETHODCALLTYPE CreateInstanceWithSourceAndType( 
        GUID iidType,
        ICommandBase *pSource,
        CommandOptions modelOptions,
        CommandViewModelOptions viewModelOptions,
        IInspectable *outer,
        IInspectable **inner,
        ICommandViewModel **instance) override;
    
};
