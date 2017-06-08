/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeDictionary.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"

#include <foundation/interfaces/dictionary.h>
#include <foundation/library/dictionary_class.h>
#include <foundation/array_wrapper.h>
#include <foundation/hstring_wrapper.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Dictionary_CreateInstance(IDictionary **pDictionary)
{
    return library::CreateDictionaryClass(pDictionary);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Dictionary_HasKey(IDictionary *pDictionary, LPCSTR_t key, bool *result)
{
	return pDictionary->HasKey(HStringRef(key), result);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Dictionary_Lookup(IDictionary *pDictionary, LPCSTR_t key, foundation::IInspectable **ppValue)
{
	return pDictionary->Lookup(HStringRef(key), ppValue);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Dictionary_Insert(IDictionary *pDictionary, LPCSTR_t key, foundation::IInspectable *pValue,bool *replaced)
{
	return pDictionary->Insert(HStringRef(key), pValue, replaced);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Dictionary_Remove(IDictionary *pDictionary, LPCSTR_t key)
{
	return pDictionary->Remove(HStringRef(key));
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Dictionary_GetView(
	IDictionary *pDictionary,
	UINT32 *pSize,
	IDictionaryPair ***ppPairs)
{
	return pDictionary->GetView(pSize, ppPairs);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE DictionaryPair_GetKey(
	IDictionaryPair *pDictionaryPair,
	CHAR_t **ppKey)
{
	HStringPtr hstr;
	_IFR_(pDictionaryPair->GetKey(hstr.GetAddressOf()));
	return ReturnString(hstr, ppKey);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE DictionaryPair_GetValue(
	IDictionaryPair *pDictionaryPair,
	foundation::IInspectable **ppValue)
{
	return pDictionaryPair->GetValue(ppValue);
}
