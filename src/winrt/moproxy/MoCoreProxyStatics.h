/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreProxyStatics.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include <winrt/com_activation.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoCoreProxyStatics
//
// Purpose: Factory class for a IProxyStatics
//
//------------------------------------------------------------------------------
class MoCoreProxyStatics :
    public ctlwinrt::AbstractActivationFactory,
    public IProxyFactoryStatics
{
public:
    HRESULT STDMETHODCALLTYPE ToProxy(
        IInspectable *pInspectable,
        IInspectable **result) override;

    HRESULT STDMETHODCALLTYPE FromProxy(
        IInspectable *pInspectable,
        IInspectable **ppResult) override;

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if (iid == __uuidof(IProxyFactoryStatics))
        {
            *ppInterface = static_cast<IProxyFactoryStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};
