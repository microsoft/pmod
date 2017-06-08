/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoLibraryFactory.cpp
****/

#include "pch.h"

#include "comModule.h"

#include "MoObjectDispatchFactory.h"
#include "MoObservableObjectFactory.h"
#include "MoAsyncOperationFactory.h"
#include "MoObservableCollectionInfoFactory.h"
#include "MoEnumTypeInfoFactory.h"
#include "MoCommandInfoFactory.h"
#include "MoObservableObjectInfoFactory.h"
#include "MoObservableCollectionFactory.h"
#include "MoCommandFactory.h"
#include "MoViewModelFactoryFactory.h"

// View Model headers
#include "MoViewModelInfoFactory.h"

#include "MoViewModelFactory.h"
#include "MoCollectionViewModelFactory.h"
#include "MoMultiSourceCollectionViewModelFactory.h"
#include "MoMultiSourceViewModelFactory.h"
#include "MoCommandViewModelFactory.h"

using namespace ctlwinrt;
module ctlwinrt::__module;

ACTIVATION_FACTORY_MAP_BEGIN()
    // type info support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.EnumTypeInfo", ActivationFactoryCreator<MoEnumTypeInfoFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.m_parameterName", ActivationFactoryCreator<MoMethodInfoFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.CommandInfo", ActivationFactoryCreator<MoCommandInfoFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.ObservableCollectionInfo", ActivationFactoryCreator<MoObservableCollectionInfoFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.ObservableObjectInfo", ActivationFactoryCreator<MoObservableObjectInfoFactory>::CreateActivationFactory)
    
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.ViewModelInfo", ActivationFactoryCreator<MoViewModelInfoFactory>::CreateActivationFactory)

    // model support
	ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.ObjectDispatch", ActivationFactoryCreator<MoObjectDispatchFactory>::CreateActivationFactory)
	ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.ObservableObject", ActivationFactoryCreator<MoObservableObjectFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.AsyncOperation", ActivationFactoryCreator<MoAsyncOperationFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.ObservableCollection", ActivationFactoryCreator<MoCollectionModelFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.Command", ActivationFactoryCreator<MoCommandFactory>::CreateActivationFactory)

    // misc support
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.ViewModelFactory", ActivationFactoryCreator<MoViewModelFactoryFactory>::CreateActivationFactory)

    // View Model factories
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.CollectionViewModel", ActivationFactoryCreator<MoCollectionViewModelFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.ViewModel", ActivationFactoryCreator<MoPropertyViewModelFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.MultiSourceCollectionViewModel", ActivationFactoryCreator<MoMultiSourceCollectionViewModelFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.MultiSourceViewModel", ActivationFactoryCreator<MoMultiSourcePropertyViewModelFactory>::CreateActivationFactory)
    ACTIVATION_FACTORY_ENTRY(L"Microsoft.PropertyModel.Library.CommandViewModel", ActivationFactoryCreator<MoCommandViewModelFactory>::CreateActivationFactory)


    ACTIVATION_FACTORY_MAP_END()

