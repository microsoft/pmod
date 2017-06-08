/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODDispatcherQueueAdapter.mm
****/

#import "PMODDispatcherQueueAdapter.h"
#import "dispatch_queue_adapter.h"
#import <foundation/com_ptr.h>

using namespace foundation;
using namespace foundation::library;

@implementation PMODDispatcherQueueAdapter

+(PMODInspectable *) mainQueue
{
    ComPtr<foundation::library::IDispatcher> dispatcherPtr;
    ::create_dispatch_main_queue_adapter(dispatcherPtr.GetAddressOf());
    return [PMODInspectable withInspectable:reinterpret_cast<IInspectable *>(dispatcherPtr.Get())];
}

@end
