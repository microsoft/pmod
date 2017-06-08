/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyDataImplTest.cpp
****/
#include "pch.h"

#include <foundation/library/object_dispatch_impl.h>
#include "../shared/PropertyDataTest.h"

using namespace foundation;
using namespace foundation::library;

class CPropertyDataImplTest : public CUnitTestBase
{
protected:
    void Test()
    {
        class CPropertyDataImplFactory :
            public ITestPropertyDataFactory
        {
        public:
            HRESULT Create(
                const IID& iidType,
                foundation::IObjectDispatch **ppPropertyData) const override
            {
                foundation::ComPtr<ObjectDispatchImpl> classPtr;
                IFR_ASSERT(CreateObjectDispatchImpl(classPtr.GetAddressOf(),iidType));
                *ppPropertyData = classPtr.Detach();
                return S_OK;
            }
        };

        CPropertyDataTest test;
        test.Test(CPropertyDataImplFactory());
    }

public:
    BeginTestMethodMap(CPropertyDataImplTest)
        TestMethod(Test)
    EndTestMethodMap()

};
ImplementTestClass(CPropertyDataImplTest);


