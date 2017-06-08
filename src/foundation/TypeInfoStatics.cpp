/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeInfoStatics.cpp
****/
#include "pch.h"

#include "TypeInfoStatics.h"

#include "TypeInfo.h"
#include "EnumTypeClassInfoCache.h"
#include "AsyncOperationClassInfoCache.h"
#include "PropertyTypeInfo.h"
#include "ActivationFactory.h"

#include <foundation/string_t.h>

#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;

// {DE7BD90F-06D8-46B8-AA9A-3D114FD1C983}
const GUID foundation::IID_ITypeInfoStatics =
{ 0x9dcfb3f5, 0x21b7, 0x457d,{ 0xac, 0x45, 0x97, 0x60, 0xc3, 0x59, 0xf1, 0xd0 } };

ITypeInfoStatics * CTypeInfoStatics::GetInstance()
{
    return library::_GetFactoryInstance<CTypeInfoStatics, ITypeInfoStatics>();
}

HRESULT CTypeInfoStatics::GetOrCreateInstance(ITypeInfoStatics **ppModelTypeInfoStatics)
{
    IFCPTR_ASSERT(ppModelTypeInfoStatics);
    *ppModelTypeInfoStatics = GetInstance();
    (*ppModelTypeInfoStatics)->AddRef();
    return S_OK;
}

STDMETHODIMP CTypeInfoStatics::GetTypeInfos(
    _In_opt_ _TypeInfoFilter *pTypeInfoFilter,
    _Out_ UINT32 *size,
    _Outptr_ IObjectTypeInfo ***ppTypeInfos)
{
    IFCPTR_ASSERT(size);
    IFCPTR_ASSERT(ppTypeInfos);
    IFR_ASSERT(CTypeInfoCache::GetTypeInfos(pTypeInfoFilter, size, ppTypeInfos));
    return S_OK;
}

STDMETHODIMP CTypeInfoStatics::GetTypeInfo(
            GUID iidType,
            IObjectTypeInfo **ppTypeInfo)
{
    IFCPTR_ASSERT(ppTypeInfo);
    IFR_ASSERT(CTypeInfoCache::GetTypeInfo(iidType, ppTypeInfo));
    return S_OK;
}

STDMETHODIMP CTypeInfoStatics::GetEnumTypeInfo(
    UINT32 enumTypeId,
    IEnumTypeInfo **ppEnumTypeInfo)
{
    IFCPTR_ASSERT(ppEnumTypeInfo);

    CEnumTypeInfo *pEnumTypeInfo = CEnumTypeClassInfoCache::LookupTypeByTypeId(enumTypeId);
    if(pEnumTypeInfo == nullptr)
    {
        return foundation::E_TYPE_INFO_ENUM_NOT_REGISTERED;
    }
    *ppEnumTypeInfo = pEnumTypeInfo;
    (*ppEnumTypeInfo)->AddRef();
    return S_OK;
}

STDMETHODIMP CTypeInfoStatics::GetEnumTypeInfoByName(
    HSTRING enumTypeName,
    IEnumTypeInfo **ppEnumTypeInfo)
{
    IFCPTR_ASSERT(ppEnumTypeInfo);

    CEnumTypeInfo *pEnumTypeInfo = CEnumTypeClassInfoCache::LookupTypeByName(_pal_GetStringRawBuffer(enumTypeName,nullptr));
    if(pEnumTypeInfo == nullptr)
    {
        return foundation::E_TYPE_INFO_ENUM_NOT_REGISTERED;
    }
    *ppEnumTypeInfo = pEnumTypeInfo;
    (*ppEnumTypeInfo)->AddRef();
    return S_OK;
}

STDMETHODIMP CTypeInfoStatics::GetEnumTypeInfoByNamespaceAndName(
    HSTRING Namespace,
    HSTRING Name,
    IEnumTypeInfo **ppEnumTypeInfo)
{
    IFCPTR_ASSERT(ppEnumTypeInfo);

    CEnumTypeInfo *pEnumTypeInfo = CEnumTypeClassInfoCache::LookupTypeByNamespaceAndName(
        _pal_GetStringRawBuffer(Namespace, nullptr),
        _pal_GetStringRawBuffer(Name, nullptr)
        );
    if (pEnumTypeInfo == nullptr)
    {
        return foundation::E_TYPE_INFO_ENUM_NOT_REGISTERED;
    }
    *ppEnumTypeInfo = pEnumTypeInfo;
    (*ppEnumTypeInfo)->AddRef();
    return S_OK;
}

