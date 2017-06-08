/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Movies.cpp
****/

#include "MoviesTypeInfo.g.h"
#include "MoviesInterfaces.g.h"

extern "C" void RegisterMoviesIf()
{
    static bool bRegistered = false;

    if(!bRegistered)
    {
        bRegistered = true;
        Movies::RegisterTypeInfos();
        Movies::RegisterModelTypeAdapters();
    }
}
