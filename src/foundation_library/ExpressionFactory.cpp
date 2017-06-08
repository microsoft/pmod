/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ExpressionFactory.cpp
****/
#include "pch.h"

#include "ExpressionFactory.h"

#include "Expression.h"
#include <foundation/library/com_ptr_singleton.h>
#include <foundation/library/activation_factory.h>

using namespace foundation;
// {180760C2-00DA-4A28-A189-5FFB6BB7A963}
const GUID library::IID_IExpressionFactory =
{ 0x180760c2, 0xda, 0x4a28,{ 0xa1, 0x89, 0x5f, 0xfb, 0x6b, 0xb7, 0xa9, 0x63 } };

library::IExpressionFactory *CExpressionFactory::GetInstance()
{
	return library::_GetFactoryInstance<CExpressionFactory, library::IExpressionFactory>();
}

HRESULT CExpressionFactory::GetOrCreateInstance(library::IExpressionFactory **ppExpressionFactory)
{
    IFCPTR_ASSERT(ppExpressionFactory);
    *ppExpressionFactory = GetInstance();
    (*ppExpressionFactory)->AddRef();
    return S_OK;
}

STDMETHODIMP CExpressionFactory::CreateExpressionInstance(
    _In_ LPCSTR_t expression,
    _COM_Outptr_ foundation::IExpression **ppExpression)
{
    return CExpression::CreateInstance(expression,ppExpression);
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.Expression"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CExpressionFactory::GetInstance)
    );

