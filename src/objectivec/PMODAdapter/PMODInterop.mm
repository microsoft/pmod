/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODInterop.mm
****/

#import <string>

#import "PMODInterop.h"
#import "PMODEnumValue.h"
#import "PMODGuid.h"
#import "PMODObjectBase.h"
#import "PMODDictionary.h"
#import "PMODInspectable.h"

#import "guidhelper.h"
#import <foundation/pv_util.h>
#import <foundation/type_info_statics_util.h>

using namespace pmod;
using namespace foundation;

template<typename TArrayWrapper>
static NSArray *CreateNSArray(
IPropertyValue *pPropertyValue,
HRESULT  (STDMETHODCALLTYPE foundation::IPropertyValue::*pGetArray)(UINT32 *,typename TArrayWrapper::_Item_Type **),
id (*pToNSValue)(typename TArrayWrapper::_Item_Type item)
)
{
    TArrayWrapper arrayWrapper;
    (pPropertyValue->*pGetArray)(arrayWrapper.GetSizeAddressOf(),arrayWrapper.GetBufferAddressOf());
    NSMutableArray *nsArray = nil;
    if(arrayWrapper.GetSize() != 0)
    {
        nsArray = [[[NSMutableArray alloc] initWithCapacity:arrayWrapper.GetSize()] autorelease];
        for(UINT32 index = 0;index < arrayWrapper.GetSize();++index)
        {
            [nsArray addObject:(*pToNSValue)(arrayWrapper[index])];
        }
    }
    return nsArray;
}

@implementation PMODException
{
@private
    HRESULT _hr;
}

-(PMODException *) initWithResult: (HRESULT) hr
{
	self = [super initWithName:@"NSComException" reason:@"hresult error" userInfo:nil];
	if(self)
	{
		_hr = hr;
	}
	return self;
}

-(HRESULT) error
{
    return _hr;
}
@end

@implementation PMODInterop

+(void)throwIfFailed:(HRESULT) hr
{
	if(FAILED(hr))
	{
		@throw([[PMODException alloc] initWithResult:hr]);
	}
}

+(NSString *) toNSString:(HSTRING) hstring
{
    return [NSString stringWithUTF8String:_pal_GetStringRawBuffer(hstring,nullptr)];
}

+(HSTRING) fromNSString:(NSString *) nsString
{
	HSTRING hString;
	[PMODInterop throwIfFailed:_pal_CreateString([nsString UTF8String],(unsigned int)[nsString length],&hString)];
	
	return hString;
}

+(NSDate *) toNSDate:(foundation::IPropertyValue *) pPropertyValue
{
    foundation::DateTime dateTime;
    [PMODInterop throwIfFailed:pPropertyValue->GetDateTime(&dateTime)];
    NSTimeInterval *pTimeInterval = reinterpret_cast<NSTimeInterval *>(&dateTime.UniversalTime);
    return [NSDate dateWithTimeIntervalSinceReferenceDate:*pTimeInterval];
}

+(IFoundationInspectable *) fromNSDate:(NSDate *) nsDate
{
    foundation::DateTime dateTime;
    NSTimeInterval *pTimeInterval = reinterpret_cast<NSTimeInterval *>(&dateTime.UniversalTime);
    *pTimeInterval = nsDate.timeIntervalSinceReferenceDate;
    return pv_util::CreateValue(dateTime).Detach();
}

+(NSData *) toNSData:(foundation::IPropertyValue *) pPropertyValue
{
    foundation::UInt8ArrayWrapper items;
    [PMODInterop throwIfFailed:(pPropertyValue->GetUInt8Array(items.GetSizeAddressOf(), items.GetBufferAddressOf()))];
    return [NSData dataWithBytes:items.GetBuffer() length:items.GetSize()];
}

+(IFoundationInspectable *) fromNSData:(NSData *) nsData
{
    return pv_util::CreateValue(
                                                        (UINT32)nsData.length,
                                                        (BYTE *)nsData.bytes).Detach();
}

