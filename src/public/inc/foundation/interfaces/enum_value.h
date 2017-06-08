/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:enum_value.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>

#include "object.h"

namespace foundation {

    struct IEnumValue;
    BEGIN_DECLARE_INTERFACE(IEnumTypeInfo, IObjectTypeInfo, FOUNDATION_API)
        STDMETHOD(GetIsFlags)(bool *value) = 0;
        STDMETHOD(GetNames)(UINT32 *__valueSize, HSTRING **values) = 0;
        STDMETHOD(GetValues)(UINT32 *__valueSize, UINT32 **values) = 0;
        STDMETHOD(GetDefaultValue)(UINT32 *enumValue) = 0;
        STDMETHOD(GetStringValue)(UINT32 enumValue, HSTRING *value) = 0;
        STDMETHOD(IsEnumValid)(UINT32 enumValue, bool *value) = 0;
        STDMETHOD(GetEnumValue)(UINT32 enumValue, IEnumValue **ppEnumvalue) = 0;
        STDMETHOD(GetEnumValueByName)(HSTRING enumValueName, IEnumValue **ppEnumvalue) = 0;
    END_DECLARE_INTERFACE()

        ////////////////////// IEnumValue support ////////////////////////////    
    BEGIN_DECLARE_INTERFACE(IEnumValue, IObject, FOUNDATION_API)
        // Property methods.
        STDMETHOD(GetValue)(UINT32 *value) = 0;
        STDMETHOD(ToString)(__RPC__deref_out_opt HSTRING *value) = 0;
    END_DECLARE_INTERFACE()
}

