/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:multi_source_binding_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <foundation/library/interfaces/foundation_class_base.h>
#include <foundation/library/type_info_create_parameter.h>
#include <pmod/interfaces/binding.h>

#include "source_model_delegate.h"
#include "view_model_parameters.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // IMultiSourceBindingClass Interface
        BEGIN_DECLARE_INTERFACE(IMultiSourceBindingClass, foundation::library::IFoundationClassBase, PMOD_LIB_API)
            STDMETHOD(GetBindingSources)(_Out_ UINT32 *size, _Outptr_ IBindingBase ***ppBindings) = 0;
        END_DECLARE_INTERFACE()

        // Delegate Interface for a MultiSource Binding
        BEGIN_DECLARE_INTERFACE(IMultiSourceBindingDelegate, ISourceBaseModelDelegateBase<IBindingValueChangedEventHandler>, PMOD_LIB_API)
            STDMETHOD(OnBindingValuesChanged)(_In_ UINT32 size, foundation::IInspectable **pBindingValues, _Outptr_ foundation::IInspectable **ppValue) = 0;
        END_DECLARE_INTERFACE()

        enum class MultiSourceBindingOptions
        {
            None = 0x00
        };
        struct MultiSourcePropertyBindingCreateParameters :
            public _MultiSourceCreateParametersBase<IBindingBase>,
            public foundation::library::_CreateParameterBase<library::IMultiSourceBindingDelegate, MultiSourceBindingOptions>
        {
        };

        // IExpressionBindingClassFactory Interface
        BEGIN_DECLARE_INTERFACE(IMultiSourceBindingClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateMultiSourceBindingInstance)(
                _In_ const MultiSourcePropertyBindingCreateParameters *pMultiSourceBindingParam,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()

    }
}

