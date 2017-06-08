/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FastContainerUtil.cpp
****/

#include "pch.h"

#include "FastContainerUtil.h"

using namespace foundation;

HRESULT CFastContainerUtil::ClearPropertySlotInternal(
    foundation::PropertyType propertyType, 
    LPVOID pSlotValue)
{
    if (propertyType == PropertyType_String)
    {
        HSTRING hstring = ReadSlotValue<HSTRING>(pSlotValue);
        _pal_DeleteString(hstring);
        hstring = nullptr;
        WriteSlotValue<HSTRING>(pSlotValue, &hstring);
    }
    else if (propertyType == PropertyType_Inspectable)
    {
        foundation::IInspectable *pInspectable = ReadSlotValue<foundation::IInspectable *>(pSlotValue);
        if (pInspectable)
        {
            pInspectable->Release();
            pInspectable = nullptr;
        }
        WriteSlotValue<foundation::IInspectable *>(pSlotValue, &pInspectable);
    }
    return S_OK;
}

HRESULT CFastContainerUtil::ClearPropertySlot(
    foundation::PropertyType propertyType,
    LPVOID pSlotValue)
{
    if ((propertyType & 0x400) != 0)
    {
        // we have an array type
        foundation::library::_ArrayPropertyValueSlot arrayPropertyValueSlot =
            ReadSlotValue<foundation::library::_ArrayPropertyValueSlot>(pSlotValue);

        PropertyType itemArrayType = (PropertyType)(propertyType & 0xff);
        if (itemArrayType == PropertyType_String || itemArrayType == PropertyType_Inspectable)
        {
            LPVOID pItemPtr = arrayPropertyValueSlot._pArrayBuffer;
            for (unsigned int itemIndex = 0; 
                itemIndex < arrayPropertyValueSlot._size;
                ++itemIndex)
            {
                ClearPropertySlotInternal(itemArrayType, pItemPtr);
                // go to next slot item
                if (itemArrayType == PropertyType_String)
                {
                    pItemPtr = static_cast<UINT8 *>(pItemPtr)+sizeof(HSTRING);
                }
                else if (itemArrayType == PropertyType_Inspectable)
                {
                    pItemPtr = static_cast<UINT8 *>(pItemPtr)+sizeof(foundation::IInspectable *);
                }
            }
        }
        // cleanup arrayBuffer 
        if (arrayPropertyValueSlot._pArrayBuffer != nullptr)
        {
            _pal_MemFree(arrayPropertyValueSlot._pArrayBuffer);
            arrayPropertyValueSlot._pArrayBuffer = nullptr;
        }
        arrayPropertyValueSlot._size = 0;
        WriteSlotValue<foundation::library::_ArrayPropertyValueSlot>(pSlotValue,&arrayPropertyValueSlot);
    }
    else
    {
        ClearPropertySlotInternal(propertyType, pSlotValue);
    }
    return S_OK;
}

