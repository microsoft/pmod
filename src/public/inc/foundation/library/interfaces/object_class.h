/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_class.h
****/
#pragma once

#include "foundation_class_base.h"
#include <foundation/interfaces/object.h>
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        BEGIN_DECLARE_INTERFACE(IObjectClass, IFoundationClassBase, FOUNDATION_LIB_API)
            STDMETHOD(SetTypeInfo)(_In_ IObjectTypeInfo *pObjectTypeInfo) = 0;
        END_DECLARE_INTERFACE()
    }
}

