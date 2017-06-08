/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:com_weak_reference_source.h
****/
#pragma once

#include "com_library.h"
#include <foundation/macros.h>

namespace foundation {
    namespace ctl
    {
        typedef ImplementsInspectable
            <
            foundation::IWeakReference,
            &foundation_IID_IWeakReference
            > _ComWeakReferenceBase;
        ///
        /// @brief Implementation for IWeakReference interface
        ///
        class ComWeakReference : public _ComWeakReferenceBase
        {
        public:
            static HRESULT CreateInstance(_In_ foundation::IUnknown *pUnknown, _COM_Outptr_ ComWeakReference **ppNewInstance)
            {
                foundation_assert(pUnknown);
                foundation_assert(ppNewInstance);
                *ppNewInstance = nullptr;

                IFR_ASSERT(ComInspectableObject<ComWeakReference>::CreateInstance(ppNewInstance));
                (*ppNewInstance)->m_pUnknown = pUnknown;
                return S_OK;
            }
        public:

            // IWeakReference methods
            STDMETHODIMP Resolve(_In_ REFIID iid, _COM_Outptr_result_maybenull_ foundation::IInspectable **objectReference)
            {
                foundation_assert(objectReference);
                *objectReference = nullptr;

                for (;;)
                {
                    _pal_AtomicIntegerType ref = m_cStrongRef;
                    if (ref == 0)
                    {
                        return S_OK;
                    }

                    // attempt to increment if values hasn't changed
                    if (_pal_InterlockedCompareExchange(&m_cStrongRef, ref + 1, ref) == ref)
                    {
                        // successfully incremented
                        break;
                    }
                }

                HRESULT hr = m_pUnknown->QueryInterface(iid, reinterpret_cast<void**>(objectReference));
                // compensate loop increment
                DecrementStrongReference();
                return hr;
            }

        protected:
            ComWeakReference() :
                m_cStrongRef(1)
            {
            }

#ifdef DEBUG
            void FinalRelease() override
            {
                _ComWeakReferenceBase::FinalRelease();
            }
#endif

        public:
            _pal_AtomicIntegerType IncrementStrongReference()
            {
                return _pal_InterlockedIncrement(&m_cStrongRef);
            }

            _pal_AtomicIntegerType DecrementStrongReference()
            {
                _pal_AtomicIntegerType l = _pal_InterlockedDecrement(&m_cStrongRef);
                if (l == 0)
                {
                    // An acquire fence is required before object destruction to ensure
                    // that the destructor cannot observe values changing on other threads.
                    //UnknownBarrierAfterInterlock();
                }
                return l;
            }

            _pal_AtomicIntegerType GetStrongRefCount() const
            {
                return m_cStrongRef;
            }

        private:
            IUnknown *m_pUnknown;    // This is the weak reference
            volatile _pal_AtomicIntegerType m_cStrongRef;
        };

        ///
        /// @brief Implements the 'TRefCountImpl' pattern to be used in _ComBase template
        ///
        class _WeakReferenceSourceImpl
        {
        protected:
            _WeakReferenceSourceImpl() :
                m_pWeakReference(nullptr)
            {}
            HRESULT _Initialize_weak_ref_source_impl(foundation::IUnknown *pUnknown)
            {
                IFR_ASSERT(ComWeakReference::CreateInstance(
                    pUnknown,
                    &m_pWeakReference));
                return S_OK;
            }

            ~_WeakReferenceSourceImpl()
            {
                _Release_weak_reference();
            }

            inline _pal_AtomicIntegerType AddRefImplInternal()
            {
                return m_pWeakReference->IncrementStrongReference();
            }

            inline _pal_AtomicIntegerType ReleaseImplInternal()
            {
                return m_pWeakReference->DecrementStrongReference();
            }

            inline _pal_AtomicIntegerType GetRefCountInternal()
            {
                return m_pWeakReference->GetStrongRefCount();
            }

            inline ComWeakReference *_Get_weak_reference() { return m_pWeakReference; }

            inline void _Release_weak_reference()
            {
                inspectable::ReleaseInterface(m_pWeakReference);
            }
        private:
            ComWeakReference *m_pWeakReference;
        };

        ///
        /// @brief Template class that Implements IWeakReferenceSource interface
        ///
        template <class TBASE>
        class ComWeakReferenceSource :
            public foundation::IWeakReferenceSource,
            public TBASE
        {
        protected:
            ComWeakReferenceSource()
            {
                this->_Initialize_weak_ref_source_impl(inspectable::CastToUnknown(this));
            }
        protected:
            HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ foundation::IUnknown **ppInterface) override
            {
                if (iid == foundation_IID_IWeakReferenceSource)
                {
                    *ppInterface = static_cast<IWeakReferenceSource *>(this);
                }
                else
                {
                    return TBASE::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            void FinalRelease() override
            {
                TBASE::FinalRelease();
            }
        public:
            // IWeakReferenceSource methods
            STDMETHODIMP GetWeakReference(_COM_Outptr_ foundation::IWeakReference **ppWeakReference)
            {
                foundation_assert(this->_Get_weak_reference() != nullptr);
                // Give a new reference away
                *ppWeakReference = this->_Get_weak_reference();
                (*ppWeakReference)->AddRef();
                return S_OK;
            }
        };

        ///
        /// @brief concrete IWeakReferenceSource class that use the '_WeakReferenceSourceImpl' template
        ///
        typedef ComWeakReferenceSource <
            _ComInspectableBase<
            _ComBase<foundation::IInspectable, _WeakReferenceSourceImpl>
            >
        > ComWeakReferenceSourceInspectableBase;

        typedef ComAggregateBase <
            _Aggregated_inspectable_base<ComWeakReferenceSourceInspectableBase>,
            _Aggregated_inspectable_base < ComWeakReferenceSourceInspectableBase >> ComAggregateWeakRefSourceInspectableBase;

    }
}