HRESULT CFastContainerUtil::GetPropertyValue(
    foundation::PropertyType propertyType, 
    LPVOID pSlotValue,
    foundation::IInspectable **value)
{

    if ((propertyType & 0x400) != 0)
    {
		foundation::library::_ArrayPropertyValueSlot arrayPropertyValueSlot =
            ReadSlotValue<foundation::library::_ArrayPropertyValueSlot>(pSlotValue);

        PropertyType itemArrayType = (PropertyType)(propertyType & 0xff);
        UINT32 size = arrayPropertyValueSlot._size;
        LPVOID arrayBuffer = arrayPropertyValueSlot._pArrayBuffer;

        switch (itemArrayType)
        {
        case PropertyType_UInt8:
            return pv_util::CreateUInt8ArrayValue(
                size,
                size ? static_cast<BYTE *>(arrayBuffer) : nullptr, value);
        case PropertyType_Int16:
            return pv_util::CreateInt16ArrayValue(
                size,
                size ? static_cast<INT16 *>(arrayBuffer) : nullptr, value);
        case PropertyType_UInt16:
            return pv_util::CreateUInt16ArrayValue(
                size,
                size ? static_cast<UINT16 *>(arrayBuffer) : nullptr, value);
        case PropertyType_Int32:
            return pv_util::CreateInt32ArrayValue(
                size,
                size ? static_cast<INT32 *>(arrayBuffer) : nullptr, value);
        case PropertyType_UInt32:
            return pv_util::CreateUInt32ArrayValue(
                size,
                size ? static_cast<UINT32 *>(arrayBuffer) : nullptr, value);
        case PropertyType_Boolean:
            return pv_util::CreateBooleanArrayValue(
                size,
                size ? static_cast<boolean *>(arrayBuffer) : nullptr, value);
        case PropertyType_String:
            return pv_util::CreateStringArrayValue(
                size,
                size ? static_cast<HSTRING *>(arrayBuffer) : nullptr, value);
        case PropertyType_Inspectable:
            return pv_util::CreateInspectableArrayValue(
                size,
                size ? static_cast<foundation::IInspectable **>(arrayBuffer) : nullptr, value);
        case PropertyType_Single:
            return pv_util::CreateSingleArrayValue(
                size,
                size ? static_cast<FLOAT *>(arrayBuffer) : nullptr, value);
        case PropertyType_Double:
            return pv_util::CreateDoubleArrayValue(
                size,
                size ? static_cast<DOUBLE *>(arrayBuffer) : nullptr, value);
        case PropertyType_Guid:
            return pv_util::CreateGuidArrayValue(
                size,
                size ? static_cast<GUID *>(arrayBuffer) : nullptr, value);
        case PropertyType_DateTime:
            return pv_util::CreateDateTimeArrayValue(
                size,
                size ? static_cast<DateTime *>(arrayBuffer) : nullptr, value);
        default:
            foundation_assert(false);
            return E_NOTIMPL;
        }
    }
    else
    {
        switch (propertyType)
        {
        case PropertyType_UInt8:
            return pv_util::CreateUInt8Value(ReadSlotValue<BYTE>(pSlotValue),value);
        case PropertyType_Int16:
            return pv_util::CreateInt16Value(ReadSlotValue<INT16>(pSlotValue), value);
        case PropertyType_UInt16:
            return pv_util::CreateUInt16Value(ReadSlotValue<UINT16>(pSlotValue), value);
        case PropertyType_Int32:
            return pv_util::CreateInt32Value(ReadSlotValue<INT32>(pSlotValue), value);
        case PropertyType_UInt32:
            return pv_util::CreateUInt32Value(ReadSlotValue<UINT32>(pSlotValue), value);
        case PropertyType_Int64:
            return pv_util::CreateInt64Value(ReadSlotValue<INT64>(pSlotValue), value);
        case PropertyType_UInt64:
            return pv_util::CreateUInt64Value(ReadSlotValue<UINT64>(pSlotValue), value);
        case PropertyType_Boolean:
            return pv_util::CreateBooleanValue(ReadSlotValue<boolean>(pSlotValue), value);
        case PropertyType_String:
            return pv_util::CreateStringValue(ReadSlotValue<HSTRING>(pSlotValue), value);
        case PropertyType_Inspectable:
            *value = ReadSlotValue<foundation::IInspectable *>(pSlotValue);
            if (*value)
            {
                (*value)->AddRef();
            }
            return S_OK;
        case PropertyType_Single:
            return pv_util::CreateSingleValue(ReadSlotValue<FLOAT>(pSlotValue), value);
        case PropertyType_Double:
            return pv_util::CreateDoubleValue(ReadSlotValue<DOUBLE>(pSlotValue), value);
        case PropertyType_Guid:
            return pv_util::CreateGuidValue(ReadSlotValue<GUID>(pSlotValue), value);
        case PropertyType_DateTime:
            return pv_util::CreateDateTimeValue(ReadSlotValue<DateTime>(pSlotValue), value);
        default:
            foundation_assert(false);
            return E_NOTIMPL;
        }
    }
    return S_OK;
}

