/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestFactory.cpp
****/

#include "TestModelFastInternal.g.h"
#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::library;

HRESULT RegisterTypeInfosIf()
{
    static bool registered;
    if (!registered)
    {
        TestModel::RegisterTypeInfos();
        TestModel::RegisterModelTypeAdapters();

        registered = true;
    }
    return S_OK;
}

// factory entry points
extern HRESULT CreateMethodSupport(IDispatcher *pWorkingDisptcher, TestModel::IMethodSupport **pp);
extern HRESULT CreateTypePropertiesTest(IDispatcher *pWorkingDisptcher, TestModel::ITypePropertiesTest **pp);

typedef TestModel::_FastTestFactoryBase CTestFactory_BaseType;

class CTestFactory :
    public CTestFactory_BaseType
{
public:
    static TestModel::ITestFactory *GetInstance();
private:
    HRESULT CreateTypePropertiesInternal(TestModel::ITypeProperties ** pResult) override;
    HRESULT CreateMethodSupportInternal(foundation::IInspectable * workingDispatcher, TestModel::IMethodSupport ** pResult) override;
    HRESULT CreateTypePropertiesTestInternal(foundation::IInspectable * workingDispatcher, TestModel::ITypePropertiesTest ** pResult) override;
};

TestModel::ITestFactory *CTestFactory::GetInstance()
{
    RegisterTypeInfosIf();
    return foundation::library::_GetFactoryInstance<CTestFactory>(
        []() {
        CTestFactory *pFactory;
        CreateObjectClass(
            TestModel::TestFactory::IIDType,
            (ObservableObjectOptions)(FoundationClassOptions_NoTracking),
            &pFactory);
        return pFactory;
    });
}

HRESULT CTestFactory::CreateTypePropertiesInternal(TestModel::ITypeProperties ** pResult)
{
    typedef TestModel::_FastTypePropertiesBase CTypeProperties;
    CTypeProperties *pTypeProperties;
    CreateObjectClass(
        TestModel::TypeProperties::IIDType,
        pmod::library::ObservableObjectOptions::None,
        &pTypeProperties);
    *pResult = pTypeProperties;
    return S_OK;
}

HRESULT CTestFactory::CreateMethodSupportInternal(foundation::IInspectable * workingDispatcher, TestModel::IMethodSupport ** pResult)
{
    ComPtr<IDispatcher> dispatcherPtr;
    IFR_ASSERT(QueryInterfaceIf(workingDispatcher, dispatcherPtr.GetAddressOf()));
    return ::CreateMethodSupport(dispatcherPtr, pResult);
}

HRESULT CTestFactory::CreateTypePropertiesTestInternal(foundation::IInspectable * workingDispatcher, TestModel::ITypePropertiesTest ** pResult)
{
    ComPtr<IDispatcher> dispatcherPtr;
    IFR_ASSERT(QueryInterfaceIf(workingDispatcher, dispatcherPtr.GetAddressOf()));
    return ::CreateTypePropertiesTest(dispatcherPtr, pResult);
}

static foundation::library::_RegisterActivationFactory _factoryRegister(
    U("testModel.Test"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CTestFactory::GetInstance)
    );
