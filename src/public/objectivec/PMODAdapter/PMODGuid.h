/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODGuid.h
****/

#import <Foundation/Foundation.h>

__attribute__((visibility("default")))
@interface PMODGuid : NSObject {

}
-(PMODGuid *) initWithString:(NSString *)pString;
-(PMODGuid *) initWithIId:(void *)iid;

-(NSString*) iidString;
-(void*) iid;

+ (PMODGuid *)guidWithString:(NSString *)pString;
+ (PMODGuid *)guidWithIId:(void *)iid;

@end
