/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODDictionary.h
****/

#import <Foundation/Foundation.h>

#import "PMODInterfaces.h"

@class PMODAdapterFactory;

__attribute__((visibility("default")))
@interface PMODDictionary : NSMutableDictionary {
    
}
-(PMODDictionary *) init;
-(PMODDictionary *) initWithAdapterFactory:(PMODAdapterFactory *) adapterFactory;
// underlying Native interface
-(IFoundationInspectable *) object;

@end
