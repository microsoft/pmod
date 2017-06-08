/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODKeyPathController.h
****/

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <Foundation/NSValueTransformer.h>

@interface PMODKeyPathController : NSObject {
}
@property(readwrite,retain) id source;
@property(readwrite,retain) id target;

-(PMODKeyPathController *) init:(id)source forKeyPath:(NSString *)keyPath target:(id)target targetForKey:(NSString *) targetForKey valueTransformer:(NSValueTransformer*)transformer;

@end
