/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:base_object_impl.h
****/
#pragma once

#include "base_adapter_host.h"
#include "interfaces/critical_section.h"
#include "foundation_impl.h"
#include "critical_section.h"
#include "critical_section_impl.h"
#include "node_impl.h"
#include <foundation/type_info_statics_util.h>
#include <foundation/interfaces/node.h>
#include <pal/pal_thread.h>

EXTERN_C FOUNDATION_API UINT32 STDAPICALLTYPE GetNextObjectUniqueId();

namespace foundation {

    
    namespace library {
   
        template <
            class TInterface, 
            class THostInterface,
            class TComBase = ctl::ComInspectableBase,	
            class TFireEventHandlerContainer = _NoFireEventHandlerContainer
        >
        class _ObjectImplBase :
            public _DefaultAdapterHost<THostInterface, TComBase>,
            public TFireEventHandlerContainer,
            public TInterface
        {
        public:
            HRESULT _Initialize(bool isFreeThreaded)
            {
                if (isFreeThreaded)
                {
                    IFR_ASSERT(this->EnableFreeThreaded());
                }
                return S_OK;
            }

            bool IsObserved()
            {
                return this->IsAnyAdapterObserved() ||
                    this->IsObservedInternal();
            }

            virtual ICriticalSection *_GetCriticalSection() 
            {
                ICriticalSection *pCriticalSection = nullptr;
                this->QueryAdaptersInternal(&pCriticalSection);
                return pCriticalSection;
            }

            inline HRESULT _FireAdapters(
                IFireEventHandlerContainer *pFireEventHandlerContainer,
                foundation::IUnknown *pEventArgs)
            {
                return _base_type::FireAdaptersInternal(pFireEventHandlerContainer, pEventArgs);
            }
             
            // IObject Interface
            STDMETHOD(GetUniqueId)(UINT32 *pUniqueId) override
            {
                IFCPTR_ASSERT(pUniqueId);
                *pUniqueId = _uniqueId;
                return S_OK;
            }
            STDMETHOD(GetProcessId)(UINT32 *pProcessId) override
            {
                IFCPTR_ASSERT(pProcessId);
                *pProcessId = (UINT32)_pal_GetCurrentProcessId();
                return S_OK;
            }

            STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue) override
            {
                IFR_ASSERT(foundation::GetTypeInfoAs(TInterface::GetIID(), ppValue));
                return S_OK;
            }
            
            // Callbacks required by Fire Event Handler container
            inline bool _IsFreeThreaded()
            {
                return this->_GetCriticalSection() != nullptr;
            }

            HRESULT _OnCustomFireEvent(
                _In_ IFireEventHandlerContainer *pFireEventHandlerContainer,
                _In_ foundation::IUnknown *pEventArgs)
            {
                return this->_FireAdapters(pFireEventHandlerContainer, pEventArgs);
            }

            HRESULT SetParent(_In_opt_ IUnknown *pParent)
            {
                _INode_impl *pNode_impl = nullptr;
                if (pParent != nullptr)
                {
                    IFR_ASSERT(QueryWeakReference(pParent,&pNode_impl));
                }
                return SetParent(pNode_impl);
            }

            HRESULT SetParent(_In_opt_ _INode_impl *pParent)
            {
                _IObjectNode *pObjectNode;
                IFR_ASSERT(GetOrCreateObjectNodeAdapter(&pObjectNode));
                _ObjectNode_adapter_type *pObjectNode_adapter = static_cast<_ObjectNode_adapter_type *>(pObjectNode);
                pObjectNode_adapter->SetParent(pParent);
                return S_OK;
            }

            template <class T>
            HRESULT GetParent(_COM_Outptr_result_maybenull_ T **ppParent)
            {
                ComPtr<IObject> parent_ptr;

                _IObjectNode *pThisObjectNode = nullptr;
                IFR_ASSERT(this->QueryAdaptersInternal(&pThisObjectNode));
                if (pThisObjectNode != nullptr)
                {
                    IFR_ASSERT(pThisObjectNode->GetParent(parent_ptr.GetAddressOf()));
                }
                return QueryInterfaceIf(parent_ptr, ppParent);
            }

