/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODFoundation.h
****/


#import "PMODFoundationProtocol.h"
#import "PMODAdapter.h"

@interface PMODFoundation : PMODAdapter<PMODFoundation>

// attach/detach from underlying foundation instance
-(void) attachToFoundation;
-(void) detachFromFoundation;

-(id) toUniqueId;

-(BOOL) hasObservers;

+ (NSInvocation *)invocationWithProtocol:(Protocol*)targetProtocol selector:(SEL)selector;

@end
