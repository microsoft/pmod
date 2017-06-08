/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:aggregate_weak_reference_source.h
****/
#pragma once

#include "com_weak_reference_source.h"
#include "../weak_reference.h"

namespace foundation {
    namespace ctl
    {
        ///
        /// @brief 'TRefCountImpl' pattern that use an 'inner' ComWeakReference created class
        // to control life time of this object
        ///
        class _AggregateWeakReferenceSourceImpl
        {
        public:
            HRESULT _InitializeInnerWeakReference(_In_ foundation::IUnknown *pInner)
            {
                foundation_assert(pInner != nullptr);
                foundation::IWeakReference *pWeakRef = nullptr;
                IFR_ASSERT(foundation::AsWeakRef(pWeakRef, pInner));
                pWeakRef->Release();

                m_pWeakReference = static_cast<ComWeakReference *>(pWeakRef);
                return S_OK;
            }

        protected:
            _AggregateWeakReferenceSourceImpl() :
                m_pWeakReference(nullptr)
            {}

            inline _pal_AtomicIntegerType AddRefImplInternal()
            {
                return m_pWeakReference ? m_pWeakReference->IncrementStrongReference() : 1;
            }

            inline _pal_AtomicIntegerType ReleaseImplInternal()
            {
                return m_pWeakReference ? m_pWeakReference->DecrementStrongReference() : 1;
            }

            inline _pal_AtomicIntegerType GetRefCountInternal()
            {
                return m_pWeakReference ? m_pWeakReference->GetStrongRefCount() : 0;
            }

            void _ReleaseWeakReference()
            {
                m_pWeakReference = nullptr;
            }
        private:
            ComWeakReference *m_pWeakReference;
        };

        ///
        /// @brief a concrete ComBase class based on the '_AggregateWeakReferenceSourceImpl' pattern
        ///
        class _AggregateWeakRefComBase :
            public _ComBase
            <
            foundation::IInspectable,
            _AggregateWeakReferenceSourceImpl
            >
        {
        public:
            template <class T>
            static void _DeleteInstance(T *pT)
            {
                delete pT;
            }
        protected:
            typedef _ComBase<foundation::IInspectable, _AggregateWeakReferenceSourceImpl> _BaseType;

            void FinalRelease() override
            {
                // after this point we disconnect from the Weak reference class
                this->_ReleaseWeakReference();
                _BaseType::FinalRelease();
            }
        };

        typedef _ComInspectableBase<_AggregateWeakRefComBase> _AggregateWeakRefInspectableBase;

        ///
        /// @brief a concrete aggregate inspectable class that support the IWeakReferenceSource interface
        /// using an inner class
        ///
        typedef ComAggregateBase <
            _Aggregated_inspectable_base<_AggregateWeakRefInspectableBase>,
            _Aggregated_inspectable_base < _AggregateWeakRefInspectableBase >> _AggregateWeakReferenceSourceInspectableBase;

    }
}
