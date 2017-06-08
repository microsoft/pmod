/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:disposable_util.h
****/
#pragma once

#include "interfaces/disposable.h"

namespace foundation {

    namespace library
    {
        inline static HRESULT DisposeIf(_In_ foundation::IUnknown *pUnk)
        {
            foundation_assert(pUnk);

            foundation::library::IDisposable *pDisposable = nullptr;
            foundation::QueryWeakReference(pUnk, &pDisposable);
            if (pDisposable != nullptr)
            {
                return pDisposable->Dispose();
            }
            return S_FALSE;
        }

        __unused_attribute__
        static HRESULT Dispose(_In_ foundation::IUnknown *pUnk)
        {
            foundation_assert(pUnk);

            foundation::library::IDisposable *pDisposable = nullptr;
            IFR_ASSERT(foundation::QueryWeakReference(pUnk, &pDisposable));
            return pDisposable->Dispose();
        }

        template <class _Container>
        static HRESULT DisposeContainer(const _Container& container)
        {
            for (typename _Container::const_iterator iter = container.begin();
                iter != container.end();
                ++iter)
            {
                _IFR_(DisposeIf(*iter));
            }
            return S_OK;
        }
    }
}

