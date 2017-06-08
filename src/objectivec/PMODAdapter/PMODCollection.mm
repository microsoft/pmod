/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODCollection.mm
****/


#import "PMODCollection.h"
#import "PMODInterop.h"
#import "PMODAdapterUtil.h"

#import <pmod/interfaces/observable_collection.h>
#import <foundation/pv_util.h>
#import <pmod/event_connection.h>
#import <foundation/com_ptr.h>
#import <foundation/array_wrapper.h>

#import "PMODEventHandlerAdapter.h"

using namespace pmod;
using namespace foundation;

class CNotifyCollectionChangedHandler:
public
	CEventHandlerAdapter
	<
		CNotifyCollectionChangedHandler,
		INotifyCollectionChangedEventHandler,
		INotifyCollectionChangedEventArgs,
		&IID_INotifyCollectionChangedEventHandler
	> 
{
public:
	static void fireEventArgs(void *self,IUnknown *pSender,INotifyCollectionChangedEventArgs* pArgs);
};

static const void* TTRetainNoOp(CFAllocatorRef allocator, const void *value) { return value; }
static void TTReleaseNoOp(CFAllocatorRef allocator, const void *value) { }

NSMutableArray* TTCreateNonRetainingArray() {
    CFArrayCallBacks callbacks = kCFTypeArrayCallBacks;
    callbacks.retain = TTRetainNoOp;
    callbacks.release = TTReleaseNoOp;
    return (NSMutableArray*)CFArrayCreateMutable(nil, 0, &callbacks);
}

@implementation NSMutableArray (WeakReferences)
+ (id)mutableArrayUsingWeakReferences {
    return [self mutableArrayUsingWeakReferencesWithCapacity:0];
}

+ (id)mutableArrayUsingWeakReferencesWithCapacity:(NSUInteger)capacity {
    CFArrayCallBacks callbacks = {0, NULL, NULL, CFCopyDescription, CFEqual};
    // We create a weak reference array
    return (id)(CFArrayCreateMutable(0, capacity, &callbacks));
}
@end

NSString *const _itemsKey = @"items";

@interface PMODNotifyCollectionChangedEventArgs()

-(PMODNotifyCollectionChangedEventArgs *) initWithEventArgs: (INotifyCollectionChangedEventArgs *) pNotifyCollectionChangedEventArgs  model:(PMODCollection *)nsCollection;

@end

@implementation PMODCollection
{
@private
    pmod::IObservableCollection				*_pObservableCollection;
    pmod::INotifyCollectionChangedEventHandler	*_pEventHandler;
    NSMutableDictionary								*_oldValues;
}


+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)theKey {
	
    BOOL automatic = NO;
    if ([theKey isEqualToString:_itemsKey]) {
        automatic = NO;
    } else {
        automatic=[super automaticallyNotifiesObserversForKey:theKey];
    }
    return automatic;
}

-(PMODCollection *) initWithObservableCollection:(pmod::IObservableCollection *)pObservableCollection
{
	self = [super init];
	if(self) 
	{
		_oldValues = [[NSMutableDictionary alloc] init];
		[self setObservableCollection:pObservableCollection];
	}
	return self;
}

-(void) dispose {
	[super dispose];
    // Release 'OldValues' Dictionary
    RELEASE(_oldValues);
    RELEASE_INTERFACE(_pObservableCollection);
}

-(void) setObservableCollection:(pmod::IObservableCollection *) pObservableCollection
{
	_pObservableCollection = pObservableCollection;
	_pObservableCollection->AddRef();
	
}

-(foundation::IObject *) object
{
	return _pObservableCollection;
}

