/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:value_converter.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <foundation/interfaces/property_type_info.h>
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        BEGIN_DECLARE_INTERFACE(IValueConverterCallback, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(
            _In_ foundation::IInspectable* pValue,
            _COM_Outptr_ foundation::IInspectable **ppValue) = 0;
        END_DECLARE_INTERFACE()

        // IValueConverter Interface
        BEGIN_DECLARE_INTERFACE(IValueConverter, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Convert)(
            _In_ foundation::IInspectable* pValue,
            _In_ foundation::IPropertyTypeInfo *pTypeInfo,
            _In_ foundation::IInspectable* pParameter,
            _COM_Outptr_ foundation::IInspectable **ppValue) = 0;
        STDMETHOD(ConvertBack)(
            _In_ foundation::IInspectable* pValue,
            _In_ foundation::IPropertyTypeInfo *pTypeInfo,
            _In_ foundation::IInspectable* pParameter,
            _COM_Outptr_ foundation::IInspectable **ppValue) = 0;
        END_DECLARE_INTERFACE()

    }
}

