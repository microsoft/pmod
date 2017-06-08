/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODEnumValue.h
****/

#import "PMODInterfaces.h"
#import <Foundation/Foundation.h>

__attribute__((visibility("default")))
@interface PMODEnumValue : NSNumber
-(int) enumTypeId;
+(PMODEnumValue *) createNSEnumValue:(foundation::IEnumValue *) pEnumValue;
@end

