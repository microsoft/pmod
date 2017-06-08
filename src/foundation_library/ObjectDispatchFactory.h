/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchFactory.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/interfaces/object_dispatch_factory.h>
#include <foundation/library/base_adapter_host.h>

class CObjectDispatchFactory :
    public foundation::library::_DefaultAdapterHostType<foundation::library::IObjectDispatchFactory, foundation::ctl::ComInspectableBase, CObjectDispatchFactory>,
    public foundation::library::IObjectDispatchFactory
{
 public:
     static foundation::library::IObjectDispatchFactory *GetInstance();
         //Interface IModelFactory
	 STDMETHOD(CreateObjectDispatch)(
		 _In_ const foundation::library::ObjectDispatchCreateParameters *pCreateParams,
		 _In_opt_ foundation::IInspectable *pOuter,
		 _COM_Outptr_ foundation::IInspectable **ppNewInstance) override;
	 STDMETHOD(CreatePropertyIdPair)(
		 UINT32 propertyId,
		 _In_opt_ foundation::IInspectable *pValue,
		 _COM_Outptr_ foundation::IPropertyIdPair **ppPropertyIdPair) override;

protected:
	typedef foundation::library::_DefaultAdapterHostType<foundation::library::IObjectDispatchFactory, foundation::ctl::ComInspectableBase, CObjectDispatchFactory> _Base_Type;
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IObjectDispatchFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IObjectDispatchFactory *>(this);
        }
        else
        {
            return _Base_Type::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

};


