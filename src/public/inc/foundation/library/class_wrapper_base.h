/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:class_wrapper_base.h
****/
#pragma once

#include <foundation/assert.h>
#include <foundation/com_interfaces.h>


namespace foundation {

    template <class T, class TClassInterface>
    class _ClassWrapperBase
    {
    public:
        typedef TClassInterface _ClassInterfaceType;
    protected:
        inline TClassInterface *GetClassInterfaceInternal() const
        {            
            T *pThis = const_cast<T *>(static_cast<const T * >(this));

            TClassInterface *pClassInterface = pThis->GetClassInterface();
            foundation_assert(pClassInterface != nullptr);
            return pClassInterface;
        }

        template <class TInterface>
        inline TInterface *GetClassInterfaceInternalAs() const
        {
            TInterface *pInterface = nullptr;
            foundation::QueryWeakReference(this->GetClassInterfaceInternal(), &pInterface);
            foundation_assert(pInterface != nullptr);
            return pInterface;
        }
    };
}

