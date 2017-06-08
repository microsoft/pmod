/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ViewModelFactory.h
****/

#pragma once

#include "FoundationBase.h"
#include <pmod/library/model_library.h>

typedef
_FoundationBase
<
    pmod::IViewModelFactory,
    pmod::library::IViewModelFactoryClass,
    pmod::library::IViewModelFactoryDelegate,
    pmod::library::ViewModelFactoryCreateParameters,
    pmod::IViewModelCreatedEventSource,
    pmod::IViewModelCreatedEventHandler,
    foundation::IUnknown,
    pmod::IViewModelCreatedEventArgs
>
_ViewModelFactoryBaseType;

class CViewModelFactory :
    public _ViewModelFactoryBaseType
{
 public:
    static HRESULT CreateInstance(
        _In_ const pmod::library::ViewModelFactoryCreateParameters *pViewModelFactoryParam,
        _In_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

    HRESULT _Initialize(const pmod::library::ViewModelFactoryCreateParameters *pViewModelFactoryParam);

protected:
    CViewModelFactory()
    {
    }

    #if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override
    {
        _traceMessage(U("ViewModelFactory this:%04x"),this);
    }

    #endif
    HRESULT Initialize
        (const pmod::library::ObjectCreatorEntry *pCreatorEntries);

    HRESULT Initialize(
        const pmod::SingleSourceCreatorEntry *pSingleSourceCreatorEntries);

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::IObjectFactoryBase::GetIID())
        {
            *ppInterface = static_cast<foundation::IObjectFactoryBase *>(this);
        }
        else if (iid == foundation::IObjectFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::IObjectFactory *>(this);
        }
        else if (iid == pmod::IViewModelFactory::GetIID())
        {
            *ppInterface = static_cast<pmod::IViewModelFactory *>(this);
        }
        else if (iid == pmod::library::IViewModelFactoryClass::GetIID())
        {
            *ppInterface = static_cast<pmod::library::IViewModelFactoryClass *>(this);
        }
        else
        {
            return _ViewModelFactoryBaseType::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
    // IObjectFactory Interface
    STDMETHOD(RegisterObjectFactory)(GUID iidClassType, foundation::ICreateObjectDelegate *pCreator) override;
    STDMETHOD(UnregisterObjectFactory)(GUID iidClassType) override;
    STDMETHOD(TryCreateObject)(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject);
    STDMETHOD(ActivateObject)(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject);

    // IViewModelFactory Interface
    STDMETHOD(RegisterSingleSourceViewModelCreator)(GUID iidModelType,pmod::ICreateSingleSourceModelDelegate *pCreator);
    STDMETHOD(TryCreateSingleSourceViewModel)(REFIID iidType, foundation::IObject *pModel,pmod::ISourceModel **ppSourceModel);
    STDMETHOD(GetObjectCreatedEventSource)(pmod::IViewModelCreatedEventSource** ppEventSource);
    // IViewModelFactoryClass Interface
    STDMETHOD(CreateObjectInternal)(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject);
    STDMETHOD(RaiseObjectCreated)(_In_ foundation::IObject *pObject);

    STDMETHOD(CreateSingleSourceViewModelInternal)(REFIID iidType, foundation::IObject *pModel,pmod::ISourceModel **ppSourceModel);

private:
    HRESULT CreateSingleSourceViewModelInternal(
        const IID& iid,
        pmod::IObservableObject *pSource,
        pmod::IObservableObject **ppNewInstance);

private:
    typedef std::map<IID, foundation::ComPtr<foundation::ICreateObjectDelegate>, foundation::library::GuidLessThan>  _ObjectCreatorMap;
    typedef std::map<IID, foundation::ComPtr<pmod::ICreateSingleSourceModelDelegate>, foundation::library::GuidLessThan>  _SingleSourceModelCreatorMap;

    _ObjectCreatorMap               m_ObjectCreatorMap;
    _SingleSourceModelCreatorMap       m_SingleSourceModelCreatorMap;
};


