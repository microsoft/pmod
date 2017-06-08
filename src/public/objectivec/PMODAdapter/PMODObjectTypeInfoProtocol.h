/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODObjectTypeInfoProtocol.h
****/

#import "PMODGuid.h"

@protocol PMODObjectTypeInfo

-(PMODGuid *) iidType;
-(NSString *) name;

@end

