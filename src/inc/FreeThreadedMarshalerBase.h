/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FreeThreadedMarshalerBase.h
****/
#pragma once
#include <foundation/macros.h>

//+-----------------------------------------------------------------------------
// Class:   CFreeThreadedMarshalerBase
//
// Purpose: Base class for IMarshal support
//------------------------------------------------------------------------------
class CFreeThreadedMarshalerBase
{
protected:
    CFreeThreadedMarshalerBase(): m_pUnkFTM(nullptr)
    {
    }

    virtual ~CFreeThreadedMarshalerBase()
    {
        RELEASE_INTERFACE(m_pUnkFTM);
    }

    // Interface IModelAdapter
    HRESULT QueryMarshalInterface(IUnknown *pOuter,IUnknown **ppInterface)
    {
        IFR_ASSERT(EnsureFTM(pOuter));
        IFR_ASSERT(m_pUnkFTM->QueryInterface(IID_IMarshal, reinterpret_cast<void**>(ppInterface)));
        // compensate the last QI
        (*ppInterface)->Release();
        return S_OK;
    }
private:
    HRESULT EnsureFTM(IUnknown *pOuter)
    {
        HRESULT hr = S_OK;
        IUnknown* pUnkFTM = NULL;

        if (!m_pUnkFTM)
        {
            IFHR_ASSERT(CoCreateFreeThreadedMarshaler(pOuter, &pUnkFTM));

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

template <class TBASE>
class _TFreeThreadedMarshalerComBase:
    public TBASE,
    protected CFreeThreadedMarshalerBase,
    protected IAgileObject
{
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == IID_IMarshal)
        {
            IUnknown *pOuter = nullptr;
__if_exists (TBASE::m_pOuter)
{
            pOuter = this->m_pOuter;
}
__if_not_exists (TBASE::m_pOuter)
{
            __super::QueryInterfaceImpl(IID_IUnknown,&pOuter);
}
            return QueryMarshalInterface(pOuter,ppInterface);
        }
        else if (iid == IID_IAgileObject)
        {
            *ppInterface = static_cast<IAgileObject *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }
};
