/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODInspectable.h
****/

#import <Foundation/Foundation.h>

#import "PMODInterfaces.h"

__attribute__((visibility("default")))
@interface PMODInspectable : NSObject {
    
}
-(PMODInspectable *) initWithInspectable:(IFoundationInspectable *) pInspectable;
// underlying Native interface
-(IFoundationInspectable *) object;

+ (PMODInspectable *)withInspectable:(IFoundationInspectable *) pInspectable;

@end
