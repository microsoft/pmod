/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODDictionaryTest.m
****/
#import  "PMODDictionary.h"
#import  "PMODAdapter.h"
#import <XCTest/XCTest.h>

@interface PMODDictionaryTest : XCTestCase

@end

@implementation PMODDictionaryTest


- (void)testCreate {
    PMODDictionary *pmodDictionary = [[PMODDictionary alloc]init];
    [pmodDictionary setObject:@"rodrigov" forKey:@"key1"];
    
    id value = [pmodDictionary objectForKey:@"key1"];
    XCTAssert([value isEqualToString:@"rodrigov"]);
}

@end
