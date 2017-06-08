/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:dispatch_queue_adapter.h
****/
#pragma once
#include <foundation/library/interfaces/dispatcher.h>
#include <dispatch/dispatch.h>


namespace foundation { namespace library {
    EXTERN_C FOUNDATION_API HRESULT create_dispatch_queue_adapter(
                                       dispatch_queue_t queue,
                                          IDispatcher **instance);
    EXTERN_C FOUNDATION_API HRESULT create_dispatch_main_queue_adapter(
                                          IDispatcher **instance);
    EXTERN_C FOUNDATION_API HRESULT create_dispatch_global_queue_adapter(dispatch_queue_priority_t priority,IDispatcher **instance);
    }
}


