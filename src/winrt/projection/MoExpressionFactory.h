/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoExpressionFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoExpressionFactory
//
// Purpose: Factory class for a Expression
//
//------------------------------------------------------------------------------
class MoExpressionFactory :
        public MoModelFactoryBase<IExpressionFactory>
{
public:
    // IMultiSourceBindingFactory
    virtual HRESULT STDMETHODCALLTYPE CreateExpression( 
        HSTRING expression,
        IExpression **instance);
};
