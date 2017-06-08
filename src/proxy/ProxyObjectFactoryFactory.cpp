/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ProxyObjectFactoryFactory.cpp
****/

#include "pch.h"
#include "ProxyObjectFactory.h"

#include <foundation/library/com_ptr_singleton.h>
#include <foundation/library/activation_factory.h>

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::proxy;

typedef _ProxyObjectFactoryFactoryImplBase<> CProxyObjectFactoryFactory_BaseType;

EXTERN_C Proxy_MODEL_API HRESULT RegisterProxyIf()
{
    static bool registered;
    if (!registered)
    {
        RegisterTypeInfos();
        RegisterModelTypeAdapters();
        registered = true;
    }
    return S_OK;
}

EXTERN_C HRESULT _EnsureProxyFactory()
{
    return RegisterProxyIf();
}

class CProxyObjectFactoryFactory :
    public CProxyObjectFactoryFactory_BaseType
{
public:
    HRESULT _Initialize()
    {
        return S_OK;
    }

    static IProxyObjectFactoryFactory *GetInstance()
    {
        //_EnsureProxyServerFactory();
        return foundation::library::_GetFactoryInstance<CProxyObjectFactoryFactory>(
            []() {
            CProxyObjectFactoryFactory *pProxyObjectFactoryFactory;
            ctl::inspectable::CreateInstanceWithInitialize(&pProxyObjectFactoryFactory);
            return pProxyObjectFactoryFactory;
        });
    }
protected:
    HRESULT Create(pmod::proxy::ProxyObjectFactoryOptions options, foundation::IInspectable * workingDispatcher, foundation::IInspectable * eventDispatcher, _COM_Outptr_result_maybenull_ IProxyObjectFactory * *pResult)
    {
        CProxyObjectFactory *pProxyObjectFactory;
        IFR_ASSERT(ctl::inspectable::CreateInstanceWithInitialize(
            &pProxyObjectFactory,
            options,
            workingDispatcher,
            eventDispatcher));
        *pResult = static_cast<IProxyObjectFactory *>(pProxyObjectFactory);
        return S_OK;
    }
};

static foundation::library::_RegisterActivationFactory _factoryRegister(
    U("pmod.proxy.ProxyObjectFactory"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CProxyObjectFactoryFactory::GetInstance)
    );



