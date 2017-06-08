/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODGuid.mm
****/


#import "PMODGuid.h"
#import "Guidhelper.h"

#import <pal/guiddef.h>

@implementation PMODGuid
{
@private
    GUID	_guid;
}

-(PMODGuid *) initWithString:(NSString *)pString
{
	self = [super init];
	if(self)
	{
        const char *pUtf8 = [pString UTF8String];
        if(!FromString(pUtf8, _guid))
        {
            @throw([NSException exceptionWithName:NSInvalidArgumentException reason:pString userInfo:nil]);
        }
	}
	return self;
}

-(PMODGuid *) initWithIId:(void *)iid
{
	self = [super init];
	if(self)
	{
        _guid = *(reinterpret_cast<GUID *>(iid));
	}
	return self;
}

+ (PMODGuid *)guidWithString:(NSString *)pString
{
    PMODGuid *nsGuid = [[[PMODGuid alloc] init] autorelease];
    [nsGuid initWithString:pString];
    return nsGuid;
}

+ (PMODGuid *)guidWithIId:(void *)iid;
{
    PMODGuid *nsGuid = [[[PMODGuid alloc] init]autorelease];
    [nsGuid initWithIId:iid];
    return nsGuid;
}

-(void*) iid
{
    return &_guid;
}

-(NSString*) iidString
{
    return [NSString stringWithUTF8String:ToString(_guid).c_str()];
}


@end
