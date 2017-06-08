/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Expression.h
****/
#ifndef _EXPRESSION_DEFINED
#define _EXPRESSION_DEFINED


#include <foundation/interfaces/expression.h>
#include <foundation/ctl/com_library.h>
#include <foundation/library/base_adapter_host.h>

#include <memory>
#include <vector>
#include <foundation/array_wrapper.h>

#include "auto_delete_vector.h"

class CTokenBase;
typedef auto_delete_vector<CTokenBase>  _TokenBaseVectorType;

class CExpression:
    public foundation::library::_DefaultAdapterHost
    <
        foundation::IExpression,
        foundation::ctl::ImplementsInspectable
        <
            foundation::IExpression,
            &foundation::IID_IExpression
        >
    >
{
public:
    static HRESULT CreateInstance(
        _In_ LPCSTR_t expression,
        _COM_Outptr_ IExpression **ppExpression);
    HRESULT _Initialize(LPCSTR_t expression);
protected:
    STDMETHOD(GetReferenceTokens)(UINT32 *size, HSTRING **ppReferenceTokens);
    STDMETHOD(Evaluate)(foundation::IResolveTokenDelegate *pResolveDelegate, foundation::IPropertyValue ** ppValue);
private:

    _TokenBaseVectorType                    m_rpn;
    foundation::HStringArrayWrapper             m_TokenReferences;
};

#endif
