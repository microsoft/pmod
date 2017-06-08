/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchInfo.cpp
****/

#include "pch.h"

#include "ObjectDispatchInfo.h"
#include "PropertyInfo.h"
#include "TypeInfoFactory.h"
#include "ObjectDispatchClassInfoCache.h"
#include "MethodInfo.h"

#include <foundation/library/dictionary_class.h>

using namespace foundation;

// {05B3F8E6-BA05-4AE2-BECD-D839A01B12E1}
const GUID foundation::library::IID_IMethodInfoClass =
{ 0x5b3f8e6, 0xba05, 0x4ae2,{ 0xbe, 0xcd, 0xd8, 0x39, 0xa0, 0x1b, 0x12, 0xe1 } };

#pragma warning(push)
#pragma warning(disable : 4512)

LPCSTR_t _PropertyDataTypeInfo_Properties_Key = U("properties");
LPCSTR_t _PropertyModelTypeInfo_Methods_Key = U("methods");

LPCSTR_t _Id_Key = U("id");
LPCSTR_t _Name_Key = U("name");
LPCSTR_t _PropertyInfo_Flags_Key = U("flags");

LPCSTR_t _MethodInfo_ResultType_Key = U("resultType");
LPCSTR_t _MethodInfo_IsAsync_Key = U("isAsync");
LPCSTR_t _MethodInfo_Parameters_Key = U("parameters");


HRESULT _DeserializePropertyTypeInfoHelper(
    const foundation::DictionaryPtr& type_info_ptr,
    LPCSTR_t key,
    foundation::PropertyType& propertyType,
    IID& typeInfoType)
{
    foundation::DictionaryPtr property_type_info_ptr;
    IFR_ASSERT(type_info_ptr.GetValuePtr(key, property_type_info_ptr));

    IFR_ASSERT(CTypeInfoFactory::_DeserializePropertyTypeInfo(
        property_type_info_ptr,
        propertyType,
        typeInfoType));

    return S_OK;
}

