/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AppClass.cpp
****/

#include "AppClass.h"
#include "ItemClass.h"

#include <pal/pal_signal.h>

#include <sstream>

#ifdef _DEBUG
#include <foundation/library/logger_util.h>
#endif
#include <foundation/library/mailbox_dispatcher.h>
#include <pmod/library/pmod_lib_api.h>
#include <pmod/library/object_factory.h>

using namespace pmod;

#ifdef _WINDOWS
#define API_EXPORT    __declspec(dllexport) 
#else
#define API_EXPORT
#endif

typedef library::ObjectFactory<CAppClass, DemoApp::AppDemo::IIDType> AppClassFactory_type;
typedef library::ObjectFactory<CItemClass, DemoApp::Item::IIDType> ItemClassFactory_type;

void RegisterDemoAppIf()
{
    static bool bRegistered = false;

    if(!bRegistered)
    {
        bRegistered = true;
        DemoApp::RegisterTypeInfos();
        DemoApp::RegisterModelTypeAdapters();

        // register factory creation
        AppClassFactory_type::RegisterObjectCreatorWithContextValue<foundation::IInspectable *>();
        AppClassFactory_type::RegisterObjectCreator();
    }
}

static HRESULT  CreateDemoApp(DemoApp::IAppDemo **ppAppDemo)
{
    RegisterDemoAppIf();
    return AppClassFactory_type::CreateInstanceAs(ppAppDemo);
}

EXTERN_C API_EXPORT HRESULT  STDAPICALLTYPE CreateDemoApp(IObservableObject **ppObservableObject)
{
    return CreateDemoApp(reinterpret_cast<DemoApp::IAppDemo **>(ppObservableObject));
}

EXTERN_C API_EXPORT HRESULT  STDAPICALLTYPE CreateDemoAppWithDispatcher(
    foundation::library::IDispatcher *pDispatcher,
    IObservableObject **ppObservableObject)
{
    DemoApp::IAppDemo *pAppDemo;
    IFR_ASSERT(CreateDemoApp(&pAppDemo));
    static_cast<CAppClass *>(pAppDemo)->SetLocalWorkingDispatcher(pDispatcher);

    *ppObservableObject = pAppDemo;
    return S_OK;
}

EXTERN_C API_EXPORT HRESULT  STDAPICALLTYPE CreateDemoAppWithMailboxDispatcher(IObservableObject **ppObservableObject)
{
    foundation::ComPtr<foundation::library::IDispatcher> dispatcher_ptr;
    IFR_ASSERT(foundation::library::CreateMailboxDispatcherClass(dispatcher_ptr.GetAddressOf()));

    return CreateDemoAppWithDispatcher(dispatcher_ptr, ppObservableObject);
}

EXTERN_C API_EXPORT HRESULT  STDAPICALLTYPE DemoAppRegister()
{
    RegisterDemoAppIf();
    return S_OK;
}

HRESULT CAppClass::_InitializeModel(foundation::IInspectable *pInspectable)
{
    _IFR_(_InitializeModel());
    if (pInspectable != nullptr)
    {
        foundation::ComPtr<foundation::library::IDispatcher> dispatcherPtr;
        IFR_ASSERT(foundation::QueryInterface(pInspectable, dispatcherPtr.GetAddressOf()));
        this->SetLocalWorkingDispatcher(dispatcherPtr);
    }
    return S_OK;
}

HRESULT CAppClass::_InitializeModel()
{
    IFR_ASSERT(DemoApp::_FastAppDemoBase::_InitializeModel());

    // enable Clear items by default
    this->_clearItemsValue->RaiseCanExecuteChanged(true);

    IFR_ASSERT(library::CreateObservableCollection(
        DemoApp::ObservableCollection_IItem::IIDType,
         (library::ObservableCollectionOptions)((UINT32)library::ObservableCollectionOptions::IsReadOnly | foundation::library::FoundationClassOptions_IsFreeThreadApartment),
        _itemsClassPtr.GetAddressOf()));

    IFR_ASSERT(this->SetItems(_itemsClassPtr));
    return S_OK;
}

HRESULT CAppClass::ExecuteAddItem(HSTRING parameter)
{
    foundation::ComPtr<DemoApp::IItem> spItem;
    IFR_ASSERT(ItemClassFactory_type::CreateInstanceAs(spItem.GetAddressOf(),parameter));

    // we need to use the Internal API since the Collection is read only
    IFR_ASSERT(_itemsClassPtr.AppendItemInternal(spItem));
    return S_OK;
}
HRESULT CAppClass::ExecuteAddNextItem()
{
    foundation::ostringstream_t os;    
    os << U("item:") << _itemsClassPtr.GetCount();

    return ExecuteAddItem(foundation::HStringPtr(os.str().c_str()));
}

HRESULT CAppClass::CanExecuteAddItem(HSTRING parameter,bool* pValue)
{
    foundation_assert(pValue);
    * pValue = _pal_GetStringLen(parameter) > 0;
    return S_OK;
}

HRESULT CAppClass::ExecuteRemoveItem(DemoApp::IItem * parameter)
{
    if(parameter != nullptr)
    {
        IFR_ASSERT(_itemsClassPtr.RemoveItem(parameter));
    }

    return S_OK;
}

HRESULT CAppClass::CanExecuteRemoveItem(DemoApp::IItem * parameter,bool* pValue)
{
    foundation_assert(pValue);
    * pValue = parameter != nullptr;
    return S_OK;
}

HRESULT CAppClass::ExecuteClearItems()
{
    IFR_ASSERT(_itemsClassPtr.RemoveAll());
    return S_OK;
}

HRESULT CAppClass::CreateItemAsyncInternal(HSTRING name, const foundation::AsyncOperationClassPtr& async_operation_class_ptr)
{
#if 1
    _PAL_SIGNAL evt;
    _pal_CreateSignal(&evt);
    _pal_WaitSignalTimed(&evt,1000);

    foundation::ComPtr<DemoApp::IItem> spItem;
    IFR_ASSERT(ItemClassFactory_type::CreateInstanceAs(spItem.GetAddressOf(), name));
#endif

#if 1
    async_operation_class_ptr.SetCompleted(S_OK, spItem);
#else
    async_operation_class_ptr.SetCompleted(-40000, nullptr);
#endif
    return S_OK;
}
