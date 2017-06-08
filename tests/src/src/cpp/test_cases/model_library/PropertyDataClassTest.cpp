/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyDataClassTest.cpp
****/
#include "pch.h"

#include "../shared/PropertyDataTest.h"
#include <foundation/library/object_dispatch_class.h>

using namespace foundation;
using namespace foundation::library;


class CPropertyDataClassTest : public CUnitTestBase
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
                return CreateObjectDispatch(iidType, ObjectDispatchOptions::None, ppPropertyData);
            }
        };

        CPropertyDataTest test;
        test.Test(CPropertyDataImplFactory());
    }

public:
    BeginTestMethodMap(CPropertyDataClassTest)
        TestMethod(Test)
        EndTestMethodMap()

};
ImplementTestClass(CPropertyDataClassTest);
