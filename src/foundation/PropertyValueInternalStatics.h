/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValueInternalStatics.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/interfaces/property_value.h>
#include <foundation/library/interfaces/property_value_statics.h>
#include <iostream>

#include <string>

class CPropertyValueInternalStatics :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IPropertyValueInternalStatics
{
 public:
     static foundation::library::IPropertyValueInternalStatics *GetInstance();
     static HRESULT GetOrCreateInstance(foundation::library::IPropertyValueInternalStatics **ppPropertyValueStatics);

    static foundation::string_t ToString(foundation::IPropertyValue *pPropertyValue);
    static foundation::string_t ToString(foundation::DateTime& dateTime);

    static HRESULT Serialize(std::ostream& os,foundation::IPropertyValue *pPropertyValue);
    static HRESULT Deserialize(std::istream& is,foundation::IPropertyValue **ppPropertyValue);
    static HRESULT Serialize(std::ostream& os,foundation::PropertyType type,foundation::IPropertyValue *pPropertyValue);
    static HRESULT Deserialize(std::istream& is,foundation::PropertyType type,foundation::IPropertyValue **ppPropertyValue);

    static HRESULT CreateDefaultValueInternal(
        foundation::PropertyType type,
        /*_COM_Outptr_result_maybenull_*/ foundation::IInspectable **ppValue);

//Interface IPropertyValueInternalStatics
STDMETHOD(ComparePropertyValues)(
    _In_ foundation::IPropertyValue *pPropertyValue1,
    _In_ foundation::IPropertyValue *pPropertyValue2,
    _Out_ int *pCompareResult);

STDMETHOD(CompareInspectableValues)(
    _In_opt_ foundation::IInspectable *value1,
    _In_opt_ foundation::IInspectable *value2,
    _Out_ int *pCompareResult);

STDMETHOD(ChangeType)(
    _In_    foundation::PropertyType type,
    _Inout_ foundation::IInspectable **ppValue);

STDMETHOD(ToString)(
    _In_ foundation::IPropertyValue *pPropertyValue,
    HSTRING *hstring);
STDMETHOD(Serialize)(
    _In_ foundation::IPropertyValue *pPropertyValue,
    _Out_ UINT32 *size,
    _Outptr_ UINT8 **data);
STDMETHOD(Deserialize)(
    UINT32 size,
    _In_ UINT8 *data,
    _COM_Outptr_ foundation::IPropertyValue **ppPropertyValue
    );
STDMETHOD(CreateDefaultValue)(
    foundation::PropertyType type,
    foundation::IInspectable **ppValue);

protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IID_IPropertyValueInternalStatics)
        {
            *ppInterface = static_cast<foundation::library::IPropertyValueInternalStatics *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

private:
    static HRESULT ChangeTypeInternal(
    _In_    foundation::PropertyType type,
    _Inout_ foundation::IInspectable **ppValue);

    static HRESULT CastToArray(    
        _In_    foundation::PropertyType  targetArrayType,
        _In_    foundation::IPropertyValue *pValue,
        _Inout_ foundation::IInspectable **ppValue);

    static HRESULT CreateArrayFromInspectableArray(
        foundation::PropertyType propertyType,
        _In_  foundation::IPropertyValue *pValue,
        foundation::IInspectable **ppValue);

};
