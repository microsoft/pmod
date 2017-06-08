/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:expression_binding_class.h
****/
#pragma once

#include <pmod/pmod_api.h>

#include "property_binding_class.h"

namespace pmod
{
    namespace library
    {
        // IExpressionBindingClassFactory Interface
        BEGIN_DECLARE_INTERFACE(IExpressionBindingClassFactory, foundation::IUnknown, PMOD_LIB_API)

            STDMETHOD(CreateExpressionBindingInstance)(
                _In_ IObservableObject *pSource,
                _In_ foundation::IExpression *pExpression,
                _In_ PropertyBindingOptions bindingOptions,
                _COM_Outptr_ IBindingBase **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

