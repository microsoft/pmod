/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectFactoryTest.cpp
****/
#include "pch.h"
#include <foundation/library/object_factory_util.h>
#include <foundation/library/base_object_impl.h>

using namespace foundation;
using namespace foundation::library;

// {607F5958-A1F7-472C-AB29-FEE55EB017F0}
static const GUID IID_TestObjectClass =
{ 0x607f5958, 0xa1f7, 0x472c,{ 0xab, 0x29, 0xfe, 0xe5, 0x5e, 0xb0, 0x17, 0xf0 } };

class CObjectFactoryTest : public CUnitTestBase
{
protected:
    static HRESULT STDMETHODCALLTYPE CreateObjectTest(
        _In_opt_ foundation::IInspectable* contextParameter,
        _COM_Outptr_ IObject **ppNewInstance)
    {
        typedef _ObjectImplBase<IObject, IObject> Object_Type;
        Object_Type *p;
        CreateObjectImpl(&p);
        *ppNewInstance = p;
        return S_OK;
    }
    void Test()
    {
        UT_VERIFY_HR(RegisterObjectFactory(IID_TestObjectClass, CreateObjectTest));

        ComPtr<IObject> objectPtr;
        UT_VERIFY_HR(CreateObjectFromFactory(IID_TestObjectClass, objectPtr.GetAddressOf()));
        UINT32 uniqueId;
        UT_VERIFY_HR(objectPtr->GetUniqueId(&uniqueId));

        UT_VERIFY_HR(UnregisterObjectFactory(IID_TestObjectClass));

        HRESULT hr;
        hr = CreateObjectFromFactory(IID_TestObjectClass, objectPtr.ReleaseAndGetAddressOf());
        UT_VERIFY_BOOL(FAILED(hr));
    }

public:
    BeginTestMethodMap(CObjectFactoryTest)
        TestMethod(Test)
    EndTestMethodMap()

};
ImplementTestClass(CObjectFactoryTest);