        protected:
            typedef _DefaultAdapterHost<THostInterface, TComBase> _base_type;
            typedef _NodeImplAdapterBase<
                _IObjectNode, 
                foundation::IInspectable, 
                &foundation::IID__IObjectNode> _ObjectNode_adapter_type;

            typedef AdapterBase<foundation::IInspectable, ICriticalSection, &IID_ICriticalSection> _Critical_section_adapter_type;
            class _Critical_section_adapter:
                public _CriticalSectionBase<true,_Critical_section_adapter_type>
            {
            public:
                HRESULT _Initialize(
                    _In_ foundation::IInspectable *pOuter,
                    _In_ foundation::IInspectable *pInner
                    )
                {
                    _CriticalSectionBase<true, _Critical_section_adapter_type>::_Initialize();
                    return _Critical_section_adapter_type::_Initialize(pOuter, pInner);
                }
            };

            _ObjectImplBase()
            {
                _uniqueId = GetNextObjectUniqueId();
            }
                
            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == foundation::IObject::GetIID())
                {
                    *ppInterface = static_cast<foundation::IObject *>(this);
                }
                else if (iid == _IObjectNode::GetIID())
                {
                    return GetOrCreateObjectNodeAdapter(reinterpret_cast<_IObjectNode **>(ppInterface));
                }
                else if (iid == _INode_impl::GetIID())
                {
                    _IObjectNode *pObjectNode;
                    IFR_ASSERT(GetOrCreateObjectNodeAdapter(&pObjectNode));
                    _ObjectNode_adapter_type *pObjectNode_adapter = static_cast<_ObjectNode_adapter_type *>(pObjectNode);
                    *ppInterface = static_cast<_INode_impl *>(pObjectNode_adapter);
                }
                else if (iid == TInterface::GetIID())
                {
                    *ppInterface = static_cast<TInterface *>(this);
                }
                else
                {
                    return _base_type::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            virtual bool IsObservedInternal()
            {
                return false;
            }

            HRESULT EnableFreeThreaded()
            {
                ICriticalSection *pCriticalSection = nullptr;
                return this->template GetOrCreateAdapter<_Critical_section_adapter>(
                    this->CastToInspectable(),
                    &pCriticalSection);
            }

            template <class TEventSourceClass>
            HRESULT CreateEventSourceWithCriticalSection(TEventSourceClass **pp)
            {
                IFR_ASSERT(ctl::CreateInstance(pp));
                (*pp)->_GetLock().SetCriticalSection(this->_GetCriticalSection());
                return S_OK;
            }

        private:
            HRESULT GetOrCreateObjectNodeAdapter(_IObjectNode **pp)
            {
                return this->template GetOrCreateAdapter<_ObjectNode_adapter_type>(
                    this->CastToInspectable(),
                    pp);
            }
        private:
            UINT32 _uniqueId;
        };


        template <class T>
        HRESULT CreateObjectImpl(T **ppT,bool isFreeThreaded = false)
        {
            return ctl::inspectable::CreateInstanceWithInitialize(ppT, isFreeThreaded);
        }

        template <class TBASE,const IID&iidType >
        class _ObjectWithTypeImplBase :
            public TBASE
        {
        public:
            STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** ppValue) override
            {
                IFR_ASSERT(foundation::GetTypeInfoAs(
                    this->_pType == nullptr ? iidType : *this->_pType,
                    ppValue));
                return S_OK;
            }

            template <typename... Arguments>
            HRESULT _Initialize(const IID *pType, const Arguments&... params)
            {
                this->_pType = pType;
                return TBASE::_Initialize(params...);
            }
        protected:
            _ObjectWithTypeImplBase() :
                _pType(nullptr)
            {}
        private:
            const IID *_pType;
        };
    }
}

