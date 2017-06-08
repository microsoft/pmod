/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterFactory.mm
****/


#import "PMODAdapterFactory.h"
#import "PMODObjectBase.h"
#import "PMODObservableObject.h"
#import "PMODCommand.h"
#import "PMODCollection.h"
#import "PMODList.h"
#import "PMODAsyncOperation.h"
#import "PMODInterop.h"
#import "PMODAdapterLoggerTypeInfo.h"

#import "PMODAdapterLogCategory.h"

#import "Guidhelper.h"
#import <foundation/hstring_wrapper.h>
#import <foundation/library/logger_util.h>
#import <foundation/library/object_factory_util.h>
#import <pmod/interfaces/command.h>
#import <pmod/interfaces/observable_object.h>
#import <pmod/interfaces/observable_collection.h>


using namespace foundation;
using namespace pmod;

extern HRESULT  STDAPICALLTYPE register_nsadapter_logger_typeinfo(void);


static ILogCategory *GetPMODAdapterFactoryCategory()
{
    return CPMODAdapterLogCategory::GetPMODAdapterFactoryCategory();
}


@interface PMODCommand (Private)
-(PMODCommand *) initWithCommand: (ICommand *)pCommand;
@end

@interface PMODAsyncOperation (Private)
-(PMODAsyncOperation *) initWithAsyncOperation:(foundation::IAsyncOperation *) pAsyncOperation;
@end

@interface PMODObservableObject (Private)
    -(PMODObservableObject *) initWithObservableObject: (IObservableObject *)pObservableObject;
@end

@interface PMODCollection (Private)
-(PMODCollection *) initWithObservableCollection:(IObservableCollection *) pObservableCollection;
@end

@interface PMODList (Private)
-(PMODList *) initWithObservableList:(IObservableList *) pObservableList;
@end

@implementation PMODAdapterFactory
{
@private
    pmod::IObservableObject	*_pSource;
    NSMutableDictionary					*_modelFactoryCache;
    NSMutableDictionary					*_modelFactoryClass;
}

-(void) initializeInternal
{
    _modelFactoryCache = [[NSMutableDictionary alloc] init];
    _modelFactoryClass = [[NSMutableDictionary alloc] init];
}
-(PMODAdapterFactory *) init
{
	self = [super init];
	if(self)
	{
        [self initializeInternal];
	}
	return self;
}

-(PMODAdapterFactory *) initWithSource:(pmod::IObservableObject *)pSource;
{
	self = [super init];
	if(self) 
	{
        [self initializeInternal];
		[self setSource:pSource];
	}
	return self;
}

-(pmod::IObservableObject	*) source
{
	return _pSource;
}

-(void) setSource:(pmod::IObservableObject *)pSource
{
	_pSource = pSource;
	_pSource->AddRef();
}

-(PMODObject*) sourceModel
{
	return static_cast<PMODObject *>([self getOrCreate:_pSource]);
}

-(void) dispose {
    // dispose remaining cached objects
    NSArray *cachedInstances = _modelFactoryCache.allValues;
    for (id objectCache in cachedInstances) {
        
        PMODObjectBase *nsObjectBase = (PMODObjectBase *)[objectCache nonretainedObjectValue];
        [nsObjectBase dispose];
    }
    //ReleaseInterface(_pViewModelFactory);
    RELEASE_INTERFACE(_pSource);
}

-(void) dealloc {
    [self dispose];
	[_modelFactoryCache release];
	[_modelFactoryClass release];
	[super dealloc];
}

+(id) toObjectKey:(foundation::IObject *)pObject
{
	UINT32 uniqueId;
	pObject->GetUniqueId(&uniqueId);
	return [NSNumber numberWithUnsignedLong:uniqueId];
}

