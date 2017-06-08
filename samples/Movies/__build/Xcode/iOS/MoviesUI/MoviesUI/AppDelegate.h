/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AppDelegate.h
****/

#import <UIKit/UIKit.h>

@protocol IMoviesApiService;

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;
+ (id<IMoviesApiService>) moviesApiService;

@end

