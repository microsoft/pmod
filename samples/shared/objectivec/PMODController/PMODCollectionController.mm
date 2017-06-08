/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODCollectionController.mm
****/

#import "PMODCollectionController.h"


@implementation PMODCollectionController
{
	id<PMODCollection> _pmodCollection;
}

@synthesize cellClass;
@synthesize cellIdentifier;

-(void) dealloc {
	self.collection = nil;
}

-(id<PMODCollection>) collection
{
    return _pmodCollection;
}

-(void) setCollection:(id<PMODCollection>) source
{
    if(_pmodCollection != nil)
    {
        [_pmodCollection removeObserver:self];
    }
    _pmodCollection = source;
    
    if(_pmodCollection != nil)
    {
        [_pmodCollection addObserver:self];
    }
    if(self.isViewLoaded)
    {
        [self.tableView reloadData];
    }
}

- (void)onNotifyCollectionChanged:(id)sender withEventArgs:(PMODNotifyCollectionChangedEventArgs *)eventArgs
{
    if(eventArgs.action == PMODNotifyCollection_ItemAdded)
    {
        [self.tableView insertRowsAtIndexPaths:[PMODCollectionController createIndexPathArray:eventArgs.newStartingIndex items:eventArgs.newValues] withRowAnimation:UITableViewRowAnimationFade];
    }
    else if(eventArgs.action == PMODNotifyCollection_ItemRemoved)
    {
        [self.tableView deleteRowsAtIndexPaths:[PMODCollectionController createIndexPathArray:eventArgs.oldStartingIndex items:eventArgs.oldValues] withRowAnimation:UITableViewRowAnimationFade];
    }
    else if(eventArgs.action == PMODNotifyCollection_Reset)
    {
        [self.tableView reloadData];
    }
}

+(NSMutableArray *) createIndexPathArray:(unsigned int)nIndex items:(NSArray *)items
{
    NSMutableArray *indexPathArray = [NSMutableArray array];
    for (int i = 0; i < items.count; i++) {
        NSUInteger indexPathIndices[2];
        indexPathIndices[0] = 0;
        indexPathIndices[1] = nIndex + i;
        NSIndexPath *newPath = [NSIndexPath indexPathWithIndexes:indexPathIndices length:2];
        [indexPathArray addObject:newPath];
    }
    return indexPathArray;
}

- (NSInteger)tableView:(UITableView *)table numberOfRowsInSection:(NSInteger)section
{
	return [_pmodCollection countOfItems];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	NSString *cellIdentifierDequeue = self.cellIdentifier == NULL ?
    NSStringFromClass(self.cellClass):
    self.cellIdentifier;
    
    UITableViewCell *tvc = [tableView dequeueReusableCellWithIdentifier:cellIdentifierDequeue];
    
    if (tvc == NULL) {
        tvc = [[self.cellClass alloc] init];
    }
    
    if([tvc conformsToProtocol:@protocol(PMODCellController)])
    {
        id<PMODCellController> cellController = (id<PMODCellController>)tvc;
        // set data context
        id item = [_pmodCollection objectInItemsAtIndex:indexPath.row];
        [cellController setContext:item];
    }
	
    // cast to what is expected
    return tvc;
}
		 
@end
