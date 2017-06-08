/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:com_ptr_wrapper.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/ctl/com_library.h>

namespace foundation
{
    namespace library
    {
        template <class T, const IID& iidType>
        struct IComPtrWrapper_impl : public foundation::IInspectable
        {
        public:
            static const IID& GetIID() { return iidType; }
            typedef T _Ptr_Type;

            STDMETHOD(GetPtr)(T **ppValue) = 0;
        };

        template <class TFinalizer,class TInterface>
        class ComPtrWrapper : 
            public foundation::ctl::ComInspectableBase,
            public TInterface
        {
        public:
            HRESULT _Initialize(_In_ typename TInterface::_Ptr_Type *pPtr)
            {
                foundation_assert(pPtr != nullptr);
                _ptr = pPtr;
                return S_OK;
            }
        protected:
            typedef foundation::ctl::ComInspectableBase _BaseType;

            ComPtrWrapper() :
                _ptr(nullptr)
            {}

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == TInterface::GetIID())
                {
                    *ppInterface = static_cast<TInterface *>(this);
                }
                else
                {
                    return _BaseType::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }

            void FinalRelease() override
            {
                _BaseType::FinalRelease();
                TFinalizer::release(_ptr);
            }

            // Interface IComPtrWrapper_impl<>
            STDMETHOD(GetPtr)(typename TInterface::_Ptr_Type **ppValue)
            {
                *ppValue = _ptr;
                return S_OK;
            }
        private:
            typename TInterface::_Ptr_Type *_ptr;
        };

        template <class TInterface>
        inline static typename TInterface::_Ptr_Type * GetPtrWrapper(_In_ TInterface *pInterface)
        {
            if (pInterface == nullptr)
            {
                return nullptr;
            }
            typename TInterface::_Ptr_Type *pPtr = nullptr;
            HRESULT hr = pInterface->GetPtr(&pPtr);
            foundation_assert(hr == S_OK);
            return pPtr;
        }

        template <class TFinalizer, class TInterface>
        inline static HRESULT create_com_ptr_wrapper(
            _In_ typename TInterface::_Ptr_Type *pPtr,
            _COM_Outptr_ TInterface **ppValue)
        {
            return ctl::inspectable::CreateInstanceWithInitialize <
                ComPtrWrapper < TFinalizer, TInterface >> (ppValue, pPtr);
        }
    }
}
