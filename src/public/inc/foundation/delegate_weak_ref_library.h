/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:delegate_weak_ref_library.h
****/
#pragma once

#include "ctl/com_library.h"
#include "com_ptr.h"
#include "macros.h"
#include "weak_reference.h"
#include <foundation/library/interfaces/disposable.h>
#include <pal/pal_atomic.h>
#include <utility>
   

namespace foundation
{
    //+-----------------------------------------------------------------------------
    // Class:   DelegateWeakRefBase
    //
    // Purpose: Base class for implementing an EventHandler with a weak ref target resolver
    //------------------------------------------------------------------------------
    template <class TDelegate>
    class DelegateWeakRefBase:
        public ctl::ComInspectableBase,
        public TDelegate,
        public foundation::library::IDisposable
    {
    public:
        HRESULT _Initialize(_In_ foundation::IUnknown *pTarget)
        {
            IFR_ASSERT(AsWeakRef(_p_weak_reference_target, pTarget));
            return S_OK;
        }

        HRESULT SetCallbackEnabled(bool value)
        {
            this->_enabled = value;
            return S_OK;
        }
    protected:
        DelegateWeakRefBase() :
            _p_weak_reference_target(nullptr),
            _enabled(true)
        {
        }

        void FinalRelease()
        {
            RELEASE_INTERFACE(_p_weak_reference_target);
        }

        HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
        {
            if (iid == TDelegate::GetIID())
            {
                *ppInterface = static_cast<TDelegate *>(this);
            }
            else if (iid == foundation::library::IDisposable::GetIID())
            {
                *ppInterface = static_cast<foundation::library::IDisposable *>(this);
            }
            else
            {
                return ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
            }
            return S_OK;
        }

        // Interface IDisposable
        STDMETHOD (Dispose)()
        {
            _enabled = false;
            return S_OK;
        }

        template <class Q>
        HRESULT ResolveWeakRefTarget(Q **ppQ)
        {
            *ppQ = nullptr;
            if (!_enabled)
            {
                return S_FALSE;
            }
            IFR_ASSERT(ResolveWeakRef(_p_weak_reference_target, *ppQ));

            return *ppQ == nullptr ? S_FALSE:S_OK;
        }
    private:
        volatile bool _enabled;
        foundation::IWeakReference *_p_weak_reference_target;
    };

    //+-----------------------------------------------------------------------------
    // Class:   EventHandlerDelegateWeakRefBase
    //
    // Purpose: specialization for standard EventHandler signature => Invoke(sender,eventArgs)
    //------------------------------------------------------------------------------
    template <
    typename TSinkInterface,
    typename TSink,
    typename TDelegate,
    typename TEventArgs
    >
    class EventHandlerDelegateWeakRefBase :
        public DelegateWeakRefBase<TDelegate>
    {
    public:
        HRESULT _Initialize(
            const std::pair<foundation::IUnknown *,
            HRESULT(TSink::*)(foundation::IUnknown *, TEventArgs *)>& init_params)
        {
            _IFR_(DelegateWeakRefBase<TDelegate>::_Initialize(init_params.first));
            this->_pMethod = init_params.second;
            return S_OK;
        }

        template <
            typename _TEventArgs
        >
        HRESULT _Invoke(
        _In_ foundation::IUnknown *pSender,
        _In_ _TEventArgs *pArgs,
        _In_ HRESULT(TSink::*pMethod)(foundation::IUnknown *, _TEventArgs *))
        {
            TSinkInterface *pSinkInterface = nullptr;
            HRESULT hr = this->ResolveWeakRefTarget(&pSinkInterface);
            if (hr != S_OK)
            {
                return hr;
            }
            foundation_assert(pSinkInterface != nullptr);
            TSink *pSink = static_cast<TSink *>(pSinkInterface);
            hr = (pSink->*pMethod)(pSender, pArgs);
            pSinkInterface->Release();
            return hr;
        }
    protected:
        // EventHandler Interface
        STDMETHOD(Invoke)(
            _In_ foundation::IUnknown *pSender,
            _In_ TEventArgs *pArgs
            )
        {
            return this->_Invoke(pSender, pArgs, _pMethod);
        }


    private:
        HRESULT(TSink::*_pMethod)(foundation::IUnknown *, TEventArgs *);
    };

    //+-----------------------------------------------------------------------------
    // Method:   CreateEventHandlerDelegateWeakRefWithType
    //
    // Purpose: create weak ref event handler with type specialization 
    //------------------------------------------------------------------------------
    template <
    typename _Delegate_Type,
    typename TSink,
    typename TDelegate,
    typename TEventArgs
    >
    static HRESULT CreateEventHandlerDelegateWeakRefWithType(
        _In_ TSink *pSink,
        _In_ HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
        _COM_Outptr_ TDelegate **ppDelegate
    )
    {
        _Delegate_Type *pDelegate;
        auto param = std::make_pair(pSink->CastToInspectable(), pMethod);
        IFR_ASSERT(ctl::inspectable::CreateInstanceWithInitialize(&pDelegate, param));
        *ppDelegate = static_cast<TDelegate *>(pDelegate);
        return S_OK;
    }

    //+-----------------------------------------------------------------------------
    // Method:   CreateEventHandlerDelegateWeakRef
    //
    // Purpose: create weak ref event handler with type deduction
    //------------------------------------------------------------------------------
    template <
    typename TSinkInterface,
    typename TSink,
    typename TDelegate,
    typename TEventArgs
    >
    static HRESULT CreateEventHandlerDelegateWeakRef(
        _In_ TSink *pSink,
        _In_ HRESULT(TSink::*pMethod)(foundation::IUnknown *, TEventArgs *),
        _COM_Outptr_ TDelegate **ppDelegate
    )
    {
        typedef EventHandlerDelegateWeakRefBase<TSinkInterface,TSink, TDelegate, TEventArgs> _Delegate_Type;
        return CreateEventHandlerDelegateWeakRefWithType<_Delegate_Type>(pSink, pMethod, ppDelegate);
    }

}

