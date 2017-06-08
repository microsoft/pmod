/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:key_value_pair.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation {

    ////////////////////// Key Value Pair declare ////////////////////////////
    template <class K, class V, const IID& iidType>
    struct IKeyValuePair : public IInspectable
    {
    public:
        static const IID& GetIID() { return iidType; }

        typedef K _Key_Type;
        typedef V _Value_Type;

        STDMETHOD(GetKey)(_Out_ K *key) = 0;
        STDMETHOD(GetValue)(_Out_ V *value) = 0;
    };
}

