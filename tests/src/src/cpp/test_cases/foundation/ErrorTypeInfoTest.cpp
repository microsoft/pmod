/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ErrorTypeInfoTest.cpp
****/
#include "pch.h"

#include <foundation/type_info_statics_util.h>
#include <foundation/errors.h>

using namespace foundation;

class CErrorTypeInfoTest : public CUnitTestBase
{
protected:
    void Test()
    {
        ComPtr<IEnumTypeInfo> type_info_ptr;
        UT_VERIFY_HR(GetEnumTypeInfo(ErrorsEnum::TypeId, type_info_ptr.GetAddressOf()));

        ComPtr<IEnumValue> enum_value_ptr;
        UT_VERIFY_HR(type_info_ptr->GetEnumValue(Errors::E_POINTER_,enum_value_ptr.GetAddressOf()));

        UINT32 value;
        UT_VERIFY_HR(enum_value_ptr->GetValue(&value));
        UT_VERIFY_BOOL(value == Errors::E_POINTER_);

        UT_VERIFY_HR(GetEnumValue(Errors::E_BOUNDS_, enum_value_ptr.ReleaseAndGetAddressOf()));
    }

public:
	BeginTestMethodMap(CErrorTypeInfoTest)
        TestMethod(Test)
	EndTestMethodMap()

};
ImplementTestClass(CErrorTypeInfoTest);


