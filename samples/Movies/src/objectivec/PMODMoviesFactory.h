/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODMoviesFactory.h
****/

#import <Foundation/Foundation.h>

@protocol IMoviesApiService;
@interface PMODMoviesFactoryImpl : NSObject

// create a DemoApp Adapter Factory
+(id<IMoviesApiService>) getMoviesApiService;

@end
