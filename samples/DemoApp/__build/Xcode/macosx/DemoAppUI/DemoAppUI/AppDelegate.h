/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AppDelegate.h
****/

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSTextField *textFieldName;

- (IBAction)pressAddItem:(id)sender;
- (IBAction)pressClear:(id)sender;

@end

