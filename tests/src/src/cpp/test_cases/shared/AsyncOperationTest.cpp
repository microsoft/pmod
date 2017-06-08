/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationTest.cpp
****/
#include "pch.h"
#include "AsyncOperationTest.h"
#include <foundation/value_ptr.h>
#include <foundation/async_operation_connection.h>

using namespace foundation;
using namespace foundation::library;

void CAsyncOperationTest::Test(
    const ITestAsyncOperationFactory& factory,
    const IID& iidType,
    foundation::IInspectable *pResult)
{
    std::shared_ptr<ITestAsyncOperationClass> classImpl;

    ComPtr<IAsyncOperation> asyncOperationPtr;
    UT_VERIFY_HR(factory.Create(iidType, classImpl, asyncOperationPtr.ReleaseAndGetAddressOf()));
    ResultStatus status;
    UT_VERIFY_HR(asyncOperationPtr->GetStatus(&status));
    UT_VERIFY_BOOL(status == ResultStatus::Started);

    ComPtr<ICompletedEventArgs> lastCompletedEventArgsPtr;
    auto completedCallBack =
        [&](foundation::IUnknown *pSender, ICompletedEventArgs *pEventArgs) -> HRESULT {
        lastCompletedEventArgsPtr = pEventArgs;
        return S_OK;
    };

    AsyncOperationCompletedConnection connection(completedCallBack);
    connection.Attach(asyncOperationPtr);

    UT_VERIFY_HR(classImpl->SetCompleted(S_OK, pResult));
    UT_VERIFY_HR(asyncOperationPtr->GetStatus(&status));
    UT_VERIFY_BOOL(status == ResultStatus::Completed);
    UT_VERIFY_BOOL(lastCompletedEventArgsPtr != nullptr);

    ValuePtr resultPtr;
    UT_VERIFY_HR(asyncOperationPtr->GetResult(_InfiniteTimeout, resultPtr.GetAddressOf()));
    UT_VERIFY_BOOL(resultPtr.Compare(pResult) == 0);

    lastCompletedEventArgsPtr = nullptr;
    connection.Detach();
    connection.Attach(asyncOperationPtr);
    UT_VERIFY_BOOL(lastCompletedEventArgsPtr != nullptr);

    // test failed
    UT_VERIFY_HR(factory.Create(iidType, classImpl, asyncOperationPtr.ReleaseAndGetAddressOf()));
    lastCompletedEventArgsPtr = nullptr;
    connection.Attach(asyncOperationPtr);
    UT_VERIFY_HR(classImpl->SetCompleted(E_FAIL, nullptr));
    UT_VERIFY_HR(asyncOperationPtr->GetStatus(&status));
    UT_VERIFY_BOOL(status == ResultStatus::Error);
    UT_VERIFY_BOOL(lastCompletedEventArgsPtr != nullptr);

    // test cancel
    UT_VERIFY_HR(factory.Create(iidType, classImpl, asyncOperationPtr.ReleaseAndGetAddressOf()));
    lastCompletedEventArgsPtr = nullptr;
    connection.Attach(asyncOperationPtr);

    bool canceled = false;
    ComPtr<FuncInvoke0Delegate<IDelegateDispatchable>> delegate_ptr;
    UT_VERIFY_HR(CreateFuncDelegate([&]() {
        canceled = true;
        return S_OK;
    }, delegate_ptr.GetAddressOf()))

    UT_VERIFY_HR(classImpl->SetCancelDelegate(delegate_ptr));
    UT_VERIFY_HR(asyncOperationPtr->Cancel());
    UT_VERIFY_BOOL(canceled);
    UT_VERIFY_HR(asyncOperationPtr->GetStatus(&status));
    UT_VERIFY_BOOL(status == ResultStatus::Canceled);
    UT_VERIFY_BOOL(lastCompletedEventArgsPtr != nullptr);
}

void CAsyncOperationTest::Test(const ITestAsyncOperationFactory& factory)
{
    Test(factory, IAsyncOperation_Int32::GetIID(), ValuePtr(100));
}
