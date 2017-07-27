/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:composable_weak_reference_source.h
****/
#pragma once

#include "com_weak_reference_source.h"

namespace foundation {
    namespace ctl
    {
        ///
        /// @brief _IRefCountImpl derived interface that support access to a 'ComWeakReference'
        ///
        struct _IWeakRefSourceCountImpl : _IRefCountImpl
        {
            virtual ComWeakReference *_Get_weak_reference(foundation::IUnknown *pHost) = 0;
        };

        ///
        /// @brief delegate implementation of the '_IWeakRefSourceCountImpl' interface
        ///
        class _WeakRefSourceCountDelegeteImpl :
            public _RefCountDelegeteImpl < _IWeakRefSourceCountImpl >
        {
        protected:
            inline ComWeakReference *_Get_weak_reference(foundation::IUnknown *pHost)
            {
                return _Get_ref_count_impl()->_Get_weak_reference(pHost);
            }
        };

        ///
        /// @brief stub implementation of the '_IWeakRefSourceCountImpl' based on the
        /// 'ComWeakReference' ref counting
        ///
        class _WeakReferenceSourceStubImpl :
            public _RefCountStubImpl < _WeakReferenceSourceImpl, _IWeakRefSourceCountImpl >
        {
        public:
            _WeakReferenceSourceStubImpl(foundation::IUnknown *pHost)
            {
                _WeakReferenceSourceImpl::_Initialize_weak_ref_source_impl(pHost);
            }
        protected:
            ComWeakReference *_Get_weak_reference(foundation::IUnknown *pHost) override
            {
                return _WeakReferenceSourceImpl::_Get_weak_reference();
            }
        };

        ///
        /// @brief stub implementation of the '_IWeakRefSourceCountImpl' based on the
        /// atomic integer ref counting
        ///
        class _ComposedWeakRefSourceStubImpl :
            public _RefCountStubImpl < _RefCountImpl, _IWeakRefSourceCountImpl >
        {
        public:
            _ComposedWeakRefSourceStubImpl() :
                m_pWeakReference(nullptr)
            {
            }

        protected:
            ~_ComposedWeakRefSourceStubImpl()
            {
                inspectable::ReleaseInterface(m_pWeakReference);
            }
            ComWeakReference *_Get_weak_reference(foundation::IUnknown *pHost) override
            {
                if (!m_pWeakReference)
                {
                    ComWeakReference::CreateInstance(
                        pHost,
                        &m_pWeakReference);
                }
                return m_pWeakReference;
            }
        private:
            ComWeakReference *m_pWeakReference;
        };

        ///
        /// @brief stub implementation of the '_IWeakRefSourceCountImpl' based on the
        /// atomic integer ref counting but without 'ComWeakReference' suuport
        ///
        class _ComposedNonWeakRefSourceStubImpl :
            public _RefCountStubImpl < _RefCountImpl, _IWeakRefSourceCountImpl >
        {
        protected:
            ComWeakReference *_Get_weak_reference(foundation::IUnknown *pHost) override
            {
                return nullptr;
            }
        };


        typedef _ComInspectableBase
            <
            _ComBase < foundation::IInspectable, _WeakRefSourceCountDelegeteImpl >
            > _ComposableWeakRefSourceBaseType;

        ///
        /// @brief IWeakReferenceSource implementation that use the 
        /// composable pattern to control ref count life time
        ///
        class ComposableWeakRefSourceInspectableBase :
            public foundation::IWeakReferenceSource,
            public _ComposableWeakRefSourceBaseType
        {
        public:
            HRESULT _InitializeComposableWeakReferenceSource(
                bool isComposedSafe,
                bool isComposedWeakRef
                )
            {
                if (!this->IsComposed() || (isComposedSafe && isComposedWeakRef))
                {
                    this->_SetRefCountImpl(new _WeakReferenceSourceStubImpl(inspectable::CastToUnknown(this)));
                }
                else
                {
                    // composed case
                    if (isComposedWeakRef)
                    {
                        this->_SetRefCountImpl(new _ComposedWeakRefSourceStubImpl());
                    }
                    else
                    {
                        this->_SetRefCountImpl(new _ComposedNonWeakRefSourceStubImpl());
                    }
                }
                return S_OK;
            }
        protected:

            HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ foundation::IUnknown **ppInterface) override
            {
                if (iid == foundation_IID_IWeakReferenceSource)
                {
                    if (this->_Get_weak_reference(inspectable::CastToUnknown(this)) == nullptr)
                    {
                        *ppInterface = nullptr;
                        return E_NOINTERFACE;
                    }

                    *ppInterface = static_cast<IWeakReferenceSource*>(this);
                    return S_OK;
                }

                return _ComposableWeakRefSourceBaseType::QueryInterfaceImpl(iid, ppInterface);
            }

        public:
            // IWeakReferenceSource methods
            STDMETHODIMP GetWeakReference(_COM_Outptr_ foundation::IWeakReference **ppWeakReference)
            {
                // Give a new reference away
                *ppWeakReference = this->_Get_weak_reference(inspectable::CastToUnknown(this));
                (*ppWeakReference)->AddRef();
                return S_OK;
            }
        };
    }
}
