/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapter.mm
****/


#import "PMODAdapter.h"
#import "PMODObjectBase.h"
#import "PMODAdapterFactory.h"
#import "PMODInterop.h"
#import "PMODDictionary.h"
#import "PMODInspectable.h"

#import <foundation/pv_util.h>
#import <foundation/com_ptr.h>

#import <objc/runtime.h>

using namespace foundation;

@interface PMODDictionary (Private)
-(PMODDictionary *) initWithFoundationDictionary:(IDictionary *)pDictionary withAdapterFactory:(PMODAdapterFactory *) adapterFactory;
-(IInspectable *) object;
@end

@implementation PMODAdapter
{
    PMODAdapterFactory *_adapterFactory;
}

-(PMODAdapterFactory *) adapterFactory
{
	return _adapterFactory;
}

-(void) setAdapterFactory:(PMODAdapterFactory *)adapterFactory
{
	_adapterFactory = adapterFactory;
	[_adapterFactory retain];
}

#if _DEBUG_NSCORE_REFCNT

-(id) retain
{
	NSLog(@"retain->%@ count:%lu",self, (unsigned long)([self retainCount] + 1));
	return [super retain];
}

-(oneway void) release
{
	NSLog(@"release->%@ count:%lu",self, (unsigned long)([self retainCount] -1));
    [super release];
}

#endif

-(void) dealloc {
	[self dispose];
	[super dealloc];
}

-(void) dispose
{
	// Release the Model factory
	[_adapterFactory release];
}

+(IInspectable *) fromNSObject: (id)value
{
	return [PMODInterop fromNSObject:value];
}

-(NSObject *) toNSObject:(IInspectable *)value
{
    return [PMODAdapter toNSObject:value  withAdapterFactory:_adapterFactory];
}

+(NSObject *) toNSObject:(IInspectable *)value withAdapterFactory:(PMODAdapterFactory *) adapterFactory
{
    if(value == nil)
    {
        return [NSNull alloc];
    }
    
    PropertyValuePtr propertyValue;
    if(SUCCEEDED(pv_util::CastToPropertyValue(value,propertyValue.GetAddressOf())))
    {
        return [PMODInterop toNSValue:propertyValue];
    }
    else
    {
        ComPtr<IDictionary> dictionaryPtr;
        ComPtr<IObject> spObject;
        
        foundation::QueryInterface(value,spObject.GetAddressOf());
        if(spObject != nullptr)
        {
            return [adapterFactory getOrCreate:spObject];
        }
        else if(SUCCEEDED(QueryInterface(value,dictionaryPtr.GetAddressOf())))
        {
            PMODDictionary *nsDictionaryAdapter = [[PMODDictionary alloc] initWithFoundationDictionary:dictionaryPtr withAdapterFactory:adapterFactory];
            return nsDictionaryAdapter;
        }
        return nil;
    }
}

@end
