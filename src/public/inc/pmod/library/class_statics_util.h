/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:class_statics_util.h
****/
#pragma once

#include "interfaces/class_statics.h"


namespace pmod
{
    namespace library
    {
        inline IClassStatics *GetClassStatics()
        {
            return GetModelLibraryFactory<IClassStatics>(U("pmod.ClassStatics"));
        }

    };

}
