/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreEventHandlerAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

#include "AdapterProxyInterface.h"
#include <foundation/com_ptr.h>

template <
    class TCoreEventHandler, 
    class TMoEventHandler,
    class TCoreEventArgs,
    class TMoEventArgs
>
class TMoCoreEventHandlerAdapter :
    public TAdapterProxyInterface < TCoreEventHandler, TMoEventHandler >,
    public ICoreEventHandlerAdapter
{
protected:
    HRESULT STDMETHODCALLTYPE Invoke(
        foundation::IUnknown* pSender,
        TCoreEventArgs *pEventArgs) override
    {
        foundation::ComPtr<TMoEventArgs> spMoEventArgs;
        IFR_ASSERT(foundation::QueryInterface(pEventArgs, spMoEventArgs.GetAddressOf()));
        foundation::InspectablePtr spSender;
        IFR_ASSERT(foundation::QueryInterface(pSender, spSender.GetAddressOf()));

        return this->m_pSourceInterface->Invoke(spSender, spMoEventArgs);
    }

    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == __uuidof(ICoreEventHandlerAdapter))
        {
            *ppInterface = static_cast<ICoreEventHandlerAdapter *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};

typedef TMoCoreEventHandlerAdapter <
    pmod::INotifyCollectionChangedEventHandler,
    ABI::Microsoft::PropertyModel::INotifyCollectionChangedEventHandler,
    pmod::INotifyCollectionChangedEventArgs,
    ABI::Microsoft::PropertyModel::INotifyCollectionChangedEventArgs
> CMoNotifyCollectionChangedHandlerAdapter;
