/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:expression_class.h
****/
#pragma once

#include "interfaces/expression_factory.h"
#include <foundation/activation_factory.h>

namespace foundation
{
    namespace library
    {
        inline library::IExpressionFactory *GetExpressionFactory()
        {
            return GetFoundationLibraryFactory<library::IExpressionFactory>(U("foundation.Expression"));
        }

        inline HRESULT CreateExpression(
            _In_ LPCSTR_t                   expression,
            _COM_Outptr_ IExpression        **ppExpressionInstance)
        {
            return GetExpressionFactory()->CreateExpressionInstance(expression, ppExpressionInstance);
        }
    }

}

