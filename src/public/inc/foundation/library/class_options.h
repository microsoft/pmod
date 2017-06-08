/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:class_options.h
****/
#pragma once

#include <foundation/pal.h>

namespace foundation {
    namespace library {
        // Foundation Class Options
        const UINT32 FoundationClassOptions_IsFreeThreadApartment = 0x100;
        const UINT32 FoundationClassOptions_ContinueOnFireEventError = 0x200;
        const UINT32 FoundationClassOptions_ComposedWeakReference = 0x400;
        const UINT32 FoundationClassOptions_IsLocalObjectTypeInfo = 0x800;
        const UINT32 FoundationClassOptions_IsDelegateWeakRef = 0x1000;

        const UINT32 FoundationClassOptions_UseQuirksMode = 0x8000;

        // when no tracking is needed on runtime
        const UINT32 FoundationClassOptions_NoTracking =
#if defined(DEBUG)
            0x10000;
#else
            0x00000;
#endif
        // Convert Options to a multi thread scenario
        template <class TModelOptions>
        static TModelOptions ToFreeThreadApartmentOptions(TModelOptions options)
        {
            return (TModelOptions)(static_cast<int>(options) | FoundationClassOptions_IsFreeThreadApartment);
        }

        // Convert Options to be delegate weak ref
        template <class TOptions>
        static TOptions ToDelegateWeakRefOptions(TOptions options)
        {
            return (TOptions)(static_cast<int>(options) | FoundationClassOptions_IsDelegateWeakRef);
        }

    }
}
