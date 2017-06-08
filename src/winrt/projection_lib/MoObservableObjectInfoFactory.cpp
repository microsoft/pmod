/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectInfoFactory.cpp
****/

#include "pch.h"

#include "MoObservableObjectInfoFactory.h"

#include <pmod/library/type_info_factory_util.h>
#include <pmod/errors.h>

#include <vector>

STDMETHODIMP MoMethodInfoFactory::CreateInstance( 
        unsigned int methodId,
        HSTRING name,
        ABI::Windows::Foundation::PropertyType resultType,
        GUID resultModelType,
        boolean isAsync,
        UINT32 length,
        ParameterInfo *parametersInfo,
        IMethodInfo **instance)
{
    foundation::ComPtr<foundation::library::IMethodInfoClass> spMethodInfoClass;

    IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreateMethodInfo
        (
            methodId,
            name,
            resultType,
            IsEqualGUID(resultModelType,IID_NULL) ? nullptr:&resultModelType,
            isAsync ? true:false,
            spMethodInfoClass.GetAddressOf()
        ));

    for(UINT32 i=0;i < length; ++i)
    {
        IFR_ASSERT(spMethodInfoClass->AddParameterInfo(
            parametersInfo[i].Name,
            parametersInfo[i].PropertyType,
            IsEqualGUID(parametersInfo[i].ObjectType, IID_NULL) ? nullptr : &parametersInfo[i].ObjectType));
    }

    return foundation::QueryInterface(spMethodInfoClass,instance);
}

STDMETHODIMP MoObservableObjectInfoFactory::GetPropertyModelInfo( 
                            GUID iidType,
                            IObservableObjectInfo **ppPropertyModelInfo)
{
    foundation::ComPtr<foundation::IObjectTypeInfo> spObjectTypeInfo;
    _IFR_(foundation::GetTypeInfo(iidType, spObjectTypeInfo.GetAddressOf()));
    return foundation::QueryInterface(spObjectTypeInfo, ppPropertyModelInfo);
}

HRESULT MoObservableObjectInfoFactory::AddTypeInfo(
        pmod::library::IObservableObjectInfoClass *pPropertyModelInfoClass,
        UINT32 length_properties,
        PropertyInfo *propertyInfos,
        UINT32 length_methods,
        IMethodInfo **methodInfos,
        UINT32 length_events,
        EventInfo *eventInfos)
{
    UNREFERENCED_PARAMETER(length_events);
    UNREFERENCED_PARAMETER(eventInfos);

    // Add Properties
    for(UINT32 i=0;i < length_properties; ++i)
    {
        foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
        IFR_ASSERT(foundation::library::GetTypeInfoFactory()->CreatePropertyInfo
        (
            propertyInfos->Id,
            propertyInfos->Name,
            propertyInfos->PropertyType,
            propertyInfos->Flags,
            propertyInfos->ObjectType,
            spPropertyInfo.GetAddressOf()
        ));
        IFR_ASSERT(pPropertyModelInfoClass->AddProperty(spPropertyInfo));
        // Next Struct
        ++propertyInfos;
    }
    // Add Methods
    for(UINT32 i=0;i < length_methods; ++i)
    {
        foundation::ComPtr<foundation::IMethodInfo> spMethodInfo;
        IFR_ASSERT(foundation::QueryInterface(methodInfos[i],spMethodInfo.GetAddressOf()));
        IFR_ASSERT(pPropertyModelInfoClass->AddMethod(spMethodInfo));
    }
    return S_OK;
}

STDMETHODIMP MoObservableObjectInfoFactory::CreateInstance(
        int typeId,
        GUID iidType,
        HSTRING name,
        IObservableObjectInfo *baseType,
        UINT32 length_properties,
        PropertyInfo *propertyInfos,
        UINT32 length_methods,
        IMethodInfo **methodInfos,
        UINT32 length_events,
        EventInfo *eventInfos,
        IObservableObjectInfo **ppInstance)
{

    pmod::IObservableObjectInfo *pBaseType = nullptr;
    
    if(baseType)
    {
        IFR(foundation::QueryWeakReference(baseType,&pBaseType));
    }
    
    foundation::ComPtr<pmod::IObservableObjectInfo> spPropertyModelInfo;
    IFR(pmod::library::GetModelTypeInfoFactory()->CreateObservableObjectInfo
        (
            typeId,
            iidType,
            name,
            pBaseType,
            spPropertyModelInfo.GetAddressOf()
        ));

    foundation::ComPtr<pmod::library::IObservableObjectInfoClass> spPropertyModelInfoClass;
    IFR(foundation::QueryInterface(spPropertyModelInfo,spPropertyModelInfoClass.GetAddressOf()));

    IFR_ASSERT(AddTypeInfo(
        spPropertyModelInfoClass,
        length_properties,
        propertyInfos,
        length_methods,
        methodInfos,
        length_events,
        eventInfos));

    return foundation::QueryInterface(spPropertyModelInfo,ppInstance);
}

STDMETHODIMP MoObservableObjectInfoFactory::RegisterType(IObservableObjectInfo *pPropertyModelInfo)
{
    IFCPTR(pPropertyModelInfo);

    pmod::IObservableObjectInfo *pModelTypeInfo = nullptr;
    IFR_ASSERT(foundation::QueryInterface(pPropertyModelInfo, &pModelTypeInfo));

    IFR_ASSERT(pmod::library::GetModelTypeInfoFactory()->RegisterObjectTypeInfo(pModelTypeInfo));
    return S_OK;
}
