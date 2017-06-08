/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODList.mm
****/


#import "PMODList.h"
#import "PMODInterop.h"
#import <foundation/com_ptr.h>


#import <pmod/interfaces/observable_collection.h>

using namespace foundation;
using namespace pmod;

@interface PMODCollection (Private)
-(PMODCollection *) initWithObservableCollection:(pmod::IObservableList *) pObservableList;
@end

@implementation PMODList
{
@private
    pmod::IObservableList				*_pObservableList;
}


-(PMODList *) initWithObservableList:(IObservableList *) pObservableList
{
	self = [super initWithObservableCollection:pObservableList ];
	if(self) 
	{
		[self setObservableList:pObservableList];
	}
	return self;
}

-(void) setObservableList:(pmod::IObservableList *) pObservableList
{
	_pObservableList = pObservableList;
	
}

- (void)insertObject:(NSObject *)item inItemsAtIndex:(NSUInteger)index
{
	InspectablePtr itemValue;
	itemValue.Attach([PMODAdapter fromNSObject:item]);
	[PMODInterop throwIfFailed:(_pObservableList->InsertItem((UINT32)index,itemValue))];
}

- (void)removeObjectFromItemsAtIndex:(NSUInteger)index
{
	[PMODInterop throwIfFailed:(_pObservableList->RemoveAt((UINT32)index))];
}



@end
