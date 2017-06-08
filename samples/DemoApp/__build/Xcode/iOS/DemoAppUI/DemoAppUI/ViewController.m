/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ViewController.m
****/

#import "ViewController.h"
#import "AppDelegate.h"

#import "PMODAdapterFactory.h"
#import "PMODCommand.h"
#import "PMODDemoApp.g.h"

#import "PMODKeyPathController.h"
#import "PMODCollectionController.h"

@interface ItemTableCell : UITableViewCell<PMODCellController>
{
}

@end

@implementation ItemTableCell
{
    PMODKeyPathController *_nameController;
    id<IItem> _item;
}

-(ItemTableCell *) init
{
    self = [self initWithStyle:UITableViewCellStyleDefault reuseIdentifier:NSStringFromClass([ItemTableCell class])];
    UILabel* nameLabel;
    
    nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(5.0, 0.5, 235.0, 15.0)];
    nameLabel.font = [UIFont boldSystemFontOfSize:14.0];
    nameLabel.textAlignment = NSTextAlignmentLeft;
    nameLabel.textColor = [UIColor blackColor];
    nameLabel.autoresizingMask = UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleHeight;
    [self.contentView addSubview:nameLabel];
    
    // create key path controller for 'Name' property
    _nameController = [[PMODKeyPathController alloc]
                       init:nil
                       forKeyPath:@"name"
                       target:nameLabel
                       targetForKey:@"text"
                       valueTransformer:nil];
    return self;
}

-(id)context
{
    return _item;
}

-(void) setContext:(id)item
{
    _item = (id<IItem>)item;
    //NSLog(@"ItemTableCell.setContext:%d",_item.uniqueId);
    
    _nameController.source = _item;
}

@end

@interface ViewController ()

@end

@implementation ViewController
{
    PMODCollectionController *_itemsController;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    [self.buttonChangeItem setEnabled:FALSE];
    // create a NSCollectionModelController for the 'items' property
    _itemsController = [[PMODCollectionController alloc] init];
    _itemsController.collection = [AppDelegate demoApp].items;
    _itemsController.cellClass = [ItemTableCell class];
    
    self.itemsTableView.dataSource = _itemsController;
    _itemsController.tableView = self.itemsTableView;

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)pressAddItem:(id)sender
{
    [[AppDelegate demoApp].addItem execute:_nameFieldProperty.text];
}

- (IBAction)pressRemoveItem:(id)sender
{
    [[AppDelegate demoApp].addItem execute:_nameFieldProperty.text];
}

- (IBAction)pressChangeItem:(id)sender
{
    NSIndexPath *selectedRowIndexPath = self.itemsTableView.indexPathForSelectedRow;
    UITableViewCell *selectedCell = [self.itemsTableView cellForRowAtIndexPath:selectedRowIndexPath];
    // static cast DataContext to an IItem
    id<IItem> itemObject  = (id<IItem>)((id<PMODCellController>)selectedCell).context;
    [itemObject setName:_nameFieldProperty.text];
}

- (IBAction)pressRemoveAll:(id)sender
{
    [[AppDelegate demoApp].clearItems execute:Nil];
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self.buttonChangeItem setEnabled:TRUE];
}

- (void)tableView:(UITableView *)tableView didDeselectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self.buttonChangeItem setEnabled:FALSE];
}
@end
