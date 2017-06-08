/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_info_create_parameter.h
****/

#pragma once

#include <foundation/com_interfaces.h>
#include "parameter_base.h"

namespace foundation {

    namespace library {

        struct _TypeInfoCreateParameter
        {
            _TypeInfoCreateParameter() :
            m_iid_type(foundation_GUID_NULL)
            {
            }

            IID     m_iid_type;        // the TypeInstance
        };

        template <class TObjectDelegate, class TOptions>
        struct _TypeInfoCreateParameterBase :
            public _TypeInfoCreateParameter,
            public _CreateParameterBase<TObjectDelegate, TOptions>
        {
        };
    }
}

