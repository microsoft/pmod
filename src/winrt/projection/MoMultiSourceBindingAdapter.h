/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoMultiSourceBindingAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

// Public Headers
#include <foundation/library/adapter_base.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   CMoMultiSourceBindingAdapter
//
// Purpose: Implementation of IMultiSourceBinding Interface
//
//------------------------------------------------------------------------------
class CMoMultiSourceBindingAdapter :
    public foundation::AdapterBase
    <
    pmod::IMultiSourceBinding,
        IMultiSourceBinding,
        &__uuidof(IMultiSourceBinding)
    >
{
public:
};

//------------------------------------------------------------------------------
// Class:   CMoMultiSourceBindingProtected
//
// Purpose: Implementation of IMultiSourceBindingProtected Interface
//
//------------------------------------------------------------------------------
class CMoMultiSourceBindingProtectedAdapter :
    public foundation::AdapterBase
    <
        pmod::library::IMultiSourceBindingClass,
        IMultiSourceBindingProtected,
        &__uuidof(IMultiSourceBindingProtected)
    >
{
public:
};

