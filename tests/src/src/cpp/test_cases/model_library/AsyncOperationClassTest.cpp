/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationClassTest.cpp
****/
#include "pch.h"

#include "../shared/AsyncOperationTest.h"
#include <foundation/library/async_operation_class.h>

using namespace foundation;
using namespace foundation::library;

class CAsyncOperationClassAdapter :
    public ITestAsyncOperationClass
{
public:
    CAsyncOperationClassAdapter(IAsyncOperationClass *pClass) :
        _pClass(pClass)
    {}

    HRESULT SetCompleted(HRESULT hr, foundation::IInspectable *pValue)
    {
        return _pClass->SetCompleted(hr, pValue);
    }
    HRESULT SetCancelDelegate(foundation::library::IDelegateDispatchable *pValue)
    {
        return _pClass->SetCancelDelegate(pValue);
    }
private:
    IAsyncOperationClass *_pClass;
};


class CAsyncOperationClassTest : public CUnitTestBase
{
protected:
    void Test()
    {
        class CFactory :
            public ITestAsyncOperationFactory
        {
        public:
            HRESULT Create(
                const IID& iidType,
                std::shared_ptr<ITestAsyncOperationClass>& classImpl,
                foundation::IAsyncOperation **ppAsyncOperation) const override
            {
                IFR_ASSERT(CreateAsyncOperationClass(iidType, ppAsyncOperation));
                classImpl = std::make_shared<CAsyncOperationClassAdapter>(CastTo<IAsyncOperationClass>(*ppAsyncOperation));
                return S_OK;
            }
        };
        CAsyncOperationTest test;
        test.Test(CFactory());
    }

public:
    BeginTestMethodMap(CAsyncOperationClassTest)
        TestMethod(Test)
        EndTestMethodMap()

};
ImplementTestClass(CAsyncOperationClassTest);


