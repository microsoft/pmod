/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectFactoryProxy.h
****/

#pragma once

#include <pmod/library/interfaces/proxy_interfaces.h>
#include <foundation/library/base_adapter_host.h>
#include <foundation/foundation_assert.h>

class CObjectFactoryProxy:
    public foundation::library::_DefaultAdapterHost
    <
        foundation::IObjectFactory,
        foundation::ctl::ImplementsInspectable<foundation::IObjectFactoryBase, &foundation::IID_IObjectFactoryBase>
    >
{
public:
    static HRESULT CreateInstance(
        foundation::IObjectFactoryBase *pSourceModelFactory,
        pmod::proxy::library::IProxyObjectFactoryClass *pProxyObjectFactoryClass,
        foundation::IObjectFactoryBase **pModelFactoryProxy)
    {
        CObjectFactoryProxy *pModelFactoryProxyClass = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CObjectFactoryProxy>::CreateInstance(&pModelFactoryProxyClass));
        pModelFactoryProxyClass->_sourceObjectFactory = pSourceModelFactory;
        pModelFactoryProxyClass->_ProxyObjectFactoryClass = pProxyObjectFactoryClass;
        *pModelFactoryProxy = static_cast<foundation::IObjectFactoryBase *>(pModelFactoryProxyClass);
        return S_OK;
    }

protected:
    // Attempt to Create a Model from our registry
    STDMETHOD( TryCreateObject )(
        REFIID iidType,
        IInspectable *pContextParameter,
        foundation::IObject **ppObject)
    {
        IFCPTR(ppObject);

        foundation_assert(_sourceObjectFactory != nullptr);
        foundation_assert(_ProxyObjectFactoryClass != nullptr);
        foundation::ComPtr<foundation::IObject> spObject;
        IFR(_sourceObjectFactory->TryCreateObject(iidType, pContextParameter, spObject.GetAddressOf()));
        if (spObject != nullptr)
        {
            foundation::InspectablePtr spValue = (foundation::IObject *)spObject;
            IFR(_ProxyObjectFactoryClass->ToProxyValue(spValue.GetAddressOfPtr()));
            IFR(foundation::QueryInterface(spValue, ppObject));
        }
        else
        {
            *ppObject = nullptr;
        }
        return S_OK;
    }

private:
    foundation::ComPtr<foundation::IObjectFactoryBase>                                _sourceObjectFactory;
    foundation::ComPtr<pmod::proxy::library::IProxyObjectFactoryClass>   _ProxyObjectFactoryClass;
};
