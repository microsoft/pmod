/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAsyncOperation.mm
****/


#import "PMODAsyncOperation.h"
#import "PMODInterop.h"
#import "PMODAdapterUtil.h"

#import <foundation/interfaces/async_operation.h>
#import <foundation/pv_util.h>
#import <pmod/event_connection.h>
#import <foundation/com_ptr.h>
#import <foundation/async_operation_connection.h>

#import "PMODEventHandlerAdapter.h"

using namespace pmod;
using namespace foundation;

class CCompletedHandler:
public
	CEventHandlerAdapter
	<
		CCompletedHandler,
		ICompletedEventHandler,
		ICompletedEventArgs,
		&IID_ICompletedEventHandler
	> 
{
public:
	static void fireEventArgs(void *self,IUnknown *pSender,ICompletedEventArgs* pArgs);
};

@implementation PMODCompletedEventArgs

@end


@interface CompletedBlockDelegate : NSObject<PMODCompletedObserver>
{
    AsyncOperationCompletedBlock callback;
}

@end

@implementation CompletedBlockDelegate

- (id) initWithBlock:(AsyncOperationCompletedBlock) block
{
    if(self = [super init])
    {
        callback = Block_copy(block);
    }
    return self;
}

- (void)onCompleted:(id)sender withEventArgs:(PMODCompletedEventArgs *)eventArgs
{
    id<PMODAsyncOperation> asyncOperation = (id<PMODAsyncOperation>)sender;
    id result  = [asyncOperation getResult:0];
    callback(asyncOperation.status,result);
    
    // remove after completion
    [asyncOperation removeObserver:self];
}

@end

@implementation PMODAsyncOperation
{
    foundation::IAsyncOperation			*_pAsyncOperation;
    foundation::ICompletedEventHandler       *_pEventHandler;
}
-(PMODAsyncOperation *) initWithAsyncOperation:(IAsyncOperation *) pAsyncOperation
{
	self = [super init];
	if(self) 
	{
		[self setAsycnOperation:pAsyncOperation];
	}
	return self;
}

-(void) dispose {
	[super dispose];
    RELEASE_INTERFACE(_pAsyncOperation);
}

-(void) setAsycnOperation:(foundation::IAsyncOperation *)pAsyncOperation
{
	_pAsyncOperation = pAsyncOperation;
	_pAsyncOperation->AddRef();
}

-(foundation::IObject *) object
{
	return _pAsyncOperation;
}

- (id) getResult:(unsigned int)timeout
{
	InspectablePtr spResult;
	[PMODInterop throwIfFailed:(_pAsyncOperation->GetResult(timeout,spResult.GetAddressOf()))];
	return [self toNSObject:spResult];
}
-(PMODResultStatus) status
{
    foundation::ResultStatus status;
	[PMODInterop throwIfFailed:(_pAsyncOperation->GetStatus(&status))];
    return (PMODResultStatus)status;
}

-(PMODResultStatus) wait:(unsigned int)timeout
{
    foundation::ResultStatus status;
	[PMODInterop throwIfFailed:(_pAsyncOperation->Wait(timeout,&status))];
    return (PMODResultStatus)status;
}

-(void) fireCompleted:(ICompletedEventArgs *)eventArgs
{
    // notify protocol observers
    if([self hasObservers])
    {
        PMODCompletedEventArgs *nsCompletedEventArgs = [PMODCompletedEventArgs alloc];
        [nsCompletedEventArgs autorelease];
        
        NSInvocation* inv = [PMODFoundation invocationWithProtocol:@protocol(PMODCompletedObserver)
                                                      selector:@selector(onCompleted:withEventArgs:)];
        PMODAsyncOperation *nsAsyncOperation = self;
        [inv setArgument:&nsAsyncOperation atIndex:2];
        [inv setArgument:&nsAsyncOperation atIndex:3];
        [self notifyObservers:inv];
    }

}

void CCompletedHandler::fireEventArgs(void *self,IUnknown *pSender,ICompletedEventArgs* pArgs)
{
	[(id) self fireCompleted:pArgs];
}

-(void) attachToFoundation
{
    [PMODAdapterUtil throwIfNull:_pAsyncOperation];
	NSAssert(_pEventHandler == nil,@"_pEventHandler == nil");
	
	// attach my custom handler
	CCompletedHandler::CreateInstance(self,&_pEventHandler);
	AsyncOperationCompletedListenerBase::AttachEventHandler(_pAsyncOperation,_pEventHandler);
	
}
-(void) detachFromFoundation
{
    [PMODAdapterUtil throwIfNull:_pAsyncOperation];
	NSAssert(_pEventHandler != nil,@"_pEventHandler != nil");
	
	AsyncOperationCompletedListenerBase::DetachEventHandler(_pAsyncOperation,_pEventHandler);
	// dispose in case it was queued in an Event notification
	static_cast<CCompletedHandler *>(_pEventHandler)->Dispose();
	// Release count
	_pEventHandler->Release();
	_pEventHandler = nil;
}

-(void) setCompletedBlock:(AsyncOperationCompletedBlock) completedBlock;
{
    if(self.status != PMODResultStatus_Started)
    {
        completedBlock(self.status,[self getResult:0]);
    }
    else
    {
        CompletedBlockDelegate *observerDelegate = [[CompletedBlockDelegate alloc] initWithBlock:completedBlock];
        [self addObserver:observerDelegate];
    }
}


@end
