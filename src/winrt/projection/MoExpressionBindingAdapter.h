/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoExpressionBindingAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

// Public Headers
#include <foundation/library/adapter_base.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoExpressionBindingAdapter
//
// Purpose: Implementation of IExpressionBinding Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoExpressionBindingAdapter :
    public foundation::AdapterBase
    <
        pmod::IBindingBase,
        IExpressionBinding,
        &__uuidof(IExpressionBinding)
    >
{
public:
    // Interface IExpressionBinding
};