+(NSArray *) toNSArray:(foundation::IPropertyValue *) pPropertyValue
{
	foundation::PropertyType propertyType;
	pPropertyValue->get_Type(&propertyType);
    if((propertyType & 0x400) == 0)
    {
        [PMODInterop throwIfFailed:foundation::Errors::E_PV_E_WRONG_TYPE];
    }
    foundation::PropertyType itemArrayType = (foundation::PropertyType)(propertyType & 0xff);
    switch (itemArrayType) {
        case PropertyType_Boolean:
        {
            return CreateNSArray<foundation::BooleanArrayWrapper>(
                                                             pPropertyValue,
                                                             &IPropertyValue::GetBooleanArray,
                                                             [](bool item) -> id
                                                             {
                                                                 return [NSNumber numberWithBool:item ? YES:NO];
                                                             });
        }
        case PropertyType_String:
        {
            return CreateNSArray<foundation::HStringArrayWrapper>(
                                                                    pPropertyValue,
                                                                    &IPropertyValue::GetStringArray,
                                                                    [](HSTRING item) -> id
                                                                    {
                                                                        return [PMODInterop toNSString: item];
                                                                    });
        }
        case PropertyType_Int16:
        {
            return CreateNSArray<foundation::Int16ArrayWrapper>(
                                                                    pPropertyValue,
                                                                    &IPropertyValue::GetInt16Array,
                                                                    [](INT16 item) -> id
                                                                    {
                                                                        return [NSNumber numberWithShort: item];
                                                                    });
        }
        case PropertyType_UInt16:
        {
            return CreateNSArray<foundation::UInt16ArrayWrapper>(
                                                                  pPropertyValue,
                                                                  &IPropertyValue::GetUInt16Array,
                                                                  [](UINT16 item) -> id
                                                                  {
                                                                      return [NSNumber numberWithUnsignedShort: item];
                                                                  });
        }
        case PropertyType_Int32:
        {
            return CreateNSArray<foundation::Int32ArrayWrapper>(
                                                                  pPropertyValue,
                                                                  &IPropertyValue::GetInt32Array,
                                                                  [](INT32 item) -> id
                                                                  {
                                                                      return [NSNumber numberWithInt: item];
                                                                  });
        }
        case PropertyType_UInt32:
        {
            return CreateNSArray<foundation::UInt32ArrayWrapper>(
                                                                   pPropertyValue,
                                                                   &IPropertyValue::GetUInt32Array,
                                                                   [](UINT32 item) -> id
                                                                   {
                                                                       return [NSNumber numberWithUnsignedInt: item];
                                                                   });
        }
        case PropertyType_Int64:
        {
            return CreateNSArray<foundation::Int64ArrayWrapper>(
                                                                  pPropertyValue,
                                                                  &IPropertyValue::GetInt64Array,
                                                                  [](INT64 item) -> id
                                                                  {
                                                                      return [NSNumber numberWithLongLong: item];
                                                                  });
        }
        case PropertyType_UInt64:
        {
            return CreateNSArray<foundation::UInt64ArrayWrapper>(
                                                                   pPropertyValue,
                                                                   &IPropertyValue::GetUInt64Array,
                                                                   [](UINT64 item) -> id
                                                                   {
                                                                       return [NSNumber numberWithUnsignedLongLong: item];
                                                                   });
        }
        case PropertyType_Single:
        {
            return CreateNSArray<foundation::SingleArrayWrapper>(
                                                                   pPropertyValue,
                                                                   &IPropertyValue::GetSingleArray,
                                                                   [](FLOAT item) -> id
                                                                   {
                                                                       return [NSNumber numberWithFloat: item];
                                                                   });
        }
        case PropertyType_Double:
        {
            return CreateNSArray<foundation::DoubleArrayWrapper>(
                                                                  pPropertyValue,
                                                                  &IPropertyValue::GetDoubleArray,
                                                                  [](DOUBLE item) -> id
                                                                  {
                                                                      return [NSNumber numberWithDouble: item];
                                                                  });
        }
        default:
            NSAssert(NO,@"unsupported type");
            break;
    }
    return nil;
}

+(IFoundationInspectable *) fromNSArray:(NSArray *) nsArray
{
    // TODO: we always convert back an Inspectable array, maybe a type safe array is better
    std::vector<InspectablePtr> items;
    for(NSInteger index = 0;index < nsArray.count;++index)
    {
        items.push_back(InspectablePtr::AttachPtr([PMODInterop fromNSValue:[nsArray objectAtIndex:index]]));
    }
    foundation::IInspectable *pArrayValue = nullptr;
    pv_util::CreateInspectableArrayValue(
        (UINT32)items.size(),
        items.size() ? items.front().GetAddressOfPtr() : nullptr,
        &pArrayValue);
                                                                  
    return pArrayValue;
}

