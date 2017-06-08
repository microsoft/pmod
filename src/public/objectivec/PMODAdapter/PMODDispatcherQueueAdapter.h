/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODDispatcherQueueAdapter.h
****/


#import "PMODInspectable.h"

__attribute__((visibility("default")))
@interface PMODDispatcherQueueAdapter : NSObject
    +(PMODInspectable *) mainQueue;
@end
