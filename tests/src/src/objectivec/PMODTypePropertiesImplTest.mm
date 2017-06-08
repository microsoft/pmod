/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODTypePropertiesImplTest.mm
****/
#import  "PMODTypePropertiesTest.h"
#import  "PMODAdapterFactory.h"

#import "TestModelTypeInfo.g.h"
#import "TestModelInterfaces.g.h"
#import "TestModelImplInternal.g.h"
#import <pmod/library/observable_object_impl.h>

using namespace foundation;
using namespace pmod;
using namespace pmod::library;

extern HRESULT RegisterTypesIf();

@interface PMODTypePropertiesImplTest : PMODTypePropertiesTest

@end

@implementation PMODTypePropertiesImplTest


- (void)testTypeProperties {
    RegisterTypesIf();
    
    ComPtr<ObservableObjectImpl> typePropertiesImplPtr;
    CreateObservableObjectImpl(typePropertiesImplPtr.GetAddressOf(), TestModel::TypeProperties::IIDType);
 
    PMODAdapterFactory *factory = [[PMODAdapterFactory alloc] init];
    [PMODTestModelFactory registerAdapterFactories:factory];
    
    id<ITypeProperties> typeProperties = (id<ITypeProperties>)[factory createObject:typePropertiesImplPtr.Get()];
    
    [self testTypeProperties:typeProperties];

}

@end
