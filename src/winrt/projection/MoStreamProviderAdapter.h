/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoStreamProviderAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include <foundation/library/adapter_base.h>
#include <foundation/interfaces/stream_provider.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoStreamProviderAdapter
//
// Purpose: Implementation of IStreamProvider Interface
//
//------------------------------------------------------------------------------
class CMoStreamProviderAdapter :
    public foundation::AdapterBase
        <
        foundation::IStreamProvider,
        IStreamProvider,
        &__uuidof(IStreamProvider)
        >
{
public:
    // Interface IStreamProvider
    virtual HRESULT STDMETHODCALLTYPE Serialize(
        IInspectable *pValue,
        IInspectable **ppValue) override
    {
        return m_pInner->Serialize(pValue, ppValue);
    }

    virtual HRESULT STDMETHODCALLTYPE Deserialize(
       IInspectable *pValue,
       IInspectable **ppValue) override
    {
        return m_pInner->Deserialize(pValue, ppValue);
    }
};
