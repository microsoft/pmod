/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:source_model_delegate.h
****/
#pragma once

#include <foundation/library/interfaces/delegate_base.h>

namespace pmod
{
    namespace library
    {
        // The Source Base Model Delegate Base
        template <class TEventHandler>
        struct ISourceBaseModelDelegateBase : public foundation::library::IDelegateBase<TEventHandler>
        {
            // Callback when the Source has been attached
            STDMETHOD(OnSourceAttached)() = 0;
            // Callback when the Source has been dettached
            STDMETHOD(OnSourceDetached)() = 0;
        };

        // The SingleSource Model Delegate Base
        template <class TEventHandler, class TModel>
        struct ISingleSourceModelDelegateBase : public ISourceBaseModelDelegateBase<TEventHandler>
        {
            // When the Source Property has changed
            STDMETHOD(OnSourceChanged)(_In_ TModel *) = 0;
        };

    }
}

