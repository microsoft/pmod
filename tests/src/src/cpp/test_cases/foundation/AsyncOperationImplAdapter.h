/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationImplAdapter.h
****/
#pragma once

#include <foundation/library/async_operation_impl.h>
#include "../shared/AsyncOperationTest.h"


class CAsyncOperationImplAdapter :
    public ITestAsyncOperationClass
{
public:
    CAsyncOperationImplAdapter(foundation::library::AsyncOperationImpl *pClass) :
        _pClass(pClass)
    {}

    HRESULT SetCompleted(HRESULT hr, foundation::IInspectable *pValue)
    {
        return _pClass->_SetCompleted(hr, pValue);
    }
    HRESULT SetCancelDelegate(foundation::library::IDelegateDispatchable *pValue)
    {
        return _pClass->SetCancelDelegate(pValue);
    }

private:
    foundation::library::AsyncOperationImpl *_pClass;
};

class CAsyncOperationImplFactory :
    public ITestAsyncOperationFactory
{
public:
    HRESULT Create(
        const IID& iidType,
        std::shared_ptr<ITestAsyncOperationClass>& classImpl,
        foundation::IAsyncOperation **ppAsyncOperation) const override
    {
        foundation::ComPtr<foundation::library::AsyncOperationImpl> classPtr;
        IFR_ASSERT(foundation::library::CreateAsyncOperationImpl(iidType, classPtr.GetAddressOf()));
        classImpl = std::make_shared<CAsyncOperationImplAdapter>(classPtr.Get());
        *ppAsyncOperation = classPtr.Detach();
        return S_OK;
    }
};

