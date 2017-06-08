/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:RunTests.mm
****/

#import <XCTest/XCTest.h>
#import "UnitTestBase.h"

extern HRESULT RegisterTypesIf();

@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testRunTest {
    RegisterTypesIf();
   
    bool result = TestClassDeclaration::RunAll();
    XCTAssert(result, @"Pass");
}


@end
