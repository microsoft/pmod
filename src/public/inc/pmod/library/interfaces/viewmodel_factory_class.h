/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:viewmodel_factory_class.h
****/
#pragma once

#include <pmod/pmod_api.h>

#include <pmod/interfaces/source_model.h>
#include <foundation/library/interfaces/foundation_class_base.h>
#include <foundation/library/parameter_base.h>
#include <foundation/library/object_factory_util.h>
#include <pmod/library/single_source_creator_entry.h>

#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        BEGIN_DECLARE_INTERFACE(IViewModelFactoryClass, foundation::library::IFoundationClassBase, PMOD_LIB_API)
            STDMETHOD(CreateObjectInternal)(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject) = 0;
            STDMETHOD(RaiseObjectCreated)(_In_ foundation::IObject *pObject) = 0;

            STDMETHOD(CreateSingleSourceViewModelInternal)(REFIID iidType, foundation::IObject *pModel, ISourceModel **ppSourceModel) = 0;
        END_DECLARE_INTERFACE()

        // Delegate Interface for the View Model Factory Class Implementation
        BEGIN_DECLARE_INTERFACE(IViewModelFactoryDelegate, foundation::library::IDelegateBase<IViewModelCreatedEventHandler>, FOUNDATION_LIB_API)
        END_DECLARE_INTERFACE()

        // ObjectCreatorEntry Structure
        // Summary: Describe a record Structure for a View Model mapping
        struct ObjectCreatorEntry
        {
        public:
#ifdef _MSC_VER
            ObjectCreatorEntry(
                const IID& iidType,
                foundation::library::CreateObjectCallback callback
            ) :
                IIdType(iidType),
                CreatorCallback(callback)
            {}

            ObjectCreatorEntry() :
                IIdType(foundation_GUID_NULL),
                CreatorCallback(nullptr)
            {}

            ObjectCreatorEntry & operator=(const ObjectCreatorEntry &) {}
#endif
            // IID of the Object to be registered
            const IID                                       IIdType;
            // the Creator Method
            foundation::library::CreateObjectCallback       CreatorCallback;
        };

        enum class ViewModelFactoryOptions
        {
            None = 0x00,
        };

        struct ViewModelFactoryCreateParameters :
            public foundation::library::_CreateParameterBase<IViewModelFactoryDelegate, ViewModelFactoryOptions>
        {
            ViewModelFactoryCreateParameters() :
                m_p_object_creator_entries(nullptr),
                m_p_single_source_model_factory_entries(nullptr)
            {}
            const ObjectCreatorEntry *m_p_object_creator_entries;
            const SingleSourceCreatorEntry *m_p_single_source_model_factory_entries;
        };

        BEGIN_DECLARE_INTERFACE(IViewModelFactoryClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateViewModelFactoryInstance)(
                _In_ const ViewModelFactoryCreateParameters *pViewModelFactoryParam,
                _In_opt_ foundation::IInspectable *pOuter,
                _Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()
    }
}

