/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODTypePropertiesTest.m
****/

#import "PMODTypePropertiesTest.h"
#import "PMODObservableObject.h"

@interface NSTestObserver : NSObject
@property(retain) NSString *keyPath;
@property(retain) id object;
@property(retain) NSDictionary *change;
@end
@implementation NSTestObserver

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    self.keyPath = keyPath;
    self.object = object;
    self.change = change;
}

@end

@implementation PMODTypePropertiesTest


- (void)testTypeProperties:(id<ITypeProperties>)typeProperties
{
    NSObject *typePropertiesObject = (NSObject *)typeProperties;

    NSTestObserver *nsTestObserver = [NSTestObserver alloc];
    [typePropertiesObject addObserver:nsTestObserver forKeyPath:@"shortProperty" options:NSKeyValueObservingOptionNew context:nil];
    
    // try short
    [typeProperties setShortProperty:-100];
    short shortVal = [typeProperties shortProperty];
    XCTAssert(shortVal == -100,@"expected -100");
    XCTAssert([nsTestObserver.keyPath isEqualToString:@"shortProperty"]);
   
    [typePropertiesObject removeObserver:nsTestObserver forKeyPath:@"shortProperty"];

    // try unsigned short
    [typeProperties setUShortProperty:0xf000];
    unsigned short uShortVal = [typeProperties uShortProperty];
    XCTAssertTrue(uShortVal == 0xf000,@"expected 0xf000");
    
    // try int
    [typeProperties setIntProperty:0xf0000000];
    int iVal= [typeProperties intProperty];
    XCTAssertTrue(iVal == 0xf0000000,@"expected 0xf0000000");
    
    // try unsigned int
    [typeProperties setUIntProperty:0xff000000];
    unsigned int uiVal= [typeProperties uIntProperty];
    XCTAssertTrue(uiVal == 0xff000000,@"expected 0xff000000");
    
    // try long
    [typeProperties setLongProperty:0x1000000000000000];
    long long lVal= [typeProperties longProperty];
    XCTAssertTrue(lVal == 0x1000000000000000,@"expected 0x01000000000000000");
    
    // try unsigned long
    [typeProperties setULongProperty:0xf100000000000000];
    unsigned long long ulVal = [typeProperties uLongProperty];
    XCTAssertTrue(ulVal == 0xf100000000000000,@"expected 0xf100000000000000");
    
    // try BOOL
    [typeProperties setBooleanProperty:TRUE];
    BOOL bVal = [typeProperties booleanProperty];
    XCTAssertTrue(bVal == TRUE,@"expected TRUE");
    
    id value = [typeProperties valueForProperty:TypeProperties_Property_BooleanProperty];
    XCTAssertTrue([value isEqual:@(YES)] ,@"expected TRUE");
    
    // try FLOAT
    [typeProperties setFloatProperty:5.5];
    float floatVal = [typeProperties floatProperty];
    XCTAssertTrue(floatVal == 5.5,@"expected TRUE");
    
    // try DOUBLE
    const double _dblVal = 3.41;
    [typeProperties setDoubleProperty:_dblVal];
    double doubleVal = [typeProperties doubleProperty];
    XCTAssertTrue(doubleVal == _dblVal,@"expected TRUE");
    
    // try Date Time
    NSDate *nsSetDate = [NSDate date];
    [typeProperties setDateTimeProperty:nsSetDate];
    NSDate *nsGetDate = [typeProperties dateTimeProperty];
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateStyle:NSDateFormatterShortStyle];
    [dateFormatter setTimeStyle:NSDateFormatterShortStyle];
    
    NSString *date1str = [dateFormatter stringFromDate:nsSetDate];
    NSString *date2str = [dateFormatter stringFromDate:nsGetDate];
    XCTAssertTrue( [date1str compare:date2str]==NSOrderedSame,@"expected TRUE");
    
    // try GUID
    [typeProperties setGuidProperty:[PMODGuid guidWithString:[PMODTestModelFactory TypePropertiesType]]];
    
    PMODGuid *nsGuid = [typeProperties guidProperty];
    XCTAssertTrue( [[nsGuid iidString] isEqualToString:[PMODTestModelFactory TypePropertiesType]],@"expected TRUE" );
}
@end
