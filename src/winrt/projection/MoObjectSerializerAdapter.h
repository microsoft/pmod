/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObjectSerializerAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include <foundation/library/adapter_base.h>
#include <foundation/interfaces/object_serializer.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoObjectSerializerAdapter
//
// Purpose: Implementation of IObjectSerializer Interface
//
//------------------------------------------------------------------------------
class CMoObjectSerializerAdapter :
    public foundation::AdapterBase
        <
        foundation::IObjectSerializer,
        IObjectSerializer,
        &__uuidof(IObjectSerializer)
        >
{
public:
    // Interface IObjectSerializer
    HRESULT STDMETHODCALLTYPE GetContent(
        HSTRING serializerActivationId,
        IInspectable *pContentOptions,
        IInspectable **ppSerializedContent) override
    {
        return m_pInner->GetContent(serializerActivationId, pContentOptions, ppSerializedContent);
    }

    HRESULT STDMETHODCALLTYPE SetContent(
        HSTRING serializerActivationId,
        IInspectable *pSerializedContent) override
    {
        return m_pInner->SetContent(serializerActivationId, pSerializedContent);
    }

};
