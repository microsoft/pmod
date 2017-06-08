/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pplxtasks.h
****/
#pragma once

#include <foundation/interfaces/async_operation.h>
#include <foundation/async_operation_connection.h>
#include <foundation/exception.h>

// Casablanca required headers
#if (defined(_MSC_VER) && (_MSC_VER >= 1800))
#include <ppltasks.h>
namespace pplx = Concurrency;
#else 
#include <cpprest/basic_types.h>
#include "pplx/pplxtasks.h"
#endif

#include <memory>

namespace foundation
{

    template <class T>
    static HRESULT _Set_task_result(
        _In_ IAsyncOperation *pAsyncOperation,
        _In_ const pplx::task_completion_event<T>& tce)
    {
        foundation_assert(pAsyncOperation);
        HRESULT hr;

        // retrieve result from Core Async Operation
        InspectablePtr resultPtr;
        IFHR_ASSERT(pAsyncOperation->GetResult(_InfiniteTimeout, resultPtr.GetAddressOf()));

        foundation_assert(resultPtr != nullptr);
        // get type safe value
        T value;
        IFHR_ASSERT(GetValue(resultPtr, value));
        // signal completion event
        tce.set(value);
        return S_OK;
    }


    template <>
    static HRESULT _Set_task_result(
        _In_ IAsyncOperation *pAsyncOperation,
        _In_ const pplx::task_completion_event<void>& tce)
    {
        UNREFERENCED_PARAMETER(pAsyncOperation);
        tce.set();
        return S_OK;
    }

    /// <summary>
    /// create a task operation from a IAsyncOperation
    /// </summary>
    template <class T>
    static pplx::task<T> create_task(_In_ IAsyncOperation *pAsyncOperation)
    {
        foundation_assert(pAsyncOperation != nullptr);

        // our task completion event
        pplx::task_completion_event<T> tce;

        // our cancelation token source
        pplx::cancellation_token_source cts;

        AsyncOperationCompletedConnection::SetSinkOnce(pAsyncOperation, [=](ICompletedEventArgs *pEventArgs) {
            UNREFERENCED_PARAMETER(pEventArgs);

            HRESULT hr;
            ResultStatus status;
            IFHR_ASSERT(pAsyncOperation->GetStatus(&status));

            if (status == ResultStatus::Canceled)
            {
                cts.cancel();
            }
            else if (status == ResultStatus::Error)
            {
                HRESULT error;
                IFHR_ASSERT(pAsyncOperation->GetError(&error));
                tce.set_exception(com_exception(error));
            }
            else
            {
                IFHR_ASSERT(_Set_task_result(pAsyncOperation, tce));
            }
            return S_OK;
        });
        // return a task from a completion event
        return pplx::create_task(tce, cts.get_token());
    }

    /// <summary>
    /// create a task operation from a IAsyncOperation_xx
    /// </summary>
    template <class T,class TInterface>
    static pplx::task<T> create_task(_In_ TInterface *pAsyncOperation)
    {
        ComPtr<IAsyncOperation> async_operation_ptr;
        QueryInterface(pAsyncOperation, async_operation_ptr.GetAddressOf());
        return create_task<T>(async_operation_ptr.get());
    }

    /// <summary>
    /// create a task operation from a async_operation_ptr
    /// </summary>
    template <class T, class AsyncOperationPtr>
    inline static pplx::task<T> create_task(const AsyncOperationPtr& ptr )
    {
        return create_task<T>(ptr.Get());
    }

}

