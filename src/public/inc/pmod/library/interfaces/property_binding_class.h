/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:property_binding_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <foundation/library/type_info_create_parameter.h>
#include <foundation/library/interfaces/foundation_class_base.h>
#include <pmod/interfaces/binding.h>
#include <pmod/interfaces/observable_object.h>

#include "view_model_parameters.h"
#include "source_model_delegate.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // IPropertyBindingClass Interface
        BEGIN_DECLARE_INTERFACE(IPropertyBindingClass, foundation::library::IFoundationClassBase, PMOD_LIB_API)
            STDMETHOD(GetBindingsSteps)(
                _Out_ UINT32 *size_path_steps,
                IObservableObject ***ppPathSteps,
                _Out_ UINT32 *size_property_steps,
                UINT32 **ppPropertySteps) = 0;
        END_DECLARE_INTERFACE()

        // Delegate Interface for a Property Binding
        typedef ISingleSourceModelDelegateBase<IBindingValueChangedEventHandler, IObservableObject> IPropertyBindingDelegateBase;
        BEGIN_DECLARE_INTERFACE(IPropertyBindingDelegate, IPropertyBindingDelegateBase, PMOD_LIB_API)
            STDMETHOD(OnValueChanged)(_In_ foundation::IInspectable *value, _In_ bool isValueSet) = 0;
        END_DECLARE_INTERFACE()

            enum class PropertyBindingOptions
        {
            None = 0x00,
            UseSourceWeakReference = 0x01,
            UsePathStepWeakReference = 0x02
        };

        struct PropertyBindingCreateParameters :
            public _SingleSourceModelCreateParametersBase
            <
            foundation::library::_TypeInfoCreateParameterBase
            <
            IPropertyBindingDelegate,
            PropertyBindingOptions
            >,
            IObservableObject
            >
        {
            foundation::IInspectable               *m_p_path;                 // Path for this binding
        };

        BEGIN_DECLARE_INTERFACE(IPropertyBindingClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreatePropertyBindingInstance)(
                _In_ const PropertyBindingCreateParameters *pBindingParam,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()

    }
}

