/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterUtil.h
****/
#import <Foundation/Foundation.h>

@interface PMODAdapterUtil : NSObject 
+(void) throwIfNull:(void *) ptr;
@end

#define RELEASE(x) \
    [x release];\
    x = nil;
