/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_dispatch_parameters.h
****/
#pragma once

#include <foundation/library/parameter_base.h>
#include <foundation/library/type_info_create_parameter.h>


namespace foundation {

    enum class ObjectDispatchOptions
    {
        None = 0x00,
	};

    namespace library {

        struct ObjectDispatchCreateParameters :
			public _TypeInfoCreateParameter
        {
			ObjectDispatchOptions m_options;
        };
    }
}

