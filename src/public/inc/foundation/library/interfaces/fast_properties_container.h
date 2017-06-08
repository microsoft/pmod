/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:fast_properties_container.h
****/
#pragma once

#include <foundation/foundation_api.h>
#include "properties_container.h"
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        // IFastPropertiesContainer Interface
        BEGIN_DECLARE_INTERFACE(IFastPropertiesContainer, IPropertiesContainer, FOUNDATION_LIB_API)
            STDMETHOD(GetPropertType)(_In_ UINT32 propertyId, foundation::PropertyType* pPropertyType) = 0;
            STDMETHOD(ReadCustomData)(_In_ UINT32 propertyId, LPVOID pCustomData) = 0;
            STDMETHOD(WriteCustomData)(_In_ UINT32 propertyId, LPVOID pCustomData) = 0;
        END_DECLARE_INTERFACE()

    // IFastPropertiesContainer Delegate Interface
        BEGIN_DECLARE_INTERFACE(IFastPropertiesContainerDelegate, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(OnGetSlotProperty)(_In_ UINT32 propertyId, _COM_Outptr_result_maybenull_ foundation::IInspectable **value) = 0;
            STDMETHOD(OnSetSlotProperty)(_In_ UINT32 propertyId, _In_opt_ foundation::IInspectable *value) = 0;
        END_DECLARE_INTERFACE()
    }
}

