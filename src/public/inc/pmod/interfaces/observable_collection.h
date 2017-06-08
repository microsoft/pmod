/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_collection.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <foundation/interfaces/dictionary.h>
#include <foundation/interfaces/property_type_info.h>
#include <foundation/interfaces/readonly_collection.h>
#include <foundation/interfaces/object.h>
#include <foundation/foundation_macros.h>

namespace pmod {

    BEGIN_DECLARE_IID(IReadOnlyCollection_Inspectable, PMOD_API);
    typedef foundation::IReadOnlyCollection_impl<foundation::IInspectable *, IID_IReadOnlyCollection_Inspectable> IReadOnlyCollection_Inspectable;

    /////////////////////////////////// Observable Collection //////////////////////////////////////////////
    BEGIN_DECLARE_INTERFACE(INotifyCollectionChangedEventArgs, foundation::IInspectable, PMOD_API)
        STDMETHOD(GetAction)(_Out_ foundation::NotifyCollectionModelChangedAction* action) = 0;
        STDMETHOD(GetNewItems)(_Out_ UINT32 *size, _COM_Outptr_ foundation::IInspectable*** newItems) = 0;
        STDMETHOD(GetNewStartingIndex)(_Out_ UINT32* index) = 0;
        STDMETHOD(GetOldStartingIndex)(_Out_ UINT32* index) = 0;
        STDMETHOD(GetOldItems)(_Out_ UINT32 *size, _COM_Outptr_ foundation::IInspectable*** oldItems) = 0;
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(INotifyCollectionChanged, PMOD_API)

    template <class T, const IID& iidType, const IID& iidBaseType = iidType>
    struct IObservableCollection_impl : 
        public foundation::IReadOnlyCollection_impl<T, iidBaseType,foundation::IObject>
    {
    public:
        static const IID& GetIID() { return iidType; }

        STDMETHOD(GetItems)(_Out_ UINT32* size, T** items) = 0;
        STDMETHOD(IndexOf)(_In_ T item, _Out_ UINT32* pnIndex) = 0;

        // Event methods.
        STDMETHOD(GetNotifyCollectionChangedEventSource)(INotifyCollectionChangedEventSource** ppEventSource) = 0;
    };

    BEGIN_DECLARE_IID(IObservableCollection, PMOD_API);
    typedef IObservableCollection_impl<foundation::IInspectable *, IID_IObservableCollection,foundation::IID_IEnumerable> IObservableCollection;

    template <class T, const IID& iidType, class TBASE = IObservableCollection_impl<T, iidType> >
    struct IObservableList_impl : public TBASE
    {
    public:
        static const IID& GetIID() { return iidType; }
        typedef T _Item_Type;

        STDMETHOD(SetItem)(_In_ UINT32 nIndex, _In_ T item) = 0;
        STDMETHOD(AppendItem)(_In_ T item) = 0;
        STDMETHOD(InsertItem)(_In_ UINT32 nIndex, _In_ T item) = 0;
        STDMETHOD(RemoveItem)(_In_ T item) = 0;
        STDMETHOD(RemoveAt)(_In_ UINT32 nIndex) = 0;
        STDMETHOD(RemoveAll)() = 0;
    };

    BEGIN_DECLARE_IID(IObservableList, PMOD_API);
    typedef IObservableList_impl<foundation::IInspectable *, IID_IObservableList, IObservableCollection > IObservableList;

    // type safe collections
    BEGIN_DECLARE_IID(IObservableCollection_String, PMOD_API);
    typedef IObservableCollection_impl<HSTRING, IID_IObservableCollection_String, foundation::IID_IEnumerable_String> IObservableCollection_String;
    BEGIN_DECLARE_IID(IObservableCollection_Int32, PMOD_API);
    typedef IObservableCollection_impl<INT32, IID_IObservableCollection_Int32, foundation::IID_IEnumerable_Int32> IObservableCollection_Int32;
    BEGIN_DECLARE_IID(IObservableCollection_UInt32, PMOD_API);
    typedef IObservableCollection_impl<UINT32, IID_IObservableCollection_UInt32, foundation::IID_IEnumerable_UInt32> IObservableCollection_UInt32;
    BEGIN_DECLARE_IID(IObservableCollection_Int16, PMOD_API);
    typedef IObservableCollection_impl<INT16, IID_IObservableCollection_Int16, foundation::IID_IEnumerable_Int16> IObservableCollection_Int16;
    BEGIN_DECLARE_IID(IObservableCollection_UInt16, PMOD_API);
    typedef IObservableCollection_impl<UINT16, IID_IObservableCollection_UInt16, foundation::IID_IEnumerable_UInt16> IObservableCollection_UInt16;
    BEGIN_DECLARE_IID(IObservableCollection_Int64, PMOD_API);
    typedef IObservableCollection_impl<INT64, IID_IObservableCollection_Int64, foundation::IID_IEnumerable_Int64> IObservableCollection_Int64;
    BEGIN_DECLARE_IID(IObservableCollection_UInt64, PMOD_API);
    typedef IObservableCollection_impl<UINT64, IID_IObservableCollection_UInt64, foundation::IID_IEnumerable_UInt64> IObservableCollection_UInt64;
    BEGIN_DECLARE_IID(IObservableCollection_UInt8, PMOD_API);
    typedef IObservableCollection_impl<BYTE, IID_IObservableCollection_UInt8, foundation::IID_IEnumerable_UInt8> IObservableCollection_UInt8;
    BEGIN_DECLARE_IID(IObservableCollection_DateTime, PMOD_API);
    typedef IObservableCollection_impl<foundation::DateTime, IID_IObservableCollection_DateTime, foundation::IID_IEnumerable_DateTime> IObservableCollection_DateTime;
    BEGIN_DECLARE_IID(IObservableCollection_Boolean, PMOD_API);
    typedef IObservableCollection_impl<bool, IID_IObservableCollection_Boolean, foundation::IID_IEnumerable_Boolean> IObservableCollection_Boolean;

    typedef IObservableCollection IObservableCollection_Inspectable;
}


