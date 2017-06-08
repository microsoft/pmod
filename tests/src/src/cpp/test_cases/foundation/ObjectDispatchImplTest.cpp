/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchImplTest.cpp
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

class CTypeStruct :
    public TestModel::_TypeStructImplBase<>
{
protected:
    CTypeStruct() :
        _property2(0)
    {}
public:
    HRESULT GetProperty2(UINT32* pValue) override
    {
        *pValue = _property2;
        return S_OK;
    }
    HRESULT SetProperty2(UINT32 value) override
    {
        _property2 = value;
        return S_OK;
    }
private:
    UINT32 _property2;
};

class CTypeStructWithMethods :
    public TestModel::_TypeStructWithMethodsImplBase<>
{
public:
    HRESULT ReturnInt(INT32 arg, INT32* pResult) override
    {
        *pResult = arg;
        return S_OK;
    }
};


class CObjectDispatchImplTest : public CUnitTestBase
{
protected:

    void TypeStructImpl()
    {
        ComPtr<CTypeStruct> typeStructClassPtr;
        UT_VERIFY_HR(foundation::library::CreateObjectImpl(typeStructClassPtr.GetAddressOf()));

        ComPtr<TestModel::ITypeStruct> typeStructPtr;
        UT_VERIFY_HR(foundation::QueryInterface(typeStructClassPtr->CastToInspectable(), typeStructPtr.GetAddressOf()));

        UT_VERIFY_HR(typeStructPtr->SetProperty2(100));
        UINT32 value;
        UT_VERIFY_HR(typeStructPtr->GetProperty2(&value));
        UT_VERIFY_BOOL(value == 100);

        ValuePtr valuePtr;
        UT_VERIFY_HR(typeStructPtr->GetProperty(TestModel::TypeStruct::Property_Property2, valuePtr.GetAddressOf()));
        UT_VERIFY_BOOL(valuePtr.GetValue<UINT32>() == 100);
    }

    void TypeStructWithMethodsImpl()
    {
        ComPtr<CTypeStructWithMethods> typeStructWithMethodsClassPtr;
        UT_VERIFY_HR(foundation::library::CreateObjectImpl(typeStructWithMethodsClassPtr.GetAddressOf()));

        ComPtr<TestModel::ITypeStructWithMethods> typeStructWithMethodsPtr;
        UT_VERIFY_HR(foundation::QueryInterface(typeStructWithMethodsClassPtr->CastToInspectable(), typeStructWithMethodsPtr.GetAddressOf()));

        INT32 value;
        UT_VERIFY_HR(typeStructWithMethodsPtr->ReturnInt(100,&value));
        UT_VERIFY_BOOL(value == 100);

        ComPtr<IObjectTypeInfo> objectTypeInfoPtr;
        UT_VERIFY_HR(typeStructWithMethodsPtr->GetTypeInfo(objectTypeInfoPtr.GetAddressOf()));

        ValuePtr params(100);
        ValuePtr resultPtr;
        UT_VERIFY_HR(typeStructWithMethodsPtr->InvokeMethod(
            TestModel::TypeStructWithMethods::Method_ReturnInt,
            1,
            params.GetAddressOfPtr(),
            resultPtr.GetAddressOf()));

        UT_VERIFY_BOOL(resultPtr.GetValue<UINT32>() == 100);
    }

public:
    BeginTestMethodMap(CObjectDispatchImplTest)
        TestMethod(TypeStructImpl)
        TestMethod(TypeStructWithMethodsImpl)
    EndTestMethodMap()

};
ImplementTestClass(CObjectDispatchImplTest);


