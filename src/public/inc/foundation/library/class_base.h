/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:class_base.h
****/
#pragma once

#include <foundation/com_ptr.h>
#include <foundation/ctl/com_library.h>
#include <foundation/ctl/com_weak_reference_source.h>

// Note: uncomment this line if you want tracking ref count on IObject type class
//#define _TRACK_OBJECT_CLASS_REF_COUNT

#if defined(DEBUG) && defined(_TRACK_OBJECT_CLASS_REF_COUNT)
#include"debug_library.h"
#endif

namespace foundation {
    namespace library {

        template <
            class TPublicInterface,         // the Public Interface
            class TClassInterface,          // the Class Interface
            class TDelegateImpl,
            class TInterfaceWrapper,
            class TClassInterfaceWrapper
        >
        class _FoundationClassBase :
            public ctl::_InnerContainer<>,
            public TInterfaceWrapper,
            public TClassInterfaceWrapper,
            protected TDelegateImpl
        {
        public:
            _FoundationClassBase() :
                _interface(nullptr),
                _interface_class(nullptr)
            {
            }

            // empty model initializer
            HRESULT _InitializeModel()
            {
                return S_OK;
            }

            TPublicInterface *GetInterface() const
            {
                foundation_assert(_interface);
                return _interface;
            }
            bool IsInterfaceAvailable() { return _interface != nullptr; }

            TClassInterface *GetClassInterface()
            {
                return this->GetInterfaceProtected();
            }

        protected:
            TClassInterface *GetInterfaceProtected()
            {
                foundation_assert(_interface_class);
                return _interface_class;
            }
        protected:

            void _CleanupWeakReferences()
            {
                _interface = nullptr;
                _interface_class = nullptr;
            }

            ~_FoundationClassBase()
            {
                _Cleanup();
            }

            void _Cleanup()
            {
                // Cleanup my weak references
                _CleanupWeakReferences();
            }

        protected:
            HRESULT QueryHostInstance(IUnknown *pInnerUnk)
            {
                IFR_ASSERT(QueryWeakReference(pInnerUnk, &_interface));
                IFR_ASSERT(QueryWeakReference(pInnerUnk, &_interface_class));

                return S_OK;
            }

        protected:
            TPublicInterface*                     _interface;
            TClassInterface*                  _interface_class;
        };

        template <class TBASE>
        class _AggregateClassBase :
            public ctl::ComAggregateBase<_AggregateClassBase<TBASE>, ctl::ComWeakReferenceSourceInspectableBase>,
            public TBASE
        {
        };

        template <class TBASE>
        class _AggregateObjectClassBase :
            public _AggregateClassBase<TBASE>
        {
        protected:
#if defined(DEBUG) && defined(_TRACK_OBJECT_CLASS_REF_COUNT)
            _pal_AtomicIntegerType AddRefImpl() override
            {
                _pal_AtomicIntegerType cRef = this->_AggregateClassBase<TBASE>::AddRefImpl();
                
                ComPtr<IObjectTypeInfo> type_info_ptr;
                this->GetTypeInfo(type_info_ptr.GetAddressOf());

                debug_util::TrackRefCount(
                    type_info_ptr,
                    0,
                    TrackingInfoType::AddRef,
                    U("object_class_base::AddRefImpl"),
                    this->GetUniqueId,
                    cRef);
                return cRef;
            }

            _pal_AtomicIntegerType ReleaseImpl() override
            {
                ComPtr<IObjectTypeInfo> type_info_ptr;
                this->GetTypeInfo(type_info_ptr.GetAddressOf());
                UINT32 uniqueId = this->GetUniqueId;

                _pal_AtomicIntegerType cRef = this->_AggregateClassBase<TBASE>::ReleaseImpl();
                debug_util::TrackRefCount(
                    type_info_ptr,
                    0,
                    cRef != 0 ? TrackingInfoType::Release : TrackingInfoTyp::FinalRelease,
                    U("object_class_base::ReleaseImpl"),
                    uniqueId,
                    cRef);
                return cRef;
            }
#endif
        };

        template <class T, class TOptions>
        static HRESULT CreateClassWithInner(
            const IID& iidType,
            TOptions options,
            _COM_Outptr_ T **ppT)
        {
            ComPtr<T, ctl::_ComBasePtrImpl> ptr;
            IFR_ASSERT(ctl::inspectable::CreateInstance(ptr.GetAddressOf()));
            IFR_ASSERT(ptr->_CreateInnerInstance(iidType, ptr->CastToInspectable(), options));
            *ppT = ptr.Detach();
            return S_OK;
        }

        template <class T, class TOptions,typename... Arguments>
        static HRESULT CreateObjectClassWithParent(
            const IID& iidType,
            TOptions options,
            _COM_Outptr_ T **ppT,
            _In_opt_ foundation::IObject *pParent,
            const Arguments&... params)
        {
            ComPtr<T, ctl::_ComBasePtrImpl> ptr;
            IFR_ASSERT(CreateClassWithInner(iidType, options,ptr.GetAddressOf()));
            if (pParent != nullptr)
            {
                IFR_ASSERT(ptr->SetParentObject(pParent));
            }
            IFR_ASSERT(ptr->_InitializeModel(params...));
            *ppT = ptr.Detach();
            return S_OK;
        }

        template <class T, class TOptions, typename... Arguments>
        inline HRESULT CreateObjectClass(
            const IID& iidType,
            TOptions options,
            _COM_Outptr_ T **ppT,
            const Arguments&... params)
        {
            return CreateObjectClassWithParent(iidType, options, ppT, nullptr, params...);
        }

    }
}
