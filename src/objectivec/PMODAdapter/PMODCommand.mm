/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODCommand.mm
****/


#import "PMODCommand.h"
#import "PMODInterop.h"
#import "PMODAdapterUtil.h"

#import <pmod/interfaces/command.h>
#import <foundation/pv_util.h>
#import <pmod/event_connection.h>
#import <foundation/com_ptr.h>

#import "PMODEventHandlerAdapter.h"

using namespace pmod;
using namespace foundation;

class CCanExecuteChangedEventHandler:
public
	CEventHandlerAdapter
	<
	CCanExecuteChangedEventHandler,
	ICanExecuteChangedEventHandler,
	ICanExecuteChangedEventArgs,
	&IID_ICanExecuteChangedEventHandler
	> 
{
public:
	static void fireEventArgs(void *self,IUnknown *pSender,ICanExecuteChangedEventArgs* pArgs);
};

@implementation PMODCanExecuteChangedEventArgs

@synthesize oldState = _oldState;
@synthesize newState = _newState;


-(PMODCanExecuteChangedEventArgs *) initWithEventArgs: (ICanExecuteChangedEventArgs *) pCanExecuteChangedEventArgs  commandModel:(PMODCommand *)command
{
	self = [super init];
	if(self)
	{
        UINT32 state;
        pCanExecuteChangedEventArgs->GetOldState(&state);
        _oldState = state;
        pCanExecuteChangedEventArgs->GetNewState(&state);
        _newState = state;
	}
	return self;
}
@end


@implementation PMODCommand
{
@private
    pmod::ICommand                    *_pCommand;
    pmod::ICanExecuteChangedEventHandler	*_pEventHandler;
}

-(PMODCommand *) initWithCommand: (ICommand *)pCommand
{
	self = [super init];
	if(self) 
	{
		[self setCommand:pCommand];
	}
	return self;
}

-(void) dispose {
	[super dispose];
    RELEASE_INTERFACE(_pCommand);
}

-(void) setCommand:(ICommand *)pCommand
{
	_pCommand = pCommand;
	pCommand->AddRef();
}

-(void) fireCanExecuteChangedChanged:(ICanExecuteChangedEventArgs *)eventArgs
{
    // state property
    [self willChangeValueForKey:@"state"];
    [self didChangeValueForKey:@"state"];

    // isEnabled property
    [self willChangeValueForKey:@"isEnabled"];
    [self didChangeValueForKey:@"isEnabled"];

    // notify protocol observers
    if([self hasObservers])
    {
        PMODCanExecuteChangedEventArgs *nsCanExecuteChangedEventArgs = [[PMODCanExecuteChangedEventArgs alloc] initWithEventArgs:eventArgs commandModel:self];
        [nsCanExecuteChangedEventArgs autorelease];
        
        NSInvocation* inv = [PMODFoundation invocationWithProtocol:@protocol(PMODCanExecuteChangedObserver)
                                                      selector:@selector(onCanExecuteChanged:withEventArgs:)];
        PMODCommand *nsCommand = self;
        [inv setArgument:&nsCommand atIndex:2];
        [inv setArgument:&nsCanExecuteChangedEventArgs atIndex:3];
        [self notifyObservers:inv];
    }
}

-(foundation::IObject *) object
{
	return _pCommand;
}


void CCanExecuteChangedEventHandler::fireEventArgs(void *self,IUnknown *pSender,ICanExecuteChangedEventArgs* pArgs)
{
	[(id) self fireCanExecuteChangedChanged:pArgs];
}

-(void) attachToFoundation
{
    [PMODAdapterUtil throwIfNull:_pCommand];
	NSAssert(_pEventHandler == nil,@"_pEventHandler == nil");
	
	// attach my custom handler
	CCanExecuteChangedEventHandler::CreateInstance(self,&_pEventHandler);
	CommandListenerBase::AttachEventHandler(_pCommand,_pEventHandler);
	
}

-(void) detachFromFoundation
{
    [PMODAdapterUtil throwIfNull:_pCommand];
	NSAssert(_pEventHandler != nil,@"_pEventHandler != nil");
	
	CommandListenerBase::DetachEventHandler(_pCommand,_pEventHandler);
	// dispose in case it was queued in an Event notification
	static_cast<CCanExecuteChangedEventHandler *>(_pEventHandler)->Dispose();
	// Release count
	_pEventHandler->Release();
	_pEventHandler = nil;
}

- (BOOL) canExecute:(id)parameter
{
    NSAssert(_pCommand != nil,@"_pCommand != nil");
 	InspectablePtr spParameter;
	spParameter.Attach([PMODAdapter fromNSObject:parameter]);
    bool canExecute;
    [PMODInterop throwIfFailed:_pCommand->CanExecute(spParameter, &canExecute)];
    return canExecute ? TRUE:FALSE;
}


- (id) execute:(id)parameter
{
    NSAssert(_pCommand != nil,@"_pCommand != nil");
 	InspectablePtr spParameter;
	spParameter.Attach([PMODAdapter fromNSObject:parameter]);
 	InspectablePtr spResult;
    [PMODInterop throwIfFailed:_pCommand->Execute(spParameter, spResult.GetAddressOf())];
	return [self toNSObject:spResult];    
}

- (id) execute
{
    return [self execute:nil];
}

- (BOOL) isEnabled
{
    return self.state == pmod::CommandStateEnabled;
}

- (int) state
{
    NSAssert(_pCommand != nil,@"_pCommand != nil");
    UINT32 state;
    [PMODInterop throwIfFailed:_pCommand->GetState(&state)];
    return state;
}

@end
