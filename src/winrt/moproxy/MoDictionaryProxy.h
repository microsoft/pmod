/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoDictionaryProxy.h
****/

#pragma once

#include <foundation/assert.h>

#include "Microsoft.PropertyModel.h"
#include "MoAdapterProxyInterface.h"
#include <pmod/library/model_library.h>
#include "MoCoreProxyFactory.h"
#include <foundation/library/base_adapter_host.h>

//------------------------------------------------------------------------------
// Class:   CMoDictionaryProxy
//
// Purpose: Implementation of foundation::IDictionary
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoDictionaryProxy :
    public foundation::library::_DefaultAdapterHost
    <
     foundation::IDictionary,
     TAdapterProxyInterface
     <
        foundation::IDictionary,
        ABI::Microsoft::PropertyModel::IDictionary
     >
    >
{
public:
    HRESULT SetSourceInterface(ABI::Microsoft::PropertyModel::IDictionary *pMoInterface)
    {
        IFR_ASSERT(__super::SetSourceInterface(pMoInterface));
        IFR_ASSERT(foundation::QueryInterface(pMoInterface, &this->_pMapFoundationPtr));
        return S_OK;
    }
protected:
    CMoDictionaryProxy() :
        _pMapFoundationPtr(nullptr)
    {
    }

    void FinalRelease() override
    {
        RELEASE_INTERFACE(_pMapFoundationPtr);
        __super::FinalRelease();
    }
    // Interface foundation::IDictionary
    STDMETHOD(GetView)(_Out_ UINT32* size, _Outptr_ foundation::IDictionaryPair ***ppKvpInterface)
    {
        foundation::ComPtr<ABI::Windows::Foundation::Collections::IIterable< ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING, foundation::IInspectable*> *>> iterable_ptr;
        _IFR_(foundation::QueryInterface(_pMapFoundationPtr, iterable_ptr.GetAddressOf()));
        foundation::ComPtr<ABI::Windows::Foundation::Collections::IIterator< ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING, foundation::IInspectable*> *>> iterator_ptr;
        _IFR_(iterable_ptr->First(iterator_ptr.GetAddressOf()));

        std::vector<foundation::ComPtr<foundation::IDictionaryPair> > properties;

        boolean hasCurrent = FALSE;
        _IFR_(iterator_ptr->get_HasCurrent(&hasCurrent));
        while (hasCurrent)
        {
            foundation::ComPtr<ABI::Windows::Foundation::Collections::__FIKeyValuePair_2_HSTRING_IInspectable_t> item_ptr;
            _IFR_(iterator_ptr->get_Current(item_ptr.GetAddressOf()));
            _IFR_(iterator_ptr->MoveNext(&hasCurrent));

            foundation::HStringPtr key;
            foundation::InspectablePtr valuePtr;
            _IFR_(item_ptr->get_Key(key.GetAddressOf()));
            _IFR_(item_ptr->get_Value(valuePtr.GetAddressOf()));

            foundation::InspectablePtr proxy_value_ptr;
            _IFR_(CMoCoreProxyFactory::ToProxy(valuePtr, proxy_value_ptr.GetAddressOf()));

            foundation::ComPtr<foundation::IDictionaryPair> spPropertyNamePair;
            IFR_ASSERT(foundation::library::CreateDictionaryPair(
                key,
                proxy_value_ptr,
                spPropertyNamePair.GetAddressOf()));
            properties.push_back(spPropertyNamePair);
        }
        ContainerRefCountToArray(properties, size, ppKvpInterface);
        return S_OK;
    }

    STDMETHOD(GetSize)(_Out_ UINT32* size)
    {
        return E_NOTIMPL;
    }
    STDMETHOD(Lookup)(_In_ HSTRING key, foundation::IInspectable** value)
    {
        foundation::InspectablePtr valuePtr;
        _IFR_(this->_pMapFoundationPtr->Lookup(key, valuePtr.GetAddressOf()));
        return CMoCoreProxyFactory::ToProxy(valuePtr, value);
    }
    STDMETHOD(HasKey)(_In_ HSTRING key, _Out_ bool* found)
    {
        boolean b;
        _IFR_(this->_pMapFoundationPtr->HasKey(key, &b));
        *found = b ? true : false;
        return S_OK;
    }

    STDMETHOD(Insert)(_In_ HSTRING key, _In_ foundation::IInspectable* value, _Out_ bool* replaced)
    {
        foundation::InspectablePtr valuePtr;
        IFR_ASSERT(CMoCoreProxyFactory::FromProxy(value, valuePtr.GetAddressOf()));
        boolean _replaced;
        _IFR_(this->_pMapFoundationPtr->Insert(key, valuePtr, &_replaced));
        *replaced = _replaced ? true : false;
        return S_OK;
    }
    STDMETHOD(Remove)(_In_ HSTRING key)
    {
        return this->_pMapFoundationPtr->Remove(key);
    }
    STDMETHOD(Clear)()
    {
        return this->_pMapFoundationPtr->Clear();
    }
    STDMETHOD(Keys)(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first)
    {
        return E_NOTIMPL;
    }
private:
    ABI::Windows::Foundation::Collections::__FIMap_2_HSTRING_IInspectable_t *_pMapFoundationPtr;
};
