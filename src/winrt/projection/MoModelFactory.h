/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelFactory.h
****/

#pragma once

#include <winrt/com_activation.h>
#include "FreeThreadedMarshalerBase.h"

//------------------------------------------------------------------------------
// Class:   MoModelFactoryBase
//
// Purpose: Base Class for Factory implementation of our Classic COM objects
//
//------------------------------------------------------------------------------
template <
    class TModelFactory	// the Model Factory Interface
>
class MoModelFactoryBase :
        public TModelFactory,
        public ctlwinrt::AbstractActivationFactory,
        public CFreeThreadedMarshalerBase
{
public:
    BEGIN_IID_MAP
        UUIDOF_INTERFACE_ENTRY(TModelFactory)
    END_IID_MAP(ctlwinrt::AbstractActivationFactory)

protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if (iid == __uuidof(TModelFactory))
        {
            *ppInterface = static_cast<TModelFactory *>(this);
        }
        else if (iid == IID_IMarshal)
        {
            return QueryMarshalInterface(static_cast<foundation::ctl::ComInspectableBase *>(this), ppInterface);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   MoModelFactoryStaticsBase
//
// Purpose: Factory with Statics support
//
//------------------------------------------------------------------------------
template <
    class TModelFactory,	// the Model Factory Interface
    class TModelStatics	// the Model Statics Interface
>
class MoModelFactoryStaticsBase :
        public TModelStatics,
        public MoModelFactoryBase<TModelFactory>
{
public:
    BEGIN_IID_MAP
        UUIDOF_INTERFACE_ENTRY(TModelFactory)
        UUIDOF_INTERFACE_ENTRY(TModelStatics)
    END_IID_MAP(ctlwinrt::AbstractActivationFactory)
protected:
    __override HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface)
    {
        if(iid == __uuidof(TModelStatics))
        {
            *ppInterface = static_cast<TModelStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};