STDMETHODIMP CTypeInfoStatics::GetEnumValueByName(
    HSTRING enumValueName,
    IEnumValue **ppEnumValue)
{
    IFCPTR_ASSERT(ppEnumValue);

    foundation::string_t full_enum_value_name_str_t = _pal_GetStringRawBuffer(enumValueName, nullptr);
    size_t pos;

    if ((pos = full_enum_value_name_str_t.find_last_of('.')) == foundation::string_t::npos)
    {
        return E_UNEXPECTED;
    }

    foundation::string_t enum_type_name_str_t = full_enum_value_name_str_t.substr(0, pos);
    foundation::string_t enum_value_name_str_t = full_enum_value_name_str_t.substr(pos + 1);

    foundation::ComPtr<IEnumTypeInfo> spEnumTypeInfo;
    IFR_ASSERT(this->GetEnumTypeInfoByName(foundation::HStringRef(enum_type_name_str_t.c_str()), spEnumTypeInfo.GetAddressOf()));
    return spEnumTypeInfo->GetEnumValueByName(foundation::HStringRef(enum_value_name_str_t.c_str()), ppEnumValue);
}

STDMETHODIMP CTypeInfoStatics::GetEnumValueByValue(
    UINT32 enumValue,
    IEnumValue **ppEnumValue)
{
    IFCPTR_ASSERT(ppEnumValue);

    return CEnumTypeClassInfoCache::LookupEnumValue(enumValue, ppEnumValue);
}

STDMETHODIMP CTypeInfoStatics::GetAsyncOperationTypeInfo(
    IPropertyTypeInfo *pResultTypeInfo,
    IAsyncOperationTypeInfo **ppAsyncOperationTypeInfo)
{
    IFCPTR_ASSERT(pResultTypeInfo);
    IFCPTR_ASSERT(ppAsyncOperationTypeInfo);

    CAsyncOperationTypeInfo *pAsyncOperationTypeInfo = CAsyncOperationClassInfoCache::LookupTypeByPropertyTypeInfo(pResultTypeInfo);
    if (pAsyncOperationTypeInfo == nullptr)
    {
        return foundation::E_TYPE_INFO_NOT_REGISTERED;
    }
    *ppAsyncOperationTypeInfo = pAsyncOperationTypeInfo;
    (*ppAsyncOperationTypeInfo)->AddRef();
    return S_OK;
}

STDMETHODIMP CTypeInfoStatics::IsInstanceOfType(foundation::IObject *pObject, REFIID iidInstanceType, bool *pResult)
{
    return IsTypeOfInternal(pObject, iidInstanceType, pResult);
}

STDMETHODIMP CTypeInfoStatics::VerifyType(foundation::IObject *pObject, REFIID iidExpectedType)
{
    bool result;
    _IFR_(IsInstanceOfType(pObject, iidExpectedType, &result));
    return result ? S_OK : foundation::E_WRONG_TYPE;
}

STDMETHODIMP CTypeInfoStatics::IsInstanceOfTypeWithOptions(
    foundation::IObject *pObject,
    REFIID iidInstanceType,
    foundation::InstanceTypeOptions options,
    bool *pResult)
{
    return IsTypeOfInternalWithOptions(pObject, iidInstanceType, options, pResult);
}

STDMETHODIMP CTypeInfoStatics::VerifyPropertyTypeInfo(
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
    _Inout_ foundation::IInspectable **ppValue)
{
    return _VerifyPropertyTypeInfo(pPropertyTypeInfo, ppValue);
}

