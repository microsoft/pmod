/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAdapterProxyInterface.h
****/

#pragma once

#include "AdapterProxyInterface.h"

template <
    class TCoreInterface,
    class TCoreBaseInterface,
    class TMoInterface,
    class TMoBaseInterface
>
class TMoAdapterProxyInterfaceWithBase :
    public TAdapterProxyInterface<TCoreInterface,TMoInterface>
{
public:
    HRESULT SetSourceInterface(TMoInterface *pMoInterface)
    {
        IFR_ASSERT(__super::SetSourceInterface(pMoInterface));
        IFR_ASSERT(foundation::QueryInterface(pMoInterface,&this->m_pMoBaseInterface));
        return S_OK;
    }

protected:
    TMoAdapterProxyInterfaceWithBase():
        m_pMoBaseInterface(nullptr)
    {
    }


    void FinalRelease() override
    {
        RELEASE_INTERFACE(m_pMoBaseInterface);
        __super::FinalRelease();
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == TCoreBaseInterface::GetIID() )
        {
            *ppInterface = static_cast<TCoreBaseInterface *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
protected:
    TMoBaseInterface *m_pMoBaseInterface;
};

