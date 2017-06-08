/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ClassStatics.cpp
****/
#include "pch.h"

#include "ClassStatics.h"
#include "Logger.h"

// public headers
#include <pmod/library/library_util.h>

using namespace pmod;

// {F1B7978C-5B45-4A59-A63A-3C568C573ED0}
const GUID pmod::library::IID_IClassStatics =
{ 0xf1b7978c, 0x5b45, 0x4a59,{ 0xa6, 0x3a, 0x3c, 0x56, 0x8c, 0x57, 0x3e, 0xd0 } };

library::IClassStatics *CClassStatics::GetInstance()
{
	return foundation::library::_GetFactoryInstance<CClassStatics, library::IClassStatics>();
}

HRESULT CClassStatics::GetOrCreateInstance(library::IClassStatics **ppPropertyModelInstance)
{
    IFCPTR_ASSERT(ppPropertyModelInstance);
    *ppPropertyModelInstance = GetInstance();
    (*ppPropertyModelInstance)->AddRef();
    return S_OK;
}

STDMETHODIMP CClassStatics::GetEventDispatcher(foundation::library::IDispatcher **ppEventDispatcher)
{
    return _eventDispatcher.CopyTo(ppEventDispatcher);
}

STDMETHODIMP CClassStatics::SetEventDispatcher(foundation::library::IDispatcher *pEventDispatcher)
{
    _eventDispatcher = pEventDispatcher;
    return S_OK;
}

STDMETHODIMP CClassStatics::GetWorkingDispatcher(foundation::library::IDispatcher **ppWorkingDispatcher)
{
    return _workingDispatcher.CopyTo(ppWorkingDispatcher);
}

STDMETHODIMP CClassStatics::SetWorkingDispatcher(foundation::library::IDispatcher *pWorkingDispatcher)
{
    _workingDispatcher = pWorkingDispatcher;
    return S_OK;
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.ClassStatics"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CClassStatics::GetInstance)
    );
