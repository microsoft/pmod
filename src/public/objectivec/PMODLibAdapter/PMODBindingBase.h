/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODBindingBase.h
****/

#import "PMODBindingProtocol.h"
#import "PMODFoundation.h"
#import "PMODPropertyModelProtocol.h"
#import "PMODAdapterFactory.h"

@interface NSBindingBase : PMODFoundation<NSBinding> {
	
}
+(NSBindingBase *) createBinding:(PMODAdapterFactory *)adapterFactory source:(id<PMODPropertyModel>) withSource path:(NSString *) withPath;
+(NSBindingBase *) createExpressionBinding:(PMODAdapterFactory *)adapterFactory source:(id<PMODPropertyModel>) withSource bindingExpression:(NSString *) withBindingExpression;

@end
