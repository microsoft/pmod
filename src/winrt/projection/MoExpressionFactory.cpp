/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoExpressionFactory.cpp
****/

#include "pch.h"

#include "MoExpressionFactory.h"
#include <foundation/library/library_util.h>

STDMETHODIMP MoExpressionFactory::CreateExpression( 
    HSTRING expression,
    IExpression **instance)
{
    IFCPTR(instance);

    foundation::ComPtr<foundation::IExpression> spExpression;
    IFR(foundation::library::CreateExpression(
        _pal_GetStringRawBuffer(expression,nullptr),
        spExpression.GetAddressOf()));

    IFR(foundation::QueryInterface(spExpression,instance));

    return S_OK;
}
