/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceBindingFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoMultiSourceBindingFactory
//
// Purpose: Factory class for a MultiSourceBinding
//
//------------------------------------------------------------------------------
class MoMultiSourceBindingFactory :
        public MoModelFactoryBase<IMultiSourceBindingFactory>
{
public:
    // IMultiSourceBindingFactory
    HRESULT STDMETHODCALLTYPE CreateInstanceWithSource( 
        MultiSourceBindingOptions options,
        IMultiSourceBinding **instance) override;

};