HRESULT CFastContainerUtil::SetPropertyValue(
    foundation::PropertyType propertyType,
    LPVOID pSlotValue,
    foundation::IInspectable *value)
{
    ClearPropertySlot(propertyType,pSlotValue);
    if ((propertyType & 0x400) != 0)
    {
        if (value == nullptr)
        {
            return S_OK;
        }
		foundation::library::_ArrayPropertyValueSlot arrayPropertyValueSlot;
        arrayPropertyValueSlot._size = 0;
        arrayPropertyValueSlot._pArrayBuffer = nullptr;

        PropertyType itemArrayType = (PropertyType)(propertyType & 0xff);
        UINT32 *pSize = &arrayPropertyValueSlot._size;
        LPVOID *ppArrayBufferPtr = &arrayPropertyValueSlot._pArrayBuffer;

        switch (itemArrayType)
        {
        case PropertyType_UInt8:
            _IFR_(pv_util::GetUInt8Array(
                value, 
                pSize, 
                reinterpret_cast<BYTE **>(ppArrayBufferPtr)));
            break;
        case PropertyType_Int16:
            _IFR_(pv_util::GetInt16Array(
                value, 
                pSize, 
                reinterpret_cast<INT16 **>(ppArrayBufferPtr)));
            break;
        case PropertyType_UInt16:
            _IFR_(pv_util::GetUInt16Array(
                value, 
                pSize, 
                reinterpret_cast<UINT16 **>(ppArrayBufferPtr)));
            break;
        case PropertyType_Int32:
            _IFR_(pv_util::GetInt32Array(
                value, 
                pSize, 
                reinterpret_cast<INT32 **>(ppArrayBufferPtr)));
            break;
        case PropertyType_UInt32:
            _IFR_(pv_util::GetUInt32Array(
                value, 
                pSize, 
                reinterpret_cast<UINT32 **>(ppArrayBufferPtr)));
            break;
        case PropertyType_Boolean:
            _IFR_(pv_util::GetBooleanArray(
                value, 
                pSize, 
                reinterpret_cast<boolean **>(ppArrayBufferPtr)));
            break;
        case PropertyType_String:
            _IFR_(pv_util::GetStringArray(
                value, 
                pSize, 
                reinterpret_cast<HSTRING **>(ppArrayBufferPtr)));
            break;
        case PropertyType_Inspectable:
            _IFR_(pv_util::GetInspectableArray(
                value, 
                pSize, 
                reinterpret_cast<foundation::IInspectable ***>(ppArrayBufferPtr)));
            break;
        case PropertyType_Single:
            _IFR_(pv_util::GetSingleArray(
                value, 
                pSize, 
                reinterpret_cast<FLOAT **>(ppArrayBufferPtr)));
            break;
        case PropertyType_Double:
            _IFR_(pv_util::GetDoubleArray(
                value, 
                pSize, 
                reinterpret_cast<DOUBLE **>(ppArrayBufferPtr)));
            break;
        case PropertyType_Guid:
            _IFR_(pv_util::GetGuidArray(
                value, 
                pSize, 
                reinterpret_cast<GUID **>(ppArrayBufferPtr)));
            break;
        case PropertyType_DateTime:
            _IFR_(pv_util::GetDateTimeArray(
                value, 
                pSize, 
                reinterpret_cast<DateTime **>(ppArrayBufferPtr)));
            break;
        default:
            foundation_assert(false);
            return E_NOTIMPL;
        }
        WriteSlotValue<foundation::library::_ArrayPropertyValueSlot>(pSlotValue, &arrayPropertyValueSlot);
    }
    else
    {
        switch (propertyType)
        {
        case PropertyType_UInt8:
            return SetSlotValue<BYTE>(value,pSlotValue);
        case PropertyType_Int16:
            return SetSlotValue<INT16>(value, pSlotValue);
        case PropertyType_UInt16:
            return SetSlotValue<UINT16>(value, pSlotValue);
        case PropertyType_Int32:
            return SetSlotValue<INT32>(value, pSlotValue);
        case PropertyType_UInt32:
            return SetSlotValue<UINT32>(value, pSlotValue);
        case PropertyType_Int64:
            return SetSlotValue<INT64>(value, pSlotValue);
        case PropertyType_UInt64:
            return SetSlotValue<UINT64>(value, pSlotValue);
        case PropertyType_Boolean:
            {
                boolean boolValue;
                _IFR_(pv_util::GetBooleanValue(value, &boolValue));
                WriteSlotValue(pSlotValue, &boolValue);
                return S_OK;
            }
        case PropertyType_String:
            return SetSlotValue<HSTRING>(value, pSlotValue);
        case PropertyType_Inspectable:
            WriteSlotValue(pSlotValue, &value);
            if (value)
            {
                (value)->AddRef();
            }
            return S_OK;
        case PropertyType_Single:
            return SetSlotValue<FLOAT>(value, pSlotValue);
        case PropertyType_Double:
            return SetSlotValue<DOUBLE>(value, pSlotValue);
        case PropertyType_Guid:
            return SetSlotValue<GUID>(value, pSlotValue);
        case PropertyType_DateTime:
            return SetSlotValue<DateTime>(value, pSlotValue);
        default:
            foundation_assert(false);
            return E_NOTIMPL;
        }
    }
    return S_OK;
}

size_t CFastContainerUtil::GetSlotSize(foundation::PropertyType propertyType)
{
    size_t slot_size = 0;
    if ((propertyType & 0x400) != 0)
    {
        slot_size = sizeof(foundation::library::_ArrayPropertyValueSlot);
    }
    else
    {
        switch (propertyType)
        {
        case PropertyType_UInt8:
            slot_size = sizeof(BYTE);
            break;
        case PropertyType_Int16:
        case PropertyType_UInt16:
            slot_size= sizeof(INT16);
            break;
        case PropertyType_Int32:
        case PropertyType_UInt32:
            slot_size= sizeof(INT32);
            break;
        case PropertyType_Int64:
        case PropertyType_UInt64:
            slot_size= sizeof(INT64);
            break;
        case PropertyType_Boolean:
            slot_size= sizeof(boolean);
            break;
        case PropertyType_String:
            slot_size= sizeof(HSTRING);
            break;
        case PropertyType_Inspectable:
            slot_size= sizeof(foundation::IInspectable *);
            break;
        case PropertyType_Single:
            slot_size= sizeof(FLOAT);
            break;
        case PropertyType_Double:
            slot_size= sizeof(DOUBLE);
            break;
        case PropertyType_Guid:
            slot_size= sizeof(GUID);
            break;
        case PropertyType_DateTime:
            slot_size= sizeof(DateTime);
            break;
        default:
            foundation_assert(false);
            return E_NOTIMPL;
        }
    }
    return slot_size;
}
