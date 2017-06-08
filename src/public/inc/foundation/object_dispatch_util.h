/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_dispatch_util.h
****/
#pragma once

#include "interfaces/object_dispatch.h"
#include "boxing_util.h"

#include <vector>
namespace foundation
{
    // Create a std::vector<UINT32> from a property ptr
    static void CreateProperties(
        std::vector<UINT32>& properties,
        _In_ UINT32 *pProperties)
    {
        UINT32 propertyId;
        while ((propertyId = *pProperties) != (UINT32)-1)
        {
            properties.push_back(propertyId);
            ++pProperties;
        }
    }
    // Create a std::vector<UINT32> from a propertyId marker
    static void CreateProperties(
        std::vector<UINT32>& properties,
        _In_ UINT32 propertyIdStart,
        _In_ va_list marker)
    {

        UINT32 propertyId = propertyIdStart;
        while (propertyId != (UINT32)-1)
        {
            properties.push_back(propertyId);
            // go to next property id
            propertyId = va_arg(marker, int);
        }
    }

    // Create a property path from a marker
    __unused_attribute__
    static InspectablePtr CreatePropertyPath(
        _In_ UINT32 propertyIdStart, 
        _In_ va_list marker)
    {
        std::vector<UINT32> propertyPath;
        CreateProperties(propertyPath, propertyIdStart, marker);
        return CreateValueFromVector(propertyPath);
    }

}
