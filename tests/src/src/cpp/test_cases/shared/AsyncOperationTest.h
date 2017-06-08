/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationTest.h
****/
#pragma once

#include <foundation/interfaces/async_operation.h>
#include <foundation/library/interfaces/dispatcher.h>
#include <memory>

class ITestAsyncOperationClass
{
public:
    virtual HRESULT SetCompleted(HRESULT hr,foundation::IInspectable *pValue) = 0;
    virtual HRESULT SetCancelDelegate(foundation::library::IDelegateDispatchable *pValue) = 0;
};

class ITestAsyncOperationFactory
{
public:
    virtual HRESULT Create(
        const IID& iidType,
        std::shared_ptr<ITestAsyncOperationClass>& classImpl,
        foundation::IAsyncOperation **ppAsyncOperation) const = 0;
};

class CAsyncOperationTest
{
public:
    void Test(const ITestAsyncOperationFactory& factory);
private:
    void Test(
        const ITestAsyncOperationFactory& factory,
        const IID& iidType,
        foundation::IInspectable *pResult);
};
