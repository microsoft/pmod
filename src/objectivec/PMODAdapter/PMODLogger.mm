/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODLogger.mm
****/

#import "PMODLogger.h"
#import <foundation/library/logger_util.h>

using namespace foundation;

@implementation PMODLogger

+ (PMODLogger *)logger
{
    return [[PMODLogger alloc] initWithInspectable:logger_util::GetFoundationLogger()];
}

+ (void)setCategoryLevel:(PMODLoggingLevel)level
{
    logger_util::SetCategoryLevel((LoggingLevel)level);
}

@end
