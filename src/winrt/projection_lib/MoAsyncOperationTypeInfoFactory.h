/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationTypeInfoFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;

//------------------------------------------------------------------------------
// Class:   MoAsyncOperationModelInfoFactory
//
// Purpose: Factory class for a AsyncOperationModelInfo
//
// History: virivera    2/14/2014     Created
//
//------------------------------------------------------------------------------
class MoAsyncOperationModelInfoFactory :
        public MoModelFactoryStaticsBase
        <
            IAsyncOperationTypeInfoFactory,
            IAsyncOperationTypeInfoStatics
        >
{
public:
    // Interface IAsyncOperationModelInfo
    HRESULT STDMETHODCALLTYPE CreateInstance(
            int typeId,
        GUID iidType,
        HSTRING name,
        PropertyTypeInfo resultTypeInfo,
        IAsyncOperationTypeInfo **instance) override;

    // Interface IAsyncOperationModelInfoStatics
    HRESULT STDMETHODCALLTYPE RegisterType(
        IAsyncOperationTypeInfo *asyncOperationModelInfo) override;

};
