/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectFactoryLibTest.cpp
****/
#include "pch.h"
#include <foundation/library/object_factory_util.h>
#include "TestModelInterfaces.g.h"
#include "TestModelTypeInfo.g.h"

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::library;

class CObjectFactoryLibTest : public CUnitTestBase
{
protected:
    void Test()
    {
        // activate a generic ITypeProperties
        ComPtr<TestModel::ITypeProperties> typePropertiesPtr;
        UT_VERIFY_HR(ActivateObject(TestModel::TypeProperties::IIDType,nullptr, typePropertiesPtr.GetAddressOf()));
        // activate a IPropertyData
        ComPtr<IObjectDispatch> propertyDataPtr;
        UT_VERIFY_HR(ActivateObject(TestModel::TypeStruct::IIDType, nullptr, propertyDataPtr.GetAddressOf()));
        // activate a ICommand
        ComPtr<ICommand> commandPtr;
        UT_VERIFY_HR(ActivateObject(pmod::SimpleCommandTypeInfo::IIDType, nullptr, commandPtr.GetAddressOf()));
        // activate a ICollection
        ComPtr<IObservableCollection> observableCollectionPtr;
        UT_VERIFY_HR(ActivateObject(pmod::IObservableCollection_Inspectable::GetIID(), nullptr, observableCollectionPtr.GetAddressOf()));
    }

public:
    BeginTestMethodMap(CObjectFactoryLibTest)
        TestMethod(Test)
    EndTestMethodMap()

};
ImplementTestClass(CObjectFactoryLibTest);
