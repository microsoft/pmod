/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:parameter_base.h
****/
#pragma once

namespace foundation
{
    namespace library {

        template <class TObjectDelegate, class TOptions>
        struct _CreateParameterBase
        {
            _CreateParameterBase() :
            m_p_delegate(nullptr),
            m_options((TOptions)0)
            {
            }

            TObjectDelegate         *m_p_delegate;    // callback Delegate
            TOptions                m_options;   // Core Options
        };
    }
}

