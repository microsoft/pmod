/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCommandInfoFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;


//------------------------------------------------------------------------------
// Class:   MoCommandInfoFactory
//
// Purpose: Factory class for a CommandTypeInfo
//
//------------------------------------------------------------------------------
class MoCommandInfoFactory :
        public MoModelFactoryStaticsBase
        <
            ICommandInfoFactory,
            ICommandInfoStatics
        >
{
public:
    // Interface ICommandModelInfoFactory
    HRESULT STDMETHODCALLTYPE CreateInstance(
            int typeId,
        GUID iidType,
        HSTRING name,
        PropertyTypeInfo parameterTypeInfo,
        PropertyTypeInfo resultTypeInfo,
        boolean isAsync,
        ICommandInfo **instance) override;

    // Interface ICommandModelInfoStatics
    HRESULT STDMETHODCALLTYPE RegisterType(
        ICommandInfo *commandModelInfo) override;

};
