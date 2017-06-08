/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectImplTest.cpp
****/
#include "pch.h"

#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"
#include "TestModelImplInternal.g.h"
#include <pmod/library/observable_object_impl.h>
#include <foundation/library/type_info_factory_util.h>
#include <foundation/value_ptr.h>

using namespace foundation;
using namespace pmod;
using namespace pmod::library;

class CTypeProperties :
    public TestModel::_TypePropertiesImplBase<>
{
protected:
    CTypeProperties() :
        _boolValue(false)
    {}
public:
    HRESULT GetBooleanProperty(bool* pValue) override
    {
        *pValue = _boolValue;
        return S_OK;
    }
    HRESULT SetBooleanProperty(bool value) override
    {
        _boolValue = value;
        this->FirePropertyChangedWithCallback(TestModel::TypeProperties::Property_BooleanProperty);
        return S_OK;
    }
private:
    bool _boolValue;
};

HRESULT CreateTypePropertiesImpl(TestModel::ITypeProperties **ppTypeProperties)
{
    ComPtr<CTypeProperties> typePropertiesPtr;
    IFR_ASSERT(foundation::library::CreateObjectImpl(typePropertiesPtr.GetAddressOf()));
    *ppTypeProperties = typePropertiesPtr.Detach();
    return S_OK;
}

class CObservableObjectImplTest : public CUnitTestBase
{
protected:
    void TypePropertiesFastImpl()
    {
        ComPtr<TestModel::ITypeProperties> typePropertiesPtr;
        UT_VERIFY_HR(CreateTypePropertiesImpl(typePropertiesPtr.GetAddressOf()));
        UT_VERIFY_HR(typePropertiesPtr->SetBooleanProperty(true));

        ValuePtr value;
        UT_VERIFY_HR(typePropertiesPtr->GetProperty(
            TestModel::TypeProperties::Property_BooleanProperty,
            value.GetAddressOf()));
        UT_VERIFY_BOOL(value.GetValue<bool>() == true);
    }

    void TypePropertiesImpl()
    {
        ComPtr<ObservableObjectImpl> typePropertiesImplPtr;
        UT_VERIFY_HR(CreateObservableObjectImpl(typePropertiesImplPtr.GetAddressOf(), TestModel::TypeProperties::IIDType));

        ComPtr<TestModel::ITypeProperties> typePropertiesPtr;
        UT_VERIFY_HR(QueryInterface(typePropertiesImplPtr->CastToInspectable(), typePropertiesPtr.GetAddressOf()));

        UT_VERIFY_HR(typePropertiesPtr->SetIntProperty(100));
        INT32 value;
        UT_VERIFY_HR(typePropertiesPtr->GetIntProperty(&value));
        UT_VERIFY_BOOL(value == 100);
    }

public:
    BeginTestMethodMap(CObservableObjectImplTest)
        TestMethod(TypePropertiesFastImpl)
        TestMethod(TypePropertiesImpl)
    EndTestMethodMap()

};
ImplementTestClass(CObservableObjectImplTest);


