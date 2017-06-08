/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:com_activation.h
****/
#pragma once

#include <foundation/library/logger_macros.h>
#include <foundation/ctl/com_library.h>
#include "activation.h"

namespace ctlwinrt
{

    template <class TBASE>
    class CachedComInspectableObject: public foundation::ctl::ComInspectableObject<TBASE>
    {
    public:

        __override IFACEMETHODIMP_(ULONG) AddRef()
        {
            ULONG nRefs = ComInspectableObject<TBASE>::AddRef();
            if (nRefs == 2)
            {
                IncrementObjectCount();
            }
            return nRefs;
        }

        __override IFACEMETHODIMP_(ULONG) Release()
        {
            ULONG nRefs = ComInspectableObject<TBASE>::Release();
            if (nRefs == 1)
            {
                DecrementObjectCount();
            }
            return nRefs;
        }

    public:

        static __checkReturn HRESULT CreateInstance(IInspectable **ppNewInstance)
        {
            HRESULT hr = S_OK;
            CachedComInspectableObject* pNewInstance = new CachedComInspectableObject<TBASE>();
            IFCOOM(pNewInstance);
            __if_exists (T::Initialize)
            {
                hr = pNewInstance->Initialize();
                if(FAILED(hr))
                {
                    ReleaseInterface(pNewInstance);
                    return hr;
                }
            }
            *ppNewInstance = static_cast<IInspectable *>(static_cast<ComInspectableBase *>(pNewInstance));
            return hr;
        }

        template <class T>
        static __checkReturn HRESULT CreateInstance(T **ppNewInstance)
        {
            HRESULT hr = S_OK;
            CachedComInspectableObject* pNewInstance = new CachedComInspectableObject<TBASE>();
            IFCOOM(pNewInstance);
            __if_exists (T::Initialize)
            {
                hr = pNewInstance->Initialize();
                if(FAILED(hr))
                {
                    ReleaseInterface(pNewInstance);
                    return hr;
                }
            }
            *ppNewInstance = static_cast<T *>(pNewInstance);
            return hr;
        }
    };

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

    class SupportErrorInfo: 
        public ISupportErrorInfo,
        public foundation::ctl::ComInspectableBase
    {
    protected:
        SupportErrorInfo()
        {}

        ~SupportErrorInfo()
        {
        }

        BEGIN_IID_MAP
            UUIDOF_INTERFACE_ENTRY(ISupportErrorInfo)
        END_IID_MAP(foundation::ctl::ComInspectableBase)

    protected:

        __override HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface)
        {
            if (iid == __uuidof(ISupportErrorInfo))
            {
                *ppInterface = static_cast<ISupportErrorInfo *>(this);
            }
            else
            {
                return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
            }

            return S_OK;
        }

    public:

        // ISupportErrorInfo implementation
        IFACEMETHODIMP InterfaceSupportsErrorInfo(_In_ REFIID iid)
        {
            UNREFERENCED_PARAMETER(iid);
            return S_OK;
        }

    };
#else
    typedef foundation::ctl::ComInspectableBase SupportErrorInfo;
#endif

    template <class T>
    class ActivationFactoryCreator
    {
    public:
        static IActivationFactory* CreateActivationFactory()
        {
            T *pNewFactory = nullptr;
            HRESULT hr = S_OK;

            hr = CachedComInspectableObject<T>::CreateInstance(&pNewFactory);
            if (FAILED(hr))
            {
                return nullptr;
            }

            return static_cast<IActivationFactory *>(pNewFactory);
        }
    };

    
    template <class T>
    class ActivationFactory: 
        public IActivationFactory, 
        public SupportErrorInfo
    {
        BEGIN_IID_MAP
            UUIDOF_INTERFACE_ENTRY(IActivationFactory)
        END_IID_MAP(SupportErrorInfo)

    protected:

        HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
        {
            if (iid == IID_IActivationFactory)
            {
                *ppInterface = static_cast<IActivationFactory *>(this);
            }
            else
            {
                return SupportErrorInfo::QueryInterfaceImpl(iid, ppInterface);
            }

            return S_OK;
        }

    public:

        // IActivationFactory interface
        IFACEMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable **instance) override
        {
            return ComObject<T>::CreateInstance(instance);
        }
    };

    class AbstractActivationFactory:
        public IActivationFactory,
        public SupportErrorInfo
    {
        BEGIN_IID_MAP
            UUIDOF_INTERFACE_ENTRY(IActivationFactory)
        END_IID_MAP(SupportErrorInfo)

    protected:

        HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
        {
            if (iid == IID_IActivationFactory)
            {
                *ppInterface = static_cast<IActivationFactory *>(this);
            }
            else
            {
                return SupportErrorInfo::QueryInterfaceImpl(iid, ppInterface);
            }

            return S_OK;
        }

    public:

        IFACEMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable **instance) override
        {
            UNREFERENCED_PARAMETER(instance);
            return E_NOTIMPL;
        }
    };

    template <class T>
    class AggregableActivationFactory:
        public ActivationFactory<T>
    {
    public:

        __checkReturn HRESULT ActivateInstance(_In_opt_ IInspectable* pOuter, _COM_Outptr_ IInspectable **instance)
        {
            HRESULT hr = S_OK;
            T* pObj = nullptr;
            AggregableComInspectableObject<T>* pObjAsAggregable = nullptr;

            if (pOuter)
            {
                IFHR(AggregableComInspectableObject<T>::CreateInstance(pOuter, &pObj));
                pObjAsAggregable = static_cast<AggregableComInspectableObject<T>*>(pObj);
                IFHR(pObjAsAggregable->NonDelegatingQueryInterface(IID_IInspectable, (void **)instance));
                pObjAsAggregable->NonDelegatingRelease();
            }
            else
            {
                IFHR(ComInspectableObject<T>::CreateInstance(instance));
            }
            return hr;
        }
    };

    template <class T>
    class AggregableAbstractActivationFactory:
        public AbstractActivationFactory
    {
    public:

        __checkReturn HRESULT ActivateInstance(_In_opt_ IInspectable* pOuter, _COM_Outptr_ IInspectable **instance)
        {
            HRESULT hr = S_OK;
            T* pObj = nullptr;
            AggregableComInspectableObject<T>* pObjAsAggregable = nullptr;

            IFCPTR(pOuter);
            IFC(AggregableComInspectableObject<T>::CreateInstance(pOuter, &pObj));
            
            pObjAsAggregable = static_cast<AggregableComInspectableObject<T>*>(pObj);
            IFC(pObjAsAggregable->NonDelegatingQueryInterface(IID_IInspectable, (void **)instance));
            pObjAsAggregable->NonDelegatingRelease();
            pObj = nullptr;

            return hr;
        }
    };

}
