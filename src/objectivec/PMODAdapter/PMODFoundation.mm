/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODFoundation.mm
****/


#import "PMODFoundation.h"
#import "PMODObjectBase.h"
#import "PMODAdapterFactory.h"
#import "PMODInterop.h"
#import "PMODAdapterLogCategory.h"
#import "PMODAdapterLoggerTypeInfo.h"

#import <foundation/pv_util.h>
#import <foundation/com_ptr.h>
#import <foundation/ctl/com_library.h>
#import <foundation/library/logger_util.h>

#import <objc/runtime.h>

using namespace foundation;

@implementation PMODFoundation
{
    BOOL			_isAttached;
    unsigned int	_observerCount;
    NSMutableSet*   _observers;
}

-(void) dispose
{
	if(_isAttached)
	{
		[self detachFromFoundation];
	}
    [super dispose];
}

- (void)onAddObserver
{
	++_observerCount;
	if(!_isAttached)
	{
		[self attachToFoundation];
		_isAttached = TRUE;
		// TODO:
		[self retain];
	}    
}

- (void)onRemoveObserver
{
	--_observerCount;
	if(_observerCount == 0 && _isAttached)
	{
		[self detachFromFoundation];
		_isAttached = FALSE;
		// TODO:
		[self autorelease];
	}
}

- (void)addObserver:(NSObject *)observer forKeyPath:(NSString *)keyPath options:(NSKeyValueObservingOptions)options context:(void *)context
{
    if(logger_util::IsAvailable() && logger_util::IsLevelEnabled(
                                     CPMODAdapterLogCategory::GetPMODFoundationCategory(),
                                     LoggingLevel::Debug,
                                     nsadapter::NSFoundation_Category_AddObserver,
                                     nullptr))
    {
        InspectablePtr spUniqueId;
        spUniqueId.Attach([PMODInterop fromNSValue:[self toUniqueId]]);
        
        InspectablePtr spKeyPath;
        spKeyPath.Attach([PMODInterop fromNSValue:keyPath]);
 
        logger_util::Log(CPMODAdapterLogger::GetPMODAdapterLogger(),
                         nullptr,
                         LoggingLevel::Debug,
                         nsadapter::NSFoundation_Category_AddObserver_Id,
                         CPMODAdapterLogCategory::GetPMODFoundationCategory(),
                         nsadapter::AddObserver_LogRecordEnum::TypeId,
                         // UniqueId
                         nsadapter::AddObserver_LogRecord_UniqueId,
                         spUniqueId.Get(),
                         // KeyPath
                         nsadapter::AddObserver_LogRecord_KeyPath,
                         spKeyPath.Get(),
                         // Options
                         nsadapter::AddCache_LogRecord_Name,
                         pv_util::CreateValue((UINT32)options).Get(),
                         -1);

    }
	[super addObserver:observer forKeyPath:keyPath options:options context:context];
    [self onAddObserver];
}

- (void)removeObserver:(NSObject *)observer forKeyPath:(NSString *)keyPath
{
    if(logger_util::IsAvailable() && logger_util::IsLevelEnabled(
                                     CPMODAdapterLogCategory::GetPMODFoundationCategory(),
                                     LoggingLevel::Debug,
                                     nsadapter::NSFoundation_Category_RemoveObserver,
                                     nullptr))
    {
        InspectablePtr spUniqueId;
        spUniqueId.Attach([PMODInterop fromNSValue:[self toUniqueId]]);
        
        InspectablePtr spKeyPath;
        spKeyPath.Attach([PMODInterop fromNSValue:keyPath]);
        
        logger_util::Log(CPMODAdapterLogger::GetPMODAdapterLogger(),
                         nullptr,
                         LoggingLevel::Debug,
                         nsadapter::NSFoundation_Category_RemoveObserver_Id,
                         CPMODAdapterLogCategory::GetPMODFoundationCategory(),
                         nsadapter::RemoveObserver_LogRecordEnum::TypeId,
                         // UniqueId
                         nsadapter::RemoveObserver_LogRecord_UniqueId,
                         spUniqueId.Get(),
                         // KeyPath
                         nsadapter::RemoveObserver_LogRecord_KeyPath,
                         spKeyPath.Get(),
                         -1);
        
    }
	[super removeObserver:observer forKeyPath:keyPath];
    [self onRemoveObserver];
}

-(id) toUniqueId
{
	NSAssert(NO,@"toUniqueId needs to be override");
    return 0;
}

-(void) attachToFoundation
{
	NSAssert(NO,@"attachToFoundation needs to be override");
}

-(void) detachFromFoundation
{
	NSAssert(NO,@"detachFromFoundation needs to be override");
}

- (void)addObserver:(id<NSObject>)observer {
    if(_observers == nil)
    {
        _observers = (NSMutableSet*)CFSetCreateMutable(NULL, 0, NULL);
    }
    [_observers addObject:observer];
    [self onAddObserver];
}

- (void)removeObserver:(id<NSObject>)observer {
    [_observers removeObject:observer];
    [self onRemoveObserver];
}

- (void)notifyObservers:(NSInvocation*)invocation {
	for (id<NSObject> observer in [_observers allObjects]) {
		if ([observer respondsToSelector:[invocation selector]]) {
			[invocation setTarget:observer];
			[invocation invoke];
		}
	}
}

- (BOOL)containsObserver:(id<NSObject>)observer {
	return [_observers containsObject:observer];
}

-(BOOL) hasObservers {
    return _observers != nil && [_observers count] > 0;
}

+ (NSInvocation *)invocationWithProtocol:(Protocol*)targetProtocol selector:(SEL)selector {
    struct objc_method_description desc;
    BOOL required = YES;
    desc = protocol_getMethodDescription(targetProtocol, selector, required, YES);
    if (desc.name == NULL) {
        required = NO;
        desc = protocol_getMethodDescription(targetProtocol, selector, required, YES);
    }
    if (desc.name == NULL)
        return nil;
    
    NSMethodSignature* sig = [NSMethodSignature signatureWithObjCTypes:desc.types];
    NSInvocation* inv = [NSInvocation invocationWithMethodSignature:sig];
    [inv setSelector:selector];
    return inv;
}

@end
