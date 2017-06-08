/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODInterop.h
****/

#import <Foundation/Foundation.h>

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#import "PMODInterfaces.h"
#import <foundation/pal.h>

@interface PMODException : NSException
-(HRESULT) error;
@end

__attribute__((visibility("default")))
@interface PMODInterop : NSObject

+(void)throwIfFailed:(HRESULT) hr;

+(NSString *) toNSString:(HSTRING) hstring;
+(HSTRING) fromNSString:(NSString *) nsString;

+(NSDate *) toNSDate:(foundation::IPropertyValue *) pPropertyValue;
+(IFoundationInspectable *) fromNSDate:(NSDate *) nsDate;

+(NSData *) toNSData:(foundation::IPropertyValue *) pPropertyValue;
+(IFoundationInspectable *) fromNSData:(NSData *) nsData;

+(NSArray *) toNSArray:(foundation::IPropertyValue *) pPropertyValue;
+(IFoundationInspectable *) fromNSArray:(NSArray *) nsArray;

+(id) toNSValue:(foundation::IPropertyValue *) pPropertyValue;
+(IFoundationInspectable *) fromNSValue:(id) value;

+(IFoundationInspectable *) fromNSObject:(id) nsObject;

+(NSString *) toIIDString:(REFIID) iid;
+(IID) fromIIDString:(NSString *) iidStr;

@end
