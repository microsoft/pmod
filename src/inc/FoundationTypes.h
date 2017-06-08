/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationTypes.h
****/
#pragma once

#include <foundation/com_ptr.h>
#include <foundation/library/interfaces/dispatcher.h>

#include <vector>
typedef std::vector<foundation::ComPtr<foundation::library::IDelegateDispatchable>> _Dispatchable_ptr_vector_type;
typedef std::vector<foundation::InspectablePtr>    _Inspectable_ptr_vector_type;
