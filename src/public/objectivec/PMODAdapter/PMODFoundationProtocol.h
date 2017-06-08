/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODFoundationProtocol.h
****/

#import "PMODAdapterProtocol.h"


@protocol PMODFoundation <PMODAdapter>

// Observers are _not_ retained, make sure to remove an observer before it is destroyed.
- (void)addObserver:(id<NSObject>)observer;
- (void)removeObserver:(id<NSObject>)observer;
- (BOOL)containsObserver:(id<NSObject>)observer;

- (void)notifyObservers:(NSInvocation*)invocation;

@end
