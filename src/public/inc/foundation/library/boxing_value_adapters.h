/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:boxing_value_adapters.h
****/
#pragma once

#include <foundation/boxing_util.h>

namespace foundation {
    namespace library
    {

        struct _UnboxAdapter
        {
            template <class T>
            static HRESULT _From_inspectable_value(foundation::IInspectable *pValue,T& value)
            {
                return foundation::GetValue(pValue,value);
            }
        };
        struct _NoUnboxAdapter
        {
            template <class T>
            static HRESULT _From_inspectable_value(foundation::IInspectable *pValue,T& value)
            {
                foundation_assert(false);
                return E_INVALIDARG;
            }
        };

        // value adapter for ComPtr of classes
        template <class TInterface>
        struct _UnboxComClassPtrAdapter
        {
            template <class com_ptr_class_t>
            static HRESULT _From_inspectable_value(foundation::IInspectable *pValue,com_ptr_class_t& value)
            {
                foundation::ComPtr<TInterface> ptr;
                IFR_ASSERT(foundation::QueryInterfaceIf(pValue, ptr.GetAddressOf()));
                if(ptr != nullptr)
                {
                    value = static_cast<typename com_ptr_class_t::_PtrClass *>(ptr.Get());
                }
                return S_OK;
            }
        };

        // value adapter for ComBase of classes
        template <class TInterface>
        struct _UnboxComBaseAdapter
        {
            template <class com_base_ptr>
            static HRESULT _From_inspectable_value(foundation::IInspectable *pValue, com_base_ptr& value)
            {
                foundation::ComPtr<TInterface> ptr;
                IFR_ASSERT(foundation::QueryInterfaceIf(pValue, ptr.GetAddressOf()));
                if(ptr != nullptr)
                {
                    auto ptrClass = static_cast<com_base_ptr>(ptr.Get());
                    value = ptrClass;
                }
                else
                {
                    value = nullptr;
                }
                return S_OK;
            }
        };
    }
}
