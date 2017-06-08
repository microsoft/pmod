/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:activation_factory.h
****/
#pragma once

#include <foundation/activation_factory.h>

namespace foundation {
    namespace library {

        typedef foundation::IUnknown *(*GetActivationFactoryInstanceFunc)();


        EXTERN_C FOUNDATION_API HRESULT STDAPICALLTYPE RegisterActivationFactory(
            LPCSTR_t activationId,
            GetActivationFactoryInstanceFunc func);

        class _RegisterActivationFactory
        {
        public:
            _RegisterActivationFactory(LPCSTR_t activationId, GetActivationFactoryInstanceFunc pGetActivationFactoryInstance)
            {
                RegisterActivationFactory(activationId, pGetActivationFactoryInstance);
            }
        };
    
    }

}

