/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:properties_container.h
****/
#pragma once

#include "disposable.h"
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        // IPropertiesContainer Interface
        BEGIN_DECLARE_INTERFACE(IPropertiesContainer, foundation::library::IDisposable, FOUNDATION_LIB_API)
            STDMETHOD(HasProperty)(_In_ UINT32 propertyId, _Out_ bool *found) = 0;
            STDMETHOD(GetValue)(_In_ UINT32 propertyId, _COM_Outptr_result_maybenull_ foundation::IInspectable **value) = 0;
            STDMETHOD(SetValue)(_In_ UINT32 propertyId, _In_opt_ foundation::IInspectable *value) = 0;
            STDMETHOD(RemoveProperty)(_In_ UINT32 propertyId) = 0;
            STDMETHOD(GetSize)(_Out_ UINT32* size) = 0;
        END_DECLARE_INTERFACE()
    }
}

