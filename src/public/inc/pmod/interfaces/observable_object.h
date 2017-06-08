/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_object.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <foundation/interfaces/object_dispatch.h>
#include <foundation/interfaces/object_dispatch_info.h>

namespace pmod {


    BEGIN_DECLARE_INTERFACE(IEventInfo, foundation::IInspectable, PMOD_API)
        STDMETHOD(GetId)(_Out_ UINT32* eventId) = 0;
        STDMETHOD(GetEventName)(_Out_ HSTRING* methodName) = 0;
        STDMETHOD(GetEventArgsTypeInfo)(_COM_Outptr_ foundation::IPropertyTypeInfo **ppVarTypeInfo) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IObservableObjectInfo, foundation::IObjectDispatchInfo, PMOD_API)
        STDMETHOD(GetEvents)(UINT32 eventFlags, _Out_ UINT32* size, IEventInfo ***ppEvents) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IViewModelInfo, IObservableObjectInfo, PMOD_API)
        STDMETHOD(GetIsInherited)(_Out_ bool *value) = 0;
        STDMETHOD(GetSourceType)(_COM_Outptr_ IObservableObjectInfo** ppPropertyModelInfo) = 0;
    END_DECLARE_INTERFACE()

    /////////////////////////////////// Property Model //////////////////////////////////////////////
    BEGIN_DECLARE_INTERFACE(IPropertyChangedEventArgs, foundation::IInspectable, PMOD_API)
        STDMETHOD(GetProperty)(_Out_ UINT32* pValue) = 0;
        STDMETHOD(GetPropertyName)(_Out_ HSTRING* pValue) = 0;
        STDMETHOD(GetOldValue)(_COM_Outptr_ foundation::IInspectable **ppValue) = 0;
        STDMETHOD(GetNewValue)(_COM_Outptr_ foundation::IInspectable **ppValue) = 0;
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(IPropertyChanged, PMOD_API)

    BEGIN_DECLARE_INTERFACE(IBatchPropertyChangedEventArgs, foundation::IInspectable, PMOD_API)
        STDMETHOD(GetPropertyChanges)(_Out_ UINT32* size, _Outptr_ IPropertyChangedEventArgs ***ppPropertyChangedEventArgs) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IBatchPropertyChangedEventHandler, foundation::IUnknown, PMOD_API)
        STDMETHOD(Invoke)(_In_ foundation::IUnknown* pSender, _In_ IBatchPropertyChangedEventArgs* pArgs) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(IEventModelEventArgs, foundation::IInspectable, PMOD_API)
        STDMETHOD(GetEvent)(_Out_ UINT32* pVal) = 0;
        STDMETHOD(GetEventArgs)(_COM_Outptr_ foundation::IInspectable **ppValue) = 0;
    END_DECLARE_INTERFACE()

    DECLARE_EVENT(IEventModel, PMOD_API)

    BEGIN_DECLARE_INTERFACE(IObservableObject, foundation::IObjectDispatch, PMOD_API)
        // Event methods.
        STDMETHOD(GetPropertyChangedEventSource)(_COM_Outptr_ IPropertyChangedEventSource** ppEventSource) = 0;
        STDMETHOD(GetEventModelEventSource)(_COM_Outptr_ IEventModelEventSource** ppEventSource) = 0;
    END_DECLARE_INTERFACE()

    // reserved properties
    const UINT32 Property_Parent = foundation::Property_ResevervedMask + 1;
    const UINT32 Property_Source = foundation::Property_ResevervedMask + 2;
    const UINT32 Property_Root = foundation::Property_ResevervedMask + 3;
    const UINT32 Property_IsInModelChanges = foundation::Property_ResevervedMask + 4;
    const UINT32 Property_All = (UINT32)-1;

    const UINT32 PropertyModel_ViewModel_Start = 0x1000;

}


