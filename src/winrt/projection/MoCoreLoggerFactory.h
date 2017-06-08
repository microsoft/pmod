/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreLoggerFactory.h
****/

#pragma once

#include <pmod/library/model_library.h>

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoLoggerFactory
//
// Purpose: Factory class for a PropertyModel
//
//------------------------------------------------------------------------------
class MoLoggerFactory :
        public MoModelFactoryBase<ILoggerFactory>
{
public:
    // ICoreLoggerFactory
    HRESULT STDMETHODCALLTYPE CreateInstance(
        UINT32 rootCategoryTypeId,
        LoggerOptions options,
        ILogger **instance) override;

};
