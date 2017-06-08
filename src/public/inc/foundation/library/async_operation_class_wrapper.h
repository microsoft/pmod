/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_class_wrapper.h
****/
#pragma once

#include "class_wrapper.h"
#include "interfaces/async_operation_class.h"
#include <foundation/delegate_library.h>

namespace foundation {


#if _HAS_CPP0X
    typedef foundation::FuncInvoke0Delegate<library::IDelegateDispatchable>     cancel_delegate_func;
    
    template <class _Func>
    static HRESULT SetCancelDelegate(
        _In_ library::IAsyncOperationClass *pAsyncOperationClass,
        _In_ const _Func& func)
    {
        ComPtr<cancel_delegate_func> delegate_ptr;
        IFR_ASSERT(CreateFuncDelegate(func, delegate_ptr.GetAddressOf()));
        return pAsyncOperationClass->SetCancelDelegate(delegate_ptr);
    }

#endif

    template <class T, class TResult>
    class _AsyncOperationClassWrapper :
        public _FoundationClassWrapper<T,library::IAsyncOperationClass>
    {
    public:
        HRESULT GetCancelDelegate(_COM_Outptr_result_maybenull_ library::IDelegateDispatchable **ppCancelDelegate) const
        {
            return this->GetClassInterfaceInternal()->GetCancelDelegate(ppCancelDelegate);
        }

        HRESULT SetCancelDelegate(_In_ library::IDelegateDispatchable *pCancelDelegate) const
        {
            return this->GetClassInterfaceInternal()->SetCancelDelegate(pCancelDelegate);
        }

#if _HAS_CPP0X
        template <class _Func>
        HRESULT SetCancelDelegate(const _Func& func) const
        {
            return foundation::SetCancelDelegate(
                this->GetClassInterfaceInternal(),
                func);
        }
#endif
        HRESULT SetCompleted(HRESULT hResult, TResult result) const
        {
            InspectablePtr spResult = foundation::CreateValue(result);
            return this->GetClassInterfaceInternal()->SetCompleted(hResult, spResult);
        }

        HRESULT SetCompleted(HRESULT hResult) const
        {
            return this->GetClassInterfaceInternal()->SetCompleted(hResult, nullptr);
        }

        HRESULT SetCompleted(_In_ foundation::IAsyncOperation *pOtherAsyncOperation) const
        {
            return this->GetClassInterfaceInternal()->SetCompleted(pOtherAsyncOperation);
        }
    };


}

