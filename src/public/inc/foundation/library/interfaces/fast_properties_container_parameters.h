/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:fast_properties_container_parameters.h
****/
#pragma once

#include <foundation/library/parameter_base.h>
#include <foundation/interfaces/object_dispatch_info.h>
#include "fast_properties_container.h"

namespace foundation {

    enum class FastPropertiesContainerOptions
    {
        None = 0x00,
        IsDelegateWeakRef = 0x10
    };

    namespace library {

        struct FastPropertiesContainerParameters :
            public _CreateParameterBase
            <
            IFastPropertiesContainerDelegate,
            FastPropertiesContainerOptions
            >
        {
            FastPropertiesContainerParameters() :
				_pPropertyDataInfo(nullptr),
                _slotPrefixSize(0)
            {
            }
            IObjectDispatchInfo      *_pPropertyDataInfo;
            UINT32                  _slotPrefixSize;
        };
    }

}

