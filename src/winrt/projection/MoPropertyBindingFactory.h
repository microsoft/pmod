/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoPropertyBindingFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoPropertyBindingFactory
//
// Purpose: Factory class for a PropertyBinding
//
//------------------------------------------------------------------------------
class MoPropertyBindingFactory :
        public MoModelFactoryStaticsBase
        <
        IPropertyBindingFactory,
        IPropertyBindingStatics
        >
{
public:
    // IPropertyBindingFactory
    HRESULT STDMETHODCALLTYPE CreateInstanceWithSource( 
        IObservableObject *pSource,
        IInspectable *path,
        PropertyBindingOptions options,
        IPropertyBinding **instance) override;
    
};
