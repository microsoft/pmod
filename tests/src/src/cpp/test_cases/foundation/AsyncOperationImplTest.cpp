/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationImplTest.cpp
****/
#include "pch.h"


#include "AsyncOperationImplAdapter.h"

using namespace foundation;
using namespace foundation::library;

class CAsyncOperationImplTest : public CUnitTestBase
{
protected:
    void Test()
    {
        CAsyncOperationTest test;
        test.Test(CAsyncOperationImplFactory());
    }

public:
    BeginTestMethodMap(CAsyncOperationImplTest)
        TestMethod(Test)
    EndTestMethodMap()

};
ImplementTestClass(CAsyncOperationImplTest);


