/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PplxTasksTest.cpp
****/
#include "pch.h"
#include "PplxTasksTest.h"
#include <foundation/pplxtasks.h>
#include <foundation/value_ptr.h>
#include <pal/library/signal.h>

using namespace foundation;
using namespace foundation::library;

static void AsyncAdd(
    const ITestAsyncOperationFactory& factory, 
    UINT32 val1, 
    UINT32 val2, 
    ComPtr<IAsyncOperation>& ptr)
{
    std::shared_ptr<ITestAsyncOperationClass> classImpl;
    factory.Create(IAsyncOperation_Int32::GetIID(), classImpl, ptr.ReleaseAndGetAddressOf());

    pplx::task<void> add_task([=]() {

        // complete the async operation
        classImpl->SetCompleted(S_OK, ValuePtr(val1 + val2));
    });
}

static void CancelTask(
    const ITestAsyncOperationFactory& factory,
    ComPtr<IAsyncOperation>& ptr)
{
    std::shared_ptr<ITestAsyncOperationClass> classImpl;
    factory.Create(IAsyncOperation::GetIID(), classImpl, ptr.ReleaseAndGetAddressOf());

    auto sig = std::make_shared<pal::library::Signal>();
    pplx::task<void> cancel_task([=]() {
        sig->Wait();
    });

    ComPtr<FuncInvoke0Delegate<IDelegateDispatchable>> delegate_ptr;
    CreateFuncDelegate([=]() {
        sig->set();
        return S_OK;
    }, delegate_ptr.GetAddressOf());
    classImpl->SetCancelDelegate(delegate_ptr);
}

static void FailedTask(
    const ITestAsyncOperationFactory& factory,
    ComPtr<IAsyncOperation>& ptr)
{
    std::shared_ptr<ITestAsyncOperationClass> classImpl;
    factory.Create(IAsyncOperation::GetIID(), classImpl, ptr.ReleaseAndGetAddressOf());

    pplx::task<void> add_task([=]() {

        // complete the async operation
        classImpl->SetCompleted(E_NOTIMPL, nullptr);
    });
}

void CPplxTasks::Test(const ITestAsyncOperationFactory& factory)
{
    ComPtr<IAsyncOperation> ptr1,ptr2;
    AsyncAdd(factory, 100, 150, ptr1);

    auto task =
        foundation::create_task<UINT32>(ptr1).then([&](UINT32 addedValues) {
        AsyncAdd(factory, addedValues, 200, ptr2);

        return foundation::create_task<UINT32>(ptr2);
    });

    UINT32 final = task.get();
    UT_VERIFY_BOOL(final == 450);

    CancelTask(factory, ptr1);

    bool isCanceled = false;
    auto task2 = foundation::create_task<void>(ptr1).then([&](pplx::task<void> task) {
        try
        {
            // task returned has to be canceled
            task.get();
        }
        catch (pplx::task_canceled&)
        {
            isCanceled = true;
            throw;
        }
    });
    ptr1->Cancel();
    // wait for cancellation to happen
    pplx::task_status status = task2.wait();

    UT_VERIFY_BOOL(status == pplx::task_status::canceled);
    UT_VERIFY_BOOL(isCanceled);

    FailedTask(factory, ptr1);

    HRESULT error = S_FALSE;
    auto task3 = foundation::create_task<void>(ptr1).then([&](pplx::task<void> task) {
        try
        {
            task.get();
        }
        catch (foundation::com_exception& e)
        {
            error = e.result();
        }
    });
    // wait for failure
    status = task3.wait();
    UT_VERIFY_BOOL(error == E_NOTIMPL);

}
