/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoJsonUtils.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"
#include <winrt/com_activation.h>

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoJsonUtilsStatics
//
// Purpose: Factory class for a PropertyModelInstance
//
//------------------------------------------------------------------------------
class MoJsonUtilsStatics :
    public ctlwinrt::AbstractActivationFactory,
    public IJsonUtilsStatics
{
public:
    HRESULT STDMETHODCALLTYPE get_StreamProvider(
        IStreamProvider **value) override;

    HRESULT STDMETHODCALLTYPE ToJson(
            IInspectable *pInspectable,
            HSTRING *result) override;
                        
    HRESULT STDMETHODCALLTYPE FromJson( 
            HSTRING json,
            IInspectable **ppResult) override;

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if(iid == __uuidof(IJsonUtilsStatics))
        {
            *ppInterface = static_cast<IJsonUtilsStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};
