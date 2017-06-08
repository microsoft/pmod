/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_object_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <pmod/interfaces/observable_object.h>
#include <pmod/interfaces/binding.h>
#include <foundation/library/interfaces/delegate_base.h>
#include <foundation/library/interfaces/disposable.h>
#include <foundation/library/interfaces/properties_container.h>
#include <foundation/library/type_info_create_parameter.h>

#include "value_converter.h"
#include "object_node_class.h"
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        // IObservableObjectClass Interface

        // IMethodDelegate Interface
        BEGIN_DECLARE_INTERFACE(IMethodOnInvokeDelegate, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke) (_In_ foundation::IMethodInfo *pMethodInfo, _In_ UINT32 methodId, _In_ UINT32 size, _In_ foundation::IInspectable **parameters, _Outptr_ foundation::IInspectable **ppValue) = 0;
        END_DECLARE_INTERFACE()

        enum PropertyRuntimeClassOptions
        {
            None = 0x00,
            // use 'Working Dispatcher' when GetProperty is being invoked
            UseWorkingDispatcherOnGetProperty = 0x01,
            UseWorkingDispatcherOnSetProperty = 0x02,
        };

        BEGIN_DECLARE_INTERFACE(IObservableObjectClass, IObjectNodeClass, PMOD_LIB_API)
            STDMETHOD(SetPropertyInternal)(_In_ UINT32 propertyId, _In_opt_ foundation::IInspectable *newValue, _In_ bool fireNotifyChanged) = 0;
            STDMETHOD(FirePropertyChanged)(_In_ UINT32 propertyId, _In_ bool useCallback) = 0;
            STDMETHOD(FireEventModel)(_In_ UINT32 eventId, _In_ foundation::IInspectable *pEventArgs) = 0;
            STDMETHOD(SetBinding)(_In_ UINT32 propertyId, _In_ IBindingBase *pPropertyBinding) = 0;
            STDMETHOD(ClearBinding)(_In_ UINT32 propertyId) = 0;
            STDMETHOD(SetPropertyConverter)(_In_ UINT32 propertyId, _In_opt_ IValueConverter *pValueConverter) = 0;
            STDMETHOD(GetPropertyRuntimeClassOptions)(_In_ UINT32 propertyId, _Out_ PropertyRuntimeClassOptions* propertyOptions) = 0;
            STDMETHOD(SetPropertyRuntimeClassOptions)(_In_ UINT32 propertyId, _In_ PropertyRuntimeClassOptions propertyOptions) = 0;
            STDMETHOD(GetMethodOnInvokeDelegate)(_COM_Outptr_result_maybenull_ library::IMethodOnInvokeDelegate **ppExecuteDelegate) = 0;
            STDMETHOD(SetMethodOnInvokeDelegate)(_In_opt_ library::IMethodOnInvokeDelegate *pExecuteDelegate) = 0;
            STDMETHOD(EnsurePropertiesContainer)() = 0;
            STDMETHOD(GetPropertiesContainer)(_COM_Outptr_ foundation::library::IPropertiesContainer **ppPropertiesContainer) = 0;
            END_DECLARE_INTERFACE()

        // Delegate Interface for a Property Model Implementation
        BEGIN_DECLARE_INTERFACE(IObservableObjectDelegate, foundation::library::IDelegateBase<IPropertyChangedEventHandler>, PMOD_LIB_API)
            STDMETHOD(OnCreatePropertiesContainer)(_In_ IObservableObjectInfo* pPropertyModelInfo, _In_ UINT32 size, _COM_Outptr_ foundation::library::IPropertiesContainer **ppValue) = 0;
            STDMETHOD(OnGetProperty)(_In_ UINT32 propertyId, _Inout_ foundation::IInspectable **ppValue) = 0;
            STDMETHOD(OnSetProperty)(_In_ UINT32 propertyId, bool isPublic, _In_ foundation::IInspectable *oldValue, _In_ foundation::IInspectable *newValue) = 0;
            STDMETHOD(OnInvokeMethod) (_In_ foundation::IMethodInfo *pMethodInfo, _In_ UINT32 methodId, _In_ UINT32 size, _In_opt_ foundation::IInspectable **parameters, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppValue) = 0;
        END_DECLARE_INTERFACE()

            enum class ObservableObjectOptions
        {
            None = 0x00,
            InitializeProperties = 0x01,
            SupportRootProperty = 0x02,
            NoAutoParent = 0x04,
            UseGetPropertyDispatcher = 0x08,
            UseSetPropertyDispatcher = 0x10,
            UseGetPropertiesDispatcher = 0x20,
            UseFastPropertiesContainer = 0x40,
            ClassOnSetProperty = 0x80,
            AllowReturnAsyncOnVoidResult = 0x100,
            IsContainerWeakRef = 0x200,
        };

        struct ObservableObjectCreateParameters :
            public foundation::library::_TypeInfoCreateParameterBase
            <
            IObservableObjectDelegate,
            ObservableObjectOptions
            >
        {
            ObservableObjectCreateParameters()
            {
            }
        };

        BEGIN_DECLARE_INTERFACE(IObservableObjectClassFactory, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(CreateObservableObjectInstance)(
                _In_ const ObservableObjectCreateParameters *pCreateParams,
                _In_opt_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance) = 0;
        END_DECLARE_INTERFACE()

    }
}

