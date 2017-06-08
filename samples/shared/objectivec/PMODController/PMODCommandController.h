/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODCommandController.h
****/

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

#import "PMODCommand.h"

@interface PMODCommandController : NSObject {
}
@property(readwrite,retain) id commandParameter;

-(PMODCommandController *)
    init:(NSObject<PMODCommand> *)command
    commandParameter:(id) commandParameter
    target:(UIControl *)target
    forControlEvents:(UIControlEvents)controlEvents;


@end
