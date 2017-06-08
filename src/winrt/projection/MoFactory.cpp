/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoFactory.cpp
****/

#include "pch.h"

#include "comModule.h"

#include "MoModelModelFactory.h"
#include "MoModelTypeInfoStatics.h"
#include "MoPropertyBindingFactory.h"
#include "MoMultiSourceBindingFactory.h"
#include "MoExpressionFactory.h"
#include "MoExpressionBindingFactory.h"
#include "MoDispatcherStatics.h"
#include "MoCoreLoggerFactory.h"
#include "MoLogParameterFactory.h"
#include "MoLogRecordFactory.h"
#include "MoDictionaryFactory.h"
#include "MoJsonUtils.h"

#include "..\moproxy\MoCoreProxyStatics.h"

using namespace ctlwinrt;
module ctlwinrt::__module;

ACTIVATION_FACTORY_MAP_BEGIN()
    // type info support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.ObjectTypeInfo", ActivationFactoryCreator<MoObjectTypeInfoStatics>::CreateActivationFactory)
    
    // model support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Model", ActivationFactoryCreator<MoModelModelFactory>::CreateActivationFactory)
    // binding support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.PropertyBinding", ActivationFactoryCreator<MoPropertyBindingFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.MultiSourceBinding", ActivationFactoryCreator<MoMultiSourceBindingFactory>::CreateActivationFactory)
    // expression support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Expression", ActivationFactoryCreator<MoExpressionFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.ExpressionBinding", ActivationFactoryCreator<MoExpressionBindingFactory>::CreateActivationFactory)

    // misc support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Dictionary", ActivationFactoryCreator<MoDictionaryFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Dispatcher", ActivationFactoryCreator<MoDispatcherStatics>::CreateActivationFactory)

    // logging support

    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.LogParameter", ActivationFactoryCreator<MoLogParameterFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.LogRecord", ActivationFactoryCreator<MoLogRecordFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Logger", ActivationFactoryCreator<MoLoggerFactory>::CreateActivationFactory)

    // core proxy support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.ProxyFactory", ActivationFactoryCreator<MoCoreProxyStatics>::CreateActivationFactory)

    // json utils support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.JsonUtils", ActivationFactoryCreator<MoJsonUtilsStatics>::CreateActivationFactory)

    ACTIVATION_FACTORY_MAP_END()

