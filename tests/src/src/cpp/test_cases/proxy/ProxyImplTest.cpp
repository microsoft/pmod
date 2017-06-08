/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ProxyImplTest.cpp
****/
#include "pch.h"

#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"
#include "TestModelImplInternal.g.h"
#include <pmod/proxy/proxy_util.h>

#include <pmod/library/observable_collection_impl.h>
#include <pmod/library/observable_object_impl.h>

using namespace foundation;
using namespace pmod;
using namespace pmod::library;
using namespace pmod::proxy;

class CProxyImplTest : public CUnitTestBase
{
protected:
    void Test()
    {
        ComPtr<IProxyObjectFactory> spProxyObjectFactory;
        UT_VERIFY_HR(CreateProxyObjectFactory(
            ProxyObjectFactoryOptions::NoPurgeState,
            nullptr,
            nullptr,
            spProxyObjectFactory.GetAddressOf()));

        ComPtr<ObservableVectorImplInt32> vec_int32;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(vec_int32.GetAddressOf()));

        ComPtr<IObject> spProxy;
        spProxyObjectFactory->ToProxyObject(
            vec_int32.Get(),
            spProxy.GetAddressOf());
        ComPtr<IObject> spProxy2;
        spProxyObjectFactory->ToProxyObject(
            vec_int32.Get(),
            spProxy2.GetAddressOf());
        UT_VERIFY_BOOL(spProxy.Get() == spProxy2.Get());

        ComPtr<ObservableObjectImpl> typePropertiesImplPtr;
        UT_VERIFY_HR(CreateObservableObjectImpl(typePropertiesImplPtr.GetAddressOf(), TestModel::TypeProperties::IIDType));
        spProxyObjectFactory->ToProxyObject(
            typePropertiesImplPtr.Get(),
            spProxy.ReleaseAndGetAddressOf());

        ComPtr<TestModel::ITypeProperties> typePropertiesProxyPtr;
        UT_VERIFY_HR(::QueryInterface(spProxy, typePropertiesProxyPtr.GetAddressOf()));

    }

public:
    BeginTestMethodMap(CProxyImplTest)
        TestMethod(Test)
    EndTestMethodMap()

};
ImplementTestClass(CProxyImplTest);


