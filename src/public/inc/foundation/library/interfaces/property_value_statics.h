/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:property_value_statics.h
****/
#pragma once

#include <foundation/interfaces/property_value.h>

namespace foundation
{
    namespace library
    {
        //------------------------------------------------------------------------------
        // Interface:   IPropertyValueStatics
        //
        // Purpose: An Interface to provide property Value Utilities
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_INTERFACE(IPropertyValueInternalStatics, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(ComparePropertyValues)(
                _In_ foundation::IPropertyValue *pPropertyValue1,
                _In_ foundation::IPropertyValue *pPropertyValue2,
                _Always_(_Out_) int *pCompareResult) = 0;

            STDMETHOD(CompareInspectableValues)(
                _In_opt_ foundation::IInspectable *value1,
                _In_opt_ foundation::IInspectable *value2,
                _Always_(_Out_) int *pCompareResult) = 0;

            STDMETHOD(ChangeType)(
                _In_    foundation::PropertyType type,
                _Inout_ foundation::IInspectable **ppValue) = 0;

            STDMETHOD(ToString)(
                _In_ foundation::IPropertyValue *pPropertyValue,
                __RPC__deref_out_opt HSTRING *hstring) = 0;

            STDMETHOD(Serialize)(
                _In_ foundation::IPropertyValue *pPropertyValue,
                _Out_ UINT32 *size,
                _Outptr_ UINT8 **data) = 0;
            STDMETHOD(Deserialize)(
                UINT32 size,
                _In_ UINT8 *data,
                _COM_Outptr_ foundation::IPropertyValue **ppPropertyValue
                ) = 0;
            STDMETHOD(CreateDefaultValue)(
                _In_ foundation::PropertyType type,
                _COM_Outptr_ foundation::IInspectable **ppValue) = 0;

        END_DECLARE_INTERFACE()
    }
}

