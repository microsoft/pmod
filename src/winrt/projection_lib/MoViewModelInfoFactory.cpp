/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModelInfoFactory.cpp
****/

#include "pch.h"

#include "MoViewModelInfoFactory.h"
#include "MoObservableObjectInfoFactory.h"

#include <pmod/library/type_info_factory_util.h>
#include <pmod/errors.h>


STDMETHODIMP MoViewModelInfoFactory::CreateInstance(
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
        IObservableObjectInfo *sourceType,
        boolean isInherited,
        IViewModelInfo **ppInstance)
{
    IFCPTR(sourceType);

    pmod::IObservableObjectInfo *pSourceType = nullptr;
    IFR_ASSERT(foundation::QueryWeakReference(sourceType,&pSourceType));

    pmod::IObservableObjectInfo *pBaseType = nullptr;
    
    if(baseType)
    {
        IFR_ASSERT(foundation::QueryWeakReference(baseType,&pBaseType));
    }
    
    foundation::ComPtr<pmod::IViewModelInfo> spPropertyViewModelInfo;

    IFR_ASSERT(pmod::library::GetModelTypeInfoFactory()->CreateViewModelInfo
        (
            typeId,
            iidType,
            name,
            pBaseType,
            pSourceType,
            isInherited ? true:false,
            spPropertyViewModelInfo.GetAddressOf()
        ));

    foundation::ComPtr<pmod::library::IObservableObjectInfoClass> spPropertyViewModelInfoClass;
    IFR_ASSERT(foundation::QueryInterface(spPropertyViewModelInfo,spPropertyViewModelInfoClass.GetAddressOf()));

    IFR_ASSERT(MoObservableObjectInfoFactory::AddTypeInfo(
        spPropertyViewModelInfoClass,
        length_properties,
        propertyInfos,
        length_methods,
        methodInfos,
        length_events,
        eventInfos));

    return foundation::QueryInterface(spPropertyViewModelInfo,ppInstance);
}
