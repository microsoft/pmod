/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:service_provider_impl.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/interfaces/service_provider.h>
#include "util_stl.h"

#include <map>
namespace foundation
{
    namespace library
    {
        class ServiceProviderImpl :
            public foundation::ctl::ComInspectableBase,
            public IServiceProvider
        {
        public:
            static inline HRESULT CreateInstance(
                _In_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance)
            {
                return ctl::inspectable::CreateInstanceWithInitialize<ServiceProviderImpl>(pOuter, ppNewInstance);
            }

            HRESULT _Initialize()
            {
                return S_OK;
            }

            HRESULT RegisterService(REFIID guidService,_In_ IUnknown *pService)
            {
                IFCPTR_ASSERT(pService);

                _services[guidService] = pService;
                return S_OK;
            }

            STDMETHOD(QueryService)(
                REFIID guidService,
                REFIID riid,
                void **ppv)
            {
                _ServiceMap_Type::iterator iter = _services.find(guidService);
                if (iter != _services.end())
                {
                    return (*iter).second->QueryInterface(riid, ppv);
                }
                return E_NOTIMPL;
            }
        protected:
            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == IServiceProvider::GetIID())
                {
                    *ppInterface = static_cast<IServiceProvider *>(this);
                }
                else
                {
                    return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

        private:
            typedef std::map<IID, ComPtr<IUnknown>,library::GuidLessThan> _ServiceMap_Type;
            _ServiceMap_Type _services;
        };

        template <class TBASE>
        class _HostServiceProvider :
            public TBASE
        {
        protected:
            _HostServiceProvider() :
                _p_InnerServiceProvider(nullptr)
            {
            }

            void FinalRelease() override
            {
                RELEASE_INTERFACE(_p_InnerServiceProvider);
                TBASE::FinalRelease();
            }

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == IServiceProvider::GetIID())
                {
                    if (_p_InnerServiceProvider == nullptr)
                    {
                        IFR_ASSERT(ServiceProviderImpl::CreateInstance(
                            this->CastToInspectable(),
                            &_p_InnerServiceProvider));
                    }
                    return foundation::QueryWeakReference(_p_InnerServiceProvider, iid, ppInterface);
                }
                else
                {
                    return TBASE::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }
        private:
            IInspectable *_p_InnerServiceProvider;
        };
    }
}

