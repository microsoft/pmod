/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectFactory.cpp
****/

#include "pch.h"

#include "ObjectFactory.h"

#include <foundation/library/logger_util.h>
#include <foundation/library/object_factory_util.h>
#include <foundation/errors.h>
#include <foundation/library/com_ptr_singleton.h>

using namespace foundation;

// {AC2994FE-F596-493F-A42D-1E87D45B189B}
const GUID foundation::IID_IObjectFactory =
{ 0xac2994fe, 0xf596, 0x493f,{ 0xa4, 0x2d, 0x1e, 0x87, 0xd4, 0x5b, 0x18, 0x9b } };

EXTERN_C FOUNDATION_API CObjectFactory *STDMETHODCALLTYPE GetObjectFactoryInstance()
{
    return static_cast<CObjectFactory *>(CObjectFactory::GetInstance());
}

HRESULT CObjectFactory::SetActivateDefaultObjectCallback(ActivateDefaultObjectCallback_Type callback)
{
    m_activateDefaultCallback = callback;
    return S_OK;
}

ILogCategory*CObjectFactory::GetLogCategory()
{
    if (!logger_util::IsAvailable())
    {
        return nullptr;
    }
    static library::_ComPtrSingleton<ILogCategory> _objectBaseLogCategoryInstance;

    return _objectBaseLogCategoryInstance._GetInstance([&]() {
        ILogCategory *pLogCategory = nullptr;
        logger_util::GetCategory(
            logger_util::GetFoundationLogger(),
            &pLogCategory,
            Foundation_CategoryEnum::TypeId,
            Object_CategoryEnum::TypeId,
            (UINT32)-1);
        return pLogCategory;
    });
}

IObjectFactory *CObjectFactory::GetInstance()
{
    return library::_GetFactoryInstance<CObjectFactory, IObjectFactory>();
}

HRESULT CObjectFactory::CreateObjectInternal(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject)
{
    ILogCategory *pObjectBaseCategory = GetLogCategory();
    if (pObjectBaseCategory != nullptr &&
        logger_util::IsDebugEnabled(
            pObjectBaseCategory,
            ObjectFactory_Category_CreateObject_Id))
    {
        logger_util::Log(
            logger_util::GetFoundationLogger(),
            this->CastToInspectable(),
            foundation::LoggingLevel::Debug,
            // Message Enum Value
            ObjectFactory_Category_CreateObject_Id,
            // Log Category
            pObjectBaseCategory,
            ObjectFactory_LogRecordEnum::TypeId,
            // TypeIID
            ObjectFactory_LogRecord::TypeIID,
            foundation::CreateValue(iidType).Get(),
            // Context
            ObjectFactory_LogRecord::Context,
            // Note: avoid using the Context directly unitl we resolve the recursive problem
            // when Dictionaries have reference to parent in some leaves
            nullptr,
            -1);
    }

    _ObjectCreatorMap_type::const_iterator iter = m_ObjectCreatorMap.find(iidType);
    if (iter != m_ObjectCreatorMap.end())
    {
        _IFR_(iter->second->Invoke(pContextParameter, ppObject));
        return S_OK;
    }
    else
    {
        *ppObject = nullptr;
        return (HRESULT)foundation::Errors::E_OBJECT_ENTRY_NOTFOUND;
    }
}

STDMETHODIMP CObjectFactory::ActivateObject(
    REFIID iidType,
    _In_ foundation::IInspectable *pContextParameter,
    _COM_Outptr_ foundation::IObject **ppObject)
{
    foundation_assert(ppObject);

    // first attempt the model factory
    HRESULT hr = TryCreateObject(
        iidType,
        pContextParameter,
        ppObject);
    _IFHR_(hr);
    if (hr == S_OK)
    {
        return S_OK;
    }
    if (m_activateDefaultCallback != nullptr)
    {
        return (*m_activateDefaultCallback)(iidType, pContextParameter, ppObject);
    }
    return E_NOTIMPL;
}

STDMETHODIMP CObjectFactory::TryCreateObject(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject)
{
    IFCPTR_ASSERT(ppObject);

    HRESULT hr = CreateObjectInternal(iidType, pContextParameter, ppObject);
    if (hr == foundation::E_OBJECT_ENTRY_NOTFOUND)
        return S_FALSE;
    _IFHR_(hr);
    foundation_assert(*ppObject);
    return S_OK;
}

STDMETHODIMP CObjectFactory::RegisterObjectFactory(
    GUID iidClassType,
    ICreateObjectDelegate *pCreator)
{
    ILogCategory *pObjectBaseCategory = GetLogCategory();
    if (pObjectBaseCategory != nullptr &&
        logger_util::IsDebugEnabled(
            pObjectBaseCategory,
            ObjectFactory_Category_RegisterObject_Id))
    {
        logger_util::Log(
            logger_util::GetFoundationLogger(),
            this->CastToInspectable(),
            foundation::LoggingLevel::Debug,
            // Message Enum Value
            ObjectFactory_Category_RegisterObject_Id,
            // Log Category
            pObjectBaseCategory,
            ObjectFactory_LogRecordEnum::TypeId,
            // TypeIID
            ObjectFactory_LogRecord::TypeIID,
            foundation::CreateValue(iidClassType).Get(),
            -1);
    }

    m_ObjectCreatorMap[iidClassType] = pCreator;
    return S_OK;
}

STDMETHODIMP CObjectFactory::UnregisterObjectFactory(GUID iidClassType)
{
    _ObjectCreatorMap_type::const_iterator iter = m_ObjectCreatorMap.find(iidClassType);
    if (iter != m_ObjectCreatorMap.end())
    {
        m_ObjectCreatorMap.erase(iter);
        return S_OK;
    }
    return S_FALSE;
}

class CObjectFactoryFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::IObjectFactoryFactory
{
public:
    static foundation::IObjectFactoryFactory *GetInstance()
    {
        return library::_GetFactoryInstance<CObjectFactoryFactory, IObjectFactoryFactory>();
    }

    STDMETHOD(CreateObjectFactory)(IObjectFactory **ppNewInstance)
    {
        IFCPTR_ASSERT(ppNewInstance);

        CObjectFactory *pObjectFactory;
        IFR_ASSERT(ctl::inspectable::CreateInstance(&pObjectFactory));
        *ppNewInstance = pObjectFactory;
        return S_OK;
    }
};

static foundation::library::_RegisterActivationFactory objectFactoryRegister(
    U("foundation.ObjectFactory"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObjectFactory::GetInstance)
    );

static foundation::library::_RegisterActivationFactory objectFactoryFactoryRegister(
    U("foundation.ObjectFactoryFactory"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObjectFactoryFactory::GetInstance)
    );

