/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:palApi.cpp
****/
#include "foundation/pal.h"
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

LPVOID _pal_MemAlloc(ULONG size)
{
	return malloc(size);
}

void _pal_MemFree(LPVOID pMem)
{
	free(pMem);
}


