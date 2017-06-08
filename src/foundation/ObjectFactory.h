/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectFactory.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/foundation_library.h>
#include <foundation/library/util_stl.h>
#include <map>

typedef HRESULT(*ActivateDefaultObjectCallback_Type)(
    REFIID iidType,
    foundation::IInspectable *pContextParameter,
    foundation::IObject **ppObject);

class CObjectFactory :
    public foundation::ctl::ComInspectableBase,
    public foundation::IObjectFactory
{
public:
    static foundation::IObjectFactory *GetInstance();
    // Note : marked as virtual to allow a direct jump without any exported symbol
    virtual HRESULT SetActivateDefaultObjectCallback(ActivateDefaultObjectCallback_Type callback);
protected:
    CObjectFactory():
        m_activateDefaultCallback(nullptr)
    {
    }

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
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
public:
    // IObjectFactory Interface
    STDMETHOD(RegisterObjectFactory)(GUID iidClassType, foundation::ICreateObjectDelegate *pCreator) override;
    STDMETHOD(UnregisterObjectFactory)(GUID iidClassType) override;
    STDMETHOD(TryCreateObject)(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject);
    STDMETHOD(ActivateObject)(
        REFIID iidType,
        foundation::IInspectable *pContextParameter,
        foundation::IObject **ppObject) override;


private:
    HRESULT CreateObjectInternal(REFIID iidType, foundation::IInspectable *pContextParameter, foundation::IObject **ppObject);
    static foundation::ILogCategory* GetLogCategory();

private:
    typedef std::map<IID, foundation::ComPtr<foundation::ICreateObjectDelegate>, foundation::library::GuidLessThan>  _ObjectCreatorMap_type;

    _ObjectCreatorMap_type              m_ObjectCreatorMap;
    ActivateDefaultObjectCallback_Type  m_activateDefaultCallback;
};

