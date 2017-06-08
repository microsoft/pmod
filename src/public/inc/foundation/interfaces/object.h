/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>

namespace foundation {

    ////////////////////// Type Info support ////////////////////////////    
    BEGIN_DECLARE_INTERFACE(IObjectTypeInfo, IInspectable, FOUNDATION_API)
        // Property methods.
        STDMETHOD(GetTypeId)(_Out_ UINT32* typeId) = 0;
        STDMETHOD(GetName)(_Out_ HSTRING* typeName) = 0;
        STDMETHOD(GetType)(_Out_ IID* iidType) = 0;
        STDMETHOD(GetBaseType)(_COM_Outptr_result_maybenull_ IObjectTypeInfo **ppModelTypeInfo) = 0;
        STDMETHOD(GetIsRuntimeCreated)(_Out_ bool *pValue) = 0;
    END_DECLARE_INTERFACE()

    ////////////////////// IObject support ////////////////////////////    
    BEGIN_DECLARE_INTERFACE(IObject, IInspectable, FOUNDATION_API)
        // Property methods.
        STDMETHOD(GetTypeInfo)(_COM_Outptr_ IObjectTypeInfo** ppModelTypeInfo) = 0;
        STDMETHOD(GetUniqueId)(_Out_ UINT32 *uniqueId) = 0;
        STDMETHOD(GetProcessId)(_Out_ UINT32 *processId) = 0;
    END_DECLARE_INTERFACE()

}

