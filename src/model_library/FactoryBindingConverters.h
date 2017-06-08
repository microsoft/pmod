/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FactoryBindingConverters.h
****/
#pragma once
#include <map>

#include <foundation/library/util_stl.h>
#include <pmod/interfaces/binding.h>
#include <pmod/library/pmod_lib_api.h>

EXTERN_C PMOD_LIB_API const IID IID_LowerCaseConverter;
EXTERN_C PMOD_LIB_API const IID IID_UpperCaseConverter;

class CFactoryBindingConverters
{
public:
    static HRESULT GetBindingConverter(IID iidType, pmod::IBindingConverter **ppBindingConverter);
    static HRESULT GetBindingConverter(LPCSTR_t name, pmod::IBindingConverter **ppBindingConverter);
private:
    static CFactoryBindingConverters& GetInstance();
    CFactoryBindingConverters();

    template <class T>
    void AddBindingConverter(IID iidType, LPCSTR_t name);

    HRESULT GetBindingConverterInternal(IID iidType, pmod::IBindingConverter **ppBindingConverter);
    HRESULT GetBindingConverterInternal(LPCSTR_t name, pmod::IBindingConverter **ppBindingConverter);
private:
    typedef std::map<IID, foundation::ComPtr<pmod::IBindingConverter>, foundation::library::GuidLessThan> _BindingConverterMap_Type;
    typedef std::map<LPCSTR_t, IID, foundation::library::StringLessThan> _NameMap_Type;
    _BindingConverterMap_Type   _factoryBindingTypes;
    _NameMap_Type               _factoryNameTypes;
};

