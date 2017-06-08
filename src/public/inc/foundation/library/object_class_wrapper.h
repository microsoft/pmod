/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_class_wrapper.h
****/
#pragma once

#include "class_wrapper.h"

namespace foundation {

    template <class T, class TClassInterface>
    class _ObjectClassWrapper : public _FoundationClassWrapper<T, TClassInterface>
    {
    public:
        HRESULT SetTypeInfo(_In_ foundation::IObjectTypeInfo *pObjectTypeInfo)
        {
            return this->GetClassInterfaceInternal()->SetTypeInfo(pObjectTypeInfo);
        }

    };

}

