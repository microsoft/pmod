/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODCollectionController.h
****/

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "PMODCollectionProtocol.h"

@protocol PMODCellController

@property(readwrite,retain) id context;

@end

@interface PMODCollectionController : UITableViewController {
}

@property(readwrite,retain) id<PMODCollection> collection;
@property(readwrite,retain) Class cellClass;
@property(readwrite,retain) NSString *cellIdentifier;

@end
