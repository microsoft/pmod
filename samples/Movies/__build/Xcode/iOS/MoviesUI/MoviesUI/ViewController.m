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
#import "PMODAsyncOperation.h"
#import "PMODMovies.g.h"

#import "PMODCollectionController.h"

@interface UIImage (Helpers)

+ (void) loadFromURL: (NSURL*) url callback:(void (^)(UIImage *image))callback;

@end

@implementation UIImage (Helpers)

+ (void) loadFromURL: (NSURL*) url callback:(void (^)(UIImage *image))callback {
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0ul);
    dispatch_async(queue, ^{
        NSData * imageData = [NSData dataWithContentsOfURL:url];
        dispatch_async(dispatch_get_main_queue(), ^{
            UIImage *image = [UIImage imageWithData:imageData];
            callback(image);
        });
    });
}

@end

@interface MovieTableCell : UITableViewCell<PMODCellController>

    @property (assign) IBOutlet UILabel     *titleLabel;

@end

@implementation MovieTableCell
{
    id<IMovie> movie;
}

-(id)context
{
    return movie;
}

-(void) setContext:(id)item
{
    movie = (id<IMovie>)item;
    UILabel *titleLabel = [self viewWithTag:10];
    titleLabel.text = movie.title;
    UIImageView *posterImageView = [self viewWithTag:20];
    NSString *imageUrl = [NSString stringWithFormat:@"%@%@",
                         @"https://image.tmdb.org/t/p/w342",
                         movie.posterUri];
    [UIImage loadFromURL:[NSURL URLWithString:imageUrl] callback:^(UIImage *image) {
        posterImageView.image = image;
    }];
    
    UILabel *votesLabel = [self viewWithTag:30];
    votesLabel.text = [@(movie.voteCount) stringValue];

}

@end


@interface ViewController ()

@end

@implementation ViewController
{
    PMODCollectionController *moviesTableViewController;
}

- (void) getNowPlayingAsync {
    PMODAsyncOperation *asyncOperation = [[AppDelegate moviesApiService] getNowPlaying:1 language:@""];
    [asyncOperation setCompletedBlock:^(PMODResultStatus status,id result) {
        if(status == PMODResultStatus_Completed)
        {
            id<IPageMovies> pageMovies = (id<IPageMovies>)result;
            moviesTableViewController.collection = [pageMovies movies];
        }
    }];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    moviesTableViewController  = [[PMODCollectionController alloc] init];
    moviesTableViewController.cellIdentifier = @"tvcMovie";
    
    self.moviesTableView.dataSource = moviesTableViewController;
    moviesTableViewController.tableView = self.moviesTableView;
    
    [self getNowPlayingAsync];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
