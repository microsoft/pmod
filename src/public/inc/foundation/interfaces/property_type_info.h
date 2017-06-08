/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:property_type_info.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include "object.h"

namespace foundation {

    BEGIN_DECLARE_INTERFACE(IPropertyTypeInfo, IInspectable, FOUNDATION_API)
        STDMETHOD(GetPropertyType)(_Out_ PropertyType *propertyType) = 0;
        STDMETHOD(GetTypeInfo)(_COM_Outptr_result_maybenull_ IObjectTypeInfo** ppModelTypeInfo) = 0;
    END_DECLARE_INTERFACE()

}

