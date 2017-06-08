/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ViewController.h
****/

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController<UITableViewDelegate>

@property (assign) IBOutlet UITextField     *nameFieldProperty;
@property (assign) IBOutlet UITableView     *itemsTableView;
@property (assign) IBOutlet UIButton        *buttonChangeItem;

- (IBAction)pressAddItem:(id)sender;
- (IBAction)pressRemoveItem:(id)sender;
- (IBAction)pressChangeItem:(id)sender;
- (IBAction)pressRemoveAll:(id)sender;

@end

