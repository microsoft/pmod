/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FtmBase.h
****/

#pragma once

#include <foundation/ctl/com_library.h>

template <class TBASE>
class _TFtmBase: 
    public TBASE,
    public IAgileObject
{
protected:
    _TFtmBase(): m_pUnkFTM(NULL)
    {
    }
      
    virtual ~_TFtmBase()
    {
        RELEASE_INTERFACE(m_pUnkFTM);
    }
protected:

    // override foundation::ctl::ComBase
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == IID_IAgileObject)
        {
            *ppInterface = static_cast<IAgileObject *>(this);
        }
        else if (iid == IID_IMarshal)
        {
            IFR(EnsureFTM());
            IFR(m_pUnkFTM->QueryInterface(iid, reinterpret_cast<void**>(ppInterface)));
            // compensate the last QI
            (*ppInterface)->Release();
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }

        return S_OK;
    }

private:
    HRESULT EnsureFTM()
    {
        HRESULT hr = S_OK;
        IUnknown* pUnkFTM = NULL;

        if (!m_pUnkFTM)
        {
        
            IFHR(CoCreateFreeThreadedMarshaler(static_cast<TBASE *>(this), &pUnkFTM));

            if (NULL == InterlockedCompareExchangePointer(reinterpret_cast<void**>(&m_pUnkFTM), pUnkFTM, NULL))
            {
                pUnkFTM = NULL;
            }
        }

        RELEASE_INTERFACE(pUnkFTM);

        RRETURN(hr);
    }

private:
    IUnknown* m_pUnkFTM;
};

typedef _TFtmBase<foundation::ctl::ComBase> _FtmBase;
typedef _TFtmBase<foundation::ctl::ComInspectableBase> _FtmInspectableBase;
