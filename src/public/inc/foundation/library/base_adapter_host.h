/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:base_adapter_host.h
****/
#pragma once

#include "interface_adapter_host_base.h"
#include "util_stl.h"
#include "interface_adapter_util.h"

#include <foundation/interfaces/object.h>

namespace foundation
{
    namespace library
    {

        // uncomment to debug Model Adapter creation
        //#define _DEBUG_CREATE_INTERFACE_ADAPTER 1

        //------------------------------------------------------------------------------
        // Class:   _AdapterHostResolverBase
        //
        // Purpose: Default Host resolver for Adapters, will use the global scope
        //
        //------------------------------------------------------------------------------
        class _AdapterHostResolverBase
        {
        public:
            static HRESULT _QuerCcreateAdapterInterfaceImpl(
                _In_ foundation::IInspectable* pOuter,
                _In_ foundation::IInspectable* pInner,
                REFIID iid,
                REFIID iidHostType,
                _COM_Outptr_ foundation::IInspectable **ppAdapterNewInstance)
            {
                IFCPTR_ASSERT(pOuter);
                IFCPTR_ASSERT(ppAdapterNewInstance);

                *ppAdapterNewInstance = nullptr;

                // Use the Model Global Wide scope Model Factory
                HRESULT hr = foundation::library::GetInterfaceAdapterFactoryHost()->QueryCreateInterfaceAdapter(
                    pOuter,
                    pInner,
                    iidHostType,
                    iid,
                    ppAdapterNewInstance);
                return hr;
            }

            static bool _IsFactoryInterface(REFIID iid)
            {
                return 
                    iid == foundation::IObject::GetIID() ||
                    iid == foundation_IID_IPropertyValue ||
                    iid == foundation_IID_IWeakReferenceSource;
            }
        };

        //------------------------------------------------------------------------------
        // Class:   _BaseAdapterHost
        //
        // Purpose: Base class for a Host Adapter
        //
        //------------------------------------------------------------------------------
        template <class THostInterface, class TBASE, class TAdapterHostResolver>
        class _BaseAdapterHost :
            public TBASE,
            public _InterfaceAdapterHostBase
            <
            _BaseAdapterHost < THostInterface, TBASE, TAdapterHostResolver >
            >
        {
        public:
            typedef typename TBASE::_ComInspectableBaseType _ComInspectableBaseType;

            HRESULT _QueryInheritedAdapterInterface(REFIID iid, foundation::IUnknown **ppInterface)
            {
                UNREFERENCED_PARAMETER(iid);
                UNREFERENCED_PARAMETER(ppInterface);
                // we already invoke the Base Class if we arrive here
                return E_NOINTERFACE;
            }

            HRESULT _QueryCreateAdapterInterface(REFIID iid, foundation::IInspectable **ppModeAdapterNewInstance)
            {
                foundation::IInspectable* pInner = this->GetInnerInterface();
                foundation::IInspectable* pOuter;

                if (this->IsComposed())
                {
                    // Find 'Outer' object
                    foundation::QueryWeakReference(
                        static_cast<_ComInspectableBaseType *>(this),
                        foundation_IID_IInspectable,
                        &pOuter);
                    IFCPTR_ASSERT(pOuter);
                }
                else
                {
                    pOuter = static_cast<_ComInspectableBaseType *>(this);
                }

                TAdapterHostResolver *pThis = this->GetAdapterHostResolver();
                IID iidHostInterface = pThis->_GetHostInterfaceType(iid);

                HRESULT hr = pThis->_QuerCcreateAdapterInterfaceImpl(
                    pOuter,
                    pInner,
                    iid,
                    iidHostInterface,
                    ppModeAdapterNewInstance);
#ifdef _DEBUG_CREATE_INTERFACE_ADAPTER
                IIDTypeBufferType iidBuffer;
                IIDTypeBufferType iidHostBuffer;

                _iidToBuffer(iid,iidBuffer);
                _iidToBuffer(iidHostInterface,iidHostBuffer);

                _traceMessage(U("_QueryCreateAdapterInterface-> iid:%s iidHost:%s hr=%04x threadid=%d\n"),iidBuffer,iidHostBuffer,hr,_pal_CurrentThreadId());

#endif
                return hr;
            }
        protected:
            TAdapterHostResolver *GetAdapterHostResolver()
            {
                return static_cast<TAdapterHostResolver *>(this);
            }

            // Override of ComBase
            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == IInterfaceAdapterHost::GetIID())
                {
                    *ppInterface = static_cast<IInterfaceAdapterHost *>(this);
                    return S_OK;
                }
                else if (iid == foundation::library::IInterfaceAdapterOverrides::GetIID())
                {
                    *ppInterface = static_cast<foundation::library::IInterfaceAdapterOverrides *>(this);
                    return S_OK;
                }

                HRESULT hr = TBASE::QueryInterfaceImpl(iid, ppInterface);
                if (hr == E_NOINTERFACE && !_AdapterHostResolverBase::_IsFactoryInterface(iid))
                {
                    // for unknown interfaces use the Adapter QI
                    hr = this->QueryAdapterInterfaceImpl(iid, ppInterface);
                }
                return hr;
            }
        };

        //------------------------------------------------------------------------------
        // Class:   _DefaultAdapterHost
        //
        // Purpose: Base class for a Host Adapter
        //
        //------------------------------------------------------------------------------
        template <class THostInterface, class TBASE>
        class _DefaultAdapterHost :
            public _BaseAdapterHost<THostInterface, TBASE, _DefaultAdapterHost<THostInterface, TBASE> >,
            public _AdapterHostResolverBase
        {
        public:
            static IID _GetHostInterfaceType(REFIID iid)
            {
                UNREFERENCED_PARAMETER(iid);
                return THostInterface::GetIID();
            }
        };

        //------------------------------------------------------------------------------
        // Class:   _DefaultAdapterHostType
        //
        // Purpose: Base class for a Host Adapter
        //
        //------------------------------------------------------------------------------
        template <class THostInterface, class TBASE, class T>
        class _DefaultAdapterHostType :
            public _BaseAdapterHost<THostInterface, TBASE, T >,
            public _AdapterHostResolverBase
        {
        public:
            static IID _GetHostInterfaceType(REFIID iid)
            {
                UNREFERENCED_PARAMETER(iid);
                return THostInterface::GetIID();
            }
        };

    }
}
