/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelFreeThreadMarshalAdapter.h
****/

#pragma once

#include <foundation/com_ptr.h>

// Public Headers
#include <foundation/library/adapter_base.h>

//------------------------------------------------------------------------------
// Class:   CMoObjectFreeThreadMarshalAdapter
//
// Purpose: Implement a IAgileObject interface for an IObjectNode
//
//------------------------------------------------------------------------------
class CMoObjectFreeThreadMarshalAdapter :
    public foundation::AdapterBase
    <
        foundation::IObject,
        IAgileObject,
        &IID_IAgileObject
    >
{
protected:
    CMoObjectFreeThreadMarshalAdapter():
        m_pUnkFTM(nullptr)
    {
    }

    void FinalRelease() override
    {
        if(m_pUnkFTM)
        {
            m_pUnkFTM->Release();
            m_pUnkFTM = nullptr;
        }
    }

    // Interface IModelAdapter
    STDMETHOD (QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface) 
    {
        if (iid == IID_IMarshal)
        {
            IFR(EnsureFTM());
            IFR(foundation::QueryWeakReference(m_pUnkFTM,iid,ppInterface));
        }
        else
        {
            return __super::QueryAdapterInterface(iid,ppInterface);
        }
        return S_OK;
    }
private:
    HRESULT EnsureFTM()
    {
        HRESULT hr = S_OK;
        IUnknown* pUnkFTM = nullptr;

        if (!m_pUnkFTM)
        {
        
            IFHR(CoCreateFreeThreadedMarshaler(m_pOuter,&pUnkFTM));

            if (nullptr == InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_pUnkFTM), pUnkFTM, nullptr))
            {
                pUnkFTM = nullptr;
            }
        }

        RELEASE_INTERFACE(pUnkFTM);

        RRETURN(hr);
    }
private:
    IUnknown* m_pUnkFTM;

};


