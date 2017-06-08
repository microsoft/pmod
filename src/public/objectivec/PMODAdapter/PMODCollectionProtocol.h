/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODCollectionProtocol.h
****/


#import "PMODObjectNodeProtocol.h"

typedef enum
{
	PMODNotifyCollection_ItemAdded = 1,
    PMODNotifyCollection_ItemRemoved = 2,
    PMODNotifyCollection_ItemReplaced = 3,
    PMODNotifyCollection_Reset = 4
} PMODNotifyCollectionModelChangedAction;

@interface PMODNotifyCollectionChangedEventArgs : NSObject

@property (readonly) PMODNotifyCollectionModelChangedAction action;
@property (readonly) unsigned int oldStartingIndex;
@property (readonly) unsigned int newStartingIndex;
@property (readonly) NSArray * oldValues;
@property (readonly, getter=itemNewValues) NSArray * newValues;

@end


@protocol PMODNotifyCollectionChangedObserver
- (void)onNotifyCollectionChanged:(id)sender withEventArgs:(PMODNotifyCollectionChangedEventArgs *)eventArgs;
@end

typedef void (^PMODItemEnumeratorBlock)(id item);

@protocol PMODCollection <PMODObjectNode>
- (NSUInteger)countOfItems;
- (id)objectInItemsAtIndex:(NSUInteger)index;
- (NSArray *)getItems;
- (void)enumerateItems:(PMODItemEnumeratorBlock) block;
@end