-(void) addObject:(PMODObjectBase *)objectBase
{
	[objectBase setAdapterFactory:self];
	id key = [PMODAdapterFactory toObjectKey:[objectBase object]];
	// add to our cache dictionary	
	[_modelFactoryCache setObject:[NSValue valueWithNonretainedObject:objectBase] forKey:key];
	    
    if(logger_util::IsAvailable() && logger_util::IsLevelEnabled(
            GetPMODAdapterFactoryCategory(),
            LoggingLevel::Info,
            nsadapter::NSFactory_Category_AddCache,
            nullptr))
    {
        UINT32 uniqueId;
        [objectBase object]->GetUniqueId(&uniqueId);
        ComPtr<foundation::IObjectTypeInfo>  spTypeInfo;
        [objectBase object]->GetTypeInfo(spTypeInfo.GetAddressOf());
	
        IID iidType;
        spTypeInfo->GetType(&iidType);
        foundation::HStringPtr name;
        spTypeInfo->GetName(name.GetAddressOf());
        
        logger_util::Log(CPMODAdapterLogger::GetPMODAdapterLogger(),
                            nullptr,
                            LoggingLevel::Info,
                            nsadapter::NSFactory_Category_AddCache_Id,
                            GetPMODAdapterFactoryCategory(),
                            nsadapter::AddCache_LogRecordEnum::TypeId,
                         // UniqueId
                            nsadapter::AddCache_LogRecord_UniqueId,
                            pv_util::CreateValue((UINT32)uniqueId).Get(),
                         // TypeIId
                            nsadapter::AddCache_LogRecord_TypeIId,
                            pv_util::CreateValue(iidType).Get(),
                         // Name
                            nsadapter::AddCache_LogRecord_Name,
                            pv_util::CreateValue(name).Get(),
                         -1);
    }
}

-(void) removeObject:(PMODObjectBase *)objectBase
{	
	id key = [PMODAdapterFactory toObjectKey:[objectBase object]];
    if(logger_util::IsAvailable() && logger_util::IsLevelEnabled(
             GetPMODAdapterFactoryCategory(),
             LoggingLevel::Info,
             nsadapter::NSFactory_Category_RemoveCache,
             nullptr))
    {
        logger_util::Log(CPMODAdapterLogger::GetPMODAdapterLogger(),
                         nullptr,
                         LoggingLevel::Info,
                         nsadapter::NSFactory_Category_RemoveCache_Id,
                         GetPMODAdapterFactoryCategory(),
                         nsadapter::RemoveCache_LogRecordEnum::TypeId,
                         // UniqueId
                         nsadapter::RemoveCache_LogRecord_UniqueId,
                         pv_util::CreateValue([(NSNumber *)key intValue]).Get(),
                         -1);
    }
	[_modelFactoryCache removeObjectForKey:key];
}

-(PMODObjectBase *) lookupById:(unsigned int) uniqueId
{
	NSValue *value = [_modelFactoryCache objectForKey:[NSNumber numberWithUnsignedInt:uniqueId]];
	if(value != nil)
	{
		return [value nonretainedObjectValue];
	}
	return nil;
}

-(PMODObjectBase *) lookupByObject:(foundation::IObject *) pObject
{
	UINT32 uniqueId;
	pObject->GetUniqueId(&uniqueId);
	return [self lookupById:uniqueId];
}

-(PMODObjectBase *) getActivationFactory:(NSString* )activationId
{
    IUnknown *pFactory = GetActivationFactoryInstance([activationId UTF8String]);
    if(pFactory == nullptr)
    {
        return NULL;
    }
    IObject *pFactoryObject;
    ::QueryInterface(pFactory,&pFactoryObject);
    foundation_assert(pFactoryObject != nullptr);
    return [self createObject:pFactoryObject];
}

-(PMODObjectBase *) activateObject:(NSString* )typeId withContext:(id)context
{
    IID iidType;
    FromString([typeId UTF8String], iidType);
    IObject *pActivatedObject = nullptr;
    InspectablePtr contextPtr;
    contextPtr.Attach([PMODAdapter fromNSObject:context]);

    [PMODInterop throwIfFailed:library::ActivateObject(iidType,contextPtr,&pActivatedObject)];

    return [self createObject:pActivatedObject];
}

