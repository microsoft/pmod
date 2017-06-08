/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionInfoFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"

using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;


//------------------------------------------------------------------------------
// Class:   MoObservableCollectionInfoFactory
//
// Purpose: Factory class for a CommandTypeInfo
//
//------------------------------------------------------------------------------
class MoObservableCollectionInfoFactory :
        public MoModelFactoryStaticsBase
        <
            IObservableCollectionInfoFactory,
            IObservableCollectionInfoStatics
        >
{
public:
    // Interface ICollectionModelInfoFactory
    HRESULT STDMETHODCALLTYPE CreateInstance(
        int typeId,
        GUID iidType,
        HSTRING name,
        PropertyTypeInfo itemTypeInfo,
        IEnumerableTypeInfo **instance) override;

    // Interface ICollectionModelInfoStatics
    HRESULT STDMETHODCALLTYPE RegisterType(
        IEnumerableTypeInfo *collectionModelInfo) override;

};
