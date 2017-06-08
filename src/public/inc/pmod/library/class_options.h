/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:class_options.h
****/
#pragma once

#include <foundation/pal.h>

namespace pmod {

    namespace library {
        // Pmod Options
        const UINT32 CommonOptions_NotifyInModelChanges = 0x20000;
        const UINT32 CommonOptions_UseHierarchyHostAdapterFactory = 0x40000;
        const UINT32 CommonOptions_VerifyTypeInfoInternal = 0x80000;
    }
}
