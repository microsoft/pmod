/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryFactory.cpp
****/
#include "pch.h"

#include "DictionaryFactory.h"
#include "DictionaryBase.h"
#include <foundation/library/com_ptr_singleton.h>

#include <foundation/library/activation_factory.h>

using namespace foundation;
// {A28FE5AF-EAA6-4227-88D1-0D7F97812D50}
const GUID foundation::library::IID_IDictionaryFactory =
{ 0xa28fe5af, 0xeaa6, 0x4227,{ 0x88, 0xd1, 0xd, 0x7f, 0x97, 0x81, 0x2d, 0x50 } };

library::IDictionaryFactory *CDictionaryFactory::GetInstance()
{
	return library::_GetFactoryInstance<CDictionaryFactory, library::IDictionaryFactory>();
}

HRESULT CDictionaryFactory::GetOrCreateInstance(library::IDictionaryFactory **ppDictionaryFactory)
{
    IFCPTR_ASSERT(ppDictionaryFactory);
    *ppDictionaryFactory = GetInstance();
    (*ppDictionaryFactory)->AddRef();
    return S_OK;
}

STDMETHODIMP CDictionaryFactory::CreateDictionary(_COM_Outptr_ IDictionary **ppNewInstance)
{
    return CDictionaryClass::CreateInstance(ppNewInstance);
}

STDMETHODIMP CDictionaryFactory::CreateDictionaryPair(
    HSTRING key,
    _In_opt_ IInspectable *pValue,
    _COM_Outptr_ IDictionaryPair **ppNewInstance)
{
    return foundation::library::CreateDictionaryPairImpl(key, pValue, ppNewInstance);
}

EXTERN_C foundation::IUnknown *GetDictionaryFactoryInstance()
{
    return CDictionaryFactory::GetInstance();
}

static foundation::library::_RegisterActivationFactory _dictionaryFactory(
    U("foundation.Dictionary"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&GetDictionaryFactoryInstance)
    );
