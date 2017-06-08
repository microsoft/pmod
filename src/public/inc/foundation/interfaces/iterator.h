/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:iterator.h
****/
#pragma once

#include <foundation/com_interfaces.h>

namespace foundation {

    ////////////////////// IIterator declare ////////////////////////////
    template <class T, const IID& iidType = foundation_GUID_NULL>
    struct IIterator_impl : public foundation::IInspectable
    {
    public:
        typedef T _Item_Type;
        static const IID& GetIID() { return iidType; }

        STDMETHOD(GetCurrent)(T *current) = 0;
        STDMETHOD(GetHasCurrent)(_Out_ bool *hasCurrent) = 0;
        STDMETHOD(MoveNext)(_Out_ bool *hasCurrent) = 0;
    };

    BEGIN_DECLARE_IID(IIterator_Inspectable, FOUNDATION_API);
    typedef IIterator_impl<IInspectable *, IID_IIterator_Inspectable> IIterator_Inspectable;

}

