/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODObjectNode.mm
****/


#import "PMODObjectNode.h"
#import "PMODAdapterFactory.h"

#import <foundation/node_statics_util.h>
#import <foundation/com_ptr.h>

using namespace pmod;
using namespace foundation;

@implementation PMODObjectNode


-(id<PMODObjectNode>)  getParent
{
    ComPtr<IObject> spParent;
    GetObjectParent(reinterpret_cast<IObject *>([self object]),spParent.GetAddressOf());
    
    if(spParent != nullptr)
    {
        return (id<PMODObjectNode>)[self.adapterFactory getOrCreate:spParent];
    }
    return nil;
}


@end
