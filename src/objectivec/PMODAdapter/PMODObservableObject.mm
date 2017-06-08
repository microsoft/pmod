/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODObservableObject.mm
****/


#import "PMODObservableObject.h"
#import "PMODInterop.h"
#import "PMODAdapterUtil.h"

#import <foundation/pv_util.h>
#import <pmod/event_connection.h>
#import <foundation/com_ptr.h>
#import <foundation/type_info_statics_util.h>
#import <pmod/interfaces/observable_object.h>
#import <pmod/eventhandler_util.h>

#import "PMODEventHandlerAdapter.h"

using namespace pmod;
using namespace foundation;

@interface PMODObjectDispatch (Private)
-(void) initObjectDispatch:(IObjectDispatch *)pObjectDispatch;
@end

//----------------------------------------------------------------------
// Class:CPropertyChangedEventHandler
//----------------------------------------------------------------------
class CPropertyChangedEventHandler:
public
	CEventHandlerAdapter
	<
	CPropertyChangedEventHandler,
	IPropertyChangedEventHandler,
	IPropertyChangedEventArgs,
	&IID_IPropertyChangedEventHandler
	> 
{
public:
	static void fireEventArgs(void *self,IUnknown *pSender,IPropertyChangedEventArgs* pArgs);
};
//----------------------------------------------------------------------
// CEventModelEventHandler
//----------------------------------------------------------------------
class CEventModelEventHandler:
public
CEventHandlerAdapter
<
CEventModelEventHandler,
IEventModelEventHandler,
IEventModelEventArgs,
&IID_IEventModelEventHandler
>
{
public:
	static void fireEventArgs(void *self,IUnknown *pSender,IEventModelEventArgs* pArgs);
};

@implementation PMODPropertyChangedEventArgs
{
    PMODObservableObject *_nsObject;
    IPropertyChangedEventArgs *_pPropertyChangedEventArgs;
}
-(unsigned int) property
{
    UINT32 _property;
    _pPropertyChangedEventArgs->GetProperty(&_property);
    return _property;
}
-(NSString *) propertyName
{
    HStringPtr _propertyName;
    _pPropertyChangedEventArgs->GetPropertyName(_propertyName.GetAddressOf());
    return [PMODInterop toNSString:_propertyName];
}

-(id) oldValue
{
    InspectablePtr value;
    _pPropertyChangedEventArgs->GetOldValue(value.ReleaseAndGetAddressOf());
    return [_nsObject toNSObject:value];
}

-(id) newValue
{
    InspectablePtr value;
    _pPropertyChangedEventArgs->GetNewValue(value.ReleaseAndGetAddressOf());
    return [_nsObject toNSObject:value];
}

-(PMODPropertyChangedEventArgs *) initWithEventArgs: (IPropertyChangedEventArgs *) pPropertyChangedEventArgs  model:(PMODObservableObject *)nsObject
{
	self = [super init];
	if(self)
	{
        _pPropertyChangedEventArgs = pPropertyChangedEventArgs;
        _pPropertyChangedEventArgs->AddRef();
        _nsObject = nsObject;
        [_nsObject retain];
	}
	return self;
}

-(void) dealloc {
    RELEASE_INTERFACE(_pPropertyChangedEventArgs);
    [_nsObject release];
	[super dealloc];
}

@end

@implementation PMODModelEventArgs
{
    PMODObservableObject *_nsObject;
    IEventModelEventArgs *_pEventModelEventArgs;
}
-(unsigned int) event
{
    UINT32 _event;
    _pEventModelEventArgs->GetEvent(&_event);
    return _event;
}
-(id) eventArgs;
{
    InspectablePtr value;
    _pEventModelEventArgs->GetEventArgs(value.ReleaseAndGetAddressOf());
    return [_nsObject toNSObject:value];
}

-(PMODModelEventArgs *) initWithEventArgs: (IEventModelEventArgs *) pEventModelEventArgs  model:(PMODObservableObject *)nsObject
{
	self = [super init];
	if(self)
	{
        _pEventModelEventArgs = pEventModelEventArgs;
        _pEventModelEventArgs->AddRef();
        _nsObject = nsObject;
        [_nsObject retain];
	}
	return self;
}

-(void) dealloc {
    RELEASE_INTERFACE(_pEventModelEventArgs);
    [_nsObject release];
	[super dealloc];
}

@end

@implementation PMODObservableObject
{
    pmod::IPropertyChangedEventHandler	*_pPropertyChangedEventHandler;
    pmod::IEventModelEventHandler	*_pEventModelEventHandler;
    NSMutableDictionary								*_oldValues;

}

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)theKey {
	
    BOOL automatic = NO;
    return automatic;
}

-(PMODObservableObject *) initWithObservableObject: (IObservableObject *)pObservableObject
{
	self = [super init];
	if(self) 
	{
		_oldValues = [[NSMutableDictionary alloc] init];
		[self initObservableObject:pObservableObject];
	}
	return self;
}

-(void) initObservableObject: (IObservableObject *) pObservbaleObject
{
    [self initSource:pObservbaleObject];
}

-(void) initSource:(IObservableObject *)pObservableObject
{
    [super initObjectDispatch:pObservableObject];
}

