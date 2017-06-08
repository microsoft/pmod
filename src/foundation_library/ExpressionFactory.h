/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ExpressionFactory.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/interfaces/expression_factory.h>

class CExpressionFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IExpressionFactory
{
 public:
     static foundation::library::IExpressionFactory *GetInstance();
     static HRESULT GetOrCreateInstance(foundation::library::IExpressionFactory **ppExpressionFactory);
    //Interface IExpressionFactory
    STDMETHOD(CreateExpressionInstance)(
        _In_ LPCSTR_t expression,
        _COM_Outptr_ foundation::IExpression **ppExpression);
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IExpressionFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IExpressionFactory *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

};


