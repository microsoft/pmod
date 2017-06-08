/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterProtocol.h
****/

@class PMODAdapterFactory;

#import <Foundation/Foundation.h>

@protocol PMODAdapter<NSObject>

// its Adapter Factory property
- (PMODAdapterFactory *) adapterFactory;

@end
