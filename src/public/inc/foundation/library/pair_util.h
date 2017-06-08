/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pair_util.h
****/
#pragma once

#include "interfaces/pair.h"
#include <foundation/macros.h>
#include <foundation/ctl/com_library.h>

namespace foundation {

    namespace library {
        //------------------------------------------------------------------------------
        // Class:   pair_impl
        //
        // Purpose: IPair implementation
        //
        //------------------------------------------------------------------------------
        class pair_impl :
            public foundation::ctl::ComInspectableBase,
            public IPair
        {
        public:
            HRESULT _Initialize(
                foundation::IUnknown *pFirst,
                foundation::IUnknown *pSecond)
            {
                _first_ptr = pFirst;
                _second_ptr = pSecond;
                return S_OK;
            }

        protected:
            HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ foundation::IUnknown **ppInterface) override
            {
                if (iid == IPair::GetIID())
                {
                    *ppInterface = static_cast<IPair *>(this);
                }
                else
                {
                    return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            STDMETHOD(GetFirst)(
                _COM_Outptr_result_maybenull_ foundation::IUnknown **ppValue) override
            {
                return _first_ptr.CopyTo(ppValue);
            }

            STDMETHOD(GetSecond)(
                _COM_Outptr_result_maybenull_ foundation::IUnknown **ppValue) override
            {
                return _second_ptr.CopyTo(ppValue);
            }

        private:
            ComPtr<IUnknown> _first_ptr;
            ComPtr<IUnknown> _second_ptr;
        };

        static inline HRESULT create_pair(
            foundation::IUnknown *pFirst,
            foundation::IUnknown *pSecond,
            IPair **ppPair)
        {
            return ctl::inspectable::CreateInstanceWithInitialize<pair_impl>(
                ppPair,
                pFirst,
                pSecond);
        }

        template <class T>
        inline HRESULT get_first(
            IPair *pPair,
            T **ppValue)
        {
            foundation_assert(pPair != nullptr);
            ComPtr<IUnknown> first_ptr;
            IFR_ASSERT(pPair->GetFirst(first_ptr.GetAddressOf()));
            return foundation::QueryInterfaceIf(first_ptr, ppValue);
        }

        template <class T>
        inline HRESULT get_second(
            IPair *pPair,
            T **ppValue)
        {
            foundation_assert(pPair != nullptr);
            ComPtr<IUnknown> second_ptr;
            IFR_ASSERT(pPair->GetSecond(second_ptr.GetAddressOf()));
            return foundation::QueryInterfaceIf(second_ptr, ppValue);
        }

    }
}
 
