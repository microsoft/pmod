/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapter.h
****/


#import "PMODAdapterProtocol.h"
#import "PMODInterfaces.h"

__attribute__((visibility("default")))
@interface PMODAdapter : NSObject<PMODAdapter>

-(void) setAdapterFactory:(PMODAdapterFactory *)adapterFactory;

+(IFoundationInspectable *) fromNSObject: (id)value;
-(NSObject *) toNSObject:(IFoundationInspectable *)value;
+(NSObject *) toNSObject:(IFoundationInspectable *)value withAdapterFactory:(PMODAdapterFactory *) adapterFactory;

// dispose pattern
-(void) dispose;

@end
