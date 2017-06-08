/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODObservableObjectProtocol.h
****/

#import "PMODObjectDispatchProtocol.h"

@interface PMODPropertyChangedEventArgs : NSObject

-(unsigned int) property;
-(NSString *) propertyName;
-(id) oldValue;
-(id) newValue;

@end

@interface PMODModelEventArgs : NSObject

-(unsigned int) event;
-(id) eventArgs;

@end

@protocol PMODPropertyChangedObserver
- (void)onPropertyChanged:(id)sender withEventArgs:(PMODPropertyChangedEventArgs *)eventArgs;
@end

@protocol PMODModelEventObserver
- (void)onModelEvent:(id)sender withEventArgs:(PMODModelEventArgs *)eventArgs;
@end

@protocol PMODObservableObject <PMODObjectDispatch>
- (IPmodObservableObject *) observableObject;
@end

