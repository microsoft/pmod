/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODTypePropertiesLibTest.mm
****/
#import  "PMODTypePropertiesTest.h"
#import  "PMODAdapterFactory.h"

#import "TestModelTypeInfo.g.h"
#import "TestModelInterfaces.g.h"
#import <pmod/library/factory_util.h>

using namespace foundation;
using namespace pmod;
using namespace pmod::library;

extern HRESULT RegisterTypesIf();

@interface PMODTypePropertiesLibTest : PMODTypePropertiesTest

@end

@implementation PMODTypePropertiesLibTest


- (void)testTypeProperties {
    RegisterTypesIf();
    
    ComPtr<IObservableObject> typePropertiesPtr;
    CreateObservableObject(
                TestModel::TypeProperties::IIDType,
                ObservableObjectOptions::InitializeProperties,
                typePropertiesPtr.GetAddressOf());
 
    PMODAdapterFactory *factory = [[PMODAdapterFactory alloc] init];
    [PMODTestModelFactory registerAdapterFactories:factory];
    
    id<ITypeProperties> typeProperties = (id<ITypeProperties>)[factory createObject:typePropertiesPtr.Get()];
    
    [self testTypeProperties:typeProperties];

}

@end
