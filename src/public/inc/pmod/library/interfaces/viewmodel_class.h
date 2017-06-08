/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:viewmodel_class.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <pmod/pmod_api.h>
#include <pmod/interfaces/observable_object.h>

#include "source_model_delegate.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // IViewModelClass Interface
        BEGIN_DECLARE_INTERFACE(IViewModelClass, foundation::IUnknown, PMOD_LIB_API)
        END_DECLARE_INTERFACE()

        // Delegate Interface for a Property View Model Implementation
        typedef ISingleSourceModelDelegateBase<IPropertyChangedEventHandler, IObservableObject> IViewModelDelegateBase;
        BEGIN_DECLARE_INTERFACE(IViewModelDelegate, IViewModelDelegateBase, PMOD_LIB_API)
            // When a property in the Source object has changed
            STDMETHOD(OnSourcePropertyChanged)(_In_ UINT32 propertyId, _In_ foundation::IInspectable *oldValue, _In_ foundation::IInspectable *newValue) = 0;
            STDMETHOD(OnSourceEventModel)(_In_ UINT32 eventId, _In_ foundation::IInspectable *pEventArgs) = 0;
        END_DECLARE_INTERFACE()

        enum class ViewModelOptions
        {
            None = 0x00,
            UseSourceWeakReference = 0x01
        };

        struct ViewModelCreateParameters : public
            _SingleSourceViewModelCreateParametersBase
            <
            ObservableObjectCreateParameters,
            IViewModelDelegate,
            ViewModelOptions,
            IObservableObject
            >
        {
        };

        BEGIN_DECLARE_INTERFACE(IViewModelClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateViewModelInstance)(
                _In_ const ViewModelCreateParameters *pModelImpl,
                _In_opt_ foundation::IInspectable *pOuter,
                _Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()

    }
}

