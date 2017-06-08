/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:expression_factory.h
****/
#pragma once

#include <foundation/interfaces/expression.h>
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        // IExpressionFactory Interface
        BEGIN_DECLARE_INTERFACE(IExpressionFactory, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(CreateExpressionInstance)(
            _In_ LPCSTR_t expression,
            _COM_Outptr_ IExpression **ppExpression) = 0;
        END_DECLARE_INTERFACE()
    }
}

