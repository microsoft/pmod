/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AppClassFactory.cpp
****/
#include "DemoAppFastInternal.g.h"

#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::library;

extern void RegisterDemoAppIf();

EXTERN_C HRESULT  STDAPICALLTYPE CreateDemoAppWithDispatcher(
    foundation::library::IDispatcher *pDispatcher,
    IObservableObject **ppObservableObject);

typedef DemoApp::_FastAppDemoFactoryBase CAppDemoFactory_BaseType;

class CAppDemoFactory :
    public CAppDemoFactory_BaseType
{
public:
    static DemoApp::IAppDemoFactory *GetInstance();
private:
    HRESULT CreateAppDemoInternal(
        foundation::IInspectable * workingDispatcher,
        DemoApp::IAppDemo ** pResult) override;
};

DemoApp::IAppDemoFactory *CAppDemoFactory::GetInstance()
{
    return foundation::library::_GetFactoryInstance<CAppDemoFactory>(
        []() {
        RegisterDemoAppIf();
        CAppDemoFactory *pFactory;
        CreateObjectClass(
            DemoApp::AppDemoFactory::IIDType,
            (ObservableObjectOptions)(FoundationClassOptions_NoTracking),
            &pFactory);
        return pFactory;
    });
}

HRESULT CAppDemoFactory::CreateAppDemoInternal(
    foundation::IInspectable * workingDispatcher, 
    DemoApp::IAppDemo ** pResult)
{
    ComPtr<IDispatcher> dispatcherPtr;
    IFR_ASSERT(QueryInterfaceIf(workingDispatcher, dispatcherPtr.GetAddressOf()));

    ComPtr<IObservableObject> demoAppPtr;
    IFR_ASSERT(CreateDemoAppWithDispatcher(dispatcherPtr, demoAppPtr.GetAddressOf()));

    return ::QueryInterface(demoAppPtr, pResult);
}

static foundation::library::_RegisterActivationFactory _factoryRegister(
    U("samples.DemoApp"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CAppDemoFactory::GetInstance)
    );
