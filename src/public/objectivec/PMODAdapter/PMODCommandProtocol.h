/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODCommandProtocol.h
****/


#import "PMODObjectNodeProtocol.h"

@interface PMODCanExecuteChangedEventArgs : NSObject

@property (readonly) int oldState;
@property (readonly) int newState;

@end

@protocol PMODCanExecuteChangedObserver
- (void)onCanExecuteChanged:(id)sender withEventArgs:(PMODCanExecuteChangedEventArgs *)eventArgs;
@end

@protocol PMODCommand <PMODObjectNode>
- (BOOL) canExecute:(id)parameter;
- (id) execute:(id)parameter;
- (int) state;
@end
