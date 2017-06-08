/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:expression.h
****/
#pragma once

#include <foundation/interfaces/property_value.h>
#include <foundation/foundation_api.h>

namespace foundation {

    /////////////////////////////////// Expression Support //////////////////////////////////////////////
    BEGIN_DECLARE_INTERFACE(IResolveTokenDelegate, foundation::IUnknown, FOUNDATION_API)
        STDMETHOD(Invoke)(LPCSTR_t referenceToken, foundation::IPropertyValue ** ppValue) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IExpression, foundation::IUnknown, FOUNDATION_API)
        STDMETHOD(GetReferenceTokens)(UINT32 *size, HSTRING **ppReferenceTokens) = 0;
        STDMETHOD(Evaluate)(IResolveTokenDelegate *pResolveDelegate, foundation::IPropertyValue ** ppValue) = 0;
    END_DECLARE_INTERFACE()


}

