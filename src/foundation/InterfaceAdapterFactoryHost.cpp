/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InterfaceAdapterFactoryHost.cpp
****/
#include "pch.h"

#include "InterfaceAdapterFactoryHost.h"
#include <foundation/library/com_ptr_singleton.h>
#include "ActivationFactory.h"

using namespace foundation;
using namespace foundation::library;

// {E0EBEEB3-2106-49B0-90F4-7A07585AF0C4}
const GUID foundation::library::IID_IInterfaceAdapterFactoryHost =
{ 0xe0ebeeb3, 0x2106, 0x49b0,{ 0x90, 0xf4, 0x7a, 0x7, 0x58, 0x5a, 0xf0, 0xc4 } };

IInterfaceAdapterFactoryHost*CInterfaceAdapterFactoryHost::GetInstance()
{
    return library::_GetFactoryInstance<CInterfaceAdapterFactoryHost, IInterfaceAdapterFactoryHost>();
}

static ::_RegisterActivationFactory _interfaceAdapterFactoryHostRegister(
    U("foundation.InterfaceAdapterFactoryHost"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CInterfaceAdapterFactoryHost::GetInstance)
    );
