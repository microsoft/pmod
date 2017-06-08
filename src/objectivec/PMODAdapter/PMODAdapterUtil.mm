/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterUtil.mm
****/

#import "PMODAdapterUtil.h"

@implementation PMODAdapterUtil

+(void) throwIfNull:(void *) ptr
{
    if(ptr == nullptr)
    {
        NSException *e = [NSException
                          exceptionWithName:NSObjectNotAvailableException
                          reason:@"ptr == nullptr"
                          userInfo:nil];
        
        @throw e;
    }
}
@end

