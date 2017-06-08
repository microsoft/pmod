/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODLogger.h
****/

#import "PMODInspectable.h"

typedef enum
{
    LoggingLevel_Off,
    LoggingLevel_Fatal,
    LoggingLevel_Error,
    LoggingLevel_Warning,
    LoggingLevel_Info,
    LoggingLevel_Debug,
    LoggingLevel_Trace
} PMODLoggingLevel;

__attribute__((visibility("default")))
@interface PMODLogger : PMODInspectable {
   
}

+ (PMODLogger *)logger;
+ (void)setCategoryLevel:(PMODLoggingLevel)level;

@end
