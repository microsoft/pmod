/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pair.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation
{
    namespace library
    {
        template <class TFirst, class TSecond, const IID& iidType>
        struct IPair_Impl : public foundation::IInspectable
        {
            static const IID& GetIID() { return iidType; }
            STDMETHOD(GetFirst)(
                _COM_Outptr_result_maybenull_ TFirst **ppValue) = 0;
            STDMETHOD(GetSecond)(
                _COM_Outptr_result_maybenull_ TSecond **ppValue) = 0;
        };

        //------------------------------------------------------------------------------
        // Interface:   IPair
        //
        // Purpose: An Interface defining a Dispatcher pattern
        //
        //------------------------------------------------------------------------------
        BEGIN_DECLARE_IID(IPair, FOUNDATION_API);
        typedef IPair_Impl<IUnknown, IUnknown, IID_IPair> IPair;
    }
}

