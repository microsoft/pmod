/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODObjectBaseProtocol.h
****/

#import "PMODFoundationProtocol.h"
#import "PMODInterfaces.h"
#import "PMODObjectTypeInfoProtocol.h"

@protocol PMODObjectBase <PMODFoundation>

-(uint32_t) uniqueId;
-(uint32_t) processId;

-(id<PMODObjectTypeInfo>) objectTypeInfo;

// underlying Native interface
-(IFoundationObject *) object;

@end

