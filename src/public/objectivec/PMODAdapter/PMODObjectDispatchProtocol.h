/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODObjectDispatchProtocol.h
****/

#import "PMODObjectNodeProtocol.h"

@protocol PMODObjectDispatch <PMODObjectNode>
- (NSArray *)allProperties;
- (id) valueForProperty:(unsigned int)propertyId;
- (void) setValueForProperty:(unsigned int)propertyId value:(id) value;
- (id) invoke:(unsigned int)methodId parameters:(NSArray *)parameters;
- (IFoundationObjectDispatch *) objectDispatch;
@end

