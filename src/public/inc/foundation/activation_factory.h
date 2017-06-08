/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:activation_factory.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <pal/library/pal_once_util.h>

#ifdef DEBUG
#include <pal/library/trace_util.h>
#endif

namespace foundation {

    EXTERN_C FOUNDATION_API IUnknown *STDAPICALLTYPE GetActivationFactoryInstance(LPCSTR_t activationId);

    inline bool IsActivationFactoryAvailable(LPCSTR_t activationId)
    {
        return GetActivationFactoryInstance(activationId) != nullptr;
    }

    template <class T>
    class _ActivationFactory
    {
    public:
        _ActivationFactory() :
            _pActivationFactory(nullptr)
        {}

        template <class _FuncCast>
        T *_GetActivationFactory(LPCSTR_t activationId,const _FuncCast& func)
        {
            pal::library::pal_CallOnce_t(
                this->_once_flag,
                [=]()
            {
                foundation::IUnknown *pFactory = GetActivationFactoryInstance(activationId);
#ifdef DEBUG
                if (pFactory == nullptr)
                {
                    pal::library::TraceMessage(U("GetActivationFactoryInstance return nullptr activationId:%s\n"), activationId);
                }
#endif
                foundation_assert(pFactory != nullptr);
                this->_pActivationFactory = func(pFactory);
                return S_OK;
            });

            return this->_pActivationFactory;
        }
    private:
        pal::library::PalOnceFlag    _once_flag;
        T *_pActivationFactory;
    };

    template <class T>
    static T *GetActivationFactory(LPCSTR_t activationId)
    {
        static _ActivationFactory<T> _ActivationFactory;

        return _ActivationFactory._GetActivationFactory(activationId, [](IUnknown *pFactoryUnk) {
            return reinterpret_cast<T *>(pFactoryUnk);
        });
    }

    template <class T>
    static T *GetActivationFactoryWithCast(LPCSTR_t activationId)
    {
        static _ActivationFactory<T> _ActivationFactory;

        return _ActivationFactory._GetActivationFactory(activationId, [](IUnknown *pFactoryUnk) {
            return CastTo<T>(pFactoryUnk);
        });
    }
}

