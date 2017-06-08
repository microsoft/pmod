/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FreeThreadAdapterFactory.cpp
****/
#include "pch.h"

#include "FreeThreadAdapterFactory.h"
#include "FreeThreadedMarshalerBase.h"
#include <foundation/library/adapter_base.h>

class free_thread_not_supported_adapter:
    public foundation::NotSupportedAdapter<&IID_IAgileObject>
{
protected:
    STDMETHOD(QueryAdapterInterface)(REFIID iid, foundation::IUnknown **ppInterface)
    {
        if (iid == IID_IMarshal)
        {
            return foundation::E_ADAPTER_INTERFACE_NOT_SUPPORTED;
        }
        else
        {
            return __super::QueryAdapterInterface(iid, ppInterface);
        }
        return S_OK;
    }
};
//+-----------------------------------------------------------------------------
// Class:   CFreeThreadedMarshalerAdapter
//
// Purpose: Class Adapter for Agile objects support
//------------------------------------------------------------------------------
class CFreeThreadedMarshalerAdapter :
    public foundation::AdapterBase<IUnknown,IAgileObject,&IID_IAgileObject>,
    protected CFreeThreadedMarshalerBase
{
protected:
    CFreeThreadedMarshalerAdapter()
    {
    }

    // Interface IModelAdapter
    STDMETHOD (QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface) 
    {
        if (iid == IID_IMarshal)
        {
            return QueryMarshalInterface(this->m_pOuter,ppInterface);
        }
        else
        {
            return __super::QueryAdapterInterface(iid,ppInterface);
        }

        return S_OK;
    }
};

STDMETHODIMP CFreeThreadAdapterFactory::TryCreateInterfaceAdapter(
            _In_ REFIID riid_outer,
            _In_ REFIID riid,
            _In_ IInspectable *pOuter,
            _In_ IInspectable *pInner,
            _Outptr_ IInspectable **ppInner)
{
    if(riid == IID_IAgileObject || riid==IID_IMarshal)
    {
        bool isFreeThreadApartment = true;

        // check the IFoundationClassBase interface that will tell me if
        // a class is 'FreeThread'
        foundation::library::IFoundationClassBase *pFoundationClassBase = nullptr;
        if (SUCCEEDED(foundation::QueryWeakReference(pInner, &pFoundationClassBase)))
        {
            UINT16 options;
            pFoundationClassBase->GetFoundationOptions(&options);
            isFreeThreadApartment = (options &
                foundation::library::FoundationClassOptions_IsFreeThreadApartment) != 0;
        }

        if(isFreeThreadApartment)
        {
            IFR_ASSERT(foundation::CreateAdapter<CFreeThreadedMarshalerAdapter>(pOuter, pInner, ppInner));
        }
        else
        {
            IFR_ASSERT(foundation::CreateAdapter<free_thread_not_supported_adapter>(pOuter, pInner, ppInner));
        }
        return S_OK;
    }
 
    return E_NOINTERFACE;
}


