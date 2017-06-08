/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoExpressionBindingFactory.cpp
****/

#include "pch.h"

#include "MoExpressionBindingFactory.h"
#include <pmod/library/library_util.h>

STDMETHODIMP MoExpressionBindingFactory::CreateBindingExpression( 
    IObservableObject *pSource,
    IExpression *bindingExpression,
    PropertyBindingOptions options,
    IExpressionBinding **instance)
{
    IFCPTR(instance);
    IFCPTR(bindingExpression);

    foundation::ComPtr<pmod::IObservableObject> spSource;
    if(pSource != nullptr)
    {
        IFR(foundation::QueryInterface(pSource,spSource.GetAddressOf()));
    }

    foundation::ComPtr<foundation::IExpression> spBindingExpression;
    IFR(foundation::QueryInterface(bindingExpression,spBindingExpression.GetAddressOf()));

    foundation::ComPtr<pmod::IBindingBase> spBindingBase;
    IFR(pmod::library::CreateExpressionBinding(
        spSource,
        spBindingExpression,
        (pmod::library::PropertyBindingOptions)options,
        spBindingBase.GetAddressOf()));

    IFR(foundation::QueryInterface(spBindingBase,instance));

    return S_OK;
}