-(void) dispose {
	// Release 'OldValues' Dictionaries
    RELEASE(_oldValues);
	[super dispose];
}

- (pmod::IObservableObject *) observableObject
{
    return (pmod::IObservableObject *)[super objectDispatch];
}

-(void) firePropertyChanged:(IPropertyChangedEventArgs *)eventArgs
{
	UINT32 propertyId;
	eventArgs->GetProperty(&propertyId);
	
	
	foundation::HStringPtr hPropertyName;
	[PMODInterop throwIfFailed:(eventArgs->GetPropertyName(hPropertyName.GetAddressOf()))];
	
    string_t propertyName = foundation::get_string_t(hPropertyName);
    
    propertyName[0] = tolower(propertyName[0]);
	NSString *nsPropertyName = [NSString stringWithUTF8String:propertyName.c_str()];
	
	InspectablePtr oldValue;
	[PMODInterop throwIfFailed:(eventArgs->GetOldValue(oldValue.GetAddressOf()))];
	
	id nsValue = [self toNSObject:oldValue];
	NSAssert(nsValue != nil,@"nsValue != nil");
	
	[_oldValues setObject:nsValue forKey:nsPropertyName];
	
	[self willChangeValueForKey:nsPropertyName];
	
	// now commit our changes
	[_oldValues removeAllObjects];
	
	[self didChangeValueForKey:nsPropertyName];
	
    // notify protocol observers
    if([self hasObservers])
    {
        PMODPropertyChangedEventArgs *nsPropertyChangedEventArgs = [[[PMODPropertyChangedEventArgs alloc] initWithEventArgs:eventArgs model:self] autorelease];
        
        NSInvocation* inv = [PMODObjectBase invocationWithProtocol:@protocol(PMODPropertyChangedObserver)
                                                       selector:@selector(onPropertyChanged:withEventArgs:)];
        PMODObservableObject *nsObject = self;
        [inv setArgument:&nsObject atIndex:2];
        [inv setArgument:&nsPropertyChangedEventArgs atIndex:3];
        [self notifyObservers:inv];
    }
}

-(void) fireEventModel:(IEventModelEventArgs *)eventArgs
{
    // notify protocol observers
    if([self hasObservers])
    {
        PMODModelEventArgs *nsModelEventArgs = [[[PMODModelEventArgs alloc] initWithEventArgs:eventArgs model:self] autorelease];
        
        NSInvocation* inv = [PMODObjectBase invocationWithProtocol:@protocol(PMODModelEventObserver)
                                                        selector:@selector(onModelEvent:withEventArgs:)];
        PMODObservableObject *nsObject = self;
        [inv setArgument:&nsObject atIndex:2];
        [inv setArgument:&nsModelEventArgs atIndex:3];
        [self notifyObservers:inv];
    }
    
}

-(id) copyWithZone: (NSZone *) zone
{
    PMODObservableObject *nsObjectCopy = [[PMODObservableObject allocWithZone: zone] initWithObservableObject:[self observableObject]];
    
    [nsObjectCopy setAdapterFactory:self.adapterFactory];
    return nsObjectCopy;
}

void CPropertyChangedEventHandler::fireEventArgs(void *self,IUnknown *pSender,IPropertyChangedEventArgs* pArgs)
{
	[(id) self firePropertyChanged:pArgs];
}

void CEventModelEventHandler::fireEventArgs(void *self,IUnknown *pSender,IEventModelEventArgs* pArgs)
{
	[(id) self fireEventModel:pArgs];
}

-(void) attachToFoundation
{
    pmod::IObservableObject *pObservableObject = [self observableObject];
    [PMODAdapterUtil throwIfNull:pObservableObject];
	NSAssert(_pPropertyChangedEventHandler == nil,@"_pPropertyChangedEventHandler == nil");
	
	// attach my custom handler
	CPropertyChangedEventHandler::CreateInstance(self,&_pPropertyChangedEventHandler);
	ObservableObjectListenerBase::AttachEventHandler(pObservableObject,_pPropertyChangedEventHandler);

    CEventModelEventHandler::CreateInstance(self,&_pEventModelEventHandler);
	EventModelListenerBase::AttachEventHandler(pObservableObject,_pEventModelEventHandler);
}

-(void) detachFromFoundation
{
    pmod::IObservableObject *pObservableObject = [self observableObject];
    [PMODAdapterUtil throwIfNull:pObservableObject];
	NSAssert(_pPropertyChangedEventHandler != nil,@"_pPropertyChangedEventHandler != nil");
	
	ObservableObjectListenerBase::DetachEventHandler(pObservableObject,_pPropertyChangedEventHandler);
	// dispose in case it was queued in an Event notification
	static_cast<CPropertyChangedEventHandler *>(_pPropertyChangedEventHandler)->Dispose();
    
 	EventModelListenerBase::DetachEventHandler(pObservableObject,_pEventModelEventHandler);
	// dispose in case it was queued in an Event notification
	static_cast<CEventModelEventHandler *>(_pEventModelEventHandler)->Dispose();
   
	// Release count
	_pPropertyChangedEventHandler->Release();
	_pPropertyChangedEventHandler = nil;
	_pEventModelEventHandler->Release();
	_pEventModelEventHandler = nil;
}


@end
