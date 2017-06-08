/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dispatcher_util.h
****/
#pragma once

#include "interfaces/dispatcher.h"
#include "async_operation_class.h"
#include "base_adapter_host.h"
#include <foundation/macros.h>
#include <foundation/ctl/com_library.h>

namespace foundation {

    //------------------------------------------------------------------------------
    // Class:   DispatchableFuncBase
    //
    // Purpose: template class to invoke a _Func callback by implementing dispatchable_t
    //
    //------------------------------------------------------------------------------
    template <class _Func, class dispatchable_t>
    class DispatchableFuncBase :
        public library::_DefaultAdapterHost<dispatchable_t, foundation::ctl::ComInspectableBase>,
        public dispatchable_t
    {
    public:
        template <class T>
        static HRESULT CreateInstance(
            _Func func, 
            _COM_Outptr_ T **pp)
        {
            return ctl::ComInspectableObject<T>::CreateInstance(pp, func);
        }
    protected:
        typedef library::_DefaultAdapterHost<dispatchable_t, foundation::ctl::ComInspectableBase> _Base_type;
        DispatchableFuncBase(const _Func func) :
            _func(func)
        {
        }

        HRESULT QueryInterfaceImpl(_In_ REFIID iid, foundation::IUnknown **ppInterface) override
        {
            IFCPTR_ASSERT(ppInterface);

            if (iid == dispatchable_t::GetIID())
            {
                *ppInterface = static_cast<dispatchable_t *>(this);
            }
            else
            {
                return _Base_type::QueryInterfaceImpl(iid, ppInterface);
            }
            return S_OK;
        }
        _Func& get_dispatchable_func() { return _func; }
    private:
        _Func _func;
    };

    //------------------------------------------------------------------------------
    // Class:   DispatchableFunc
    //
    // Purpose: template class to invoke a _Func callback by implementing IDelegateDispatchable
    //
    //------------------------------------------------------------------------------
    template <class _Func, class dispatchable_t>
    class DispatchableFunc :
        public DispatchableFuncBase<_Func, dispatchable_t>
    {
    protected:
        DispatchableFunc(const _Func func) :
            DispatchableFuncBase<_Func, dispatchable_t>(func)
        {
        }

        STDMETHOD(Invoke)(_In_ typename dispatchable_t::_TParameter_Type parameter)
        {
            return this->get_dispatchable_func()(parameter);
        }
    };

    //------------------------------------------------------------------------------
    // Class:   DispatchableFunc
    //
    // Purpose: template class to invoke a _Func callback by implementing IDelegateDispatchable
    //
    //------------------------------------------------------------------------------
    template <class _Func>
    class DispatchableFunc<_Func,library::IDelegateDispatchable> :
        public DispatchableFuncBase<_Func, library::IDelegateDispatchable>
    {
    protected:
        DispatchableFunc(const _Func func) :
            DispatchableFuncBase<_Func, library::IDelegateDispatchable>(func)
        {
        }

        STDMETHOD(Invoke)()
        {
            return this->get_dispatchable_func()();
        }
    };

    template <class dispatcher_t,class _Func>
    static HRESULT RunAsync(
        _In_ dispatcher_t *pDispatcher,
        _Func func,
        _COM_Outptr_opt_result_maybenull_ foundation::IAsyncOperation **ppAsyncAction)
    {
        foundation_assert(pDispatcher);

        typedef typename dispatcher_t::_TDispatchable_Type dispatchable_t;
        typedef  DispatchableFunc<_Func, dispatchable_t> dispatchable_func_t;

        dispatchable_func_t *p_dispatchable_func = nullptr;
        IFR_ASSERT(dispatchable_func_t::CreateInstance(func, &p_dispatchable_func));
        HRESULT hr = pDispatcher->RunAsync(p_dispatchable_func, ppAsyncAction);
        ctl::ReleaseInterface(p_dispatchable_func);
        return hr;
    }

    template <class dispatcher_t,class _Func>
    HRESULT RunSync(
        _In_ dispatcher_t *pDispatcher,
        _Func func)
    {
        ComPtr<foundation::IAsyncOperation> spAsyncOperation;

        _IFR_(RunAsync(pDispatcher, func, spAsyncOperation.GetAddressOf()));
        if (spAsyncOperation != nullptr)
        {
            ResultStatus status;
            // if asyc operation was returned Wait for completion signal
            _IFR_(spAsyncOperation->Wait(foundation::_InfiniteTimeout, &status));
            HRESULT hr;
            spAsyncOperation->GetError(&hr);
            return hr;
        }
        return S_OK;
    }

    inline bool HasThreadAccess(_In_ library::IDispatcher *pDispatcher)
    {
        foundation_assert(pDispatcher);

        bool b_value;
        IGNOREHR(pDispatcher->HasThreadAccess(&b_value));
        return b_value;
    }

    template <class dispatcher_t, class _Func>
    HRESULT RunIfSync(
        _In_ dispatcher_t *pDispatcher,
        _Func func,
        bool has_thread_access_option=true)
    {
        if (pDispatcher != nullptr && 
            (!has_thread_access_option || !HasThreadAccess(pDispatcher)))
        {
            return RunSync(pDispatcher, func);
        }
        return func();
    }

    template <class dispatcher_t, class _Func>
    static HRESULT RunIfAsync(
        _In_ dispatcher_t *pDispatcher,
        _Func func,
        /*_COM_Outptr_opt_result_maybenull_ */foundation::IAsyncOperation **ppAsyncAction = nullptr,
        bool has_thread_access_option = true)
    {
        if (pDispatcher != nullptr &&
            (!has_thread_access_option || !HasThreadAccess(pDispatcher)))
        {
            return RunAsync(pDispatcher, func, ppAsyncAction);
        }
        ComPtr<library::IAsyncOperationClass> asyncOperationClassPtr;
        if (ppAsyncAction != nullptr)
        {
            IFR_ASSERT(library::CreateAsyncOperationClass(
                foundation_GUID_NULL,
                asyncOperationClassPtr.GetAddressOf()));
        }

        HRESULT hr = func();

        if (asyncOperationClassPtr != nullptr && ppAsyncAction != nullptr)
        {
            asyncOperationClassPtr->SetCompleted(hr, nullptr);
            IFR_ASSERT(foundation::QueryInterface(asyncOperationClassPtr,ppAsyncAction));
        }
        return hr;
    }
}

