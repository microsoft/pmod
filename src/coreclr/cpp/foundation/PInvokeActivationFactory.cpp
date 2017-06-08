/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeActivationFactory.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <foundation/activation_factory.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ActivationFactory_IsFactoryAvailable(
    LPCSTR_t activationId, bool *result)
{
    *result = IsActivationFactoryAvailable(activationId);
    return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ActivationFactory_GetFactory(
    LPCSTR_t activationId,
    foundation::IUnknown **ppFactory)
{
    foundation::IUnknown *pFactory = GetActivationFactoryInstance(activationId);
    if (pFactory == nullptr)
    {
        *ppFactory = nullptr;
        return S_OK;
    }
    *ppFactory = pFactory;
    pFactory->AddRef();
    return S_OK;
}
