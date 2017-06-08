/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEnumTypeInfoFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;


//------------------------------------------------------------------------------
// Class:   MoEnumTypeInfoFactory
//
// Purpose: Factory class for a CommandTypeInfo
//
//------------------------------------------------------------------------------
class MoEnumTypeInfoFactory :
        public MoModelFactoryStaticsBase
        <
            IEnumTypeInfoFactory,
            IEnumTypeInfoStatics
        >
{
public:
    // Interface IEnumTypeInfoFactory
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(
        int typeId,
        GUID iidType,
        HSTRING name,
        boolean isflags,
        UINT32 length_names,
        HSTRING *names,
        UINT32 length_values,
        UINT32 *values,
        IEnumTypeInfo **instance) override;

    // Interface ICommandModelInfoStatics
    HRESULT STDMETHODCALLTYPE RegisterType(
        IEnumTypeInfo *pEnumTypeInfo) override;

};
