/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryFactory.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/interfaces/dictionary_factory.h>
#include <foundation/library/base_adapter_host.h>

class CDictionaryFactory :
    public foundation::library::_DefaultAdapterHostType<foundation::library::IDictionaryFactory, foundation::ctl::ComInspectableBase, CDictionaryFactory>,
    public foundation::library::IDictionaryFactory
{
 public:
     static foundation::library::IDictionaryFactory *GetInstance();
     static HRESULT GetOrCreateInstance(foundation::library::IDictionaryFactory **ppDictionaryFactory);
     //Interface IDictionaryFactory
     STDMETHOD(CreateDictionary)(_COM_Outptr_ foundation::IDictionary **ppNewInstance) override;
     STDMETHOD(CreateDictionaryPair)(
         HSTRING key,
         _In_opt_ foundation::IInspectable *pValue,
         _COM_Outptr_ foundation::IDictionaryPair **ppNewInstance) override;
protected:
    typedef foundation::library::_DefaultAdapterHostType<foundation::library::IDictionaryFactory, foundation::ctl::ComInspectableBase, CDictionaryFactory> _Base_Type;
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IDictionaryFactory::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IDictionaryFactory *>(this);
        }
        else
        {
            return _Base_Type::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

};