+(id) toNSValue:(IPropertyValue *) pPropertyValue
{
	foundation::PropertyType propertyType;
	pPropertyValue->get_Type(&propertyType);
	
	NSObject *pNSValue = nil;
    
    if((propertyType & 0x400) != 0)
    {
        foundation::PropertyType itemArrayType = (foundation::PropertyType)(propertyType & 0xff);
        if(itemArrayType == PropertyType_UInt8)
        {
            return [PMODInterop toNSData:pPropertyValue];
        }
        return [PMODInterop toNSArray:pPropertyValue];
    }
    else
    {
        switch(propertyType)
        {
            case PropertyType_Boolean:
            {
                boolean boolValue;
                pPropertyValue->GetBoolean(&boolValue);
                pNSValue = [NSNumber numberWithBool:boolValue ? YES:NO];
            }
            break;
            case PropertyType_UInt8:
            {
                BYTE uint8Value;
                pPropertyValue->GetUInt8(&uint8Value);
                pNSValue = [NSNumber numberWithUnsignedChar:uint8Value];
            }
                break;
            case PropertyType_Int16:
            {
                INT16 int16Value;
                pPropertyValue->GetInt16(&int16Value);
                pNSValue = [NSNumber numberWithShort:int16Value];
            }
                break;
            case PropertyType_UInt16:
            {
                UINT16 uint16Value;
                pPropertyValue->GetUInt16(&uint16Value);
                pNSValue = [NSNumber numberWithUnsignedShort:uint16Value];
            }
                break;
            case PropertyType_Int32:
            {
                INT32 int32Value;
                pPropertyValue->GetInt32(&int32Value);
                pNSValue = [NSNumber numberWithInt:int32Value];
            }
                break;
            case PropertyType_UInt32:
            {
                UINT32 uint32Value;
                pPropertyValue->GetUInt32(&uint32Value);
                ComPtr<IEnumValue> spEnumValue;
                if(SUCCEEDED(foundation::QueryInterface(pPropertyValue, spEnumValue.GetAddressOf())))
                {
                    pNSValue = [PMODEnumValue createNSEnumValue:spEnumValue];
                }
                else
                {
                    pNSValue = [NSNumber numberWithUnsignedInt:uint32Value];
                }
            }
                break;
            case PropertyType_Int64:
            {
                INT64 int64Value;
                pPropertyValue->GetInt64(&int64Value);
                pNSValue = [NSNumber numberWithLongLong:int64Value];
            }
                break;
            case PropertyType_UInt64:
            {
                UINT64 uint64Value;
                pPropertyValue->GetUInt64(&uint64Value);
                pNSValue = [NSNumber numberWithUnsignedLongLong:uint64Value];
            }
                break;
            case PropertyType_Single:
            {
                FLOAT floatValue;
                pPropertyValue->GetSingle(&floatValue);
                pNSValue = [NSNumber numberWithFloat:floatValue];
            }
                break;
            case PropertyType_Double:
            {
                DOUBLE doubleValue;
                pPropertyValue->GetDouble(&doubleValue);
                pNSValue = [NSNumber numberWithDouble:doubleValue];
            }
                break;
            case PropertyType_String:
            {
                foundation::HStringPtr hstringValue;
                pPropertyValue->GetString(hstringValue.GetAddressOf());
                pNSValue = [PMODInterop toNSString:(HSTRING)hstringValue];
            }
                break;
            case PropertyType_DateTime:
            {
                pNSValue = [PMODInterop toNSDate:pPropertyValue];
            }
                break;
            case PropertyType_Guid:
            {
                GUID iid;
                pPropertyValue->GetGuid(&iid);
                
                pNSValue = [PMODGuid guidWithIId:&iid];
            }
                break;
            default:
                NSAssert(NO,@"unsupported type");
            break;
        }
    }
	return pNSValue;
}


