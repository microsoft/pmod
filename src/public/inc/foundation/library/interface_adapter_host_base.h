/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:interface_adapter_host_base.h
****/
#pragma once

#include "adapter_base.h"
#include "interfaces/interface_adapter_host.h"
#include "interfaces/interface_adapter.h"
#include "critical_section_lock.h"

#include <pal/library/spin_lock.h>
#if _HAS_CPP0X
#include <forward_list>
#else
#include <vector>
#endif

// uncomment to debug Interface Adapter QI
//#define _DEBUG_QI_INTERFACE_ADAPTER 

namespace foundation
{
    namespace library
    {
        typedef _CriticalSectionLock<pal::library::SpinLock> spin_lock_lock;

        template <class THost>
        class _InterfaceAdapterHostBase :
            public IInterfaceAdapterOverrides,
            public IInterfaceAdapterHost
        {
        public:
            // Interface IInterfaceAdapterHost
            STDMETHOD(QueryHostAdapterInterface)(REFIID iid, foundation::IUnknown **ppInterface)
            {
                return QueryAdapterInterfaceImpl(iid, ppInterface);
            }
            STDMETHOD(HasAnyAdapterEventHandlers)(bool *bValue)
            {
                *bValue = IsAnyAdapterObserved();
                return S_OK;
            }
            STDMETHOD(FireAdaptersEventArgs)(
                _In_ IFireEventHandlerContainer *pFireEventHandlerContainer,
                _In_ foundation::IUnknown *pEventArgs)
            {
                return FireAdaptersInternal(pFireEventHandlerContainer,pEventArgs);
            }

            STDMETHOD(IsObserved)(bool *bValue)
            {
                *bValue = IsAnyAdapterObserved();
                return S_OK;
            }

        protected:
            _InterfaceAdapterHostBase()
#ifdef _DEBUG_QI_INTERFACE_ADAPTER
                :_in_query_adapter(false)
#endif
            {
            }

            ~_InterfaceAdapterHostBase()
            {
                ReleaseInterfaceAdapters();
            }

            void ReleaseInterfaceAdapters()
            {
                // Release any created Model Adapter
                for (_InterfaceAdapterContainerType::iterator iter = _interfaceAdapters.begin();
                    iter != _interfaceAdapters.end();
                    ++iter)
                {
                    (*iter)->_GetInnerInterface()->Release();
                }
                _interfaceAdapters.clear();
            }

            // call this method after all the supported interfaces the base class
            // Please note this implementation will call QueryInterfaceImpl from 
            // ComInspectableBase
            HRESULT QueryAdapterInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface)
            {
                foundation_assert(!(iid == foundation_IID_IUnknown || iid == foundation_IID_IInspectable));

#ifdef _DEBUG_QI_INTERFACE_ADAPTER
                if(_in_query_adapter)
                {
                    foundation_assert(false);
                    return E_NOINTERFACE;
                }
                _in_query_adapter = true;
#endif

                HRESULT hr = QueryAdapterInterfaceImplInternal(iid, ppInterface);

#ifdef _DEBUG_QI_INTERFACE_ADAPTER
                _in_query_adapter = false;
#endif
                return hr;
            }

            // =============== Interface IInterfaceAdapterOverrides =======================

            STDMETHOD(OnAdapterAddHandler)(_In_ foundation::IUnknown* pSender, _In_ foundation::IUnknown* pHandler) override
            {
                UNREFERENCED_PARAMETER(pSender);
                UNREFERENCED_PARAMETER(pHandler);
                return S_OK;
            }

            STDMETHOD(OnAdapterRemoveHandler)(_In_ foundation::IUnknown* pSender, _In_ foundation::IUnknown* pHandler) override
            {
                UNREFERENCED_PARAMETER(pSender);
                UNREFERENCED_PARAMETER(pHandler);
                return S_OK;
            }

        protected:

            // Fire Model Adapter Events
            HRESULT FireAdaptersInternal(
                _In_ IFireEventHandlerContainer *pFireEventHandlerContainer,
                _In_ foundation::IUnknown *pEventArgs)
            {
                foundation_assert(pEventArgs);
                return SafeInvokeAdaptersInternal(
                    [pFireEventHandlerContainer,pEventArgs](IInterfaceAdapter *pInterfaceAdapter) -> HRESULT
                {
                    return pInterfaceAdapter->FireAdapterEvent(pFireEventHandlerContainer,pEventArgs);
                });
            }

            // Invoke 'OnHostObservedChanged' callback
            HRESULT FireOnObservedChanged(_In_ bool isObserved)
            {
                return SafeInvokeAdaptersInternal(
                    [isObserved](IInterfaceAdapter *pInterfaceAdapter) -> HRESULT
                {
                    return pInterfaceAdapter->OnHostObservedChanged(isObserved);
                });
            }

            // return true if any model adapter has event handlers
            bool IsAnyAdapterObserved()
            {
                spin_lock_lock csl(m_lock);

                // Ask our Interface Adapters if they support this interface
                for (_InterfaceAdapterContainerType::const_iterator iter = 
                    _interfaceAdapters.begin();
                    iter != _interfaceAdapters.end();
                    ++iter)
                {
                    bool bHasEventHandlers = false;
                    HRESULT hr = (*iter)->HasEventHandlers(&bHasEventHandlers);
                    if (SUCCEEDED(hr) && bHasEventHandlers)
                    {
                        return true;
                    }
                }
                return false;
            }

