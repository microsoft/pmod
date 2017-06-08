/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AppDelegate.m
****/

#import "AppDelegate.h"

#import "PMODDemoAppFactory.h"
#import "PMODDemoApp.g.h"
#import "PMODAdapterFactory.h"
#import "PMODCommand.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

{
    PMODAdapterFactory* _demoAppFactory;
}

- (id)init
{
    if(self = [super init]) {
        // create the model factory from a .mm file to abstract the C++ requirement
        _demoAppFactory = [PMODDemoAppFactoryImpl createDemoAppAdapterFactory];
    }
    return self;
}

- (NSValue<IAppDemo> *) demoApp
{
    return (NSValue<IAppDemo> *)[_demoAppFactory sourceModel];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}

- (IBAction)pressAddItem:(id)sender
{
    [self.demoApp.addItem execute:self.textFieldName.stringValue];
}
- (IBAction)pressClear:(id)sender
{
    [self.demoApp.clearItems execute:nil];
}
@end
