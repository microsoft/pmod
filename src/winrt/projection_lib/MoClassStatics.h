/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoClassStatics.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include <winrt/com_activation.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoPropertyModelInstanceStatics
//
// Purpose: Factory class for a PropertyModelInstance
//
//------------------------------------------------------------------------------
class MoPropertyModelInstanceStatics :
    public ctlwinrt::AbstractActivationFactory,
    public IPropertyModelInstanceStatics
{
public:
    HRESULT STDMETHODCALLTYPE get_ObjectFactory( 
        IObjectFactory **value) override;
    HRESULT STDMETHODCALLTYPE get_Logger( 
        ILogger **value);

    HRESULT STDMETHODCALLTYPE get_EventDispatcher(IDispatcher **value) override;
    HRESULT STDMETHODCALLTYPE put_EventDispatcher(IDispatcher *value) override;

    HRESULT STDMETHODCALLTYPE get_WorkingDispatcher(IDispatcher **value) override;
    HRESULT STDMETHODCALLTYPE put_WorkingDispatcher(IDispatcher *value) override;

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if(iid == __uuidof(IPropertyModelInstanceStatics))
        {
            *ppInterface = static_cast<IPropertyModelInstanceStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};
