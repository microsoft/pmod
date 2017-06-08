/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableObjectInfoFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;
//------------------------------------------------------------------------------
// Class:   MoMethodInfoFactory
//
// Purpose: Factory class for a m_parameterName object
//
//------------------------------------------------------------------------------
class MoMethodInfoFactory :
    public MoModelFactoryBase<IMethodInfoFactory>
{
public:
    HRESULT STDMETHODCALLTYPE CreateInstance( 
        unsigned int methodId,
        HSTRING name,
        ABI::Windows::Foundation::PropertyType resultType,
        GUID resultModelType,
        boolean isAsync,
        UINT32 length,
        ParameterInfo *parametersInfo,
        IMethodInfo **instance) override;

};

//------------------------------------------------------------------------------
// Class:   MoObservableObjectInfoFactory
//
// Purpose: Factory class for a PropertyModel
//
//------------------------------------------------------------------------------
class MoObservableObjectInfoFactory :
        public MoModelFactoryStaticsBase
        <
            IObservableObjectInfoFactory,
            IObservableObjectInfoStatics
        >
{
public:
    // Interface IPropertyModelInfoFactory
    IFACEMETHOD(CreateInstance)( 
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
        IObservableObjectInfo **instance) override;

    // Interface IPropertyModelInfoStatics
    HRESULT STDMETHODCALLTYPE GetPropertyModelInfo( 
                            GUID iidType,
                            IObservableObjectInfo **ppPropertyModelInfo);

    IFACEMETHOD(RegisterType)(IObservableObjectInfo *pPropertyModelInfo) override;

    static HRESULT AddTypeInfo(
        pmod::library::IObservableObjectInfoClass *pPropertyModelInfoClass,
        UINT32 length_properties,
        PropertyInfo *propertyInfos,
        UINT32 length_methods,
        IMethodInfo **methodInfos,
        UINT32 length_events,
        EventInfo *eventInfos);

};
