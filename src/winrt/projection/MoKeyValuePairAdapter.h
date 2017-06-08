/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoKeyValuePairAdapter.h
****/

#pragma once


#include <foundation/library/adapter_base.h>
#include "windows.foundation.collections.h"

//------------------------------------------------------------------------------
// Class:   TMoKeyValuePairAdapter
//
// Purpose: Implementation for IKeyValuePair<K,V> for pmod::IKeyValuePair
//
//------------------------------------------------------------------------------

template <class T,class K,class V,class TInner,class KInner,class VInner>
class TMoKeyValuePairAdapter :
    public foundation::AdapterBase
        <
        TInner,
        T,
        &__uuidof(T)
        >
{
protected:
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<K>::type     K_abi;
    typedef typename ABI::Windows::Foundation::Internal::GetAbiType<V>::type     V_abi;
public:

    virtual HRESULT STDMETHODCALLTYPE get_Key(_Out_ K_abi *key)
    {
        KInner _key;
        IFR(m_pInner->GetKey(&_key));
        IFR(this->CopyKey(_key,key));
        this->ReleaseKey(_key);
        return S_OK;
    }

    virtual HRESULT STDMETHODCALLTYPE get_Value(_Out_ V_abi *value)
    {
        VInner _value;
        IFR(m_pInner->GetValue(&_value));
        IFR(this->CopyValue(_value,value));
        this->ReleaseValue(_value);
        return S_OK;
    }
protected:
    virtual HRESULT CopyKey(KInner _key,_Out_ K_abi *key) = 0;
    virtual HRESULT CopyValue(VInner _value,_Out_ V_abi *value) = 0;
    virtual void ReleaseKey(KInner key)
    {
        UNREFERENCED_PARAMETER(key);
    }
    virtual void ReleaseValue(VInner value)
    {
        UNREFERENCED_PARAMETER(value);
    }
};

template <class T,class K,class V,class TInner,class KInner,class VInner>
class TMoInterfaceValueKeyValuePairAdapter :
    public TMoKeyValuePairAdapter<T,K,V,TInner,KInner,VInner>
{
protected:
    virtual HRESULT CopyValue(VInner _value,_Out_ V_abi *value)
    {
        if(_value)
        {
            return _value->QueryInterface(__uuidof(V),(void **)value);
        }
        else
        {
            *value = nullptr;
            return S_OK;
        }
    }
    virtual void ReleaseValue(VInner value)
    {
        RELEASE_INTERFACE(value);
    }
};

class CMoPropertyNamePair : 
    public TMoInterfaceValueKeyValuePairAdapter
    <
        ABI::Windows::Foundation::Collections::IKeyValuePair<HSTRING,IInspectable *>,
        HSTRING,
        IInspectable *,
        foundation::IDictionaryPair,
        HSTRING,
        IInspectable *
    >
{
protected:
    HRESULT CopyKey(HSTRING _key,_Out_ HSTRING *key) override
    {
        return WindowsDuplicateString(_key,key);
    }

    void ReleaseKey(HSTRING key) override
    {
        WindowsDeleteString(key);
    }
};

class CMoPropertyIdPair : 
    public TMoInterfaceValueKeyValuePairAdapter
    <
        ABI::Windows::Foundation::Collections::IKeyValuePair<UINT32,IInspectable *>,
        UINT32,
        IInspectable *,
        foundation::IPropertyIdPair,
        UINT32,
        IInspectable *
    >
{
protected:
    HRESULT CopyKey(UINT32 _key,_Out_ UINT32 *key) override
    {
        *key = _key;
        return S_OK;
    }
};

class CMoLogParameterPair : 
    public TMoInterfaceValueKeyValuePairAdapter
    <
    ABI::Windows::Foundation::Collections::IKeyValuePair<IEnumValue *,IInspectable *>,
    IEnumValue *,
    IInspectable *,
    foundation::ILogParameterPair,
    foundation::IEnumValue *,
    IInspectable *
    >
{
protected:
    HRESULT CopyKey(foundation::IEnumValue *_key, _Out_ IEnumValue **key) override
    {
        return _key->QueryInterface(__uuidof(IEnumValue),(void **)key);
    }

    void ReleaseKey(foundation::IEnumValue *key) override
    {
        key->Release();
    }
};
