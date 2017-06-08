/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GuidHelper.h
****/
#pragma once

#include <string>

#include "foundation/pal.h"

// convert a IID type to a string GUID representation using xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
std::string ToString(GUID iidType);

// convert a utf8 buffer into an IID
bool FromString(const char *pIIDStr, IID& iid);

// swap utils for 16/32 bits values
void SwapUInt16(UINT8 *p);
void SwapUInt32(UINT8 *p);


