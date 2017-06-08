/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObjectAdapter.h
****/

#pragma once

#include "Microsoft.PropertyModel.h"

#include "MoDispatcherAdapter.h"
#include <foundation/library/logger_macros.h>

#include "..\moproxy\MoCoreEventHandlerAdapter.h"

// Public Headers
#include <foundation/library/adapter_base.h>
#include <foundation/com_ptr.h>

using namespace ABI::Microsoft::PropertyModel;

#include <map>

//------------------------------------------------------------------------------
// Class:   CMoObjectAdapter
//
// Purpose: Implementation of IObject Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoObjectAdapter :
    public foundation::AdapterBase
        <
        foundation::IObject,
        IObject,
        &__uuidof(IObject)
        >
{
protected:
    HRESULT STDMETHODCALLTYPE get_UniqueId( 
                            unsigned int *uniqueId) override
    {
        return m_pInner->GetUniqueId(uniqueId);
    }
                        
    HRESULT STDMETHODCALLTYPE get_ProcessId(unsigned int *processId) override
    {
        return m_pInner->GetProcessId(processId);
    }
                        
    HRESULT STDMETHODCALLTYPE get_ObjectTypeInfo(IObjectTypeInfo **ppModelTypeInfo)
    {
        foundation::ComPtr<foundation::IObjectTypeInfo> spModelTypeInfo;
        IFR(m_pInner->GetTypeInfo(spModelTypeInfo.GetAddressOf()));
        if(spModelTypeInfo != nullptr)
        {
            return foundation::QueryInterface(spModelTypeInfo,ppModelTypeInfo);
        }
        else
        {
            *ppModelTypeInfo = nullptr;
            return S_OK;
        }
    }

};

////------------------------------------------------------------------------------
//// Class:   CMoObjectNodeClassAdapter
////
//// Purpose: Implementation of IObjectNodeClass Interface for WinRT
////
////------------------------------------------------------------------------------
//class CMoObjectClassAdapter :
//    public foundation::AdapterBase
//        <
//        foundation::library::IFoundationClassBase,
//        IObjectClass,
//        &__uuidof(IObjectClass)
//        >
//{
//    // Interface IModelProtected support
//    HRESULT STDMETHODCALLTYPE get_IsObserved( 
//                        boolean *value) override
//    {
//        bool isObserved;
//        IFR_ASSERT(m_pInner->GetIsObserved(&isObserved));
//        *value = isObserved ? true:false;
//        return S_OK;
//    }
//
//    HRESULT STDMETHODCALLTYPE get_FoundationOptions(
//        unsigned short *value) override
//    {
//        return m_pInner->GetFoundationOptions(value);
//    }
//
//    HRESULT STDMETHODCALLTYPE get_EventDispatcher( 
//        IDispatcher **ppDispatcher) override
//    {
//        IFCPTR(ppDispatcher);
//
//        foundation::ComPtr<foundation::library::IDispatcher> spDispatcher;
//        IFR(m_pInner->GetEventDispatcher(spDispatcher.GetAddressOf()));
//        if(spDispatcher != nullptr)
//        {
//            return MoDispatcherAdapter::CreateInstance(spDispatcher,ppDispatcher);
//        }
//        else
//        {
//            *ppDispatcher = nullptr;
//            return S_OK;
//        }
//    }
//
//    HRESULT STDMETHODCALLTYPE put_EventDispatcher( 
//        IDispatcher *pDispatcher) override
//    {
//        foundation::ComPtr<foundation::library::IDispatcher> spDispatcher;
//        if(pDispatcher != nullptr)
//        {
//            IFR_ASSERT(CDispatcherAdapter::CreateInstance(pDispatcher, spDispatcher.GetAddressOf()));
//        }
//        IFR_ASSERT(m_pInner->SetEventDispatcher(spDispatcher));
//        return S_OK;
//    }
//};

//------------------------------------------------------------------------------
// Class:   CMoEventSourceClassAdapter
//
// Purpose: Implementation of IEventSourceClass Interface for WinRT
//
//------------------------------------------------------------------------------
class CMoEventSourceClassAdapter :
    public foundation::AdapterBase
    <
    foundation::library::IEventSourceClass,
    IEventSourceClass,
    &__uuidof(IEventSourceClass)
    >
{
protected:
    HRESULT STDMETHODCALLTYPE AddEventHandler(
        GUID eventSourceType,
        _In_opt_ IDispatchable *pEventHandler,
        _In_opt_ IInspectable *pOptions,
        _COM_Outptr_ IInspectable **ppResult)override
    {
        foundation::ComPtr<IUnknown> spCoreEventHandler;
        if (pEventHandler != nullptr)
        {
            // TODO: we would need a generic mechanism to pull this from a factory and not hard code
            // the known supported event types
            if (eventSourceType == pmod::INotifyCollectionChangedEventSource::GetIID())
            {
                IFR_ASSERT(create_adapter_proxy_instance<CMoNotifyCollectionChangedHandlerAdapter>(
                    static_cast<IUnknown *>(pEventHandler),
                    spCoreEventHandler.GetAddressOf()
                    ));
            }
            else
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }
            foundation_assert(spCoreEventHandler != nullptr);
            _dispatchables[pEventHandler] = spCoreEventHandler;
        }
        IFR_ASSERT(m_pInner->AddEventHandler(eventSourceType, spCoreEventHandler, pOptions,ppResult));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE RemoveEventHandler(
        GUID eventSourceType,
        IDispatchable *pEventHandler)
    {
        _Dispatachable_Map_Type::const_iterator iter = _dispatchables.find(pEventHandler);
        if (iter != _dispatchables.end())
        {
            HRESULT hr = m_pInner->RemoveEventHandler(eventSourceType, (*iter).second);
            _dispatchables.erase(iter);
            return hr;
        }
        return E_UNEXPECTED;
    }
private:
    typedef std::map<foundation::ComPtr<IDispatchable>, foundation::ComPtr<IUnknown>> _Dispatachable_Map_Type;
    _Dispatachable_Map_Type _dispatchables;
};
