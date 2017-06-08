/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:thread.h
****/
#pragma once

#include <pal/pal_thread.h>

namespace pal {
    namespace library {

        template <class _Func>
        static void *STDAPICALLTYPE _ThreadFunc(void *p_context)
        {
            _Func *pFunc = reinterpret_cast<_Func *>(p_context);
            (*pFunc)();
            delete pFunc;
            return nullptr;
        }

        template <class _Func>
        void CreateThread(const _Func& func, _pal_ThreadHandle& thread_t)
        {
            _Func *pFunc = new _Func(func);
            _pal_CreateThread(&thread_t, &_ThreadFunc<_Func>, pFunc);
        }
    }
}
