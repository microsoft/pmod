/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoDispatcherStatics.cpp
****/

#include "pch.h"

#include "MoDispatcherAdapter.h"
#include "MoDispatcherStatics.h"
#include "FtmBase.h"
#include "windows.system.threading.h"
#include "roapi.h"

#include "FreeThreadedMarshalerBase.h"

//------------------------------------------------------------------------------
// Class:   CDispatchedHandler
//
// Purpose: A winRT implementation of IDispatchedHandler that use the 
// IDelegateDispatchable as the callback
//
//------------------------------------------------------------------------------
class CDispatchedHandler : 
    public _FtmBase,
    public ABI::Windows::UI::Core::IDispatchedHandler
{
public:
    static HRESULT CreateInstance(
        foundation::library::IDelegateDispatchable *pDelegateDispatchable,
            ABI::Windows::UI::Core::IDispatchedHandler** ppDispatchedHandler
        )
    {
        foundation::ComPtr<foundation::ctl::ComObject<CDispatchedHandler>> spDispatchedHandler;
        IFR(foundation::ctl::ComInspectableObject<CDispatchedHandler>::CreateInstance(spDispatchedHandler.GetAddressOf()));

        spDispatchedHandler->m_spDispatchable = pDelegateDispatchable;
        *ppDispatchedHandler = spDispatchedHandler.Detach();
        return S_OK;
    }
protected:
    HRESULT STDMETHODCALLTYPE Invoke( void)
    {
        return m_spDispatchable->Invoke();
    }
private:
    foundation::ComPtr<foundation::library::IDelegateDispatchable> m_spDispatchable;
};
//+-----------------------------------------------------------------------------
// Class:   CDispatcherAdapterBase
//
// Purpose: Dispatcher Adapter for winRT ICoreDispatcher
//------------------------------------------------------------------------------
class CDispatcherAdapterBase : 
    public _TFreeThreadedMarshalerComBase<MoDispatcherAdapterBase<CDispatcherAdapterBase>>,
    public foundation::library::IDispatcher
{
public:
    foundation::library::IDispatcher *GetCoreDispatcher()
    {
        return static_cast<foundation::library::IDispatcher *>(this);
    }
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IID_IDispatcher)
        {
            *ppInterface = static_cast<foundation::library::IDispatcher *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
};

