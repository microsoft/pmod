/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODObjectBase.mm
****/


#import "PMODObjectBase.h"
#import "PMODAdapterFactory.h"
#import "PMODInterop.h"
#import "PMODAdapterUtil.h"

#import "PMODAdapterLogCategory.h"
#import "PMODAdapterLoggerTypeInfo.h"

#import <foundation/interfaces/object.h>
#import <foundation/com_ptr.h>
#import <foundation/library/logger_util.h>
#import <foundation/pv_util.h>

#import <objc/runtime.h>

using namespace foundation;

@interface PMODObjectTypeInfo : NSObject<PMODObjectTypeInfo>
    + (PMODObjectTypeInfo *) createWithSource:(IObjectTypeInfo *)pSource;
@end

@implementation PMODObjectTypeInfo
{
@private
    IObjectTypeInfo				*_pObjectTypeInfo;
}

-(PMODObjectTypeInfo *) initWithSource:(IObjectTypeInfo *)pSource
{
	self = [super init];
	if(self)
	{
        _pObjectTypeInfo = pSource;
        _pObjectTypeInfo->AddRef();
	}
	return self;
    
}

-(void) dealloc {
    RELEASE_INTERFACE(_pObjectTypeInfo);
	[super dealloc];
}

-(PMODGuid *) iidType
{
    IID iidType;
    _pObjectTypeInfo->GetType(&iidType);
    return [PMODGuid guidWithIId:&iidType];
}

-(NSString *) name
{
    HStringPtr hName;
    _pObjectTypeInfo->GetName(hName.GetAddressOf());
    return [PMODInterop toNSString:hName];
}

+(PMODObjectTypeInfo *) createWithSource:(IObjectTypeInfo *)pSource
{
    return [[[PMODObjectTypeInfo alloc] initWithSource:pSource] autorelease];
}

@end


@implementation PMODObjectBase


-(void) dispose
{
    if(self.object == nil)
    {
        // prevent the dispose logic to happen twice
        return;
    }
    
    if(logger_util::IsAvailable() && logger_util::IsLevelEnabled(
                                     CPMODAdapterLogCategory::GetPMODObjectBaseCategory(),
                                     LoggingLevel::Debug,
                                     nsadapter::NSObjectBase_Category_FinalRelease,
                                     nullptr))
    {
        logger_util::Log(CPMODAdapterLogger::GetPMODAdapterLogger(),
                         nullptr,
                         LoggingLevel::Debug,
                         nsadapter::NSObjectBase_Category_FinalRelease_Id,
                         CPMODAdapterLogCategory::GetPMODObjectBaseCategory(),
                         nsadapter::FinalRelease_LogRecordEnum::TypeId,
                         // UniqueId
                         nsadapter::FinalRelease_LogRecord_UniqueId,
                         pv_util::CreateValue([self uniqueId]).Get(),
                         -1);
        
    }
    // remove myself from Model Factory cache
	[self.adapterFactory removeObject:self];
    [super dispose];
}

-(uint32_t) uniqueId
{
    foundation::IObject *pObject = [self object];
    [PMODAdapterUtil throwIfNull:pObject];
    
    UINT32 uniqueId;
    pObject->GetUniqueId(&uniqueId);
    return uniqueId;
}

-(uint32_t) processId
{
    foundation::IObject *pObject = [self object];
    [PMODAdapterUtil throwIfNull:pObject];
    
    UINT32 processId;
    pObject->GetProcessId(&processId);
    return processId;
}

-(foundation::IObject *) object
{
	NSAssert(NO,@"object property needs to be override");
	return nil;
}

-(id) toUniqueId
{
    return [NSNumber numberWithUnsignedInt:[self uniqueId]];
}

-(id<PMODObjectTypeInfo>) objectTypeInfo
{
    foundation::IObject *pObject = [self object];
    [PMODAdapterUtil throwIfNull:pObject];

    ComPtr<IObjectTypeInfo> spObjectTypeInfo;
    pObject->GetTypeInfo(spObjectTypeInfo.GetAddressOf());
    return [PMODObjectTypeInfo createWithSource:spObjectTypeInfo];
}



@end
