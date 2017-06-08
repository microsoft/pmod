/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelOverridesBaseAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.Library.h"

// Public Headers
#include <foundation/library/adapter_base.h>

using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   CMoObjectOverridesBaseAdapter
//
// Purpose: Implementation of IObjectOverridesBase Interface
//
//------------------------------------------------------------------------------
class CMoObjectOverridesBaseAdapter :
    public foundation::AdapterBase
        <
        foundation::IObject,
        IObjectOverridesBase,
        &__uuidof(IObjectOverridesBase)
        >
{
public:                        
    HRESULT STDMETHODCALLTYPE OnRuntimeInitialize( void) override
    {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnStartObserve( void) override
    {
        return S_OK;
    }
                        
    HRESULT STDMETHODCALLTYPE OnStopObserve( void) override
    {
        return S_OK;
    }
};
