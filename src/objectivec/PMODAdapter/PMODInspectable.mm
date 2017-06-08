/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODInspectable.mm
****/

#import "PMODInspectable.h"
#import <foundation/com_interfaces.h>

@implementation PMODInspectable
{
    foundation::IInspectable      *_pInspectable;
}

+ (PMODInspectable *)withInspectable:(IFoundationInspectable *) pInspectable
{
    return [[PMODInspectable alloc] initWithInspectable:pInspectable];
}

-(PMODInspectable *) initWithInspectable:(foundation::IInspectable *) pInspectable;
{
    self = [super init];
    if(self)
    {
        _pInspectable = pInspectable;
        if(_pInspectable != nullptr)
        {
            _pInspectable->AddRef();
        }
    }
    return self;
}

-(void) dealloc {
    if(_pInspectable != nullptr)
    {
        _pInspectable->Release();
        _pInspectable = nullptr;
    }
    
    [super dealloc];
}

-(IFoundationInspectable *) object
{
    return _pInspectable;
}

@end
