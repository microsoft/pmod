/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;
//------------------------------------------------------------------------------
// Class:   MoAsyncOperationFactory
//
// Purpose: Factory class for a AsyncOperation
//
//------------------------------------------------------------------------------
class MoAsyncOperationFactory :
        public MoModelFactoryBase
        <
            IAsyncOperationFactory
        >
{
public:
    // IPropertyResultModelFactory
    HRESULT STDMETHODCALLTYPE CreateInstance( 
        AsyncOperationOptions modelOptions,
        IInspectable *outer,
        IInspectable **inner,
        IAsyncOperation **instance) override;
                        
    HRESULT STDMETHODCALLTYPE CreateInstanceWithType( 
        GUID iidType,
        AsyncOperationOptions modelOptions,
        IInspectable *outer,
        IInspectable **inner,
        IAsyncOperation **instance) override;

};
