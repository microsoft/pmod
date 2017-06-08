/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchDictionaryAdapter.h
****/
#pragma once

#include <foundation/library/adapter_base.h>

template <class TInnerInterface,class TInnerClass>
class ObjectDispatchDictionaryAdapter :
	public foundation::AdapterBase
	<
		TInnerInterface,
		foundation::IDictionary,
		&foundation::IID_IDictionary
	>
{
protected:
	ObjectDispatchDictionaryAdapter()
	{}
	STDMETHOD(GetView)(UINT32* size, foundation::IDictionaryPair ***ppProperties)
	{
		return get_inner_class()->_GetView(size, ppProperties);
	}

	STDMETHOD(GetSize)(_Out_ UINT32* size)
	{
		return get_inner_class()->_GetSize(size);
	}
	STDMETHOD(Lookup)(HSTRING propertyName, foundation::IInspectable** value)
	{
		return get_inner_class()->_Lookup(propertyName, value);
	}
	STDMETHOD(HasKey)(HSTRING propertyName, bool* found)
	{
		return get_inner_class()->_HasKey(propertyName, found);
	}

	STDMETHOD(Insert)(HSTRING propertyName, foundation::IInspectable *value, bool* replaced)
	{
		return get_inner_class()->_Insert(propertyName, value, replaced);
	}
	STDMETHOD(Remove)(HSTRING propertyName)
	{
		return get_inner_class()->_Remove(propertyName);
	}
	STDMETHOD(Clear)()
	{
		return get_inner_class()->_Clear();
	}
	STDMETHOD(Keys)(_COM_Outptr_ foundation::IIterator_impl<HSTRING> **first)
	{
		return get_inner_class()->_Keys(first);
	}
private:
	TInnerClass *get_inner_class()
	{
		return static_cast<TInnerClass *>(this->GetInner());
	}
};
