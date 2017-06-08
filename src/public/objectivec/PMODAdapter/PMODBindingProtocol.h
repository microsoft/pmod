/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODBindingProtocol.h
****/


#import "PMODFoundationProtocol.h"

@interface NSBindingValueChangedEventArgs : NSObject

@property (readonly) id bindingValue;

@end

@protocol NSBindingValueChangedObserver
- (void)onBindingValueChanged:(id)sender withEventArgs:(NSBindingValueChangedEventArgs *)eventArgs;
@end

@protocol NSBinding <PMODFoundation>
@property (readonly) id bindingValue;
@end