// support for Dictionary serialize/deserialize
HRESULT DeserializeTypeInfoInternal(
	_CObjectDispatchInfoBase *pPropertyDataInfoBase,
	const foundation::DictionaryPtr& type_info_ptr)
{
	// Properties
	foundation::ArrayRefCountWrapper<foundation::IDictionary> properties;
	IFR_ASSERT(type_info_ptr.GetValue(_PropertyDataTypeInfo_Properties_Key, properties));
	for (UINT32 index = 0; index < properties.GetSize(); ++index)
	{
		foundation::DictionaryPtr property_ptr = properties[index];
		UINT32 id;
		IFR_ASSERT(property_ptr.GetValue(_Id_Key, id));
		foundation::HStringPtr name;
		IFR_ASSERT(property_ptr.GetValue(_Name_Key, name));
		UINT32 flags;
		IFR_ASSERT(property_ptr.GetValue(_PropertyInfo_Flags_Key, flags));
		foundation::PropertyType propertType;
		IID typeInfoType;
		IFR_ASSERT(CTypeInfoFactory::_DeserializePropertyTypeInfo(property_ptr, propertType, typeInfoType));

		PropertyInfo propertyInfo(
			id,
			name.GetRawBuffer(),
			propertType,
			flags,
			nullptr
			);
		foundation::ComPtr<IPropertyInfo> property_info_ptr;
		IFR_ASSERT(CPropertyInfo::CreateInstance(
			&propertyInfo,
			!IsEqualIID(typeInfoType, foundation_GUID_NULL) ? &typeInfoType : nullptr,
			true,
			property_info_ptr.GetAddressOf()));
		pPropertyDataInfoBase->AddPropertyInfoInternal(property_info_ptr);
	}

    // Methods
    foundation::ArrayRefCountWrapper<foundation::IDictionary> methods;
    IFR_ASSERT(type_info_ptr.GetValue(_PropertyModelTypeInfo_Methods_Key, methods));
    for (UINT32 index = 0; index < methods.GetSize(); ++index)
    {
        foundation::DictionaryPtr method_ptr = methods[index];
        UINT32 id;
        IFR_ASSERT(method_ptr.GetValue(_Id_Key, id));
        foundation::HStringPtr name;
        IFR_ASSERT(method_ptr.GetValue(_Name_Key, name));
        bool isAsync;
        IFR_ASSERT(method_ptr.GetValue(_MethodInfo_IsAsync_Key, isAsync));

        foundation::PropertyType returnPropertyType;
        IID returnTypeInfoType;
        IFR_ASSERT(_DeserializePropertyTypeInfoHelper(
            method_ptr,
            _MethodInfo_ResultType_Key,
            returnPropertyType,
            returnTypeInfoType));

        foundation::PropertyTypeInfo resultTypeInfo(returnPropertyType, nullptr);
        MethodInfo methodInfo(
            id,
            name.GetRawBuffer(),
            nullptr,
            &resultTypeInfo,
            isAsync
        );

        foundation::ComPtr<foundation::IMethodInfo> method_info_ptr;
        IFR_ASSERT(CMethodInfo::CreateInstance(
            &methodInfo,
            !IsEqualIID(returnTypeInfoType, foundation_GUID_NULL) ? &returnTypeInfoType : nullptr,
            method_info_ptr.GetAddressOf()));

        pPropertyDataInfoBase->AddMethodInfoInternal(method_info_ptr);

        foundation::ComPtr<foundation::library::IMethodInfoClass> method_info_class_ptr;
        IFR_ASSERT(foundation::QueryInterface(method_info_ptr, method_info_class_ptr.GetAddressOf()));

        foundation::ArrayRefCountWrapper<foundation::IDictionary> parameters;
        IFR_ASSERT(method_ptr.GetValue(_MethodInfo_Parameters_Key, parameters));
        for (UINT32 indexParameter = 0; indexParameter < parameters.GetSize(); ++indexParameter)
        {
            foundation::DictionaryPtr parameter_info_ptr = parameters[indexParameter];
            foundation::HStringPtr parameter_name;
            IFR_ASSERT(parameter_info_ptr.GetValue(_Name_Key, parameter_name));

            foundation::PropertyType parameterPropertyType;
            IID parameterTypeInfoType;
            IFR_ASSERT(CTypeInfoFactory::_DeserializePropertyTypeInfo(
                parameter_info_ptr,
                parameterPropertyType,
                parameterTypeInfoType));

            method_info_class_ptr->AddParameterInfo(
                parameter_name,
                parameterPropertyType,
                !IsEqualIID(parameterTypeInfoType, foundation_GUID_NULL) ? &parameterTypeInfoType : nullptr);
        }
    }
	return S_OK;
}

