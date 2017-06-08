/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FastModelTest.cpp
****/
#include "pch.h"

#include <pmod/library/mtl.h>
#include <pmod/event_connection.h>

#include <pmod/library/fast_template_library.h>
#include "TestModelInterfaces.g.h"
#include "TestModelTypeInfo.g.h"
#include "TestModelPrivateInterfaces.g.h" 
#include "TestModelFastInternal.g.h" 

using namespace foundation;
using namespace pmod;
using namespace pmod::library;

class MyCommandTypes : public TestModel::_FastCommandTypes<>
{
protected:
    HRESULT ExecuteGetString(HSTRING* pResult) override
    {
        return HStringPtr(U("rodrigov")).CopyTo(pResult);
    }

};

class CFastModelTest : public CUnitTestBase
{
protected:
    void Setup() 
    {
        TestModel::RegisterTypeInfos();
        TestModel::RegisterModelTypeAdapters();
    };

    void TestFastModel()
    {
        MyCommandTypes *pCommandTypes;
        UT_VERIFY_HR(foundation::library::CreateObjectClass(
            TestModel::CommandTypes::IIDType,
            pmod::library::ObservableObjectOptions::None,
            &pCommandTypes));
        ComPtr<TestModel::ICommand_void_string> get_string_command_ptr;
        UT_VERIFY_HR(pCommandTypes->GetGetString(get_string_command_ptr.GetAddressOf()));
        UT_VERIFY_BOOL(get_string_command_ptr != nullptr);
        HStringPtr str;
        UT_VERIFY_HR(get_string_command_ptr->Execute(str.GetAddressOf()));
        UT_VERIFY_BOOL(_pal_strcmp(str.GetRawBuffer(), U("rodrigov")) == 0);

        // use reflection
        foundation::InspectablePtr value_ptr;
        UT_VERIFY_HR(pCommandTypes->GetProperty(TestModel::CommandTypes::Property_GetString, value_ptr.GetAddressOf()));
        UT_VERIFY_HR(foundation::QueryInterface(value_ptr, get_string_command_ptr.ReleaseAndGetAddressOf()));

        ctl::inspectable::ReleaseInterface(pCommandTypes);
    }

public:
	BeginTestMethodMap(CFastModelTest)
        TestMethod(TestFastModel)
	EndTestMethodMap()

};
ImplementTestClass(CFastModelTest);