HRESULT CTypeInfoStatics::IsTypeOfInternal(
    IPropertyTypeInfo *pVarTypeInfo,
    IPropertyTypeInfo *pOtherVarTypeInfo,
    bool *pResult)
{
    foundation_assert(pVarTypeInfo);
    foundation_assert(pOtherVarTypeInfo);
    foundation_assert(pResult);

    foundation::PropertyType type, otherType;
    IFR_ASSERT(pVarTypeInfo->GetPropertyType(&type));
    IFR_ASSERT(pOtherVarTypeInfo->GetPropertyType(&otherType));

    *pResult = false;

    if (type != otherType)
    {
        return S_OK;
    }

    if (type != foundation::PropertyType_Inspectable)
    {
        *pResult = true;
        return S_OK;
    }

    foundation::ComPtr<IObjectTypeInfo> spTypeInfo;
    IFR_ASSERT(pVarTypeInfo->GetTypeInfo(spTypeInfo.GetAddressOf()));

    foundation::ComPtr<IObjectTypeInfo> spOtherTypeInfo;
    IFR_ASSERT(pOtherVarTypeInfo->GetTypeInfo(spOtherTypeInfo.GetAddressOf()));

    if (spTypeInfo.Get() == nullptr && spOtherTypeInfo.Get() == nullptr)
    {
        *pResult = true;
        return S_OK;
    }

    if (spTypeInfo && spOtherTypeInfo)
    {
        IID iidType;
        IFR_ASSERT(spOtherTypeInfo->GetType(&iidType));
        IFR_ASSERT(IsTypeOfInternal(spTypeInfo, iidType, pResult));
    }
    return S_OK;
}

HRESULT CTypeInfoStatics::IsTypeOfInternal(
    IObjectTypeInfo *pTypeInfo, 
    REFIID iidInstanceType, 
    bool *pResult)
{
    foundation_assert(pTypeInfo);
    foundation_assert(pResult);

    *pResult = false;

    foundation::ComPtr<IObjectTypeInfo> spType(pTypeInfo);
    while (spType != nullptr)
    {
        IFR_ASSERT(IsTypeEqual(spType, iidInstanceType, pResult));
        if (*pResult)
        {
            break;
        }
        // Try the Base Type then
        foundation::ComPtr<IObjectTypeInfo> spBaseType;
        IFR_ASSERT(spType->GetBaseType(spBaseType.GetAddressOf()));
        spType = spBaseType;
    }
    return S_OK;
}

HRESULT CTypeInfoStatics::IsTypeEqual(
    IObjectTypeInfo *pTypeInfo,
    REFIID iidInstanceType, bool *pResult)
{
    foundation_assert(pTypeInfo);
    foundation_assert(pResult);

    *pResult = false;
    IID iidType;
    IFR_ASSERT(pTypeInfo->GetType(&iidType));
    if (IsEqualIID(iidInstanceType, iidType))
    {
        *pResult = true;
    }
    return S_OK;
}

HRESULT CTypeInfoStatics::GetTypeInternal(
    _In_ IPropertyTypeInfo *pPropertyTypeInfo,
    _Out_ IID* pIIdType)
{
    foundation_assert(pPropertyTypeInfo);
    foundation_assert(pIIdType);

    foundation::ComPtr<IObjectTypeInfo> spTypeInfo;
    IFR_ASSERT(pPropertyTypeInfo->GetTypeInfo(spTypeInfo.GetAddressOf()));
    if (spTypeInfo != nullptr)
    {
        IFR_ASSERT(spTypeInfo->GetType(pIIdType));
        return S_OK;
    }
    else
    {
        *pIIdType = foundation_GUID_NULL;
        return S_FALSE;
    }
}
HRESULT CTypeInfoStatics::GetTypeInternal(
    _In_ foundation::IObject *pObject,
    _Out_ IID* pIIdType)
{
    foundation_assert(pObject);

    foundation::ComPtr<IObjectTypeInfo> spTypeInfo;
    IFR_ASSERT(pObject->GetTypeInfo(spTypeInfo.GetAddressOf()));
    IFR_ASSERT(spTypeInfo->GetType(pIIdType));
    return S_OK;
}

HRESULT CTypeInfoStatics::VerifyPropertyTypeInfoInternal(
    foundation::IPropertyTypeInfo *pVarTypeInfo,
    foundation::IPropertyTypeInfo *pOtherVarTypeInfo
    )
{
    bool result;
    IFR_ASSERT(IsTypeOfInternal(pVarTypeInfo, pOtherVarTypeInfo, &result));
    return result ? S_OK : foundation::E_WRONG_TYPE;
}

