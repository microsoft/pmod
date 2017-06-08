/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:model_class_factory.h
****/
#pragma once

#include <pmod/pmod_api.h>

#include "command_parameters.h"
#include "observable_collection_parameters.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // IModelClassFactory Interface
        BEGIN_DECLARE_INTERFACE(IModelClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateCommandInstance)(
                _In_ const CommandCreateParameters *pCreateParams,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;

            STDMETHOD(CreateObservableCollectionInstance)(
                _In_ const ObservableCollectionCreateParameters *pCreateParams,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
			
			STDMETHOD(CreateObservableObjectInstance)(
                _In_ const ObservableObjectCreateParameters *pCreateParams,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;


        END_DECLARE_INTERFACE()

    }
}
