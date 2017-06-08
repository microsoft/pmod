/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterFactory.h
****/

#import <Foundation/Foundation.h>
#import "PMODInterfaces.h"

@class PMODObjectBase;
@class PMODObject;
@class PMODCommand;
@class PMODCollection;
@class PMODAsyncOperation;

@protocol PMODObjectBase;

__attribute__((visibility("default")))
@interface PMODAdapterFactory : NSObject {

}
-(PMODAdapterFactory *) init;
-(PMODAdapterFactory *) initWithSource:(IPmodObservableObject *)pSource;
-(void)dispose;

-(PMODObject*) sourceModel;

-(IPmodObservableObject *) source;
-(void) setSource:(IPmodObservableObject *)pSource;

-(void) addObject:(PMODObjectBase *)objectBase;
-(void) removeObject:(PMODObjectBase *)objectBase;

// factory & activation
-(PMODObjectBase *) getActivationFactory:(NSString* )activationId;
-(PMODObjectBase *) activateObject:(NSString* )typeId withContext:(id)context;

// lookup
-(PMODObjectBase *) lookupById:(unsigned int) uniqueId;
-(PMODObjectBase *) lookupByObject:(IFoundationObject *) pObject;

-(PMODObjectBase *) createObject:(IFoundationObject *) pObject;
-(PMODObjectBase *) getOrCreate:(IFoundationObject *) pObject;

-(void) registerAdapterFactoryClass:(NSString *)iidType adapterClass:(Class)adapterClass;

+(PMODAdapterFactory *) createWithSource:(IPmodObservableObject *)pSource;

+(IFoundationLogger *) logger;
@end
