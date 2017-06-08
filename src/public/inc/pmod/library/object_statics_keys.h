/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_statics_keys.h
****/
#pragma once

#include <pal/pal_core.h>

namespace pmod { namespace library {

    const CHAR_t * const _KEY_OBJECT_TYPE = U("objectType");
    const CHAR_t * const _KEY_ID = U("id");
    const CHAR_t * const _KEY_TYPE_ID = U("iidType");
    const CHAR_t * const _KEY_IS_OBJECT = U("isObject");
    const CHAR_t * const _KEY_PROPERTIES = U("properties");
    const CHAR_t * const _KEY_ITEMS = U("items");
    const CHAR_t * const _KEY_ITEMS_COUNT = U("itemsCount");
    const CHAR_t * const _KEY_REFERENCE_MODELS = U("referenceModels");
    const CHAR_t * const _KEY_COMMAND_STATE = U("state");
    const CHAR_t * const _KEY_ASYNC_OPERATION_STATUS = U("status");
    const CHAR_t * const _KEY_ASYNC_OPERATION_ERROR = U("error");
    const CHAR_t * const _KEY_ASYNC_OPERATION_RESULT = U("result");
    const CHAR_t * const _KEY_TYPE_INFO = U("typeInfo");
    const CHAR_t * const _KEY_PROPERTY_TYPE = U("propertyType");
    const CHAR_t * const _KEY_PROPERTY_INFO_FLAGS = U("flags");
    const CHAR_t * const _KEY_NAME = U("name");
    const CHAR_t * const _KEY_TYPE_UNIQUED_ID = U("typeId");
}
}

