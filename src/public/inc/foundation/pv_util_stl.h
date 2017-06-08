/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pv_util_stl.h
****/
#pragma once

#include <foundation/com_ptr.h>

#include "boxing_util.h"
#include "string_t.h"
#include "exception.h"

namespace foundation
{
    inline PropertyType GetPropertyType(_In_ foundation::IInspectable *pInspectable)
    {
        PropertyType propertyType;
        THROW_IFR(pv_util::GetType(pInspectable, &propertyType));
        return propertyType;
    }

    template <class T>
    inline T getvalue(_In_ foundation::IInspectable *pInspectable)
    {
        T value;
        THROW_IFR(foundation::GetValue(pInspectable,value));
        return value;
    }

    template <>
    inline foundation::string_t getvalue(_In_ foundation::IInspectable *pInspectable)
    {
        foundation::HStringPtr hstr;
        THROW_IFR(pv_util::GetValue(pInspectable,&hstr));
        return foundation::get_string_t(hstr);
    }

    template <class T>
    inline ComPtr<T> getvalue_ptr(_In_ foundation::IInspectable *pInspectable)
    {
        ComPtr<T> spValue;
        if(pInspectable != nullptr)
        {
            THROW_IFR(foundation::QueryInterface(pInspectable,spValue.GetAddressOf()));
        }
        return spValue;
    }

    template <>
    inline ComPtr<foundation::IInspectable> getvalue_ptr(_In_ foundation::IInspectable *pInspectable)
    {
        return InspectablePtr(pInspectable);
    }
}
