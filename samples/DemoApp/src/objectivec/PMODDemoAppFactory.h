/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODDemoAppFactory.h
****/

#import <Foundation/Foundation.h>

@class PMODAdapterFactory;
__attribute__((visibility("default")))
@interface PMODDemoAppFactoryImpl : NSObject

// create a DemoApp Adapter Factory
+(PMODAdapterFactory *)createDemoAppAdapterFactory;

@end