+(IInspectable *) fromNSValue:(id) value
{
	InspectablePtr propertyValue;
	
	if([value isKindOfClass:[NSString class]])
	{
		foundation::HStringPtr hStringPtr;
		*hStringPtr.GetAddressOf() = [PMODInterop fromNSString:value];
		
		propertyValue = pv_util::CreateValue(hStringPtr);
	}
	else if([value isKindOfClass:[PMODEnumValue class]])
    {
        UINT32 enumValue = [(NSNumber *)value intValue];
        UINT32 enumTypeId = [(PMODEnumValue *)value enumTypeId];
        
        foundation::GetEnumValue(
                   enumTypeId,
                   enumValue,
                   reinterpret_cast<foundation::IEnumValue **>(propertyValue.GetAddressOf()));
    }
	else if([value isKindOfClass:[NSNumber class]])
	{
        NSNumber *number = (NSNumber *)value;
        NSString *classString = NSStringFromClass([value class]);
        if ([classString isEqualToString:@"__NSCFBoolean"]) {
            propertyValue = pv_util::CreateValue((bool)[number boolValue]);
        }
        else if ([classString isEqualToString:@"__NSCFNumber"]) {
            bool isNegative = [number decimalValue]._isNegative;
            const char* objCType = [number objCType];
            if(strcmp(objCType,"i") == 0 || strcmp(objCType,"I") == 0)
            {
                if(isNegative)
                {
                    propertyValue = pv_util::CreateValue((INT32)[number intValue]);
                }
                else
                {
                    propertyValue = pv_util::CreateValue((UINT32)[number unsignedIntValue]);
                }
            }
            else if(strcmp(objCType,"s") == 0 || strcmp(objCType,"S") == 0 || strcmp(objCType,"c") == 0)
            {
                if(isNegative)
                {
                    propertyValue = pv_util::CreateValue((INT16)[number shortValue]);
                }
                else
                {
                    propertyValue = pv_util::CreateValue((UINT16)[number unsignedShortValue]);
                }
            }
            else if(strcmp(objCType,"q") == 0 || strcmp(objCType,"Q") == 0)
            {
                if(isNegative)
                {
                    propertyValue = pv_util::CreateValue((INT64)[number longLongValue]);
                }
                else
                {
                    propertyValue = pv_util::CreateValue((UINT64)[number unsignedLongLongValue]);
                }
            }
            else if(strcmp(objCType,"f") == 0)
            {
                propertyValue = pv_util::CreateValue((FLOAT)[number floatValue]);
            }
            else if(strcmp(objCType,"d") == 0)
            {
                propertyValue = pv_util::CreateValue((DOUBLE)[number doubleValue]);
            }
        }
    }
    else if([value isKindOfClass:[NSDate class]])
	{
        return [PMODInterop fromNSDate:(NSDate *)value];
    }
    else if([value isKindOfClass:[PMODGuid class]])
	{
        propertyValue = pv_util::CreateValue(*reinterpret_cast<GUID *>([(PMODGuid *)value iid]));
    }
    else if([value isKindOfClass:[NSData class]])
	{
        NSData *nsData = (NSData *)value;
        pv_util::CreateUInt8ArrayValue((UINT32)nsData.length,(UINT8 *)nsData.bytes,propertyValue.GetAddressOf());
    }
	
	return propertyValue.Detach();
}

+(IInspectable *) fromNSObject:(id) nsObject
{
    if([nsObject isKindOfClass:[PMODObjectBase class]])
    {
        IObject *pObject = [static_cast<PMODObjectBase *>(nsObject) object];
        // add Ref before returning
        pObject->AddRef();
        return pObject;
    }
    else if([nsObject isKindOfClass:[PMODDictionary class]])
    {
        IInspectable *pObject = [static_cast<PMODDictionary *>(nsObject) object];
        // add Ref before returning
        pObject->AddRef();
        return pObject;
    }
    else if([nsObject isKindOfClass:[PMODInspectable class]])
    {
        IInspectable *pObject = [static_cast<PMODInspectable *>(nsObject) object];
        // add Ref before returning
        if(pObject != nullptr)
        {
            pObject->AddRef();
        }
        return pObject;
    }

    return [PMODInterop fromNSValue:nsObject];
}

+(NSString *) toIIDString:(REFIID) iid
{    
    return [NSString stringWithUTF8String:ToString(iid).c_str()];
}

+(unsigned int) toHex:(NSString *) iidStr fromIndex:(int)fromIndex count:(int)count
{
    NSString *hexString = [iidStr substringWithRange:NSMakeRange(fromIndex,count*2)];
    NSScanner *scanner = [NSScanner scannerWithString:hexString];
    unsigned int hexValue;
    [scanner scanHexInt:&hexValue];
    return hexValue;    
}

+(IID) fromIIDString:(NSString *) iidStr
{
    IID iid;
    if(!FromString([iidStr UTF8String],iid))
    {
        [PMODInterop throwIfFailed:E_UNEXPECTED];
    }
	return iid;
}

@end
