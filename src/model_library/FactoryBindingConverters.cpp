/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FactoryBindingConverters.cpp
****/

#include "pch.h"
#include "FactoryBindingConverters.h"
#include <foundation/library/string_util.h>
#include <foundation/ctl/com_library.h>
#include <foundation/library/logger_macros.h>

using namespace pmod;
using namespace foundation;

typedef foundation::ctl::Implements<IBindingConverter, &IID_IBindingConverter> CBaseConverter;

// {AA86044A-C96C-488F-8987-D6AE9F28F482}
const GUID IID_LowerCaseConverter =
{ 0xaa86044a, 0xc96c, 0x488f, { 0x89, 0x87, 0xd6, 0xae, 0x9f, 0x28, 0xf4, 0x82 } };

// {A4B7A815-7115-4461-A864-4BFAC9A4B209}
const GUID IID_UpperCaseConverter =
{ 0xa4b7a815, 0x7115, 0x4461, { 0xa8, 0x64, 0x4b, 0xfa, 0xc9, 0xa4, 0xb2, 0x9 } };

// {C2D64D29-F8CE-410F-B3DF-34BBA68FEEEA}
const GUID IID_SubtrConverter =
{ 0xc2d64d29, 0xf8ce, 0x410f, { 0xb3, 0xdf, 0x34, 0xbb, 0xa6, 0x8f, 0xee, 0xea } };

// {47341843-2BC2-478F-AD6B-866397B3CBE5}
const GUID IID_DateConverter =
{ 0x47341843, 0x2bc2, 0x478f, { 0xad, 0x6b, 0x86, 0x63, 0x97, 0xb3, 0xcb, 0xe5 } };

// {5D134817-99C3-4176-9D76-02033AE7B1B0}
const GUID IID_TimeConverter =
{ 0x5d134817, 0x99c3, 0x4176, { 0x9d, 0x76, 0x2, 0x3, 0x3a, 0xe7, 0xb1, 0xb0 } };

// {57B048DB-56BD-422A-8DD9-D0C646BBBDC4}
const GUID IID_DateTimeAddConverter =
{ 0x57b048db, 0x56bd, 0x422a, { 0x8d, 0xd9, 0xd0, 0xc6, 0x46, 0xbb, 0xbd, 0xc4 } };

// {9CF86080-BD18-4A42-8A63-2E11B2876A85}
const GUID IID_IsTodayConverter =
{ 0x9cf86080, 0xbd18, 0x4a42, { 0x8a, 0x63, 0x2e, 0x11, 0xb2, 0x87, 0x6a, 0x85 } };

class CLowerCaseConverter : public CBaseConverter
{
    STDMETHOD(Convert)(
        foundation::IInspectable *value, 
        foundation::IInspectable *parameter,
        foundation::IInspectable** returnValue)
    {
        UNREFERENCED_PARAMETER(parameter);

        HStringPtr hstr;
        IFR(pv_util::GetStringValue(value, hstr.GetAddressOf()));
        string_t value_string_t = hstr.GetRawBuffer();
        foundation::library::string_util::to_lower(value_string_t);

        IFR_ASSERT(pv_util::CreateStringValue(value_string_t.c_str(), returnValue));
        return S_OK;
    }
};

class CUpperCaseConverter : public CBaseConverter
{
    STDMETHOD(Convert)(
        foundation::IInspectable *value, 
        foundation::IInspectable *parameter,
        foundation::IInspectable** returnValue)
    {
        UNREFERENCED_PARAMETER(parameter);

        HStringPtr hstr;
        IFR(pv_util::GetStringValue(value, hstr.GetAddressOf()));
        string_t value_string_t = hstr.GetRawBuffer();
        foundation::library::string_util::to_upper(value_string_t);

        IFR_ASSERT(pv_util::CreateStringValue(value_string_t.c_str(), returnValue));
        return S_OK;
    }
};

class CSubtrCaseConverter : public CBaseConverter
{

    STDMETHOD(Convert)(
        foundation::IInspectable *value,
        foundation::IInspectable *parameter,
        foundation::IInspectable** returnValue)
    {
        UNREFERENCED_PARAMETER(parameter);

        if (parameter == nullptr)
        {
            IFR_ASSERT(E_UNEXPECTED);
        }

        HStringPtr hstr;
        IFR(pv_util::GetStringValue(value, hstr.GetAddressOf()));
        string_t value_string_t = hstr.GetRawBuffer();

        foundation::InspectablePtr parameter_value(parameter);

        PropertyType type;
        pv_util::GetType(parameter, &type);
        if ((type & 0x400) != 0)
        {
            IFR_ASSERT(pv_util::ChangeType(PropertyType_Int32Array, parameter_value.GetAddressOfPtr()));
            Int32ArrayWrapper arrayWrapper;
            IFR_ASSERT(foundation::GetValue(parameter_value, arrayWrapper));
            if (arrayWrapper.GetSize() != 2)
            {
                IFR_ASSERT(E_UNEXPECTED);
            }
            value_string_t = value_string_t.substr(arrayWrapper[0], arrayWrapper[1]);
        }
        else
        {
            IFR_ASSERT(pv_util::ChangeType(PropertyType_Int32, parameter_value.GetAddressOfPtr()));
            INT32 index;
            IFR_ASSERT(foundation::GetValue(parameter_value, index));
            value_string_t = value_string_t.substr(index);
        }

        IFR_ASSERT(pv_util::CreateStringValue(value_string_t.c_str(), returnValue));
        return S_OK;
    }
};

