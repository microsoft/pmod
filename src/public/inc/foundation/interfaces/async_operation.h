/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:async_operation.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>
#include <foundation/foundation_macros.h>

#include "object.h"
#include "property_type_info.h"
#include "dictionary.h"

namespace foundation {

    enum class ResultStatus
    {
        Started = 0,
        Completed = 1,
        Canceled = 2,
        Error = 3
    };


    BEGIN_DECLARE_INTERFACE(ICompletedEventArgs, IUnknown, FOUNDATION_API)
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(ICompleted, FOUNDATION_API)

    BEGIN_DECLARE_INTERFACE(IProgressEventArgs, IUnknown, FOUNDATION_API)
        STDMETHOD(GetProgressValue)(IInspectable **ppValue) = 0;
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(IProgress, FOUNDATION_API)

    BEGIN_DECLARE_INTERFACE(IAsyncOperationBase, IObject, FOUNDATION_API)
        // Property methods.
        STDMETHOD(GetError)(_Out_ HRESULT* hResult) = 0;
        STDMETHOD(GetStatus)(_Out_ ResultStatus* status) = 0;
        STDMETHOD(GetIsCancelable)(_Out_ bool* pVal) = 0;

        STDMETHOD(Cancel)() = 0;
        STDMETHOD(Wait)(UINT32 timeout, _Out_ ResultStatus* status) = 0;

