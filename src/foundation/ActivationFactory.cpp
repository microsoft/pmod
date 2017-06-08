/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ActivationFactory.cpp
****/
#include "pch.h"

#include "ActivationFactory.h"
#include <pal/library/trace_util.h>


CActivationFactory& CActivationFactory::GetInstance()
{
    static CActivationFactory _activationFactory;
    return _activationFactory;
}

HRESULT CActivationFactory::RegisterActivationFactoryEntries(_ActivationFactoryEntry *pEntries)
{
    while (pEntries->_activationId != nullptr)
    {
        RegisterActivationFactory(
            pEntries->_activationId,
            pEntries->_pGetActivationFactoryInstanceFunc);
        ++pEntries;
    }
    return S_OK;
}

HRESULT CActivationFactory::RegisterActivationFactory(
        LPCSTR_t activationId, 
        foundation::library::GetActivationFactoryInstanceFunc pGetActivationFactoryInstance)
{
#ifdef DEBUG
	pal::library::TraceMessage(U("CActivationFactory::RegisterActivationFactory->activationId:%s\n"), activationId);
#endif
    GetInstance()._registeredFactories[activationId] = pGetActivationFactoryInstance;
    return S_OK;
}

foundation::IUnknown *CActivationFactory::GetActivationFactoryInstance(LPCSTR_t activationId)
{
    return GetInstance().GetActivationFactoryInstanceInternal(activationId);
}

foundation::IUnknown *CActivationFactory::GetActivationFactoryInstanceInternal(LPCSTR_t activationId)
{
    _ActivationFactoryInstance_Map_Type::const_iterator iter =
        _registeredFactories.find(activationId);
    if (iter != _registeredFactories.end())
    {
        return (*(*iter).second)();
    }
    return nullptr;
}

EXTERN_C FOUNDATION_API foundation::IUnknown *STDAPICALLTYPE foundation::GetActivationFactoryInstance(LPCSTR_t activationId)
{
    return CActivationFactory::GetActivationFactoryInstance(activationId);
}

EXTERN_C FOUNDATION_API HRESULT STDAPICALLTYPE RegisterActivationFactory(
    LPCSTR_t activationId,
    foundation::library::GetActivationFactoryInstanceFunc func)
{
    return CActivationFactory::RegisterActivationFactory(activationId,func);
}
