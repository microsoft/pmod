/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODKeyPathController.mm
****/

#import "PMODKeyPathController.h"


@implementation PMODKeyPathController
{
	id			_source;
	NSString	*_forKeyPath;
	id			_target;
	NSString	*_targetForKey;
	NSValueTransformer* _transformer;
}

-(PMODKeyPathController *) init:(id)source forKeyPath:(NSString *)keyPath target:(id)target targetForKey:(NSString *)targetForKey valueTransformer:(NSValueTransformer*)transformer
{
	self = [super init];
	if(self) 
	{
		_source = source;
		_forKeyPath = keyPath;
		_target = target;
		_targetForKey = targetForKey;
		_transformer = transformer;
		// add Observer
		[_source addObserver:self forKeyPath:keyPath options:NSKeyValueObservingOptionNew context:nil];
        
        // initialize existing value
        [self refreshSourceValue];

	}
	return self;
}

-(void) dealloc {
	
	[_source removeObserver:self forKeyPath:_forKeyPath];
}

-(id) source
{
    return _source;
}

-(void) setSource:(id) source
{
    if(_source != nil)
    {
        [_source removeObserver:self forKeyPath:_forKeyPath];
    }
    _source = source;
    [_source addObserver:self forKeyPath:_forKeyPath options:NSKeyValueObservingOptionNew context:nil];
    
    [self refreshSourceValue];
}

-(id) target
{
    return _target;
}

-(void) setTarget:(id) target
{
    _target = target;    
    [self refreshSourceValue];
}

-(void) refreshSourceValue
{
    if(_source != nil && _target != nil)
    {
        id value = [_source valueForKey:_forKeyPath];
        [_target setValue:[self valueFromTransformer:value] forKey:_targetForKey];
    }
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	id newValue = [change objectForKey:NSKeyValueChangeNewKey];
	[_target setValue:[self valueFromTransformer:newValue] forKey:_targetForKey];
}

-(id)valueFromTransformer:(id)value
{
	id ret = value;
	
	if (_transformer != nil)
	{
		ret = [_transformer transformedValue:value];
	}
	
	return ret;
}

@end
