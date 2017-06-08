/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAsyncOperationProtocol.h
****/

#import "PMODObjectBaseProtocol.h"

@interface PMODCompletedEventArgs : NSObject

@end

@protocol PMODCompletedObserver
- (void)onCompleted:(id)sender withEventArgs:(PMODCompletedEventArgs *)eventArgs;
@end

typedef enum
{
    PMODResultStatus_Started = 0,
    PMODResultStatus_Completed = 1,
    PMODResultStatus_Canceled = 2,
    PMODResultStatus_Error = 3
} PMODResultStatus;

@protocol PMODAsyncOperation;

typedef void (^AsyncOperationCompletedBlock)(PMODResultStatus status,id result);

@protocol PMODAsyncOperation <PMODObjectBase>
- (id) getResult:(unsigned int)timeout;
-(PMODResultStatus) status;
-(PMODResultStatus) wait:(unsigned int)timeout;
-(void) setCompletedBlock:(AsyncOperationCompletedBlock) completedBlock;
@end
