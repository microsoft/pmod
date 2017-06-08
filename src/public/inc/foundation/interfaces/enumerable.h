/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:enumerable.h
****/
#pragma once

#include  "iterator.h"
#include  "property_type_info.h"
#include  "object.h"

namespace foundation {

    BEGIN_DECLARE_INTERFACE(IEnumerableTypeInfo, IObjectTypeInfo, FOUNDATION_API)
        STDMETHOD(GetItemTypeInfo)(IPropertyTypeInfo **ppVarTypeInfo) = 0;
    END_DECLARE_INTERFACE()

    ////////////////////// IEnumerable declare ////////////////////////////
    template <class T, const IID& iidType, class TBASE = IObject>
    struct IEnumerable_impl : public TBASE
    {
    public:
        static const IID& GetIID() { return iidType; }
        typedef T _Item_Type;

        STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<T> **first) = 0;
    };

    BEGIN_DECLARE_IID(IEnumerable, FOUNDATION_API);
    typedef IEnumerable_impl<foundation::IInspectable *, IID_IEnumerable> IEnumerable;

    // type safe enumerables
    BEGIN_DECLARE_IID(IEnumerable_String, FOUNDATION_API);
    typedef IEnumerable_impl<HSTRING, IID_IEnumerable_String> IEnumerable_String;

    BEGIN_DECLARE_IID(IEnumerable_Int32, FOUNDATION_API);
    typedef IEnumerable_impl<INT32, IID_IEnumerable_Int32> IEnumerable_Int32;
    BEGIN_DECLARE_IID(IEnumerable_UInt32, FOUNDATION_API);
    typedef IEnumerable_impl<UINT32, IID_IEnumerable_UInt32> IEnumerable_UInt32;
    BEGIN_DECLARE_IID(IEnumerable_Int16, FOUNDATION_API);
    typedef IEnumerable_impl<INT16, IID_IEnumerable_Int16> IEnumerable_Int16;
    BEGIN_DECLARE_IID(IEnumerable_UInt16, FOUNDATION_API);
    typedef IEnumerable_impl<UINT16, IID_IEnumerable_UInt16> IEnumerable_UInt16;
    BEGIN_DECLARE_IID(IEnumerable_Int64, FOUNDATION_API);
    typedef IEnumerable_impl<INT64, IID_IEnumerable_Int64> IEnumerable_Int64;
    BEGIN_DECLARE_IID(IEnumerable_UInt64, FOUNDATION_API);
    typedef IEnumerable_impl<UINT64, IID_IEnumerable_UInt64> IEnumerable_UInt64;
    BEGIN_DECLARE_IID(IEnumerable_UInt8, FOUNDATION_API);
    typedef IEnumerable_impl<BYTE, IID_IEnumerable_UInt8> IEnumerable_UInt8;
    BEGIN_DECLARE_IID(IEnumerable_DateTime, FOUNDATION_API);
    typedef IEnumerable_impl<foundation::DateTime, IID_IEnumerable_DateTime> IEnumerable_DateTime;
    BEGIN_DECLARE_IID(IEnumerable_Boolean, FOUNDATION_API);
    typedef IEnumerable_impl<bool, IID_IEnumerable_Boolean> IEnumerable_Boolean;

    typedef IEnumerable IEnumerable_Inspectable;


}

