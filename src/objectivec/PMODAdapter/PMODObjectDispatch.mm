/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODObjectDispatch.mm
****/


#import "PMODObjectDispatch.h"
#import "PMODInterop.h"
#import "PMODAdapterUtil.h"

#import <foundation/pv_util.h>
#import <foundation/com_ptr.h>
#import <foundation/type_info_statics_util.h>
#import <foundation/interfaces/object_dispatch.h>
#import <foundation/interfaces/object_dispatch_info.h>

using namespace foundation;

@implementation PMODObjectDispatch
{
    IObjectDispatch				*_pObjectDispatch;
    IObjectDispatchInfo			*_pObjectDispatchInfo;
}

-(PMODObjectDispatch *) initWithObjectDispatch: (IObjectDispatch *)pObjectDispatch
{
	self = [super init];
	if(self) 
	{
		[self initObjectDispatch:pObjectDispatch];
	}
	return self;
}

-(void) dispose {
	[super dispose];
    RELEASE_INTERFACE(_pObjectDispatch);
    RELEASE_INTERFACE(_pObjectDispatchInfo);
}

-(void) initObjectDispatch:(IObjectDispatch *)pObjectDispatch
{
    foundation_assert(_pObjectDispatch == nullptr);
    foundation_assert(pObjectDispatch != nullptr);
    
	_pObjectDispatch = pObjectDispatch;
	_pObjectDispatch->AddRef();
	// PropertyInfo
	foundation::GetObjectTypeInfo(_pObjectDispatch,&_pObjectDispatchInfo);
}

- (IFoundationObjectDispatch *) objectDispatch
{
    return _pObjectDispatch;
}

-(IPropertyInfo *) getPropertyInfo:(NSString *)nsPropertyName
{
    std::string propertyName = [nsPropertyName UTF8String];
    // Note: this code will assum the schema has an Upper case definition
    // Property
    propertyName[0] = toupper(propertyName[0]);
	ComPtr<foundation::IPropertyInfo> spPropertyInfo;
	[PMODInterop throwIfFailed:(_pObjectDispatchInfo->GetPropertyByName(
                     foundation::HStringRef(propertyName.c_str()),
                     spPropertyInfo.GetAddressOf()))];
	return spPropertyInfo;
}

+(std::string) getCorePath:(NSString *)nsPath
{
    std::string path_str;
    NSArray *nsPropertyNames = [nsPath componentsSeparatedByString:@"."];
    for (id nsPropertyName in nsPropertyNames) {
        std::string propertyName = [(NSString *)nsPropertyName UTF8String];
        // Note: this code will assum the schema has an Upper case definition
        // Property
        propertyName[0] = toupper(propertyName[0]);
        if(path_str.length() > 0)
        {
            path_str += '.';
        }
        path_str += propertyName;
    }
    return path_str;
}

-(void) setValue:(id)value	forKey:(NSString *)key
{	
	InspectablePtr spValue;
	spValue.Attach([PMODAdapter fromNSObject:value]);

    NSRange pathSeperatorRange = [key rangeOfString:@"."];
    if (pathSeperatorRange.location == NSNotFound)
	{
        IPropertyInfo *pPropertyInfo = [self getPropertyInfo:key];

        UINT32 propertyId;
        [PMODInterop throwIfFailed:(pPropertyInfo->GetId(&propertyId))];
        [PMODInterop throwIfFailed:(_pObjectDispatch->SetProperty(propertyId,spValue))];
    }
    else
    {
		//[PMODInterop throwIfFailed:(core_util::set_property_value(_pObservableObject,[PMODPropertyModel getCorePath:key].c_str(),spValue))];
    }
}

- (id) valueForKeyInternal:(NSString *)key
{
	InspectablePtr value;
	
	NSRange pathSeperatorRange = [key rangeOfString:@"."];
    if (pathSeperatorRange.location == NSNotFound) 
	{
		
		IPropertyInfo *pPropertyInfo = [self getPropertyInfo:key];
		
		UINT32 propertyId;
		[PMODInterop throwIfFailed:(pPropertyInfo->GetId(&propertyId))];
	
		[PMODInterop throwIfFailed:(_pObjectDispatch->GetProperty(propertyId,value.GetAddressOf()))];
	}
	else 
	{		
        //[PMODInterop throwIfFailed:(core_util::get_property_value(_pObservableObject,[PMODPropertyModel getCorePath:key].c_str(),value.GetAddressOf()))];
	}	
	return [self toNSObject:value];
}

- (void) setValueForProperty:(unsigned int)propertyId value:(id) value
{
	InspectablePtr spValue;
	spValue.Attach([PMODAdapter fromNSObject:value]);
	
 	[PMODInterop throwIfFailed:(_pObjectDispatch->SetProperty(propertyId,spValue))];
}

- (id) valueForProperty:(unsigned int)propertyId
{
	NSAssert(_pObjectDispatch,@"_pObservableObject != nil");
	
	InspectablePtr value;
	[PMODInterop throwIfFailed:(_pObjectDispatch->GetProperty(propertyId,value.GetAddressOf()))];
	return [self toNSObject:value];
}

- (id) invoke:(unsigned int)methodId parameters:(NSArray *)parameters
{
    std::vector<InspectablePtr> vector_parameters;
    vector_parameters.resize(parameters.count);
    for(NSInteger index = 0;index < parameters.count;++index)
    {
        vector_parameters[index].Attach([PMODAdapter fromNSObject:[parameters objectAtIndex:index]]);
    }
	InspectablePtr spResult;
	[PMODInterop throwIfFailed:(_pObjectDispatch->InvokeMethod(methodId,
            (UINT32)vector_parameters.size(),
            vector_parameters.size() ? vector_parameters.front().GetAddressOfPtr():nullptr,
            spResult.GetAddressOf()))];
    
	return [self toNSObject:spResult];
}

- (id) valueForKey:(NSString *)key
{	
	return [self valueForKeyInternal:key];
}

- (id)valueForKeyPath:(NSString *)keyPath
{
	return [super valueForKeyPath:keyPath];
}

- (NSArray *)allProperties
{
    ArrayRefCountWrapper<IPropertyInfo> properties;
    _pObjectDispatchInfo->GetProperties((UINT32)InstanceFlags::All,
          properties.GetSizeAddressOf(),
          properties.GetBufferAddressOf());
    
    NSMutableArray *returnProperties = [[NSMutableArray alloc] initWithCapacity:properties.GetSize()];
    
    for(UINT32 index = 0;index < properties.GetSize();++index)
    {
        HStringPtr propertyName;
        properties[index]->GetName(propertyName.GetAddressOf());
        [returnProperties addObject:[PMODInterop toNSString:propertyName]];
    }
    return returnProperties;
}

-(foundation::IObject *) object
{
	return _pObjectDispatch;
}

-(id) copyWithZone: (NSZone *) zone
{
    PMODObjectDispatch *nsObjectCopy = [[PMODObjectDispatch allocWithZone: zone] initWithObjectDispatch:_pObjectDispatch];
    
    [nsObjectCopy setAdapterFactory:self.adapterFactory];
    return nsObjectCopy;
}


@end
