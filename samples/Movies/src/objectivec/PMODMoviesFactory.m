/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODMoviesFactory.m
****/

#import "PMODMoviesFactory.h"
#import "PMODAdapterFactory.h"
#import "PMODMovies.g.h"
#import "PMODDispatcherQueueAdapter.h"

extern void _EnsureMoviesApiService();
extern void RegisterMoviesIf();


static id<IMoviesApiService> moviesApiService;

@implementation PMODMoviesFactoryImpl

+(id<IMoviesApiService>) getMoviesApiService
{
    if(moviesApiService == NULL)
    {
        _EnsureMoviesApiService();
        RegisterMoviesIf();
        PMODAdapterFactory *adapterFactory = [[PMODAdapterFactory alloc] init];
        
        // register Model Factories
        [PMODMoviesFactory registerAdapterFactories:adapterFactory];
        id<IMoviesApiServiceFactory> factory = (id<IMoviesApiServiceFactory>)[adapterFactory getActivationFactory:@"pmod.MoviesApiService"];
        
        PMODInspectable *eventDispatcher = [PMODDispatcherQueueAdapter mainQueue];

        moviesApiService = [factory createMoviesApiService:eventDispatcher];
    }
    return moviesApiService;
}

@end
