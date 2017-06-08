/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_factory.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>
#include <foundation/foundation_macros.h>

#include "object.h"

namespace foundation {

    /////////////////////////////////// Object Factory Base //////////////////////////////////////////////
    BEGIN_DECLARE_INTERFACE(IObjectFactoryBase, foundation::IInspectable, FOUNDATION_API)
        // Attempt to Create a Model from our registry
        STDMETHOD(TryCreateObject)(
            REFIID iidType,
            foundation::IInspectable *pContextParameter,
            IObject **ppObject) = 0;
        STDMETHOD(ActivateObject)(
            REFIID iidType,
            foundation::IInspectable *pContextParameter,
            IObject **ppObject) = 0;
    END_DECLARE_INTERFACE()

    /////////////////////////////////// Object Factory //////////////////////////////////////////////

    // ICreateObjectDelegate Interface
    BEGIN_DECLARE_INTERFACE(ICreateObjectDelegate, foundation::IInspectable, FOUNDATION_API)
        STDMETHOD(Invoke)(_In_ foundation::IInspectable* pContextParameter, _COM_Outptr_ foundation::IObject **ppNewInstance) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IObjectFactory, IObjectFactoryBase, FOUNDATION_API)
        // Register an Object Factory
        STDMETHOD(RegisterObjectFactory)(GUID iidClassType, ICreateObjectDelegate *pCreator) = 0;
        // Unregister an Object Factory
        STDMETHOD(UnregisterObjectFactory)(GUID iidClassType) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IObjectFactoryFactory, foundation::IInspectable, FOUNDATION_API)
        // Create a new Object Factory
        STDMETHOD(CreateObjectFactory)(IObjectFactory **ppNewInstance) = 0;
    END_DECLARE_INTERFACE()

}