- (NSArray *) toNSItems:(foundation::InspectableArrayWrapper& )items
{
    NSMutableArray *nsArray = nil;
    if(items.GetSize() != 0)
    {
        nsArray = [[[NSMutableArray alloc] initWithCapacity:items.GetSize()] autorelease];
        for(UINT32 index = 0;index < items.GetSize();++index)
        {
            [nsArray addObject:[self toNSObject:items[index]]];
        }
    }
    return nsArray;
}
- (NSArray *)getItems
{
    foundation::InspectableArrayWrapper items;
    [PMODInterop throwIfFailed:(_pObservableCollection->GetItems(items.GetSizeAddressOf(), items.GetBufferAddressOf()))];
    return [self toNSItems:items];

}
- (void)enumerateItems:(PMODItemEnumeratorBlock) block
{
    ComPtr<IIterator_impl<foundation::IInspectable*>> spIterator;
    [PMODInterop throwIfFailed:(_pObservableCollection->First(spIterator.GetAddressOf()))];
    
    while(1)
    {
        bool hasCurrent;
        spIterator->GetHasCurrent(&hasCurrent);
        if(!hasCurrent)
        {
            break;
        }
        InspectablePtr spItem;
        spIterator->GetCurrent(spItem.GetAddressOf());
        block([self toNSObject:spItem]);
        spIterator->MoveNext(&hasCurrent);
    }

}

- (NSUInteger)countOfItems
{
	UINT32 count;
	[PMODInterop throwIfFailed:(_pObservableCollection->GetCount(&count))];

	//NSLog(@"PMODCollectionModel:countOfItems is:%i",count);
	return count;
}

- (id)objectInItemsAtIndex:(NSUInteger)index
{
	
	InspectablePtr itemValue;
	[PMODInterop throwIfFailed:(_pObservableCollection->GetItem((UINT32)index,itemValue.GetAddressOf()))];
	return [self toNSObject:itemValue];
}

- (NSArray *)itemsAtIndexes:(NSIndexSet *)indexes
{
	//NSLog(@"PMODCollectionModelController:itemsAtIndexes -> %@",indexes);
	
    NSMutableArray *nsArray = TTCreateNonRetainingArray();
	[indexes enumerateIndexesUsingBlock:^(NSUInteger idx, BOOL *stop) {

		id value  = nil;
		if([_oldValues count])
		{
			value = [_oldValues objectForKey:[NSNumber numberWithInteger:idx]];
		}
		if(value == nil)
		{
			value = [self objectInItemsAtIndex:idx];
		}
 		[nsArray addObject: value];
	}];
	
	return nsArray;
}


-(void) fireCollectionChanged:(INotifyCollectionChangedEventArgs *)eventArgs
{
    foundation::InspectableArrayWrapper oldItems;
    foundation::InspectableArrayWrapper newItems;
    NotifyCollectionModelChangedAction action;
    UINT32 oldStartingIndex;
    UINT32 newStartingIndex;
	
	NSIndexSet *indexes = nullptr;
	
	eventArgs->GetAction(&action);
    if(action == NotifyCollectionModelChangedAction::ItemAdded)
	{
		
		eventArgs->GetNewStartingIndex(&newStartingIndex);
		eventArgs->GetNewItems(newItems.GetSizeAddressOf(),newItems.GetBufferAddressOf());
		
		//NSLog(@"fireCollectionChanged:newStartingIndex is:%i",newStartingIndex);
		indexes = [[[NSIndexSet alloc] initWithIndexesInRange:NSMakeRange(newStartingIndex,newItems.GetSize())] autorelease];
	
		[self willChange:NSKeyValueChangeInsertion valuesAtIndexes:indexes forKey:_itemsKey];
		
		[self didChange:NSKeyValueChangeInsertion valuesAtIndexes:indexes forKey:_itemsKey];
	}
	else if(action == NotifyCollectionModelChangedAction::ItemRemoved)
	{
		eventArgs->GetOldStartingIndex(&oldStartingIndex);
		eventArgs->GetOldItems(oldItems.GetSizeAddressOf(),oldItems.GetBufferAddressOf());

		//NSLog(@"fireCollectionChanged:oldStartingIndex is:%i",oldStartingIndex);

		indexes = [[[NSIndexSet alloc] initWithIndexesInRange:NSMakeRange(oldStartingIndex,oldItems.GetSize())] autorelease];
		
		for(UINT32 index = 0 ;index < oldItems.GetSize(); ++index)
		{
			[_oldValues setObject:[self toNSObject:oldItems[index]] forKey:[NSNumber numberWithInteger:(index + oldStartingIndex)]];
		}
		[self willChange:NSKeyValueChangeRemoval valuesAtIndexes:indexes forKey:_itemsKey];
		// now commit our changes
		[_oldValues removeAllObjects];
		
		[self didChange:NSKeyValueChangeRemoval valuesAtIndexes:indexes forKey:_itemsKey];
	}
	else if(action == NotifyCollectionModelChangedAction::Reset)
	{
        // TODO: do we need to prvide oldItems support ?
        [self willChangeValueForKey:_itemsKey];
        
        [self didChangeValueForKey:_itemsKey];
    }
	//[indexes release];
    
    // notify protocol observers
    if([self hasObservers])
    {
        PMODNotifyCollectionChangedEventArgs *nsNotifyCollectionChangedEventArgs = [[[PMODNotifyCollectionChangedEventArgs alloc] initWithEventArgs:eventArgs model:self] autorelease];
        
        NSInvocation* inv = [PMODObjectBase invocationWithProtocol:@protocol(PMODNotifyCollectionChangedObserver)
                                                       selector:@selector(onNotifyCollectionChanged:withEventArgs:)];
        PMODCollection *nsCollection = self;
        [inv setArgument:&nsCollection atIndex:2];
        [inv setArgument:&nsNotifyCollectionChangedEventArgs atIndex:3];
        [self notifyObservers:inv];
    }

}

