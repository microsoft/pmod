/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:callback_delegate_adapter_base.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/macros.h>
#include <foundation/foundation_macros.h>

namespace foundation {
    // _CallbackDelegateAdapterBase Class
    template <class T, class TCallback, class TDelegate, const IID *pIIDInterface>
    class _CallbackDelegateAdapterBase :
        public ctl::ImplementsInspectable<TDelegate, pIIDInterface>
    {
    public:
        static HRESULT CreateInstance(
            _In_ TCallback callback,
            _COM_Outptr_ TDelegate **ppDelegate)
        {
            IFCPTR_ASSERT(ppDelegate);
            *ppDelegate = nullptr;

            IFCPTR_ASSERT(callback);

            T *pCallbackAdapter = nullptr;
            IFR_ASSERT(ctl::inspectable::CreateInstance(&pCallbackAdapter));
            pCallbackAdapter->m_Callback = callback;
            *ppDelegate = static_cast<TDelegate *>(pCallbackAdapter);

            return S_OK;
        }
    protected:
        TCallback& GetCallback() { return m_Callback; }

    private:
        TCallback m_Callback;
    };

}