        // Event methods.
        STDMETHOD(GetCompletedEventSource)(_COM_Outptr_ ICompletedEventSource** ppEventSource) = 0;
        STDMETHOD(GetProgressEventSource)(_COM_Outptr_ IProgressEventSource** ppEventSource) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IAsyncOperationTypeInfo, IObjectTypeInfo, FOUNDATION_API)
        STDMETHOD(GetResultTypeInfo)(_COM_Outptr_ IPropertyTypeInfo **ppVarTypeInfo) = 0;
    END_DECLARE_INTERFACE()

    template <class T, const IID& iidType>
    struct IAsyncOperation_impl : public IAsyncOperationBase
    {
        typedef T _Result_Type;

        static const IID& GetIID() { return iidType; }

        // Property methods.
        STDMETHOD(GetResult)(UINT32 timeout, T *pResult) = 0;
    };

    BEGIN_DECLARE_IID(IAsyncOperation, FOUNDATION_API);
    typedef IAsyncOperation_impl<IInspectable *, IID_IAsyncOperation> IAsyncOperation;

    // type safe Async Operations
    BEGIN_DECLARE_IID(IAsyncOperation_String, FOUNDATION_API);
    typedef IAsyncOperation_impl<HSTRING, IID_IAsyncOperation_String> IAsyncOperation_String;
    BEGIN_DECLARE_IID(IAsyncOperation_Int32, FOUNDATION_API);
    typedef IAsyncOperation_impl<INT32, IID_IAsyncOperation_Int32> IAsyncOperation_Int32;
    BEGIN_DECLARE_IID(IAsyncOperation_UInt32, FOUNDATION_API);
    typedef IAsyncOperation_impl<UINT32, IID_IAsyncOperation_UInt32> IAsyncOperation_UInt32;
    BEGIN_DECLARE_IID(IAsyncOperation_Int16, FOUNDATION_API);
    typedef IAsyncOperation_impl<INT16, IID_IAsyncOperation_Int16> IAsyncOperation_Int16;
    BEGIN_DECLARE_IID(IAsyncOperation_UInt16, FOUNDATION_API);
    typedef IAsyncOperation_impl<UINT16, IID_IAsyncOperation_UInt16> IAsyncOperation_UInt16;
    BEGIN_DECLARE_IID(IAsyncOperation_Int64, FOUNDATION_API);
    typedef IAsyncOperation_impl<INT64, IID_IAsyncOperation_Int64> IAsyncOperation_Int64;
    BEGIN_DECLARE_IID(IAsyncOperation_UInt64, FOUNDATION_API);
    typedef IAsyncOperation_impl<UINT64, IID_IAsyncOperation_UInt64> IAsyncOperation_UInt64;
    BEGIN_DECLARE_IID(IAsyncOperation_UInt8, FOUNDATION_API);
    typedef IAsyncOperation_impl<BYTE, IID_IAsyncOperation_UInt8> IAsyncOperation_UInt8;
    BEGIN_DECLARE_IID(IAsyncOperation_DateTime, FOUNDATION_API);
    typedef IAsyncOperation_impl<DateTime, IID_IAsyncOperation_DateTime> IAsyncOperation_DateTime;
    BEGIN_DECLARE_IID(IAsyncOperation_Boolean, FOUNDATION_API);
    typedef IAsyncOperation_impl<bool, IID_IAsyncOperation_Boolean> IAsyncOperation_Boolean;
    BEGIN_DECLARE_IID(IAsyncOperation_Single, FOUNDATION_API);
    typedef IAsyncOperation_impl<FLOAT, IID_IAsyncOperation_Single> IAsyncOperation_Single;
    BEGIN_DECLARE_IID(IAsyncOperation_Double, FOUNDATION_API);
    typedef IAsyncOperation_impl<DOUBLE, IID_IAsyncOperation_Double> IAsyncOperation_Double;
    BEGIN_DECLARE_IID(IAsyncOperation_Void, FOUNDATION_API);
    typedef IAsyncOperation_impl<IInspectable *, IID_IAsyncOperation> IAsyncOperation_Inspectable;
    BEGIN_DECLARE_IID(IAsyncOperation_IObject, FOUNDATION_API);
    typedef IAsyncOperation_impl<IObject *, IID_IAsyncOperation_IObject> IAsyncOperation_IObject;
    BEGIN_DECLARE_IID(IAsyncOperation_IDictionary, FOUNDATION_API);
    typedef IAsyncOperation_impl<IDictionary *, IID_IAsyncOperation_IDictionary> IAsyncOperation_IDictionary;

    template <class T, const IID& iidType>
    struct IAsyncOperationArray_impl : public IAsyncOperationBase
    {
        static const IID& GetIID() { return iidType; }

        // Property methods.
        STDMETHOD(GetResult)(UINT32 timeout,UINT32 *pSize, T **ppResultValues) = 0;
    };

    BEGIN_DECLARE_IID(IAsyncOperation_StringArray, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<HSTRING, IID_IAsyncOperation_StringArray> IAsyncOperation_StringArray;
    BEGIN_DECLARE_IID(IAsyncOperation_Int32Array, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<INT32, IID_IAsyncOperation_Int32Array> IAsyncOperation_Int32Array;
    BEGIN_DECLARE_IID(IAsyncOperation_UInt32Array, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<UINT32, IID_IAsyncOperation_UInt32Array> IAsyncOperation_UInt32Array;
    BEGIN_DECLARE_IID(IAsyncOperation_Int16Array, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<INT16, IID_IAsyncOperation_Int16Array> IAsyncOperation_Int16Array;
    BEGIN_DECLARE_IID(IAsyncOperation_UInt16Array, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<UINT16, IID_IAsyncOperation_UInt16Array> IAsyncOperation_UInt16Array;
    BEGIN_DECLARE_IID(IAsyncOperation_Int64Array, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<INT64, IID_IAsyncOperation_Int64Array> IAsyncOperation_Int64Array;
    BEGIN_DECLARE_IID(IAsyncOperation_UInt64Array, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<UINT64, IID_IAsyncOperation_UInt64Array> IAsyncOperation_UInt64Array;
    BEGIN_DECLARE_IID(IAsyncOperation_UInt8Array, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<BYTE, IID_IAsyncOperation_UInt8Array> IAsyncOperation_UInt8Array;
    BEGIN_DECLARE_IID(IAsyncOperation_DateTimeArray, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<DateTime, IID_IAsyncOperation_DateTimeArray> IAsyncOperation_DateTimeArray;
    BEGIN_DECLARE_IID(IAsyncOperation_BooleanArray, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<bool, IID_IAsyncOperation_BooleanArray> IAsyncOperation_BooleanArray;
    BEGIN_DECLARE_IID(IAsyncOperation_InspectableArray, FOUNDATION_API);
    typedef IAsyncOperationArray_impl<IInspectable *, IID_IAsyncOperation_InspectableArray> IAsyncOperation_InspectableArray;
}