//+-----------------------------------------------------------------------------
// Class:   CCoreDispatcherAdapter
//
// Purpose: Dispatcher Adapter for winRT ICoreDispatcher
//------------------------------------------------------------------------------
class CCoreDispatcherAdapter : 
    public CDispatcherAdapterBase
{
public:
    static HRESULT 
    CreateInstance(
        ABI::Windows::UI::Core::ICoreDispatcher *pCoreDispatcher,
        ABI::Microsoft::PropertyModel::IDispatcher **instance
        )
    {
        foundation::ComPtr<foundation::ctl::ComInspectableObject<CCoreDispatcherAdapter>> spCoreDispatcherAdapter;
        IFR(foundation::ctl::ComInspectableObject<CCoreDispatcherAdapter>::CreateInstance(spCoreDispatcherAdapter.GetAddressOf()));

        spCoreDispatcherAdapter->m_spCoreDispatcher = pCoreDispatcher;
        *instance = spCoreDispatcherAdapter.Detach();
        return S_OK;
    }
protected:
    HRESULT STDMETHODCALLTYPE HasThreadAccess(bool *pValue)
    {
        IFCPTR(pValue);

        boolean value;
        IFR(m_spCoreDispatcher->get_HasThreadAccess(&value));
        *pValue = value ? true:false;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE RunAsync( 
        foundation::library::IDelegateDispatchable *pDispatchable,
        foundation::IAsyncOperation **ppAsyncAction) override
    {
        foundation::ComPtr<ABI::Windows::UI::Core::IDispatchedHandler> spDispatchedHandler;
        IFR(CDispatchedHandler::CreateInstance(pDispatchable,spDispatchedHandler.GetAddressOf()));
        foundation::ComPtr<ABI::Windows::Foundation::IAsyncAction> spAsyncAction;
        HRESULT hr = m_spCoreDispatcher->RunAsync(ABI::Windows::UI::Core::CoreDispatcherPriority_Normal, spDispatchedHandler,spAsyncAction.GetAddressOf());
        // TODO:
        *ppAsyncAction = nullptr;
        return hr;
    }
private:
    foundation::ComPtr<ABI::Windows::UI::Core::ICoreDispatcher> m_spCoreDispatcher;
};

//+-----------------------------------------------------------------------------
// Class:   CThreadingPoolDispatcherAdapter
//
// Purpose: Dispatcher Adapter for Windows::System::Threading::ThreadPool
//------------------------------------------------------------------------------
class CThreadingPoolDispatcherAdapter : 
    public CDispatcherAdapterBase
{
public:
    static HRESULT 
    CreateInstance(
        ABI::Microsoft::PropertyModel::IDispatcher **instance
        )
    {
        typedef foundation::ctl::ComInspectableObject<CThreadingPoolDispatcherAdapter> CThreadingPoolDispatcherAdapter_Com_Type;
        foundation::ComPtr<CThreadingPoolDispatcherAdapter_Com_Type> spThreadingPoolDispatcherAdapter;
        IFR(CThreadingPoolDispatcherAdapter_Com_Type::CreateInstance(spThreadingPoolDispatcherAdapter.GetAddressOf()));

        *instance = spThreadingPoolDispatcherAdapter.Detach();
        return S_OK;
    }
protected:
    class CWorkItemHandler : 
        public ImplementsBase<ABI::Windows::System::Threading::IWorkItemHandler,_FtmBase,&__uuidof(ABI::Windows::System::Threading::IWorkItemHandler)>
    {
public:
    static HRESULT 
    CreateInstance(
    foundation::library::IDelegateDispatchable *pDispatchable,
        ABI::Windows::System::Threading::IWorkItemHandler **ppWorkItemHandler
        )
    {
        typedef foundation::ctl::ComInspectableObject<CWorkItemHandler> CWorkItemHandler_Com_Type;
        foundation::ComPtr<CWorkItemHandler_Com_Type> spWorkItemHandler;
        IFR(CWorkItemHandler_Com_Type::CreateInstance(spWorkItemHandler.GetAddressOf()));

        spWorkItemHandler->_spDispatchable = pDispatchable;
        *ppWorkItemHandler = spWorkItemHandler.Detach();
        return S_OK;
    }
    protected:
        HRESULT STDMETHODCALLTYPE Invoke( 
                ABI::Windows::Foundation::IAsyncAction *operation)
        {
            UNREFERENCED_PARAMETER(operation);
            return _spDispatchable->Invoke();
        }
    private:
        foundation::ComPtr<foundation::library::IDelegateDispatchable> _spDispatchable;
    };

    HRESULT STDMETHODCALLTYPE HasThreadAccess(bool *pValue)
    {
        IFCPTR(pValue);
        *pValue = false;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE RunAsync( 
        foundation::library::IDelegateDispatchable *pDispatchable,
        foundation::IAsyncOperation **ppAsyncAction) override
    {
        foundation::HStringRef activationId(RuntimeClass_Windows_System_Threading_ThreadPool);
        foundation::ComPtr<ABI::Windows::System::Threading::IThreadPoolStatics> spThreadPoolStatics;

        IFR_ASSERT(Windows::Foundation::GetActivationFactory(activationId,spThreadPoolStatics.GetAddressOf()));
        foundation::ComPtr<ABI::Windows::System::Threading::IWorkItemHandler> spWorkItemHandler;
        IFR_ASSERT(CWorkItemHandler::CreateInstance(pDispatchable,spWorkItemHandler.GetAddressOf()));

        foundation::ComPtr<ABI::Windows::Foundation::IAsyncAction> spAsyncAction;

        return spThreadPoolStatics->RunAsync(spWorkItemHandler,spAsyncAction.GetAddressOf());
    }
private:
    foundation::ComPtr<ABI::Windows::UI::Core::ICoreDispatcher> m_spCoreDispatcher;
};

STDMETHODIMP MoDispatcherStatics::CreateDispatcher( 
    ABI::Windows::UI::Core::ICoreDispatcher *coreDisptacher,
    IDispatcher **instance)
{
    return CCoreDispatcherAdapter::CreateInstance(coreDisptacher,instance);
}

STDMETHODIMP MoDispatcherStatics::CreateThreadingPoolDispatcher( 
    IDispatcher **instance)
{
    return CThreadingPoolDispatcherAdapter::CreateInstance(instance);

}
