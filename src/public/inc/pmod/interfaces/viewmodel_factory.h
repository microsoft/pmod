/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:viewmodel_factory.h
****/
#pragma once

#include "source_model.h"
#include <foundation/interfaces/object_factory.h>

namespace pmod {

    /////////////////////////////////// Object Created Event //////////////////////////////////////////////
    BEGIN_DECLARE_INTERFACE(IViewModelCreatedEventArgs, foundation::IUnknown, PMOD_API)
        STDMETHOD(GetObject)(foundation::IObject ** ppVal) = 0;
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(IViewModelCreated, PMOD_API)

    /////////////////////////////////// View Model Factory //////////////////////////////////////////////
    // ICreateSingleSourceModelDelegate Interface
    BEGIN_DECLARE_INTERFACE(ICreateSingleSourceModelDelegate, foundation::IUnknown, PMOD_API)
        STDMETHOD(Invoke)(_In_ foundation::IObject *pModel, _COM_Outptr_ ISourceModel **ppSourceModel) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IViewModelFactory, foundation::IObjectFactory, PMOD_API)
        STDMETHOD(RegisterSingleSourceViewModelCreator)(GUID iidModelType, ICreateSingleSourceModelDelegate *pCreator) = 0;
        // Create a Single Source Model Type
        STDMETHOD(TryCreateSingleSourceViewModel)(REFIID iidType, foundation::IObject *pModel, ISourceModel **ppSourceModel) = 0;
        // Event methods.
        STDMETHOD(GetObjectCreatedEventSource)(IViewModelCreatedEventSource** ppEventSource) = 0;
    END_DECLARE_INTERFACE()

}


