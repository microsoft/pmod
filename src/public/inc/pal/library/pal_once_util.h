/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pal_once_util.h
****/
#pragma once

#include <pal/pal_init_once.h>

namespace pal {
    namespace library {

#ifdef _USE_C11_ONCE

#include <mutex>
#define PalOnceFlag   std::once_flag
#define pal_CallOnce   std::call_once

#else
        struct PalOnceFlag
        {
            PalOnceFlag()
            {
                _pal_InitializeInitOnce(&_init_once);
            }
            ~PalOnceFlag()
            {
                _pal_DeleteInitOnce(&_init_once);
            }

            _PAL_INIT_ONCE _init_once;
        };

        struct _CallOnceInit
        {
            virtual bool operator()() const = 0;
        };

        __unused_attribute__
        static void pal_CallOnce(PalOnceFlag& onceflag, const _CallOnceInit& func_init)
        {
            class _CallOnceClass
            {
            public:
                _CallOnceClass(const _CallOnceInit& func_init) :
                    _func_init(func_init)
                {
                }

                bool call_once_invoke()
                {
                    return _func_init();
                }

                static int STDMETHODCALLTYPE pal_CallOnce(
                    _PAL_INIT_ONCE *InitOnce,
                    PVOID Parameter,
                    PVOID *lpContext)
                {
                    _CallOnceClass *pT = (_CallOnceClass *)Parameter;
                    return pT->call_once_invoke() ? TRUE : FALSE;
                }
            private:
                _CallOnceClass(const _CallOnceClass& other) :
                    _func_init(other._func_init)
                {
                }
                _CallOnceClass & operator=( const _CallOnceClass &other ) { return *this; }
            private:
                const _CallOnceInit& _func_init;
            };
            _CallOnceClass callback(func_init);

            _pal_InitOnce(
                &onceflag._init_once,
                _CallOnceClass::pal_CallOnce,
                (PVOID *)&callback,
                nullptr);
        }
#endif

        template<class _Func>
        static HRESULT pal_CallOnce_t(PalOnceFlag& onceflag, const _Func& func_init)
        {
            HRESULT hr = S_OK;

            class CallOnceInit : public _CallOnceInit
            {
            public:
                CallOnceInit(const _Func& func_init, HRESULT& hr) :
                    _func_init(func_init),
                    _hr(hr)
                {
                }

                bool operator()() const
                {
                    _hr = _func_init();
                    return SUCCEEDED(_hr);
                }
            private:
                CallOnceInit(const CallOnceInit& other) :
                    _func_init(other._func_init),
                    _hr(other._hr)
                {
                }
                CallOnceInit & operator=( const CallOnceInit &other ) { return *this; }
            private:
                HRESULT&        _hr;
                const _Func&    _func_init;
            };
            pal_CallOnce(onceflag, CallOnceInit(func_init, hr));
            return hr;
        }
    }
}
