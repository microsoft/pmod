/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODDictionary.mm
****/


#import "PMODDictionary.h"
#import "PMODAdapterFactory.h"
#import "PMODInterop.h"
#import "PMODAdapter.h"

#import <foundation/interfaces/dictionary.h>
#import <foundation/macros.h>
#import <foundation/hstring_wrapper.h>
#import <foundation/library/dictionary_class.h>
#import <foundation/com_ptr.h>

using namespace foundation;

@interface NSKeyEnumeratorAdapter : NSEnumerator

@end

@implementation NSKeyEnumeratorAdapter
{
    IIterator_impl<HSTRING> *_pFirst;
}

-(NSKeyEnumeratorAdapter *) init:(IIterator_impl<HSTRING> *) iterator
{
    self = [super init];
    if(self)
    {
        _pFirst = iterator;
        _pFirst->AddRef();
    }
    return self;
}

-(void) dealloc {
    RELEASE_INTERFACE(_pFirst);
    [super dealloc];
}

- (id)nextObject
{
    bool hasCurrent;
    _pFirst->GetHasCurrent(&hasCurrent);
    if(!hasCurrent)
    {
        return nil;
    }
    HStringPtr hKey;
    [PMODInterop throwIfFailed:_pFirst->GetCurrent(hKey.GetAddressOf())];
    
    _pFirst->MoveNext(&hasCurrent);
    NSString *nsKey = [PMODInterop toNSString:hKey];
    return nsKey;
}

@end

@implementation PMODDictionary
{
    PMODAdapterFactory *_adapterFactory;
    IDictionary      *_pDictionary;
}

-(PMODDictionary *) init
{
    self = [super init];
    if(self)
    {
        foundation::library::CreateDictionaryClass(&_pDictionary);
    }
    return self;
}
-(PMODDictionary *) initWithAdapterFactory:(PMODAdapterFactory *) adapterFactory;{
    self = [super init];
    if(self)
    {
        library::CreateDictionaryClass(&_pDictionary);
        _adapterFactory = adapterFactory;
        [_adapterFactory retain];
        
    }
    return self;
}

- (instancetype)initWithObjects:(const id [])objects forKeys:(const id <NSCopying> [])keys count:(NSUInteger)cnt
{
    NSAssert(false, @"not supported");
    return nil;
}

-(PMODDictionary *) initWithFoundationDictionary:(IDictionary *)pDictionary withAdapterFactory:(PMODAdapterFactory *) adapterFactory
{
	self = [super init];
	if(self) 
	{
        _pDictionary = pDictionary;
        _pDictionary->AddRef();
        
        _adapterFactory = adapterFactory;
        [_adapterFactory retain];

	}
	return self;
}

-(void) dealloc {
    RELEASE_INTERFACE(_pDictionary);
    // Release the Model factory
    [_adapterFactory release];
    
    [super dealloc];
}

-(IFoundationInspectable *) object
{
    return _pDictionary;
}

-(NSUInteger) count
{
    UINT32 size;
    [PMODInterop throwIfFailed:_pDictionary->GetSize(&size)];
    return size;
}

- (id)objectForKey:(id)aKey
{
    //NSLog(@"objectForKey:key %@",aKey);

    foundation::HStringPtr hKey;
    *(hKey.GetAddressOf())= [PMODInterop fromNSString:[aKey description]];
    InspectablePtr value;
    [PMODInterop throwIfFailed:_pDictionary->Lookup(hKey, value.GetAddressOf())];
    return [PMODAdapter toNSObject:value withAdapterFactory:_adapterFactory];
}

- (void)removeObjectForKey:(id)aKey
{
    foundation::HStringPtr hKey;
    *(hKey.GetAddressOf())= [PMODInterop fromNSString:[aKey description]];
    [PMODInterop throwIfFailed:_pDictionary->Remove(hKey)];
}

- (void)setObject:(id)anObject forKey:(id)aKey
{
    foundation::HStringPtr hKey;
    *(hKey.GetAddressOf())= [PMODInterop fromNSString:[aKey description]];
    InspectablePtr spValue;
    spValue.Attach([PMODAdapter fromNSObject:anObject]);
    bool replaced;
    [PMODInterop throwIfFailed:_pDictionary->Insert(hKey,spValue, &replaced)];
}

- (NSEnumerator *)keyEnumerator
{
    ComPtr<IIterator_impl<HSTRING>> first;
    [PMODInterop throwIfFailed:_pDictionary->Keys(first.GetAddressOf())];
    
    return [[[NSKeyEnumeratorAdapter alloc] init:first] autorelease];
}



@end
