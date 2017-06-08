/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_object_info_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <pmod/interfaces/observable_object.h>

namespace pmod
{
    namespace library
    {
        
        // IObservableObjectInfoClass Interface
        BEGIN_DECLARE_INTERFACE(IObservableObjectInfoClass, foundation::IUnknown, PMOD_API)
            STDMETHOD(AddProperty)(_In_ foundation::IPropertyInfo *pPropertyInfo) = 0;
            STDMETHOD(AddMethod)(_In_ foundation::IMethodInfo *pMethodInfo) = 0;
            STDMETHOD(AddEvent)(_In_ IEventInfo *pEventInfo) = 0;
        END_DECLARE_INTERFACE()

    }
}

