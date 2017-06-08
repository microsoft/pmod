/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODBindingBase.mm
****/


#import "PMODBindingBase.h"
#import "PMODInterop.h"

#import "PMODPropertyModelProtocol.h"


#import <pmod/interfaces/binding.h>
#import <foundation/pv_util.h>
#import <pmod/event_connection.h>
#import <foundation/com_ptr.h>
#import <foundation/array_wrapper.h>

#import "PMODEventHandlerAdapter.h"

using namespace pmod;
using namespace foundation;

class CBindingValueChangedEventHandler:
public
	CEventHandlerAdapter
	<
		CBindingValueChangedEventHandler,
		IBindingValueChangedEventHandler,
		IBindingValueChangedEventArgs,
		&IID_IBindingValueChangedEventHandler
	> 
{
public:
	static void fireEventArgs(void *self,IUnknown *pSender,IBindingValueChangedEventArgs* pArgs);
};

@implementation NSBindingValueChangedEventArgs

@synthesize bindingValue = _bindingValue;


-(NSBindingValueChangedEventArgs *) initWithEventArgs: (IBindingValueChangedEventArgs *) pBindingValueChangedEventArgs  bindingBase:(NSBindingBase *)bindingBase
{
	self = [super init];
	if(self)
	{
        // copy new binding value
        _bindingValue = bindingBase.bindingValue;

	}
	return self;
}
@end

@implementation NSBindingBase
{
@private
    pmod::IBindingBase				*_pBindingBase;
    pmod::IBindingValueChangedEventHandler	*_pEventHandler;
    id                                          _bindingValue;
}
@synthesize bindingValue = _bindingValue;

-(NSBindingBase *) initWithBindingBase:(pmod::IBindingBase *) pBindingBase
{
	self = [super init];
	if(self) 
	{
		[self setBindingBase:pBindingBase];
	}
	return self;
}

+(NSBindingBase *) createBinding:(PMODAdapterFactory *)adapterFactory source:(id<PMODPropertyModel>) withSource path:(NSString *) withPath
{
    com_ptr<IPropertyBinding> spPropertyBinding;
    HRESULT hr = pmod::library_util::create_binding(
                                            [withSource propertyModel],
                                            [withPath UTF8String],
                                                    PropertyBindingOptions::None,
                                            spPropertyBinding.get_address_of());
    [PMODInterop throwIfFailed:hr];
    
    NSBindingBase *nsBindingBase = [NSBindingBase alloc];
    [nsBindingBase initWithBindingBase:spPropertyBinding];
    [nsBindingBase setAdapterFactory:adapterFactory];
    [nsBindingBase autorelease];
    return nsBindingBase;
}

+(NSBindingBase *) createExpressionBinding:(PMODAdapterFactory *)adapterFactory source:(id<PMODPropertyModel>) withSource bindingExpression:(NSString *) withBindingExpression
{
    
    com_ptr<IBindingBase> spBindingBase;
    HRESULT hr = pmod::library_util::create_expression_binding([withSource propertyModel],
                                                      [withBindingExpression UTF8String],
                                                               PropertyBindingOptions::None,
                                                      spBindingBase.get_address_of());
    [PMODInterop throwIfFailed:hr];
    
    NSBindingBase *nsBindingBase = [NSBindingBase alloc];
    [nsBindingBase initWithBindingBase:spBindingBase];
    [nsBindingBase setAdapterFactory:adapterFactory];
    [nsBindingBase autorelease];
    return nsBindingBase;
}

-(void) dispose {
	[super dispose];
    ReleaseInterface(_pBindingBase);
}

-(void) setBindingBase:(pmod::IBindingBase *) pBindingBase
{
	_pBindingBase = pBindingBase;
	_pBindingBase->AddRef();
}

-(void) fireBindingValueChanged:(IBindingValueChangedEventArgs *)eventArgs
{
    [self refreshBindingValue];
    
    // notify protocol observers
    if([self hasObservers])
    {
        NSBindingValueChangedEventArgs *nsBindingValueChangedEventArgs = [[NSBindingValueChangedEventArgs alloc] initWithEventArgs:eventArgs bindingBase:self];
        [nsBindingValueChangedEventArgs autorelease];
        
        NSInvocation* inv = [PMODFoundation invocationWithProtocol:@protocol(NSBindingValueChangedObserver)
                                                       selector:@selector(onBindingValueChanged:withEventArgs:)];
        NSBindingBase *bindingBase = self;
        [inv setArgument:&bindingBase atIndex:2];
        [inv setArgument:&nsBindingValueChangedEventArgs atIndex:3];
        [self notifyObservers:inv];
    }
}

-(void) refreshBindingValue
{
    inspectable_ptr spValue;
    _pBindingBase->GetValue(spValue.get_address_of());
    
    [self willChangeValueForKey:@"bindingValue"];
    _bindingValue = [self toNSObject:spValue];
    [self didChangeValueForKey:@"bindingValue"];
}

void CBindingValueChangedEventHandler::fireEventArgs(void *self,IUnknown *pSender,IBindingValueChangedEventArgs* pArgs)
{
	[(id) self fireBindingValueChanged:pArgs];
}

-(void) attachToFoundation
{
	NSAssert(_pBindingBase != nil,@"_pBindingBase != nil");
	NSAssert(_pEventHandler == nil,@"_pEventHandler == nil");
	
	// attach my custom handler
	CBindingValueChangedEventHandler::CreateInstance(self,&_pEventHandler);
	property_binding_listener::attach_eventhandler(_pBindingBase,_pEventHandler);

    [self refreshBindingValue];
}

-(void) detachFromFoundation
{
	NSAssert(_pBindingBase != nil,@"_pBindingBase != nil");
	NSAssert(_pEventHandler != nil,@"_pEventHandler != nil");
	
	property_binding_listener::detach_eventhandler(_pBindingBase,_pEventHandler);
	// dispose in case it was queued in an Event notification
	static_cast<CBindingValueChangedEventHandler *>(_pEventHandler)->Dispose();
	// Release count
	_pEventHandler->Release();
	_pEventHandler = nil;
}


@end
