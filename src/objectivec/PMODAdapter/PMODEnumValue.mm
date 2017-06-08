/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODEnumValue.mm
****/

#import "PMODEnumValue.h"

#import <foundation/com_ptr.h>
#import <foundation/type_info_statics_util.h>

using namespace foundation;

@implementation PMODEnumValue
{
    UINT32 _enumValue;
    UINT32 _enumTypedId;
}
-(int) enumTypeId
{
    return _enumTypedId;
}
- (int)intValue
{
    return (int)_enumValue;
}
- (unsigned int)unsignedIntValue
{
    return _enumValue;
}
-(PMODEnumValue *) initWithValues:(UINT32)enumTypeId enumValue:(UINT32)enumValue
{
    self = [super init];
    if(self)
    {
        _enumValue = enumValue;
        _enumTypedId = enumTypeId;
    }
    return self;
}

+(PMODEnumValue *) createNSEnumValue:(foundation::IEnumValue *) pEnumValue
{
    UINT32 enumValue;
    pEnumValue->GetValue(&enumValue);
    
    ComPtr<foundation::IEnumTypeInfo> spEnumTypeInfo;
    foundation::GetObjectTypeInfo(pEnumValue,spEnumTypeInfo.GetAddressOf());
    UINT32 typeId;
    spEnumTypeInfo->GetTypeId(&typeId);
    
    PMODEnumValue *nsEnumValue = [[[PMODEnumValue alloc] initWithValues:typeId enumValue:enumValue] autorelease];
    return nsEnumValue;
}

-(const char *) objCType
{
    return "I";
}

-(id) copyWithZone: (NSZone *) zone
{
    PMODEnumValue *nsEnumValueCopy = [[PMODEnumValue allocWithZone: zone] initWithValues:_enumTypedId enumValue:_enumValue];
    return nsEnumValueCopy;
}
@end
