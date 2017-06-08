/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODDemoAppFactory.mm
****/

#import "PMODDemoAppFactory.h"

#import "DemoAppTypeInfo.g.h"
#import "DemoAppInterfaces.g.h"

#import "PMODAdapterFactory.h"
#import "PMODDemoApp.g.h"
#import <foundation/com_ptr.h>

#import <foundation/library/logger_util.h>

EXTERN_C HRESULT  STDAPICALLTYPE CreateDemoApp(pmod::IObservableObject **ppObservableObject);


@implementation PMODDemoAppFactoryImpl

+(PMODAdapterFactory *)createDemoAppAdapterFactory
{
#ifdef DEBUG
    if(foundation::logger_util::IsAvailable())
    {
        foundation::logger_util::SetCategoryLevel(foundation::LoggingLevel::Debug);
    }
#endif
    
    foundation::ComPtr<pmod::IObservableObject> spDemoApp;
    CreateDemoApp(spDemoApp.GetAddressOf());
    
    // create the NS Object Factory
    PMODAdapterFactory *adapterFactory = [[PMODAdapterFactory alloc] initWithSource:spDemoApp];
        
    // register Model Factories
    [PMODDemoAppFactory registerAdapterFactories:adapterFactory];
    return adapterFactory;
    
    
}
@end
