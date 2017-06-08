/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_dispatch.h
****/
#pragma once

#include <foundation/foundation_macros.h>
#include "key_value_pair.h"
#include "object.h"

namespace foundation {

    BEGIN_DECLARE_IID(IPropertyIdPair, FOUNDATION_API);
    typedef foundation::IKeyValuePair<UINT32, foundation::IInspectable *, IID_IPropertyIdPair> IPropertyIdPair;

    BEGIN_DECLARE_INTERFACE(IObjectDispatch, IObject, FOUNDATION_API)
        // Property methods.
        STDMETHOD(GetProperty)(UINT32 propertyId, _COM_Outptr_result_maybenull_ foundation::IInspectable** value) = 0;
        STDMETHOD(SetProperty)(UINT32 propertyId, _In_opt_ foundation::IInspectable *value) = 0;
        STDMETHOD(GetProperties)(foundation::IInspectable *pProperties, _Out_ UINT32 *size, _Outptr_ IPropertyIdPair ***ppProperties) = 0;

        STDMETHOD(InvokeMethod)(UINT32 methodId, _In_ UINT32 size, _In_opt_ foundation::IInspectable **parameters, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppResult) = 0;

    END_DECLARE_INTERFACE()

}


