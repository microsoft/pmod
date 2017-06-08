/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoExpressionBindingFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoExpressionBindingFactory
//
// Purpose: Factory class for a ExpressionBinding
//
//------------------------------------------------------------------------------
class MoExpressionBindingFactory :
        public MoModelFactoryBase<IExpressionBindingFactory>
{
public:
    // IMultiSourceBindingFactory
    virtual HRESULT STDMETHODCALLTYPE CreateBindingExpression( 
        IObservableObject *pSource,
        IExpression *bindingExpression,
        PropertyBindingOptions options,
        IExpressionBinding **instance);
};
