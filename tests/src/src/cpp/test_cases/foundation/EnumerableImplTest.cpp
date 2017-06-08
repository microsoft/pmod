/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumerableImplTest.cpp
****/
#include "pch.h"

#include <foundation/library/enumerable_impl.h>
#include <foundation/value_ptr.h>

using namespace foundation;
using namespace foundation::library;

class CEnumerableImplTest : public CUnitTestBase
{
protected:
    void Test()
    {
        ComPtr<EnumerableVectorImplInt32> enumInt32Ptr;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(enumInt32Ptr.GetAddressOf()));

        enumInt32Ptr->GetItems() = { 10,20,30 };

        ComPtr <IIterator_impl<INT32>> iteratorInt32Ptr;
        UT_VERIFY_HR(enumInt32Ptr->First(iteratorInt32Ptr.GetAddressOf()));

        bool hasCurrent = true;
        UINT32 index = 0;
        while (hasCurrent)
        {
            INT32 value;
            iteratorInt32Ptr->GetCurrent(&value);

            UT_VERIFY_BOOL(value == enumInt32Ptr->GetItems()[index]);
            iteratorInt32Ptr->MoveNext(&hasCurrent);
            ++index;
        }

        ComPtr<EnumerableVectorImpl> enumInspectablePtr;
        UT_VERIFY_HR(CreateEnumerableVectorImpl(IID_IEnumerable_Int32,enumInspectablePtr.GetAddressOf()));

        enumInspectablePtr->GetItems().push_back(ValuePtr(100));
        enumInspectablePtr->GetItems().push_back(ValuePtr(200));

        ComPtr<IEnumerable_Int32> enumerableInt32Ptr;
        ::QueryInterface(enumInspectablePtr->CastToInspectable(), enumerableInt32Ptr.GetAddressOf());

        UT_VERIFY_HR(enumerableInt32Ptr->First(iteratorInt32Ptr.ReleaseAndGetAddressOf()));
        hasCurrent = true;
        index = 0;
        while (hasCurrent)
        {
            INT32 value;
            iteratorInt32Ptr->GetCurrent(&value);

            ValuePtr valuePtr(enumInspectablePtr->GetItems()[index]);

            UT_VERIFY_BOOL(valuePtr.GetValue<INT32>() == value);
            iteratorInt32Ptr->MoveNext(&hasCurrent);
            ++index;
        }

    }

public:
    BeginTestMethodMap(CEnumerableImplTest)
        TestMethod(Test)
	EndTestMethodMap()

};
ImplementTestClass(CEnumerableImplTest);


