/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoDispatcherStatics.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include <winrt/com_activation.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoDispatcherStatics
//
// Purpose: Factory class for a Dispatcher
//
//------------------------------------------------------------------------------
class MoDispatcherStatics :
    public ctlwinrt::AbstractActivationFactory,
    public IDispatcherStatics
{
public:
    HRESULT STDMETHODCALLTYPE CreateDispatcher( 
            ABI::Windows::UI::Core::ICoreDispatcher *coreDisptacher,
            IDispatcher **instance) override;

    HRESULT STDMETHODCALLTYPE CreateThreadingPoolDispatcher( 
            ABI::Microsoft::PropertyModel::IDispatcher **instance) override;

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if(iid == __uuidof(IDispatcherStatics))
        {
            *ppInterface = static_cast<IDispatcherStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};
