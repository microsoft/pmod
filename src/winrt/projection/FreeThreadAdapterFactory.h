/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FreeThreadAdapterFactory.h
****/
#pragma once

#include <pmod/library/model_library.h>
#include <foundation/ctl/com_library.h>
#include <foundation/library/logger_macros.h>

//------------------------------------------------------------------------------
// Class:   CFreeThreadAdapterFactory
//
// Purpose: A generic Model Adapter Factory
//
//------------------------------------------------------------------------------
class CFreeThreadAdapterFactory: 
    public foundation::ctl::ImplementsInspectable<
	foundation::library::IInterfaceAdapterFactory,
	&foundation::library::IID_IInterfaceAdapterFactory>
{
public:
    static HRESULT CreateInstance(
        _Outptr_ foundation::library::IInterfaceAdapterFactory **ppModelAdapterFactory)
    {
        foundation::ctl::ComInspectableObject<CFreeThreadAdapterFactory> *pNewInstance;
        IFR(foundation::ctl::ComInspectableObject<CFreeThreadAdapterFactory>::CreateInstance(&pNewInstance));
	    *ppModelAdapterFactory = static_cast<foundation::library::IInterfaceAdapterFactory *>(pNewInstance);
        return S_OK;
    }

protected:
    // Interface foundation::library::IInterfaceAdapterFactory
    STDMETHOD(TryCreateInterfaceAdapter)(
            _In_ REFIID riid_outer,
            _In_ REFIID riid,
            _In_ IInspectable *pOuter,
            _In_ IInspectable *pInner,
            _Outptr_ IInspectable **ppInner);

private:
};
