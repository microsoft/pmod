/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandImplTest.cpp
****/
#include "pch.h"

#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"
#include "TestModelImplInternal.g.h"
#include <pmod/library/command_impl.h>
#include <foundation/library/type_info_factory_util.h>
#include <foundation/value_ptr.h>

using namespace foundation;
using namespace pmod;
using namespace pmod::library;

class CCommandImplTest : public CUnitTestBase
{
protected:
    void TestSimpleCommand()
    {
        class CSimpleCommand :
            public _SimpleCommandImplBase
        {
        public:
            STDMETHOD(Execute)()
            {
                return S_OK;
            }
        };

        ComPtr<CSimpleCommand> simpleCommandPtr;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(simpleCommandPtr.GetAddressOf()));
        UT_VERIFY_BOOL(IsTypeOf<ICommandBase>(simpleCommandPtr->CastToInspectable()));
        UT_VERIFY_BOOL(IsTypeOf<ICommand>(simpleCommandPtr->CastToInspectable()));
        UT_VERIFY_BOOL(IsTypeOf<ISimpleCommand>(simpleCommandPtr->CastToInspectable()));
        UT_VERIFY_HR(simpleCommandPtr->Execute());
    }

    void TestGetStringCommand()
    {
        class CGetStringCommand :
            public _CommandResultValueImplBase<TestModel::ICommand_void_string>
        {
        public:
            STDMETHOD(Execute)(HSTRING *value)
            {
                return HStringPtr(U("hello-world")).CopyTo(value);
            }
        };
        ComPtr<CGetStringCommand> commandClassPtr;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(commandClassPtr.GetAddressOf()));
        ComPtr<TestModel::ICommand_void_string> commandPtr;
        UT_VERIFY_HR(QueryInterface(commandClassPtr->CastToInspectable(), commandPtr.GetAddressOf()));
        HStringPtr value;
        UT_VERIFY_HR(commandPtr->Execute(value.GetAddressOf()));
        UT_VERIFY_BOOL(_pal_strcmp(value.GetRawBuffer(), U("hello-world")) == 0);
    }

public:
    BeginTestMethodMap(CCommandImplTest)
        TestMethod(TestSimpleCommand)
        TestMethod(TestGetStringCommand)
    EndTestMethodMap()

};
ImplementTestClass(CCommandImplTest);


