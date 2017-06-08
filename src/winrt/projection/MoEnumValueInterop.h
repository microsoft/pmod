/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEnumValueInterop.h
****/

#pragma once
#include <foundation\interfaces.h>

HRESULT ObjectToEnumValue(IInspectable *pObject, foundation::IEnumValue **ppEnumValue);
