/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:binding.h
****/
#pragma once

#include <foundation/interfaces/object.h>
#include <foundation/foundation_macros.h>

#include <pmod/pmod_api.h>

namespace pmod {

    BEGIN_DECLARE_INTERFACE(IBindingValueChangedEventArgs, foundation::IUnknown, PMOD_API)
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(IBindingValueChanged, PMOD_API)

    BEGIN_DECLARE_INTERFACE(IBindingConverter, foundation::IUnknown, PMOD_API)
        STDMETHOD(Convert)(
        foundation::IInspectable *value,
        foundation::IInspectable *parameter,
        foundation::IInspectable** returnValue) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IBindingBase, foundation::IUnknown, PMOD_API)
        // Property methods.
        STDMETHOD(GetValue)(foundation::IInspectable **ppValue) = 0;
        STDMETHOD(GetConverter)(IBindingConverter **pValue) = 0;
        STDMETHOD(SetConverter)(IBindingConverter *value) = 0;
        STDMETHOD(GetConverterParameter)(foundation::IInspectable **pValue) = 0;
        STDMETHOD(SetConverterParameter)(foundation::IInspectable *value) = 0;
        // Methods.
    // Event methods.
        STDMETHOD(GetBindingValueChangedEventSource)(IBindingValueChangedEventSource** ppEventSource) = 0;
    END_DECLARE_INTERFACE()

    /////////////////////////////////// Property Binding //////////////////////////////////////////////
    BEGIN_DECLARE_INTERFACE(IPropertyBinding, IBindingBase, PMOD_API)
        // Property methods.
        STDMETHOD(GetPath)(foundation::IInspectable **ppValue) = 0;
        STDMETHOD(SetPath)(foundation::IInspectable *value) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IMultiSourceBinding, IBindingBase, PMOD_API)
    END_DECLARE_INTERFACE()

}