HRESULT SerializeTypeInfoInternal(
	_CObjectDispatchInfoBase *pPropertyDataInfoBase,
	foundation::DictionaryPtr& type_info_ptr)
{
	// Properties
	std::vector<foundation::ComPtr<foundation::IDictionary>> properties;
	for (PropertyInfoVector::const_iterator iter = pPropertyDataInfoBase->GetProperties().begin();
	iter != pPropertyDataInfoBase->GetProperties().end();
		++iter
		)
	{
		foundation::DictionaryPtr property_ptr;
		IFR_ASSERT(foundation::library::CreateDictionaryClass(property_ptr.GetAddressOf()));

		UINT32 id;
		(*iter)->GetId(&id);
		foundation::HStringPtr name;
		(*iter)->GetName(name.GetAddressOf());
		UINT32 flags;
		(*iter)->GetFlags(&flags);

		property_ptr.SetValue(_Id_Key, id);
		property_ptr.SetValue(_Name_Key, name);
		property_ptr.SetValue(_PropertyInfo_Flags_Key, flags);

		CTypeInfoFactory::_SerializePropertyTypeInfo(property_ptr, *iter);

		properties.push_back(property_ptr);
	}
	type_info_ptr.SetValue(_PropertyDataTypeInfo_Properties_Key, foundation::CreateValueFromVectorPtr(properties));

    // Methods
    std::vector<foundation::ComPtr<foundation::IDictionary>> methods;
    for (MethodInfoVector::const_iterator iter = pPropertyDataInfoBase->GetMethods().begin();
        iter != pPropertyDataInfoBase->GetMethods().end();
        ++iter
        )
    {
        foundation::DictionaryPtr method_ptr;
        IFR_ASSERT(foundation::library::CreateDictionaryClass(method_ptr.GetAddressOf()));

        UINT32 id;
        (*iter)->GetId(&id);
        foundation::HStringPtr name;
        (*iter)->GetMethodName(name.GetAddressOf());

        foundation::ComPtr<foundation::IPropertyTypeInfo> result_type_info_ptr;
        (*iter)->GetResultTypeInfo(result_type_info_ptr.GetAddressOf());

        bool isAsync;
        (*iter)->GetIsAsync(&isAsync);

        method_ptr.SetValue(_Id_Key, id);
        method_ptr.SetValue(_Name_Key, name);
        CTypeInfoFactory::_SerializePropertyTypeInfo(method_ptr, _MethodInfo_ResultType_Key, result_type_info_ptr);
        method_ptr.SetValue(_MethodInfo_IsAsync_Key, isAsync);

        methods.push_back(method_ptr);

        foundation::ArrayRefCountWrapper<foundation::IParameterInfo> parameters_info;
        (*iter)->GetParameters(
            parameters_info.GetSizeAddressOf(),
            parameters_info.GetBufferAddressOf());
        std::vector<foundation::ComPtr<foundation::IDictionary>> parameters;
        for (UINT32 index = 0; index < parameters_info.GetSize(); ++index)
        {
            foundation::DictionaryPtr parameter_ptr;
            IFR_ASSERT(foundation::library::CreateDictionaryClass(parameter_ptr.GetAddressOf()));

            foundation::HStringPtr parameter_name;
            parameters_info[index]->GetParameterName(parameter_name.GetAddressOf());
            parameter_ptr.SetValue(_Name_Key, parameter_name);
            CTypeInfoFactory::_SerializePropertyTypeInfo(parameter_ptr, parameters_info[index]);

            parameters.push_back(parameter_ptr);
        }
        method_ptr.SetValue(_MethodInfo_Parameters_Key, foundation::CreateValueFromVectorPtr(parameters));

    }
    type_info_ptr.SetValue(_PropertyModelTypeInfo_Methods_Key, foundation::CreateValueFromVectorPtr(methods));


	return S_OK;
}

HRESULT CObjectDispatchInfoClass::CreateInstance(
	_In_ const ObjectDispatchTypeInfo *pTypeInfo,
	_Outptr_ CObjectDispatchInfoClass **ppPropertyDataInfoClass)
{
	foundation_assert(ppPropertyDataInfoClass);

	HRESULT hr;
	IFR_ASSERT(foundation::ctl::ComInspectableObject<CObjectDispatchInfoClass>::CreateInstance(ppPropertyDataInfoClass));

	// Now Cache this new created Collection Model Info
	CObjectDispatchClassInfoCache::AddToCache(pTypeInfo->get_iid_type(), *ppPropertyDataInfoClass);

	// now we can initialize
	IFR_ASSERT(hr = (*ppPropertyDataInfoClass)->_Initialize(pTypeInfo));
	return S_OK;
}

HRESULT CObjectDispatchInfoClass::_Initialize(
	const ObjectDispatchTypeInfo *pTypeInfo)
{
	IFR_ASSERT(_InitializeTypeInfo(pTypeInfo));

    if (pTypeInfo->m_properties)
    {
        IFR_ASSERT(this->AddPropertyInfoArray(pTypeInfo->m_properties));
    }
    if (pTypeInfo->m_methods)
    {
        IFR_ASSERT(this->AddMethodInfoArray(pTypeInfo->m_methods));
    }
	return S_OK;
}

HRESULT CObjectDispatchInfoClass::_DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr)
{
	_IFR_(_CObjectDispatchInfoClass_BaseType::_DeserializeTypeInfo(type_info_ptr));
	IFR_ASSERT(::DeserializeTypeInfoInternal(this, type_info_ptr));
	return S_OK;
}

HRESULT CObjectDispatchInfoClass::_SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr)
{
	_IFR_(_CObjectDispatchInfoClass_BaseType::_SerializeTypeInfo(type_info_ptr));
	IFR_ASSERT(::SerializeTypeInfoInternal(this, type_info_ptr));
	return S_OK;
}

