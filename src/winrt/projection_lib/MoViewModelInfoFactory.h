/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoViewModelInfoFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.Library.h"
using namespace ABI::Microsoft::PropertyModel;
using namespace ABI::Microsoft::PropertyModel::Library;


//------------------------------------------------------------------------------
// Class:   MoViewModelInfoFactory
//
// Purpose: Factory class for a PropertyViewModelInfo
//
//------------------------------------------------------------------------------
class MoViewModelInfoFactory :
        public  MoModelFactoryBase<IViewModelInfoFactory>
{
public:
    // Interface IPropertyViewModelInfoFactory                        
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
        IObservableObjectInfo *sourceType,
        boolean isInherited,
        IViewModelInfo **instance) override;
};
