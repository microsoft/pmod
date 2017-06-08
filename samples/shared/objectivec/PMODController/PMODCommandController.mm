/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODCommandController.mm
****/

#import "PMODCommandController.h"


@implementation PMODCommandController
{
	NSObject<PMODCommand> *_pmodCommand;
    UIControl        *_uiControl;
    id              _commandParameter;
}

@dynamic commandParameter;

-(PMODCommandController *) 
    init:(NSObject<PMODCommand> *)command
    commandParameter:(id) commandParameter
    target:(UIControl *)target
    forControlEvents:(UIControlEvents)controlEvents
{
	self = [super init];
	if(self) 
	{
        _uiControl = target;
        [_uiControl addTarget:self action:@selector(onEvent:) forControlEvents:controlEvents];
		_pmodCommand = command;
		// add Observer
		[_pmodCommand
			addObserver:self 
			forKeyPath:@"state" 
			options:(NSKeyValueObservingOptionNew)
			context:nil];
        
        self.commandParameter = commandParameter;
	}
	return self;
}

-(void) setCommandParameter:(id)value {
    _commandParameter = value;
    [self updateControlState];
}

-(id) commandParameter {
    return _commandParameter;
}

-(void) updateControlState {
    BOOL canExecute = [_pmodCommand canExecute:_commandParameter];
    NSLog(@"PMODCommandModelController canExecute -> %d",canExecute);

    [_uiControl setEnabled:canExecute];
}


-(void) dealloc {
	[_pmodCommand removeObserver:self forKeyPath:@"state"];
}


-(void) onEvent:(id)sender {
    NSLog(@"PMODCommandModelController onEvent");
    [_pmodCommand execute:_commandParameter];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    [self updateControlState];	 
}
		 
@end
