/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryImplTest.cpp
****/
#include "pch.h"

#include <foundation/library/dictionary_impl.h>
#include <foundation/value_ptr.h>

using namespace foundation;
using namespace foundation::library;

class CDictionaryImplTest : public CUnitTestBase
{
protected:
    void Test()
    {
        ComPtr<DictionaryImpl> DictionaryPtr;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(DictionaryPtr.GetAddressOf()));

        UINT32 size;
        UT_VERIFY_HR(DictionaryPtr->GetSize(&size));
        UT_VERIFY_BOOL(size == 0);

        HStringPtr key1(U("key1"));

        ValuePtr value;

        bool replaced;
        UT_VERIFY_HR(DictionaryPtr->Insert(key1, ValuePtr(100), &replaced));
        UT_VERIFY_HR(DictionaryPtr->GetSize(&size));
        UT_VERIFY_BOOL(!replaced);
        UT_VERIFY_BOOL(size == 1);
        UT_VERIFY_HR(DictionaryPtr->Insert(key1, ValuePtr(200), &replaced));
        UT_VERIFY_BOOL(replaced);

        UT_VERIFY_HR(DictionaryPtr->Lookup(key1, value.GetAddressOf()));
        UT_VERIFY_BOOL(value == 200);
    }

public:
    BeginTestMethodMap(CDictionaryImplTest)
        TestMethod(Test)
	EndTestMethodMap()

};
ImplementTestClass(CDictionaryImplTest);


