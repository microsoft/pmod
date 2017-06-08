/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyDataTest.cpp
****/
#include "pch.h"
#include "PropertyDataTest.h"
#include <foundation/value_ptr.h>
#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"
#include <foundation/type_info_statics_util.h>

using namespace foundation;
using namespace foundation::library;


void CPropertyDataTest::Test(const ITestPropertyDataFactory& factory)
{
    ComPtr<IObjectDispatch> propertyDataPtr;
    UT_VERIFY_HR(factory.Create(TestModel::TypeStruct::IIDType, propertyDataPtr.GetAddressOf()));
    UT_VERIFY_HR(propertyDataPtr->SetProperty(TestModel::TypeStruct::Property_Property2, ValuePtr(100)));
    ValuePtr valuePtr;
    UT_VERIFY_HR(propertyDataPtr->GetProperty(TestModel::TypeStruct::Property_Property2, valuePtr.GetAddressOf()));
    UT_VERIFY_BOOL(valuePtr.GetValue<UINT32>() == 100);

    ComPtr<TestModel::ITypeStruct> typeStructPtr;
    UT_VERIFY_HR(::QueryInterface(propertyDataPtr, typeStructPtr.GetAddressOf()));
    UINT32 value;
    UT_VERIFY_HR(typeStructPtr->GetProperty2(&value));
    UT_VERIFY_BOOL(value == 100);

    ComPtr<IObjectDispatchInfo> propertyDataInfoPtr;
    
    UT_VERIFY_HR(foundation::GetTypeInfoAs(TestModel::TypeStructWithMethods::IIDType, propertyDataInfoPtr.GetAddressOf()));
    ComPtr<IMethodInfo> methodInfoPtr;
    UT_VERIFY_HR(propertyDataInfoPtr->GetMethodyById(TestModel::TypeStructWithMethods::Method_VoidEmpty, methodInfoPtr.GetAddressOf()));
}
