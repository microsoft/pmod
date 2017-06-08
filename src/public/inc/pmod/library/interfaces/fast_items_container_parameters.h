/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:fast_items_container_parameters.h
****/
#pragma once

#include <foundation/library/parameter_base.h>
#include "observable_collection_class.h"

namespace pmod {

    namespace library {

        enum class FastItemsContainerOptions
        {
            None = 0x00,
            IsDelegateWeakRef = 0x10
        };

        struct FastItemsContainerParameters :
            public foundation::library::_CreateParameterBase
            <
            IFastItemsContainerDelegate,
            FastItemsContainerOptions
            >
        {
            FastItemsContainerParameters() :
            _propertyType(foundation::PropertyType_Empty),
            _initialSize(0),
            _slotPrefixSize(0),
            _itemCapacitySlotSize(4)
            {
            }

            foundation::PropertyType _propertyType;
            UINT32                  _initialSize;
            UINT32                  _slotPrefixSize;
            UINT32                  _itemCapacitySlotSize;
        };
    }

}

