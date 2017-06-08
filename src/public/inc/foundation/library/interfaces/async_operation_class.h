/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation_class.h
****/
#pragma once

#include "object_class.h"
#include "delegate_base.h"
#include "dispatcher.h"
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        // IAsyncOperationClass Interface
        BEGIN_DECLARE_INTERFACE(IAsyncOperationClass, IObjectClass, FOUNDATION_LIB_API)
            // Property 'CancelDelegate'
            STDMETHOD(GetCancelDelegate)(_COM_Outptr_result_maybenull_ IDelegateDispatchable **ppCancelDelegate) = 0;
            STDMETHOD(SetCancelDelegate)(_In_opt_ IDelegateDispatchable *pCancelDelegate) = 0;
            // SetCompleted method
            STDMETHOD(SetCompleted)(_In_ HRESULT hResult, _In_opt_ foundation::IInspectable *result) = 0;
            // SetCompleted method using another IAsyncOperationModel
            STDMETHOD(SetCompleted)(_In_ IAsyncOperation *pAsyncOperation) = 0;
            // Fire 'Progress' property changed
            STDMETHOD(FireProgress)(_In_ foundation::IInspectable *pProgressValue) = 0;
        END_DECLARE_INTERFACE()

            // Delegate Interface for a Property Result Model Impl
        BEGIN_DECLARE_INTERFACE(IAsyncOperationDelegate, IDelegateBase<ICompletedEventHandler>, FOUNDATION_LIB_API)
            STDMETHOD(OnCancel)() = 0;
        END_DECLARE_INTERFACE()


    }
}

