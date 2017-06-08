/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModelFactoryFactory.h
****/

#pragma once

#include "MoModelFactory.h"

#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   MoViewModelFactoryFactory
//
// Purpose: Factory class for a PropertyViewModelFactory
//
//------------------------------------------------------------------------------
class MoViewModelFactoryFactory :
        public MoModelFactoryStaticsBase
        <
        IViewModelFactoryFactory,
        IViewModelFactoryStatics
        >
{
public:
    // IViewModelFactoryFactory
    HRESULT STDMETHODCALLTYPE CreateInstance( 
                            IInspectable *outer,
                            IInspectable **inner,
                            ABI::Microsoft::PropertyModel::IViewModelFactory **instance) override;
    
};
