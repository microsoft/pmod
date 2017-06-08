/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DemoApp.cpp
****/

#include "DemoAppTypeInfo.g.h"
#include "DemoAppInterfaces.g.h"
#include "DemoAppImplInternal.g.h"

#include <foundation/ctl/com_weak_reference_source.h>
#include <pmod/library/observable_collection_impl.h>
#include <pmod/library/command_impl.h>

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::library;

#ifdef _WINDOWS
#define API_EXPORT    __declspec(dllexport) 
#else
#define API_EXPORT
#endif

void RegisterDemoAppIf()
{
    static bool bRegistered = false;

    if(!bRegistered)
    {
        bRegistered = true;
        DemoApp::RegisterTypeInfos();
        DemoApp::RegisterModelTypeAdapters();
    }
}

class CItemImpl :
    public DemoApp::_ItemImplBase<>
{
public:
    HRESULT _Initialize(HSTRING name)
    {
        _name = name;
        return S_OK;
    }
protected:
    HRESULT SetName(HSTRING value) override
    {
        _name = value;
        return this->FirePropertyChangedWithCallback(DemoApp::Item::Property_Name);
    }
    
    HRESULT GetName(HSTRING* pValue) override
    {
        return _name.CopyTo(pValue);
    }
private:
    HStringPtr _name;
};

typedef ComPtr<CItemImpl> CItemImplPtr;
typedef _ObservableVectorImpl < CItemImplPtr, DemoApp::ObservableCollection_IItem::IIDType> ObservableVectorItemType;

class CAppDemoImpl;

class CAddItemCommand :
    public _WeakRefCommandImpl<CAppDemoImpl,_CommandParameterImplBase<DemoApp::ICommand_string_void_canExecute>>
{
public:
    STDMETHOD(Execute)(HSTRING parameter) override;
};

class CClearItemsCommand :
    public _WeakRefCommandImpl<CAppDemoImpl, _SimpleCommandImplBase>
{
public:
    STDMETHOD(Execute)() override;
};

class CAppDemoImpl :
    public DemoApp::_AppDemoImplBase
    <
        DemoApp::IAppDemo,
        _ObservableObjectImplBase<DemoApp::IAppDemo,ctl::ComWeakReferenceSourceInspectableBase>
    >
{
public:
    HRESULT _Initialize()
    {
        IFR_ASSERT(ctl::inspectable::CreateInstance(_itemsPtr.GetAddressOf()));
        IFR_ASSERT(ctl::inspectable::CreateInstanceWithInitialize(_addItemCommandPtr.GetAddressOf(),this));
        IFR_ASSERT(ctl::inspectable::CreateInstanceWithInitialize(_clearItemsCommandPtr.GetAddressOf(), this));

        _addItemCommandPtr->set_command_enabled(true);
        _clearItemsCommandPtr->set_command_enabled(true);
        return S_OK;
    }

    HRESULT GetItems(_COM_Outptr_result_maybenull_ DemoApp::ICollection_IItem ** value) override
    {
        return foundation::QueryInterface(_itemsPtr->CastToInspectable(), value);
    }

    HRESULT GetAddItem(_COM_Outptr_result_maybenull_ DemoApp::ICommand_string_void_canExecute ** value) override
    {
        *value = _addItemCommandPtr;
        (*value)->AddRef();
        return S_OK;
    }

    HRESULT GetClearItems(_COM_Outptr_result_maybenull_ pmod::ISimpleCommand ** value) override
    {
        *value = _clearItemsCommandPtr;
        (*value)->AddRef();
        return S_OK;
    }

    HRESULT CreateItemAsync(HSTRING name, _COM_Outptr_ DemoApp::IAsyncOperation_IItem* *pResult) override
    {
        return E_NOTIMPL;
    }
private:
    friend class CAddItemCommand;
    friend class CClearItemsCommand;
    HRESULT AddItem(HSTRING name)
    {
        CItemImplPtr ptrClass;
        IFR_ASSERT(ctl::inspectable::CreateInstanceWithInitialize(ptrClass.GetAddressOf(), name));
        _itemsPtr->push_back(ptrClass);
        return S_OK;
    }
    HRESULT ClearItems()
    {
        _itemsPtr->clear();
        return S_OK;
    }
protected:
    ComPtr<ObservableVectorItemType> _itemsPtr;
    ComPtr<CAddItemCommand> _addItemCommandPtr;
    ComPtr<CClearItemsCommand> _clearItemsCommandPtr;
    
};

STDMETHODIMP CAddItemCommand::Execute(HSTRING param)
{
    CAppDemoImpl *pTarget = this->get_target();
    if (pTarget != nullptr)
    {
        return pTarget->AddItem(param);
    }
    return S_FALSE;
}

STDMETHODIMP CClearItemsCommand::Execute()
{
    CAppDemoImpl *pTarget = this->get_target();
    if (pTarget != nullptr)
    {
        return pTarget->ClearItems();
    }
    return S_FALSE;
}


static HRESULT  CreateDemoApp(DemoApp::IAppDemo **ppAppDemo)
{
    RegisterDemoAppIf();
    CAppDemoImpl *pT;
    IFR_ASSERT(ctl::inspectable::CreateInstanceWithInitialize(&pT));
    *ppAppDemo = pT;
    return S_OK;
}

EXTERN_C API_EXPORT HRESULT  STDAPICALLTYPE CreateDemoAppImpl(IObservableObject **ppObservableObject)
{
    return CreateDemoApp(reinterpret_cast<DemoApp::IAppDemo **>(ppObservableObject));
}