HRESULT CTypeInfoStatics::VerifyEnumTypeVarInternal(
    foundation::IEnumTypeInfo *pEnumTypeInfo, 
    _In_ foundation::IInspectable **ppValue)
{
    foundation_assert(ppValue);
    foundation_assert(pEnumTypeInfo);
    UINT32 enumValue = 0;
    if (*ppValue != nullptr)
    {
        IFR_ASSERT(foundation::pv_util::GetValue(*ppValue, &enumValue));
    }
    bool isValid;
    IFR_ASSERT(pEnumTypeInfo->IsEnumValid(enumValue, &isValid));
    if (!isValid)
    {
        return E_WRONG_TYPE;
    }
    // ensure we have an IEnumValue
    if (*ppValue == nullptr || !foundation::IsTypeOf<foundation::IEnumValue>(*ppValue))
    {
        if (*ppValue != nullptr)
        {
            (*ppValue)->Release();
            IFR_ASSERT(pEnumTypeInfo->GetEnumValue(enumValue, reinterpret_cast<foundation::IEnumValue **>(ppValue)));
        }
    }
    return S_OK;
}

HRESULT CTypeInfoStatics::VerifyArrayEnumTypeVarInternal(
    foundation::IEnumTypeInfo *pEnumTypeInfo, 
    _In_ foundation::IInspectable **ppValue)
{
    foundation_assert(ppValue);
    if (*ppValue == nullptr)
    { 
        return S_OK;
    }
    std::vector<foundation::InspectablePtr> values;

    foundation::PropertyType propertyType;
    IFR_ASSERT(pv_util::GetType(*ppValue, &propertyType));
    if (propertyType == foundation::PropertyType_UInt32Array)
    {
        UInt32ArrayWrapper enum_values_wrapper;
        IFR_ASSERT(pv_util::GetValue(
            *ppValue,
            enum_values_wrapper.GetSizeAddressOf(),
            enum_values_wrapper.GetBufferAddressOf()));
        values.resize(enum_values_wrapper.GetSize());
        // box the values
        for (UINT32 index = 0; index < enum_values_wrapper.GetSize(); ++index)
        {
            values[index] = pv_util::CreateValue(enum_values_wrapper[index]);
        }
    }
    else
    {
        InspectableArrayWrapper enum_values_wrapper;
        IFR_ASSERT(pv_util::GetValue(
            *ppValue,
            enum_values_wrapper.GetSizeAddressOf(),
            enum_values_wrapper.GetBufferAddressOf()));
        values.resize(enum_values_wrapper.GetSize());
        for (UINT32 index = 0; index < enum_values_wrapper.GetSize(); ++index)
        {
            values[index] = enum_values_wrapper[index];
        }
    }
    for (std::vector<foundation::InspectablePtr>::iterator iter = values.begin();
        iter != values.end();
        ++iter
        )
    {
        _IFR_(VerifyEnumTypeVarInternal(pEnumTypeInfo, (*iter).GetAddressOfPtr()));
    }
    (*ppValue)->Release();
    *ppValue = nullptr;
    return pv_util::CreateInspectableArrayValue(
        (UINT32)values.size(),
        values.size() ? values.front().GetAddressOfPtr():nullptr,
        ppValue);
}

HRESULT CTypeInfoStatics::VerifyTypeVarInternal(
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
    _Inout_ foundation::IInspectable **ppValue)
{
    foundation_assert(pPropertyTypeInfo);

    // verify correct type
    foundation::PropertyType propertyType;
    IFR_ASSERT(pPropertyTypeInfo->GetPropertyType(&propertyType));

    // If 'Object' type we are ok
    if (propertyType == foundation::PropertyType_Inspectable)
    {
        return S_OK;
    }

    // If 'Empty' type && null we are ok too
    if (propertyType == foundation::PropertyType_Empty && *ppValue == nullptr)
    {
        return S_OK;
    }

    // attempt a 'ChangeType'
    _IFR_(foundation::pv_util::ChangeType(propertyType, ppValue));
    return S_OK;
}

