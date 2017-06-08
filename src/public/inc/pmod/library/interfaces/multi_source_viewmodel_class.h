/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:multi_source_viewmodel_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <pmod/interfaces/observable_object.h>

#include "observable_object_class.h"
#include "viewmodel_class.h"
#include "source_model_delegate.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        BEGIN_DECLARE_INTERFACE(IMultiSourceViewModelDelegate, ISourceBaseModelDelegateBase<IPropertyChangedEventHandler>, PMOD_LIB_API)
            STDMETHOD(OnSourcePropertyChanged)(_In_ IObservableObject *pSource, _In_ UINT32 propertyId, _In_ foundation::IInspectable *oldValue, _In_ foundation::IInspectable *newValue) = 0;
        END_DECLARE_INTERFACE()

        struct MultiSourceViewModelCreateParameters :
            public _MultiSourceViewModelCreateParametersBase
            <
            ObservableObjectCreateParameters,
            IMultiSourceViewModelDelegate,
            ViewModelOptions,
            IObservableObject
            >
        {
        };

        BEGIN_DECLARE_INTERFACE(IMultiSourceViewModelClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateMultiSourceViewModelInstance)(
                _In_ const MultiSourceViewModelCreateParameters *pModelImpl,
                _In_opt_ foundation::IInspectable *pOuter,
                _Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

