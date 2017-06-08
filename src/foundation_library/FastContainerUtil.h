/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FastContainerUtil.h
****/
#pragma once

#include <foundation/pv_util.h>

namespace foundation {
    namespace library
    {

        struct _ArrayPropertyValueSlot {
            UINT32 _size;
            LPVOID _pArrayBuffer;
        };
    }
}
//------------------------------------------------------------------------------
// Class:   CFastContainerUtil
// Fast Container utils
//------------------------------------------------------------------------------
class CFastContainerUtil
{
public:
    template <class T>
    static T ReadSlotValue(LPVOID pSlotValue)
    {
        T value;
        memcpy(&value, pSlotValue, sizeof(T));
        return value;
    }

    template <class T>
    static void WriteSlotValue(LPVOID pSlotValue,_In_  const T *pValue)
    {
        foundation_assert(pValue != nullptr);
        memcpy(pSlotValue, pValue, sizeof(T));
    }

    template <class T>
    static HRESULT SetSlotValue(foundation::IInspectable *pValue, LPVOID pSlotValue)
    {
        T value;
        _IFR_(foundation::pv_util::GetValue(pValue, &value));
        WriteSlotValue(pSlotValue, &value);
        return S_OK;
    }

    static HRESULT ClearPropertySlot(foundation::PropertyType propertyType, LPVOID pSlotValue);
    static HRESULT GetPropertyValue(
        foundation::PropertyType propertyType, 
        LPVOID pSlotValue,
        foundation::IInspectable **value);
    static HRESULT SetPropertyValue(
        foundation::PropertyType propertyType,
        LPVOID pSlotValue,
        foundation::IInspectable *value);
    static size_t GetSlotSize(foundation::PropertyType propertyType);

private:
    static HRESULT ClearPropertySlotInternal(foundation::PropertyType _propertyType, LPVOID pSlotValue);
};
