/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_object_statics.h
****/
#pragma once

#include <pmod/interfaces/observable_object.h>
#include "../pmod_lib_api.h"

namespace pmod {

    namespace library {
        /////////////////////////////////// Property Statics //////////////////////////////////////////////
        BEGIN_DECLARE_INTERFACE(IObservableObjectStatics, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(GetSource)(
                _In_ foundation::IUnknown *pUnk,
                _COM_Outptr_result_maybenull_ foundation::IObject **ppSource) = 0;
        STDMETHOD(GetPropertyValue)(
            _In_ IObservableObject *pObservableObject,
            _In_ foundation::IInspectable *path,
            _COM_Outptr_result_maybenull_ foundation::IInspectable **ppValue) = 0;
        STDMETHOD(SetPropertyValue)(
            _In_ IObservableObject *pObservableObject,
            _In_ foundation::IInspectable *path,
            _In_opt_ foundation::IInspectable *pValue) = 0;
        STDMETHOD(GetSourceOfType)(
            _In_ foundation::IUnknown *pUnk,
            REFIID iidInstanceType,
            foundation::InstanceTypeOptions options,
            bool isModelClassType,
            _COM_Outptr_result_maybenull_ foundation::IObject **ppSource) = 0;
        END_DECLARE_INTERFACE()
    }

}


