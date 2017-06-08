/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODListProtocol.h
****/

#import "PMODCollectionProtocol.h"


@protocol PMODList <PMODCollection>
- (void)insertObject:(NSObject *)item inItemsAtIndex:(NSUInteger)index;
- (void)removeObjectFromItemsAtIndex:(NSUInteger)index;
@end
