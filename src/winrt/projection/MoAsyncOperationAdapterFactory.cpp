/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoAsyncOperationAdapterFactory.cpp
****/

#pragma once
#include "pch.h"

#include "Microsoft.PropertyModel.h"
// Public headers
#include <winrt/MoAsyncOperationAdapterBase.h>
#include <foundation/library/library_util.h>
#include <foundation/library/logger_macros.h>

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<INT16>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<INT16>
> CMoAsyncOperationAdapter_Int16;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<UINT16>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<UINT16>
> CMoAsyncOperationAdapter_UInt16;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<INT32>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<INT32>
> CMoAsyncOperationAdapter_Int32;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<UINT32>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<UINT32>
> CMoAsyncOperationAdapter_UInt32;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<INT64>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<INT64>
> CMoAsyncOperationAdapter_Int64;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<UINT64>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<UINT64>
> CMoAsyncOperationAdapter_UInt64;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<HSTRING>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<HSTRING>
> CMoAsyncOperationAdapter_String;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<BYTE>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<BYTE>
> CMoAsyncOperationAdapter_UInt8;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<FLOAT>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<FLOAT>
> CMoAsyncOperationAdapter_Single;

typedef TMoAsyncOperationAdapterBase
<
ABI::Windows::Foundation::IAsyncOperation<DOUBLE>,
ABI::Windows::Foundation::IAsyncOperationCompletedHandler<DOUBLE>
> CMoAsyncOperationAdapter_Double;

class CMoAsyncOperationAdapter_Boolean : public
    TMoAsyncOperationAdapterBase
    <
    ABI::Windows::Foundation::IAsyncOperation<boolean>,
    ABI::Windows::Foundation::IAsyncOperationCompletedHandler<boolean>,
    &__uuidof(ABI::Windows::Foundation::__FIAsyncOperation_1_boolean_t)
    >
{
protected:
    HRESULT STDMETHODCALLTYPE GetResults(boolean *results) override
    {
        bool boolValue;
        IFR(foundation::GetResult(this->m_pInner, &boolValue));
        *results = boolValue ? TRUE : FALSE;
        return S_OK;
    }
};
typedef TMoAsyncOperationAdapterBase
<
    ABI::Windows::Foundation::__FIAsyncOperation_1_Microsoft__CPropertyModel__CIObject_t,
    ABI::Windows::Foundation::__FIAsyncOperationCompletedHandler_1_Microsoft__CPropertyModel__CIObject_t
> CMoAsyncOperationAdapter_Object;

typedef TMoAsyncOperationAdapterBase
<
    ABI::Windows::Foundation::__FIAsyncOperation_1_Microsoft__CPropertyModel__CIDictionary_t,
    ABI::Windows::Foundation::__FIAsyncOperationCompletedHandler_1_Microsoft__CPropertyModel__CIDictionary_t
> CMoAsyncOperationAdapter_Dictionary;


HRESULT RegisterAsyncOperationAdapterFactories()
{
    // register factory winRT async operation adapters
    static const foundation::library::_TypeAdapterEntry _asyncOperationFactoryEntries[] = {
        {
            __uuidof(ABI::Windows::Foundation::__FIAsyncOperation_1_byte_t),
            foundation::IID_IAsyncOperation_UInt8,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_UInt8>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<INT16>),
            foundation::IID_IAsyncOperation_Int16,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_Int16>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<UINT16>),
            foundation::IID_IAsyncOperation_UInt16,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_UInt16>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<INT32>), 
            foundation::IID_IAsyncOperation_Int32, 
            foundation::CreateAdapter<CMoAsyncOperationAdapter_Int32>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<UINT32>),
            foundation::IID_IAsyncOperation_UInt32,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_UInt32>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<INT64>),
            foundation::IID_IAsyncOperation_Int64,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_Int64>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<UINT64>),
            foundation::IID_IAsyncOperation_UInt64,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_UInt64>
        }, {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<HSTRING>),
            foundation::IID_IAsyncOperation_String,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_String>
        },
        {
            __uuidof(ABI::Windows::Foundation::__FIAsyncOperation_1_boolean_t),
            foundation::IID_IAsyncOperation_Boolean,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_Boolean>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<FLOAT>),
            foundation::IID_IAsyncOperation_Single,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_Single>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<DOUBLE>),
            foundation::IID_IAsyncOperation_Double,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_Double>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Microsoft::PropertyModel::IObject>),
            foundation::IID_IAsyncOperation_IObject,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_Object>
        },
        {
            __uuidof(ABI::Windows::Foundation::IAsyncOperation<ABI::Microsoft::PropertyModel::IDictionary>),
            foundation::IID_IAsyncOperation_IDictionary,
            foundation::CreateAdapter<CMoAsyncOperationAdapter_Dictionary>
        },
        { GUID_NULL, GUID_NULL, 0L }
    };

    foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spModelAdapterFactory;
    IFR_ASSERT(foundation::library::CreateTypeAdapterFactory(_asyncOperationFactoryEntries, spModelAdapterFactory.GetAddressOf()));
    IFR_ASSERT(foundation::library::RegisterInterfaceAdapterFactory(spModelAdapterFactory));
    return S_OK;
}

