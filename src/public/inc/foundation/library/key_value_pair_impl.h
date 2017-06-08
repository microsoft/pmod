/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:key_value_pair_impl.h
****/
#pragma once

#include <foundation/value_alloc.h>
#include <foundation/ctl/com_library.h>
#include "base_adapter_host.h"

namespace foundation {

    namespace library {

        template
            <
                class TKvpInterface,
                class _KeyWrapperAlloc = foundation::_ValueAlloc<typename TKvpInterface::_Key_Type>,
                class _ValueWrapperAlloc = foundation::_ValueAlloc<typename TKvpInterface::_Value_Type>,
                class TBASE = foundation::ctl::ComInspectableBase
            >
        class KeyValuePairBase :
            public TBASE,
            public TKvpInterface
        {
        public:
            typedef TKvpInterface _TKvpInterface_Type;

            HRESULT _Initialize(
                const typename TKvpInterface::_Key_Type& key, 
                typename TKvpInterface::_Value_Type value)
            {
                _KeyWrapperAlloc::CopyTo(key, _key);
                _ValueWrapperAlloc::CopyTo(value, _value);
                return S_OK;
            }

            HRESULT _SetKey(const typename TKvpInterface::_Key_Type& key)
            {
                _KeyWrapperAlloc::release(_key);
                _KeyWrapperAlloc::CopyTo(key, _key);
                return S_OK;
            }
            
            HRESULT _SetValue(typename TKvpInterface::_Value_Type value)
            {
                _ValueWrapperAlloc::release(_value);
                _ValueWrapperAlloc::CopyTo(value, _value);
                return S_OK;
            }


        protected:
            void FinalRelease() override
            {
                _KeyWrapperAlloc::release(_key);
                _ValueWrapperAlloc::release(_value);
                foundation::ctl::ComInspectableBase::FinalRelease();
            }

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == TKvpInterface::GetIID())
                {
                    *ppInterface = static_cast<TKvpInterface *>(this);
                }
                else
                {
                    return TBASE::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            STDMETHOD(GetKey)(_Out_ typename TKvpInterface::_Key_Type *key)
            {
                IFCPTR_ASSERT(key);

                _KeyWrapperAlloc::CopyTo(_key, *key);
                return S_OK;
            }

            STDMETHOD(GetValue)(_Out_ typename TKvpInterface::_Value_Type *value)
            {
                IFCPTR_ASSERT(value);

                _ValueWrapperAlloc::CopyTo(_value, *value);
                return S_OK;
            }
        private:
            typename TKvpInterface::_Key_Type       _key;
            typename TKvpInterface::_Value_Type       _value;
        };

        template<class TKvpInterface, class _KeyWrapperAlloc = _ValueAlloc<typename TKvpInterface::_Key_Type>>
        class _InspectableKeyValuePair : public KeyValuePairBase
            <
            TKvpInterface,
            _KeyWrapperAlloc,
            inspectable_ref_counted_value_alloc,
            _DefaultAdapterHost<TKvpInterface, foundation::ctl::ComInspectableBase>
            >
        {
        };

        template <class T>
        static HRESULT CreateKeyValuePair(
            const typename T::_TKvpInterface_Type::_Key_Type & key,
            const typename T::_TKvpInterface_Type::_Value_Type value,
            _COM_Outptr_ typename T::_TKvpInterface_Type **ppInterface)
        {
            ComPtr< ctl::ComInspectableObject<T> > instance_ptr;
            IFR_ASSERT(ctl::ComInspectableObject<T>::CreateInstance(instance_ptr.GetAddressOf()));

            IFR_ASSERT(instance_ptr->_Initialize(key, value));
            *ppInterface = static_cast<typename T::_TKvpInterface_Type *>(instance_ptr.Detach());
            return S_OK;
        }
    }

}

