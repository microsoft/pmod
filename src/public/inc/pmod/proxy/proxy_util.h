/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:proxy_util.h
****/
#pragma once

#include <Proxy/ProxyInterfaces.g.h>
#include <foundation/library/interfaces/dispatcher.h>

#if __APPLE__

#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
// define static lib usage for iOS
#define PROXY_STATIC_LIB 1
#endif

#endif

#if defined(PROXY_STATIC_LIB)
EXTERN_C HRESULT _EnsureProxyFactory();
#endif

namespace pmod   {

    namespace proxy
    {
        inline IProxyObjectFactoryFactory *GetProxyObjectFactoryFactory()
        {
#if defined(PROXY_STATIC_LIB)
            _EnsureProxyFactory();
#endif
            return foundation::GetActivationFactoryWithCast<IProxyObjectFactoryFactory>(U("pmod.proxy.ProxyObjectFactory"));
        }

        static HRESULT CreateProxyObjectFactory(
            _In_ ProxyObjectFactoryOptions proxyType,
            _In_opt_ foundation::library::IDispatcher *pWorkingDispatcher,
            _In_opt_ foundation::library::IDispatcher *pEventDispatcher,
            _COM_Outptr_ IProxyObjectFactory **ppProxyObjectFactory)
        {
            IFCPTR_ASSERT(ppProxyObjectFactory);

            return GetProxyObjectFactoryFactory()->Create(
                proxyType,
                reinterpret_cast<foundation::IInspectable *>(pWorkingDispatcher),
                reinterpret_cast<foundation::IInspectable *>(pEventDispatcher),
                ppProxyObjectFactory);
        }
    }
}