class CDateTimeAddConverter : public CBaseConverter
{
    STDMETHOD(Convert)(
        foundation::IInspectable *value,
        foundation::IInspectable *parameter,
        foundation::IInspectable** returnValue)
    {
        foundation::DateTime dt;
        IFR(pv_util::GetValue(value, &dt));

        foundation::InspectablePtr parameter_ptr = parameter;
        IFR(pv_util::ChangeType(foundation::PropertyType_Int32, parameter_ptr.GetAddressOfPtr()));

        INT32 offset;
        IFR(pv_util::GetValue(parameter_ptr, &offset));

        foundation::DateTime dt_converted;
        _pal_DateTimeAdd(dt, offset, &dt_converted);

        IFR_ASSERT(pv_util::CreateDateTimeValue(dt_converted, returnValue));
        return S_OK;
    }
};

class CTimeConverter : public CBaseConverter
{
    STDMETHOD(Convert)(
        foundation::IInspectable *value,
        foundation::IInspectable *parameter,
        foundation::IInspectable** returnValue)
    {

        foundation::DateTime dt;
        IFR(pv_util::GetValue(value, &dt));

        foundation::SystemTime sys_time;
        _pal_DateTimeToSystemTime(dt, &sys_time);

        sys_time.wYear = foundation::_YEAR_OFFSET;
        sys_time.wMonth = 1;
        sys_time.wDay = 1;

        bool hasMillisecs = false;
        if (parameter)
        {
            IFR(pv_util::GetValue(parameter, &hasMillisecs));
        }
        if (!hasMillisecs)
        {
            sys_time.wMilliseconds = 0;
        }
        IFR_ASSERT(pv_util::CreateSystemTimeValue(sys_time, returnValue));
        return S_OK;
    }
};

class CDateConverter : public CBaseConverter
{
    STDMETHOD(Convert)(
        foundation::IInspectable *value,
        foundation::IInspectable *parameter,
        foundation::IInspectable** returnValue)
    {
        UNREFERENCED_PARAMETER(parameter);

        foundation::DateTime dt;
        IFR(pv_util::GetValue(value, &dt));

        foundation::SystemTime sys_time;
        _pal_DateTimeToSystemTime(dt, &sys_time);

        sys_time.wHour = 0;
        sys_time.wMinute = 0;
        sys_time.wSecond = 0;
        sys_time.wMilliseconds = 0;

        IFR_ASSERT(pv_util::CreateSystemTimeValue(sys_time, returnValue));
        return S_OK;
    }
};

class CIsTodayConverter : public CBaseConverter
{
    STDMETHOD(Convert)(
        foundation::IInspectable *value,
        foundation::IInspectable *parameter,
        foundation::IInspectable** returnValue)
    {
        UNREFERENCED_PARAMETER(parameter);

        foundation::DateTime dt;
        IFR(pv_util::GetValue(value, &dt));

        foundation::SystemTime now;
        _pal_SystemTimeNow(&now);

        foundation::SystemTime sys_time;
        _pal_DateTimeToSystemTime(dt, &sys_time);

        bool is_today = (now.wYear == sys_time.wYear) &&
            (now.wMonth == sys_time.wMonth) &&
            (now.wDay == sys_time.wDay);

        IFR_ASSERT(pv_util::CreateBooleanValue(is_today, returnValue));
        return S_OK;
    }
};

template <class T>
void CFactoryBindingConverters::AddBindingConverter(IID iidType, LPCSTR_t name)
{
    foundation::ComPtr<IBindingConverter> spBindingConverter;
    foundation::ctl::ComObject<T>::CreateInstance(
        spBindingConverter.GetAddressOf());
    _factoryBindingTypes[iidType] = spBindingConverter;
    _factoryNameTypes[name] = iidType;
}

CFactoryBindingConverters::CFactoryBindingConverters()
{
    AddBindingConverter<CLowerCaseConverter>(IID_LowerCaseConverter,U("lower_case"));
    AddBindingConverter<CLowerCaseConverter>(IID_UpperCaseConverter, U("upper_case"));
    AddBindingConverter<CSubtrCaseConverter>(IID_SubtrConverter, U("substr"));
    // date/time converters
    AddBindingConverter<CDateConverter>(IID_DateConverter, U("date"));
    AddBindingConverter<CTimeConverter>(IID_TimeConverter, U("time"));
    AddBindingConverter<CDateTimeAddConverter>(IID_DateTimeAddConverter, U("date_time_add"));
    AddBindingConverter<CIsTodayConverter>(IID_IsTodayConverter, U("is_today"));
}

CFactoryBindingConverters& CFactoryBindingConverters::GetInstance()
{
    static CFactoryBindingConverters _factoryBindingConverters;
    return _factoryBindingConverters;
}

HRESULT CFactoryBindingConverters::GetBindingConverter(IID iidType, pmod::IBindingConverter **ppBindingConverter)
{
    return GetInstance().GetBindingConverterInternal(iidType, ppBindingConverter);
}

HRESULT CFactoryBindingConverters::GetBindingConverter(LPCSTR_t name, pmod::IBindingConverter **ppBindingConverter)
{
    return GetInstance().GetBindingConverterInternal(name, ppBindingConverter);
}

HRESULT CFactoryBindingConverters::GetBindingConverterInternal(IID iidType, pmod::IBindingConverter **ppBindingConverter)
{
    _BindingConverterMap_Type::iterator iter = _factoryBindingTypes.find(iidType);
    if (iter != _factoryBindingTypes.end())
    {
        return (*iter).second.CopyTo(ppBindingConverter);
    }
    return E_FAIL;
}

HRESULT CFactoryBindingConverters::GetBindingConverterInternal(LPCSTR_t name, pmod::IBindingConverter **ppBindingConverter)
{
    _NameMap_Type::iterator iter = _factoryNameTypes.find(name);
    if (iter != _factoryNameTypes.end())
    {
        return GetBindingConverterInternal((*iter).second,ppBindingConverter);
    }
    return E_FAIL;
}
