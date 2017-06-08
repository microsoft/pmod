/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelLibraryInstance.cpp
****/
#include "pch.h"

#include <foundation/library/interface_adapter_util.h>
#include "../foundation_library/Logger.h"
#include <foundation/library/dispatcher_callback_adapter.h>

#ifdef FOUNDATION_API_EXPORTS
#include "AdapterFactoryFactory.h"
#include "InterfaceAdapterFactoryHost.h"
#endif

// external entry point from generated logger type info
extern HRESULT STDMETHODCALLTYPE register_core_logger_type_info(void);


//------------------------------------------------------------------------------
// Class:   _ModelLibraryInstanceInitializer
//
// Purpose: Static Initializer for the Model Library
//
//------------------------------------------------------------------------------
struct _ModelLibraryInstanceInitializer
{
    _ModelLibraryInstanceInitializer();
    ~_ModelLibraryInstanceInitializer();
};

static _ModelLibraryInstanceInitializer _propertyModelInstanceInitializer;

// external entry point to register observable object adapters
extern HRESULT RegisterFactoryObservableObjectAdapters(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory);

// external entry point to register property data class adapters
extern HRESULT RegisterFactoryPropertyDataClassAdapters(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory);
// external entry point to register property model class adapters
extern HRESULT RegisterFactoryObservableObjectClassAdapters(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory);
// external entry point to register collection model class adapters
extern HRESULT RegisterFactoryObservableCollectionClassAdapters(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory);

// external entry point to register model serializer adapters
extern HRESULT RegisterFactoryModelSerializerAdapters(foundation::library::IGenericAdapterFactory *pGenericAdapterFactory);

_ModelLibraryInstanceInitializer::_ModelLibraryInstanceInitializer()
{
    foundation::ComPtr<foundation::library::IGenericAdapterFactory> spGenericAdapterFactory;
#ifdef FOUNDATION_API_EXPORTS
    CAdapterFactoryFactory::GetInstance()->CreateGenericAdapterFactory(spGenericAdapterFactory.GetAddressOf());
#else
    IGNOREHR(foundation::library::CreateGenericAdapterFactory(spGenericAdapterFactory.GetAddressOf()));
#endif
    foundation_assert(spGenericAdapterFactory != nullptr);

    IGNOREHR(RegisterFactoryObservableObjectAdapters(spGenericAdapterFactory));
    IGNOREHR(RegisterFactoryPropertyDataClassAdapters(spGenericAdapterFactory));
    IGNOREHR(RegisterFactoryObservableObjectClassAdapters(spGenericAdapterFactory));
    IGNOREHR(RegisterFactoryObservableCollectionClassAdapters(spGenericAdapterFactory));
    // custom serializers
    IGNOREHR(RegisterFactoryModelSerializerAdapters(spGenericAdapterFactory));

    foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spInterfaceAdapterFactory;
    foundation::QueryInterface(spGenericAdapterFactory, spInterfaceAdapterFactory.GetAddressOf());

#ifdef FOUNDATION_API_EXPORTS
    IGNOREHR(CInterfaceAdapterFactoryHost::GetInstance()->RegisterInterfaceAdapterFactory(spInterfaceAdapterFactory));
#else
    IGNOREHR(foundation::library::RegisterInterfaceAdapterFactory(spInterfaceAdapterFactory));
#endif

    // register the 'core' logger type info
    foundation::ComPtr<foundation::library::IDelegateDispatchable> spCallback;
    foundation::DispatcherCallbackAdapter::CreateInstance(&register_core_logger_type_info, spCallback.GetAddressOf());

    CLogger::register_factory_type_info(spCallback);

}

_ModelLibraryInstanceInitializer::~_ModelLibraryInstanceInitializer()
{
}

EXTERN_C void _ModelLibraryInstanceInitializerStub()
{
    // empty stub to force this module to be linked when a static lib is being used
}
