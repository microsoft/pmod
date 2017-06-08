/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ViewModelFactory.cpp
****/

#include "pch.h"

#include "ViewModelFactory.h"
#include "CreateFoundationHelper.h"

#include <foundation/library/event_args_base.h>
#include <pmod/library/library_util.h>
#include <pmod/errors.h>
#include <foundation/library/com_ptr_singleton.h>

// {2167C098-CF5D-4FD4-9B3C-064C8E2FCED8}
const GUID pmod::library::IID_IViewModelFactoryClassFactory =
{ 0x2167c098, 0xcf5d, 0x4fd4,{ 0x9b, 0x3c, 0x6, 0x4c, 0x8e, 0x2f, 0xce, 0xd8 } };

using namespace pmod;
using namespace foundation::library;
using namespace pmod::library;

//------------------------------------------------------------------------------
// Class:   CViewModelCreatedEventArgs
//
// Purpose: Implementation of the IViewModelCreatedEventArgs interface
//
//------------------------------------------------------------------------------
class CViewModelCreatedEventArgs :
    public foundation::library::_EventArgsBase<IViewModelCreatedEventArgs>
{
public:
    HRESULT _Initialize(foundation::IObject *pObject)
    {
        m_Object = pObject;
        return S_OK;
    }
protected:
    STDMETHOD(GetObject)(foundation::IObject ** ppVal)
    {
        return m_Object.CopyTo(ppVal);
    }
private:
    foundation::ComPtr<foundation::IObject> m_Object;
};

HRESULT CViewModelFactory::CreateInstance(
    _In_ const ViewModelFactoryCreateParameters *pViewModelFactoryParam,
    _In_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CreateFoundationInstance<CViewModelFactory>(pViewModelFactoryParam,pOuter,ppNewInstance);
}

HRESULT CViewModelFactory::_Initialize(const ViewModelFactoryCreateParameters *pViewModelFactoryParam)
{
    IFR_ASSERT(_ViewModelFactoryBaseType::_Initialize(pViewModelFactoryParam));
    IFR_ASSERT(Initialize(pViewModelFactoryParam->m_p_object_creator_entries));
    return Initialize(
        pViewModelFactoryParam->m_p_single_source_model_factory_entries);
}

HRESULT CViewModelFactory::Initialize(const ObjectCreatorEntry *pCreatorEntries)
{
    if (pCreatorEntries)
    {
        // Register multiple entries
        while (pCreatorEntries->CreatorCallback != nullptr)
        {
            foundation::ComPtr<foundation::ICreateObjectDelegate> spCreator;
            IFR_ASSERT(foundation::library::_CreateObjectCallbackAdapter::CreateInstance(
                pCreatorEntries->CreatorCallback,
                spCreator.GetAddressOf()));

            m_ObjectCreatorMap[pCreatorEntries->IIdType] = spCreator;
            ++pCreatorEntries;
        }
    }
    return S_OK;
}

HRESULT CViewModelFactory::Initialize
    (
    const SingleSourceCreatorEntry *pSingleSourceCreatorEntries
    )
{
    if(pSingleSourceCreatorEntries)
    {
        // Register multiple entries
        while(pSingleSourceCreatorEntries->CreatorCallback != nullptr)
        {
            foundation::ComPtr<pmod::ICreateSingleSourceModelDelegate> spCreator;
            IFR_ASSERT(CreateSingleSourceModelCallbackAdapter::CreateInstance(
                pSingleSourceCreatorEntries->CreatorCallback,
                spCreator.GetAddressOf()));

            m_SingleSourceModelCreatorMap[pSingleSourceCreatorEntries->SourceModelIIdType] = spCreator;
            ++pSingleSourceCreatorEntries;
        }
    }
    return S_OK;
}

STDMETHODIMP CViewModelFactory::CreateObjectInternal(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject)
{
    _ObjectCreatorMap::const_iterator iter = m_ObjectCreatorMap.find(iidType);
    if (iter != m_ObjectCreatorMap.end())
    {
        IFR(iter->second->Invoke(pContextParameter, ppObject));
        return S_OK;
    }
    else
    {
        *ppObject = nullptr;
        return (HRESULT)foundation::Errors::E_OBJECT_ENTRY_NOTFOUND;
    }
}

STDMETHODIMP CViewModelFactory::TryCreateObject(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject)
{
    IFCPTR_ASSERT(ppObject);

    HRESULT hr = CreateObjectInternal(iidType, pContextParameter, ppObject);
    if (hr == foundation::E_OBJECT_ENTRY_NOTFOUND)
        return S_FALSE;
    IFHR(hr);
    foundation_assert(*ppObject);
    _IFR_(RaiseObjectCreated(*ppObject));
    return S_OK;
}