            template <class TAdapter,class TInterface>
            HRESULT GetOrCreateAdapter(
                _In_ foundation::IInspectable *pInner,
                _COM_Outptr_ TInterface **pp
                )
            {
                foundation_assert(pp);

                HRESULT hr;
                IFHR_ASSERT(this->QueryAdaptersInternal(
                    TInterface::GetIID(),
                    reinterpret_cast<foundation::IUnknown **>(pp)));
                if (hr == S_FALSE)
                {
                    // create new instance of the adapter
                    foundation::IInspectable *pAdapterInner;
                    IFHR_ASSERT(CreateAdapter<TAdapter>(
                        this->GetHost()->CastToInspectable(),
                        pInner,
                        &pAdapterInner));
                    // no AddRef when adding
                    this->AddAdapter(pAdapterInner);
                    IFHR_ASSERT(QueryWeakReference(pAdapterInner, pp));
                }
                return S_OK;
            }

            template <class TInterface>
            HRESULT QueryAdaptersInternal(TInterface **pp)
            {
                return this->QueryAdaptersInternal(TInterface::GetIID(),
                    reinterpret_cast<foundation::IUnknown **>(pp));
            }
        private:
            HRESULT QueryAdaptersInternal(REFIID iid, foundation::IUnknown **ppInterface)
            {
                spin_lock_lock csl(m_lock);
                HRESULT hr;
                // Ask our Model Adapters if they support this interface
                for (_InterfaceAdapterContainerType::const_iterator iter = _interfaceAdapters.begin();
                iter != _interfaceAdapters.end();
                    ++iter)
                {
                    hr = (*iter)->QueryAdapterInterface(
                        iid,
                        ppInterface);

                    if (hr == S_OK)
                        return S_OK;
                    else if (hr == foundation::E_ADAPTER_INTERFACE_NOT_SUPPORTED)
                    {
                        // this will prevent going further with looking into adapter factories
                        // rodrigov:3/19/2014
                        return E_NOINTERFACE;
                    }
                }
                return S_FALSE;
            }

            HRESULT AddAdapter(_In_ foundation::IUnknown *pAdapterInner)
            {
                foundation_assert(pAdapterInner != nullptr);
                IInterfaceAdapter *pInterfaceAdapter = nullptr;
                IFR_ASSERT(foundation::QueryWeakReference(pAdapterInner, &pInterfaceAdapter));

                spin_lock_lock csl(m_lock);

#if _HAS_CPP0X
                _interfaceAdapters.push_front(pInterfaceAdapter);
#else
                _interfaceAdapters.push_back(pInterfaceAdapter);
#endif
                return S_OK;
            }


            // safely invoke the interface adapter list by locking first and then 
            // invoke the copied vector
            template <class _Func>
            HRESULT SafeInvokeAdaptersInternal(_In_ _Func func)
            {
                spin_lock_lock csl(m_lock);

                if (_interfaceAdapters.empty())
                {
                    return S_OK;
                }

                // when invoking use a unmutable iterator
                _InterfaceAdapterContainerType interfaceAdapters(_interfaceAdapters);

                csl.unlock();
                return InvokeAdaptersInternal(interfaceAdapters, func);
            }

            // Iterate trough all the Adapters and invoke 'FireAdapterEvent' method
            template <class TVector, class _Func>
            static HRESULT InvokeAdaptersInternal(
                _In_ const TVector& interfaceAdapters,
                _In_ _Func func)
            {
                for (_InterfaceAdapterContainerType::const_iterator iter = interfaceAdapters.begin();
                    iter != interfaceAdapters.end();
                    ++iter)
                {
                    _IFR_(func(*iter));
                }
                return S_OK;
            }

            HRESULT QueryAdapterInterfaceImplInternal(REFIID iid, foundation::IUnknown **ppInterface)
            {
                HRESULT hr = QueryAdaptersInternal(iid, ppInterface);
                if (hr != S_FALSE)
                {
                    return hr;
                }

                // attempt Inherited before Creating Interface Adapters
                hr = GetHost()->_QueryInheritedAdapterInterface(iid, ppInterface);
                // last chance will to find a Interface Adapter
                if (FAILED(hr))
                {
                    foundation::IInspectable *pInnerUnk = nullptr;
                    hr = GetHost()->_QueryCreateAdapterInterface(iid, &pInnerUnk);
                    if (SUCCEEDED(hr))
                    {
                        this->AddAdapter(pInnerUnk);

                        hr = foundation::QueryWeakReference(pInnerUnk, iid, ppInterface);
                        if (hr == foundation::E_ADAPTER_INTERFACE_NOT_SUPPORTED)
                        {
                            // this will normally happen for NotSupportedAdapter class adapter
                            hr = E_NOINTERFACE;
                        }
                    }
                }
                return hr;
            }
        protected:
#if _HAS_CPP0X
            typedef std::forward_list<IInterfaceAdapter *> _InterfaceAdapterContainerType;
#else
            typedef std::vector<IInterfaceAdapter *> _InterfaceAdapterContainerType;
#endif
            // return ReadOnly access to the Interface Adapters vector
            _InterfaceAdapterContainerType _GetInterfaceAdapters()
            {
                spin_lock_lock csl(m_lock);
                return _interfaceAdapters;
            }

        private:
            THost* GetHost() { return static_cast<THost *>(this); }
        private:
#ifdef _DEBUG_QI_INTERFACE_ADAPTER
            bool                    _in_query_adapter;
#endif
            _InterfaceAdapterContainerType   _interfaceAdapters;
            pal::library::SpinLock     m_lock;
        };
    }
}
