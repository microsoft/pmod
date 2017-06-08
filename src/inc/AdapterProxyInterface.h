/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AdapterProxyInterface.h
****/

#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/macros.h>
#include <foundation/com_ptr.h>

MIDL_INTERFACE("6FC01087-1DE9-458C-BCF6-A9730B14DB11")
IAdapterProxyInterface : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE GetSourceInterface(IUnknown **ppSourceInterface) = 0;
};

template <class T>
static HRESULT create_adapter_proxy_instance(
    typename T::TSourceInterface *pSourceInterface,
    typename T::TAdapterInterface **ppAdapterInterface)
{

    foundation_assert(pSourceInterface);
    foundation_assert(ppAdapterInterface);

    foundation::ctl::ComInspectableObject<T> *pT = nullptr;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<T>::CreateInstance(&pT));
    IFR_ASSERT(pT->SetSourceInterface(pSourceInterface));

    *ppAdapterInterface = static_cast<typename T::TAdapterInterface *>(pT);
    return S_OK;
}

template <class T>
static HRESULT create_adapter_proxy_instance(
    foundation::IUnknown *pSourceInterface,
    foundation::IUnknown  **ppInstance)
{
    foundation_assert(pSourceInterface);
    foundation::ComPtr<typename T::TSourceInterface> spSourceInterface;
    IFR_ASSERT(foundation::QueryInterface(pSourceInterface, spSourceInterface.GetAddressOf()));
    foundation::ComPtr<typename T::TAdapterInterface> spAdapterInterface;
    IFR_ASSERT(create_adapter_proxy_instance<T>(spSourceInterface, spAdapterInterface.GetAddressOf()));

    *ppInstance = spAdapterInterface.Detach();
    return S_OK;
}

template <class TAdapterInterface,class TSourceInterface,class TBASE=foundation::ctl::ComInspectableBase>
class TAdapterProxyInterface :
    public TBASE,
    public IAdapterProxyInterface,
    public TAdapterInterface
{
public:
    typedef TAdapterInterface TAdapterInterface;
    typedef TSourceInterface TSourceInterface;

    template <class T>
    static HRESULT CreateInstance(
            TSourceInterface *pSourceInterface,
            TAdapterInterface **ppAdapterInterface)
    {
        return create_adapter_proxy_instance<T>(pSourceInterface, ppAdapterInterface);
    }

    template <class T>
    static HRESULT CreateAggregableInstance(
            IInspectable *pOuter,
            TSourceInterface *pSourceInterface,
            IInspectable **ppNewInstance)
    {
        foundation_assert(pOuter);
        foundation_assert(pSourceInterface);
        foundation_assert(ppNewInstance);

        foundation::ctl::AggregableComInspectableObject<T> *pT = nullptr;
        IFR_ASSERT(foundation::ctl::AggregableComInspectableObject<T>::CreateInstance(pOuter, &pT));
        IFR_ASSERT(pT->SetSourceInterface(pSourceInterface));

        *ppNewInstance = reinterpret_cast<IInspectable*>(static_cast<foundation::ctl::INonDelegatingUnknown *>(pT));
        return S_OK;
    }

    HRESULT SetSourceInterface(TSourceInterface *pSourceInterface)
    {
        foundation_assert(pSourceInterface);
        m_pSourceInterface = pSourceInterface;
        m_pSourceInterface->AddRef();
        return S_OK;
    }

protected:
    TAdapterProxyInterface():
        m_pSourceInterface(nullptr)
    {
    }


    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == __uuidof(IAdapterProxyInterface) )
        {
            *ppInterface = static_cast<IAdapterProxyInterface *>(this);
        }
__if_exists (TAdapterInterface::GetIID)
{
        else if (iid == TAdapterInterface::GetIID())
}
__if_not_exists (TAdapterInterface::GetIID)
{
        else if (iid == __uuidof(TAdapterInterface))
}
        {
            *ppInterface = static_cast<TAdapterInterface *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // Interface IAdapterProxyInterface
    HRESULT STDMETHODCALLTYPE GetSourceInterface(IUnknown **ppSourceInterface)
    {
        foundation_assert(ppSourceInterface);
        *ppSourceInterface = m_pSourceInterface;
        (*ppSourceInterface)->AddRef();
        return S_OK;
    }

    void FinalRelease() override
    {
        RELEASE_INTERFACE(m_pSourceInterface);
        __super::FinalRelease();
    }

protected:
    TSourceInterface    *m_pSourceInterface;
};

