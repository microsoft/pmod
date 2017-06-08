/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:namespace_statics_util.h
****/
#pragma once

#include <foundation/macros.h>
#include "interfaces/namespace_statics.h"
#include <foundation/foundation_factory.h>

namespace foundation {

    namespace library
    {
        inline library::INamespaceStatics *get_namespace_statics()
        {
            return GetFoundationFactory<library::INamespaceStatics>(U("foundation.NamespaceStatics"));
        }

        inline HRESULT register_runtime_namespace(
            _In_ HSTRING foundationNamespace,
            _In_ HSTRING runtimeNamespace)
        {
            IFR_ASSERT(get_namespace_statics()->RegisterNamespace(foundationNamespace, runtimeNamespace));
            return S_OK;
        }
    }

}

