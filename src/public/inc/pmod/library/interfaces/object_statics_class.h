/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_statics_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <foundation/interfaces/object.h>
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // Default Serializer
        BEGIN_DECLARE_INTERFACE(IDefaultObjectSerializerCallback, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(SerializeObject)(_In_ foundation::IObject *pObject, _In_ bool serializeTypeInfo, _COM_Outptr_ foundation::IDictionary **ppCoreDictionary) = 0;
        END_DECLARE_INTERFACE()
    }
}

