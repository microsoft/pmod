/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:map.h
****/
#pragma once

#include <foundation/com_interfaces.h>

#include "iterator.h"

namespace foundation {

    ////////////////////// Map template declare ////////////////////////////
    template <class K, class V, class TKvpInterface, const IID& iidType>
    struct IMap : public IInspectable
    {
    public:
        static const IID& GetIID() { return iidType; }

        STDMETHOD(GetView)(_Out_ UINT32* size, _Outptr_ TKvpInterface ***ppKvpInterface) = 0;
        STDMETHOD(GetSize)(_Out_ UINT32* size) = 0;
        STDMETHOD(Lookup)(_In_ K key, V* value) = 0;
        STDMETHOD(HasKey)(_In_ K key, _Out_ bool* found) = 0;

        STDMETHOD(Insert)(_In_ K key, _In_ V value, _Out_ bool* replaced) = 0;
        STDMETHOD(Remove)(_In_ K key) = 0;
        STDMETHOD(Clear)() = 0;

        STDMETHOD(Keys)(_COM_Outptr_ IIterator_impl<K> **first) = 0;
    };
}

