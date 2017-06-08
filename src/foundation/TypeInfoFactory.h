/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeInfoFactory.h
****/
#pragma once

#include <map>

#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>
#include <foundation/library/foundation_library.h>
#include <foundation/dictionary_ptr.h>
#include <foundation/interfaces/enumerable.h>

class CPropertyTypeInfoFactoryCache
{
public:
    static CPropertyTypeInfoFactoryCache& GetInstance();
    static HRESULT CreatePropertyTypeInfoInternal(
         _In_ const foundation::PropertyTypeInfo *pTypeInfo,
        _COM_Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfo);

public:
    HRESULT CreateOrGetFactoryPropertyTypeInfo(
        foundation::PropertyType type,
        _COM_Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfo);

    HRESULT CreateOrGetFactoryPropertyTypeInfo(
        _In_ const foundation::PropertyTypeInfo *pTypeInfo,
        _COM_Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfo);
private:
    typedef std::map<foundation::PropertyType, foundation::ComPtr<foundation::IPropertyTypeInfo>> _FoundationPropertyTypeInfo_MapType;

    _FoundationPropertyTypeInfo_MapType _foundationPropertyTypeInfos;
};

class CTypeInfoFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::ITypeInfoFactory
{
 public:
     static foundation::library::ITypeInfoFactory *GetInstance();
     static HRESULT GetOrCreateInstance(foundation::library::ITypeInfoFactory **ppTypeInfoFactory);

     static HRESULT _DeserializePropertyTypeInfo(
         const foundation::DictionaryPtr& type_info_ptr,
         LPCSTR_t propertyTypeInfoKey,
         _COM_Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfo);

     static HRESULT _SerializePropertyTypeInfo(
         foundation::DictionaryPtr& type_info_ptr,
         LPCSTR_t propertyTypeInfoKey,
         _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo);

     static HRESULT _DeserializePropertyTypeInfo(
         const foundation::DictionaryPtr& property_type_info_ptr,
         foundation::PropertyType& propertyType,
         IID& typeInfoType);

     static HRESULT _SerializePropertyTypeInfo(
         foundation::DictionaryPtr& property_type_info_ptr,
         _In_ foundation::IPropertyTypeInfo *pPropertyTypeInfo);

     //Interface ITypeInfoFactory
     STDMETHOD(CreatePropertyTypeInfo)(
         foundation::PropertyType type,
         GUID modelType,
         _COM_Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfo);
     STDMETHOD(CreatePropertyInfo)(
         UINT32 Id,
         _In_ HSTRING name,
         foundation::PropertyType type,
         UINT32 flags,
         GUID modelType,
         _COM_Outptr_ foundation::IPropertyInfo **ppPropertyInfo);

     STDMETHOD(CreateMethodInfo)(
         _In_ UINT32 methodId,
         _In_ HSTRING name,
         _In_ foundation::PropertyType returnPropertyType,
         _In_ GUID *pReturnModelType,
         _In_ bool isAsync,
         _Outptr_ foundation::library::IMethodInfoClass **ppMethodInfoClass);

     STDMETHOD(CreateEnumTypeInfo)(
         UINT32 typeId,
         GUID iidType,
         _In_ HSTRING name,
         bool isFlags,
         UINT32 size,
         _In_ HSTRING *pName,
         _In_ UINT32 *pValues,
         _COM_Outptr_ foundation::IEnumTypeInfo **ppEnumTypeInfo);
     STDMETHOD(CreateAsyncOperationTypeInfo)(
         UINT32 typeId,
         GUID iidType,
         _In_ HSTRING name,
         _In_ foundation::IPropertyTypeInfo *pResultTypeInfo,
         _COM_Outptr_ foundation::IAsyncOperationTypeInfo **ppAsyncOperationTypeInfo);
     STDMETHOD(CreateEnumerableTypeInfo)(
         _In_ UINT32 typeId,
         _In_ GUID iidType,
         _In_ HSTRING name,
         _In_ foundation::IPropertyTypeInfo *pItemTypeInfo,
         _In_ UINT32  type,
         _Outptr_ foundation::IEnumerableTypeInfo **ppEnumerableTypeInfo);


     STDMETHOD(RegisterEnumTypeInfo)(_In_ foundation::IEnumTypeInfo *pEnumTypeInfo);
     STDMETHOD(RegisterAsyncOperationTypeInfo)(_In_ foundation::IAsyncOperationTypeInfo *pAsyncOperationTypeInfo);
     STDMETHOD(RegisterEnumerableTypeInfo)(_In_ foundation::IEnumerableTypeInfo *pEnumerableTypeInfo);

     STDMETHOD(RegisterTypeInfo)(
         _In_ const foundation::TypeInfo* pTypeInfoEntry);

     STDMETHOD(RegisterMultipleTypeInfos)(
         _In_ const foundation::TypeInfo* pTypeInfoEntries[]);

     STDMETHOD(RegisterMultipleTypeInfos)(
         UINT32 size,
         foundation::IDictionary **ppDictionaryTypeInfo);

     STDMETHOD(SerializeTypeInfo)(
         foundation::IObjectTypeInfo *pTypeInfo,
         foundation::IDictionary **ppDictionaryTypeInfo);
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::ITypeInfoFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::ITypeInfoFactory *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

private:
    static HRESULT CreatePropertyTypeInfoInternal(
        foundation::PropertyType type,
        GUID modelType,
        _COM_Outptr_ foundation::IPropertyTypeInfo **ppPropertyTypeInfo);

};

