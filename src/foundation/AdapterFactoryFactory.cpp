/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AdapterFactoryFactory.cpp
****/
#include "pch.h"

#include "AdapterFactoryFactory.h"
#include "GenericAdapterFactory.h"
#include "TypeAdapterFactory.h"

#include <foundation/library/com_ptr_singleton.h>
#include "ActivationFactory.h"

using namespace foundation;

// {9C9D0BA4-018D-4335-989D-E3C17E478D7C}
const GUID library::IID_IAdapterFactoryFactory =
{ 0x9c9d0ba4, 0x18d, 0x4335,{ 0x98, 0x9d, 0xe3, 0xc1, 0x7e, 0x47, 0x8d, 0x7c } };

library::IAdapterFactoryFactory *CAdapterFactoryFactory::GetInstance()
{
	return library::_GetFactoryInstance<CAdapterFactoryFactory, library::IAdapterFactoryFactory>();
}

HRESULT CAdapterFactoryFactory::GetOrCreateInstance(library::IAdapterFactoryFactory **ppAdapterFactoryFactory)
{
    IFCPTR_ASSERT(ppAdapterFactoryFactory);
    *ppAdapterFactoryFactory = GetInstance();
    (*ppAdapterFactoryFactory)->AddRef();
    return S_OK;
}

STDMETHODIMP CAdapterFactoryFactory::CreateGenericAdapterFactory(
    _COM_Outptr_ foundation::library::IGenericAdapterFactory **ppGenericAdapterFactory)
{
    IFCPTR_ASSERT(ppGenericAdapterFactory);
    IFR_ASSERT(CGenericAdapterFactory::CreateInstance(ppGenericAdapterFactory));
    return S_OK;
}

STDMETHODIMP CAdapterFactoryFactory::CreateTypeAdapterFactory(
    _In_opt_ const foundation::library::_TypeAdapterEntry iidTypes[],
    _COM_Outptr_ foundation::library::IInterfaceAdapterFactory **ppModelAdapterFactory)
{
    return CTypeAdapterFactory::CreateInstance(iidTypes,ppModelAdapterFactory);
}

static _RegisterActivationFactory _adapterFactoryFactoryRegister(
    U("foundation.AdapterFactory"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CAdapterFactoryFactory::GetInstance)
    );