-(PMODObjectBase *) createObject:(foundation::IObject *) pObject
{
    PMODObjectBase *nsObjectBase = nil;
    
    ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
    pObject->GetTypeInfo(spTypeInfo.GetAddressOf());
    IID iidType;
    spTypeInfo->GetType(&iidType);
    
    if(!IsEqualIID(iidType,foundation::GUID_NULL))
    {
        Class modelClass = [_modelFactoryClass objectForKey:[PMODInterop toIIDString:iidType]];
        if(modelClass != nil)
        {
            nsObjectBase = [modelClass alloc];
        }
    }
    
    ComPtr<ICommand> spCommand;
    ComPtr<IObservableCollection> spObservableCollection;
    ComPtr<IObservableList> spObservableList;
    ComPtr<foundation::IAsyncOperation> spAsyncOperation;
    
   if(SUCCEEDED(foundation::QueryInterface(pObject,spCommand.GetAddressOf())))
    {
        if(nsObjectBase == nil)
        {
            nsObjectBase = [PMODCommand alloc];
        }
        [(PMODCommand *)nsObjectBase initWithCommand:spCommand];
    }
   else if(SUCCEEDED(foundation::QueryInterface(pObject,spObservableList.GetAddressOf())))
    {
        if(nsObjectBase == nil)
        {
            nsObjectBase = [PMODList alloc];
        }
        [(PMODList *)nsObjectBase initWithObservableList:spObservableList];
    }
    else if(SUCCEEDED(foundation::QueryInterface(pObject,spObservableCollection.GetAddressOf())))
    {
        if(nsObjectBase == nil)
        {
            nsObjectBase = [PMODCollection alloc];
        }
        [(PMODCollection *)nsObjectBase initWithObservableCollection:spObservableCollection];
    }
    else if(SUCCEEDED(foundation::QueryInterface(pObject,spAsyncOperation.GetAddressOf())))
    {
        if(nsObjectBase == nil)
        {
            nsObjectBase = [PMODAsyncOperation alloc];
        }
        [(PMODAsyncOperation *)nsObjectBase initWithAsyncOperation:spAsyncOperation];
    }
    else
    {
        ComPtr<IObservableObject> spPropertyModel;
        foundation::QueryInterface(pObject,spPropertyModel.GetAddressOf());
        if(nsObjectBase == nil)
        {
            nsObjectBase = [PMODObservableObject alloc];
        }
        [(PMODObservableObject *)nsObjectBase initWithObservableObject:spPropertyModel];
    }
    // autorelease
    [nsObjectBase autorelease];
    // add to my cache
    [self addObject:nsObjectBase];
    // return created NS Model. Note: retain count will not be modified
    return nsObjectBase;
}

-(PMODObjectBase *) getOrCreate:(foundation::IObject *) pObject
{
	PMODObjectBase *nsObjectBase = [self lookupByObject:pObject];
	if(nsObjectBase == nil)
	{
        nsObjectBase = [self createObject:pObject];
	}
	return nsObjectBase;
}

-(void) registerAdapterFactoryClass:(NSString *)iidType adapterClass:(Class)adapterClass
{
    if(logger_util::IsAvailable() && logger_util::IsLevelEnabled(
                                     GetPMODAdapterFactoryCategory(),
                                     LoggingLevel::Debug,
                                     nsadapter::NSFactory_Category_RegisterAdapterClass,
                                     nullptr))
    {
        foundation::HStringPtr hClassName;
        *(&hClassName) = [PMODInterop fromNSString:NSStringFromClass(adapterClass)];
        foundation::HStringPtr hIIdType;
        *(&hIIdType) = [PMODInterop fromNSString:iidType];
       
        logger_util::Log(CPMODAdapterLogger::GetPMODAdapterLogger(),
                         nullptr,
                         LoggingLevel::Debug,
                         nsadapter::NSFactory_Category_RegisterAdapterClass_Id,
                         GetPMODAdapterFactoryCategory(),
                         nsadapter::RegisterAdapterClass_LogRecordEnum::TypeId,
                         // Class Name
                         nsadapter::RegisterAdapterClass_LogRecord_ClassName,
                         pv_util::CreateValue(hClassName).Get(),
                         // TypeIId
                         nsadapter::RegisterAdapterClass_LogRecord_TypeId,
                         pv_util::CreateValue(hIIdType).Get(),
                         -1);
    }
	[_modelFactoryClass setObject:adapterClass forKey:iidType];
}

+(IFoundationLogger *) logger
{
    return CPMODAdapterLogger::GetPMODAdapterLogger();
}

+(PMODAdapterFactory *) createWithSource:(IPmodObservableObject *)pSource
{
    return[[PMODAdapterFactory alloc] initWithSource:pSource];
}


@end