HRESULT CTypeInfoStatics::VerifyTypeObjectInternal(
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
    _In_ foundation::IUnknown *pValueUnknown)
{
    foundation_assert(pPropertyTypeInfo);
    foundation_assert(pValueUnknown);

    IID iidType;
    HRESULT hr = CTypeInfoStatics::GetTypeInternal(pPropertyTypeInfo, &iidType);
    _IFHR_(hr);
    if (hr == S_FALSE)
    {
        // Property Type Info does not have Type IID
        return S_OK;
    }

    foundation::ComPtr<IObject> spObject;
    if (FAILED(foundation::QueryInterface(pValueUnknown, spObject.GetAddressOf())))
    {
        // not even an IObject
        return foundation::E_WRONG_TYPE;
    }
    bool result;
    _IFR_(IsTypeOfInternal(spObject, iidType, &result));

    if (!result)
    {
        // it is an IObject but of a different type
        return foundation::E_WRONG_TYPE;
    }
    return S_OK;
}

HRESULT CTypeInfoStatics::IsTypeOfInternal(foundation::IObject *pObject, REFIID iidInstanceType, bool *pResult)
{
    return IsTypeOfInternalWithOptions(
        pObject,
        iidInstanceType,
        InstanceTypeOptions::All,
        pResult);
}

HRESULT CTypeInfoStatics::IsTypeOfInternalWithOptions(
    foundation::IObject *pObject,
    REFIID iidInstanceType,
    InstanceTypeOptions options,
    bool *pResult)
{
    IFCPTR_ASSERT(pObject);
    IFCPTR_ASSERT(pResult);

    HRESULT hr = S_OK;

    if (IsOptionEnabled(options,InstanceTypeOptions::AllowQueryInterface))
    {
        foundation::ComPtr<foundation::IUnknown> spUnk;
        if (SUCCEEDED(pObject->QueryInterface(iidInstanceType, (void **)&spUnk)))
        {
            *pResult = TRUE;
            return S_OK;
        }
    }

    foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
    IFHR_ASSERT(pObject->GetTypeInfo(spTypeInfo.GetAddressOf()));
    foundation_assert(spTypeInfo);
    if (IsOptionEnabled(options,InstanceTypeOptions::AllowBaseType))
    {
        IFHR_ASSERT(IsTypeOfInternal(spTypeInfo, iidInstanceType, pResult));
    }
    else
    {
        IFHR_ASSERT(IsTypeEqual(spTypeInfo, iidInstanceType, pResult));
    }

    ITypeInfoInternalSupport *pTypeInfoInternalSupport = nullptr;
    if (!*pResult && SUCCEEDED(foundation::QueryWeakReference(pObject, &pTypeInfoInternalSupport)))
    {
        IFHR_ASSERT(pTypeInfoInternalSupport->IsInstanceOfType(iidInstanceType, options, pResult));
    }
    return S_OK;
}

HRESULT CTypeInfoStatics::VerifyPropertyTypeInfoInternal(
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
    _Inout_ foundation::IInspectable **ppValue)
{
    foundation::PropertyType propertyType;
    pPropertyTypeInfo->GetPropertyType(&propertyType);

    IID typeInfoType = GetTypeInfoTypeInternal(pPropertyTypeInfo);

    _IFR_(CTypeInfoStatics::VerifyTypeVarInternal(pPropertyTypeInfo, ppValue));
    if (typeInfoType == foundation_GUID_NULL || *ppValue == nullptr)
    {
        // foundation::IUnknown is null no further verification is neccesary
        return S_OK;
    }
    else if (typeInfoType == foundation::IEnumTypeInfo::GetIID())
    {
        foundation::IEnumTypeInfo *pEnumTypeInfo =
            CastTypeInfoTypeInternal<foundation::IEnumTypeInfo>(pPropertyTypeInfo);

        return (propertyType & 0x400) != 0 ?
            CTypeInfoStatics::VerifyArrayEnumTypeVarInternal(pEnumTypeInfo, ppValue) :
            CTypeInfoStatics::VerifyEnumTypeVarInternal(pEnumTypeInfo, ppValue);
    }

    // we have a type being defined and we need further verification
    if (propertyType == foundation::PropertyType_InspectableArray)
    {
        // inspect the array to verify type info for each element
        foundation::InspectableArrayWrapper arrayWrapper;
        IFR_ASSERT(foundation::pv_util::GetValue(*ppValue,
            arrayWrapper.GetSizeAddressOf(),
            arrayWrapper.GetBufferAddressOf()));
        for (UINT32 index = 0; index < arrayWrapper.GetSize(); ++index)
        {
            foundation::InspectablePtr spItem = arrayWrapper[index];
            _IFR_(VerifyPropertyTypeInfoInternal(
                pPropertyTypeInfo,
                typeInfoType,
                spItem.GetAddressOfPtr()));
        }
        return S_OK;
    }
    return VerifyPropertyTypeInfoInternal(pPropertyTypeInfo, typeInfoType, ppValue);
}

