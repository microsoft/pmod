/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoLogParameterFactory.h
****/

#pragma once

#include <pmod/library/model_library.h>

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoLogParameterFactory
//
// Purpose: Factory class for a PropertyModel
//
//------------------------------------------------------------------------------
class MoLogParameterFactory :
        public MoModelFactoryBase<ILogParameterFactory>
{
public:
    // ILogParameterFactory
    HRESULT STDMETHODCALLTYPE CreateInstance( 
        IInspectable *pProperty,
        IInspectable *pValue,
        ILogParameter **instance) override;

};
