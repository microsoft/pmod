/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PplxTaskImplTest.cpp
****/
#include "pch.h"

#include "AsyncOperationImplAdapter.h"
#include "../shared/PplxTasksTest.h"

using namespace foundation;
using namespace foundation::library;


class CPplxTasksImplTest : public CUnitTestBase
{
protected:
    void Test()
    {
        CPplxTasks test;
        test.Test(CAsyncOperationImplFactory());
    }

public:
    BeginTestMethodMap(CPplxTasksImplTest)
        TestMethod(Test)
    EndTestMethodMap()

};
ImplementTestClass(CPplxTasksImplTest);