void CNotifyCollectionChangedHandler::fireEventArgs(void *self,IUnknown *pSender,INotifyCollectionChangedEventArgs* pArgs)
{
	[(id) self fireCollectionChanged:pArgs];
}

-(void) attachToFoundation
{
    [PMODAdapterUtil throwIfNull:_pObservableCollection];

	NSAssert(_pEventHandler == nil,@"_pEventHandler == nil");
	
	// attach my custom handler
	CNotifyCollectionChangedHandler::CreateInstance(self,&_pEventHandler);
	ObservableCollectionListenerBase::AttachEventHandler(_pObservableCollection,_pEventHandler);
	
}
-(void) detachFromFoundation
{
    [PMODAdapterUtil throwIfNull:_pObservableCollection];

	NSAssert(_pEventHandler != nil,@"_pEventHandler != nil");
	
	ObservableCollectionListenerBase::DetachEventHandler(_pObservableCollection,_pEventHandler);
	// dispose in case it was queued in an Event notification
	static_cast<CNotifyCollectionChangedHandler *>(_pEventHandler)->Dispose();
	// Release count
	_pEventHandler->Release();
	_pEventHandler = nil;
}

@end

@implementation PMODNotifyCollectionChangedEventArgs

@synthesize action = _action;
@synthesize oldStartingIndex = _oldStartingIndex;
@synthesize newStartingIndex = _newStartingIndex;
@synthesize oldValues  = _oldValues;
@synthesize newValues  = _newValues;

-(PMODNotifyCollectionChangedEventArgs *) initWithEventArgs: (INotifyCollectionChangedEventArgs *) pNotifyCollectionChangedEventArgs  model:(PMODCollection *)nsCollection
{
	self = [super init];
	if(self)
	{
        NotifyCollectionModelChangedAction action;
        pNotifyCollectionChangedEventArgs->GetAction(&action);
        _action = PMODNotifyCollectionModelChangedAction(action);
        
        UINT32 index;
        pNotifyCollectionChangedEventArgs->GetOldStartingIndex(&index);
        _oldStartingIndex = index;
        pNotifyCollectionChangedEventArgs->GetNewStartingIndex(&index);
        _newStartingIndex = index;
        
        foundation::InspectableArrayWrapper oldValues;
        pNotifyCollectionChangedEventArgs->GetOldItems(oldValues.GetSizeAddressOf(), oldValues.GetBufferAddressOf());
        
        NSArray *oldValuesArray = [nsCollection toNSItems:oldValues];

        foundation::InspectableArrayWrapper newValues;
        pNotifyCollectionChangedEventArgs->GetNewItems(newValues.GetSizeAddressOf(), newValues.GetBufferAddressOf());
        NSArray *newValuesArray = [nsCollection toNSItems:newValues];
        _oldValues = oldValuesArray;
        _newValues = newValuesArray;
        // retain
        [_oldValues retain];
        [_newValues retain];
	}
	return self;
}

-(void) dealloc {
	[super dealloc];
    [_oldValues release];
    [_newValues release];
}

@end