STDMETHODIMP CViewModelFactory::ActivateObject(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject)
{
    IFCPTR_ASSERT(ppObject);

    return CreateObjectInternal(iidType, pContextParameter, ppObject);
}

STDMETHODIMP CViewModelFactory::UnregisterObjectFactory(
    GUID iidClassType)
{
    _ObjectCreatorMap::const_iterator iter = m_ObjectCreatorMap.find(iidClassType);
    if (iter != m_ObjectCreatorMap.end())
    {
        m_ObjectCreatorMap.erase(iter);
        return S_OK;
    }
    return S_FALSE;
}

STDMETHODIMP CViewModelFactory::RegisterObjectFactory(
    GUID iidClassType,
    foundation::ICreateObjectDelegate *pCreator)
{
    m_ObjectCreatorMap[iidClassType] = pCreator;
    return S_OK;
}

STDMETHODIMP CViewModelFactory::RaiseObjectCreated(_In_ foundation::IObject *pObject)
{
    foundation::ComPtr<IViewModelCreatedEventArgs> spObjectCreatedEventArgs;
    IFR_ASSERT(foundation::ctl::inspectable::CreateInstanceWithInitialize<CViewModelCreatedEventArgs>(spObjectCreatedEventArgs.GetAddressOf(),pObject));

    IFR(FireWithCallback(
        &_ViewModelFactoryBaseType::FireEvent,
        spObjectCreatedEventArgs.Get()));

    return S_OK;
}

STDMETHODIMP CViewModelFactory::CreateSingleSourceViewModelInternal(REFIID iidType,foundation::IObject *pModel,pmod::ISourceModel **ppSourceModel)
{
    _SingleSourceModelCreatorMap::const_iterator iter = m_SingleSourceModelCreatorMap.find(iidType);
    if(iter != m_SingleSourceModelCreatorMap.end())
    {
        IFR_ASSERT(iter->second->Invoke(pModel, ppSourceModel));
        return S_OK;
    }
    else
    {
        *ppSourceModel = nullptr;
        return foundation::E_OBJECT_ENTRY_NOTFOUND;
    }
}

STDMETHODIMP CViewModelFactory::TryCreateSingleSourceViewModel(REFIID iidType, foundation::IObject *pModel,pmod::ISourceModel **ppSourceModel)
{
    IFCPTR_ASSERT(ppSourceModel);

    HRESULT hr = CreateSingleSourceViewModelInternal(iidType,pModel,ppSourceModel);
    if (hr == foundation::E_OBJECT_ENTRY_NOTFOUND)
        return S_FALSE;
    IFHR_ASSERT(hr);
    foundation_assert(*ppSourceModel);

    foundation::ComPtr<foundation::IObject> spObject;
    IFR_ASSERT(foundation::QueryInterface(*ppSourceModel, spObject.GetAddressOf()));
    IFR_ASSERT(this->RaiseObjectCreated(spObject));
    return S_OK;
}

STDMETHODIMP CViewModelFactory::RegisterSingleSourceViewModelCreator(GUID iidModelType,pmod::ICreateSingleSourceModelDelegate *pCreator)
{
    m_SingleSourceModelCreatorMap[iidModelType] = pCreator;
    return S_OK;
}

HRESULT CViewModelFactory::GetObjectCreatedEventSource(IViewModelCreatedEventSource** ppEventSource)
{
    IFR_ASSERT(EnsureEventSourceModel());

    *ppEventSource = GetEventSourceImpl();
    (*ppEventSource)->AddRef();
    return S_OK;
}

#if PMOD_LIB_LIBRARY_STATIC_LIB
EXTERN_C void _ModelLibraryInstanceInitializerStub();
#endif

class CViewModelFactoryClassFactory :
    public foundation::ctl::Implements<IViewModelFactoryClassFactory, &IID_IViewModelFactoryClassFactory>
{
public:
    static IViewModelFactoryClassFactory *GetInstance()
    {
        {
#if PMOD_LIB_LIBRARY_STATIC_LIB
            _ModelLibraryInstanceInitializerStub();
#endif
            return _GetFactoryInstance<CViewModelFactoryClassFactory, IViewModelFactoryClassFactory>();
        }
    }

    STDMETHOD(CreateViewModelFactoryInstance)(
        _In_ const ViewModelFactoryCreateParameters *pViewModelFactoryParam,
        _In_opt_ foundation::IInspectable *pOuter,
        _Outptr_ foundation::IInspectable **ppNewInstance) override
    {
        return CViewModelFactory::CreateInstance(pViewModelFactoryParam, pOuter, ppNewInstance);
    }
};

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("pmod.ViewModelFactoryClass"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CViewModelFactoryClassFactory::GetInstance)
);
