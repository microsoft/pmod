/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoLogRecordFactory.h
****/

#pragma once

#include <pmod/library/model_library.h>

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoLogRecordFactory
//
// Purpose: Factory class for a PropertyModel
//
//------------------------------------------------------------------------------
class MoLogRecordFactory :
        public MoModelFactoryBase<ILogRecordFactory>
{
public:
    // ILogRecordFactory
    HRESULT STDMETHODCALLTYPE CreateInstance( 
        LoggingLevel level,
        IInspectable *pMessage,
        ILogCategory *pLogCategory,
        UINT32 length,
        ILogParameter **logParameters,
        ILogRecord **instance) override;

};