HRESULT CTypeInfoStatics::VerifyPropertyTypeInfoInternal(
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
    const GUID& typeInfoType,
    _In_ foundation::IInspectable **ppValue)
{
    foundation::IInspectable *pValue = *ppValue;
    foundation_assert(pValue);

    IID iidType;
    HRESULT hr = CTypeInfoStatics::GetTypeInternal(pPropertyTypeInfo, &iidType);
    IFHR_ASSERT(hr);
    if (hr == S_FALSE)
    {
        // VarType Info does not have Type IID
        return S_OK;
    }

    // TODO: until we fully support the IObject in a IDictionary
    if (iidType == IID_IDictionary && ::IsTypeOf<IDictionary>(pValue))
    {
        // asked type is of IDictionary
        return S_OK;
    }

    // in a normal situation we expect an IObject to be verified, but still
    // pValue at this point can be an IPropertyValue with an Empty value
    foundation::ComPtr<foundation::IObject> spObject;
    hr = foundation::QueryInterface(pValue, spObject.GetAddressOf());

    // check if cast to IObject works and type verification is IID_IObject
    if (SUCCEEDED(hr) && iidType == IID_IObject)
    {
        // asked type is of IObject
        return S_OK;
    }

    // we will check if is an 'IPropertyValue' with an Empty value
    foundation::PropertyValuePtr spAsPropertyValue;
    if (FAILED(hr) &&
        SUCCEEDED(foundation::pv_util::CastToPropertyValue(pValue, spAsPropertyValue.GetAddressOf())) &&
        foundation::pv_util::IsValueEmpty(spAsPropertyValue))
    {
        // we have a null pased value in a IPropertyValue
        *ppValue = nullptr;
        pValue->Release();
        return S_OK;
    }
    // fail if QI to IObject does not succeed
    _IFHR_(hr);

    IID iidObjectType;
    IFR_ASSERT(CTypeInfoStatics::GetTypeInternal(spObject, &iidObjectType));
    if (IsEqualGUID(iidType, iidObjectType))
    {
        // exact IID match no further check
        return S_OK;
    }

    bool result = false;
    this->IsTypeOfInternal(
        spObject,
        iidType,
        pPropertyTypeInfo,
        typeInfoType,
        result);

    if (!result)
    {
        // return Wrong Type
        return foundation::E_WRONG_TYPE;
    }
    return S_OK;
}

HRESULT CTypeInfoStatics::IsTypeOfInternal(
    _In_ foundation::IObject *pObject,
    const IID& iidType,
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
    const GUID& typeInfoType,
    bool& result)
{
    _IFR_(CTypeInfoStatics::IsTypeOfInternal(pObject, iidType, &result));
    return S_OK;
}

HRESULT CTypeInfoStatics::_VerifyPropertyTypeInfo(
    _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo,
    _Inout_ foundation::IInspectable **ppValue)
{
    return 
        static_cast<CTypeInfoStatics *>(GetInstance())->VerifyPropertyTypeInfoInternal(pPropertyTypeInfo, ppValue);
}

static _RegisterActivationFactory _typeInfoStaticsRegister(
    U("foundation.TypeInfoStatics"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CTypeInfoStatics::GetInstance)
    );
