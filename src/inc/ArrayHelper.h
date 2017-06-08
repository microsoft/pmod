/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ArrayHelper.h
****/
#pragma once

template <class Vector, class T>
void CopyArrayToVector(UINT32 size, T *pArray, Vector& vector)
{
	foundation_assert(vector.size() == 0);
	vector.resize((size_t)size);
	for (UINT32 index = 0; index < size; ++index, ++pArray)
	{
		vector[index] = *pArray;
	}
}

template <class Container, class T, class _FuncAdapter>
HRESULT _CreateArrayFromContainer(
	const Container& container,
	const _FuncAdapter& adapter,
	UINT32 *size,
	T **pArray)
{
	*size = (UINT32)container.size();
	*pArray = (T *)_pal_MemAlloc((ULONG)(sizeof(T*)*container.size()));
    if (pArray == nullptr)
    {
        return E_OUTOFMEMORY;
    }
#pragma warning ( disable: 6011 )

	UINT32 index = 0;
	for (typename Container::const_iterator iter = container.begin();
	iter != container.end();
		++iter, ++index)
	{
		*(*pArray + index) = adapter(iter);
	}
    return S_OK;
}

template <class Container, class T>
HRESULT CreateArrayFromContainer(const Container& container, UINT32 *size, T **pArray)
{
	return _CreateArrayFromContainer(
		container, 
		[](typename Container::const_iterator &iter) {
			return *iter;
		},
		size, 
		pArray);
}

template <class T>
void RefCountToArray(T *pT, UINT32 *pSize, T ***ppArray)
{
	*pSize = 1;
	*ppArray = (T **)_pal_MemAlloc(sizeof(T *));
	(**ppArray) = pT;
	(**ppArray)->AddRef();
}

template <class Container, class T>
void ContainerRefCountToArray(const Container& container, UINT32 *size, T **ppArray)
{
	CreateArrayFromContainer(container, size, ppArray);
	for (typename Container::const_iterator iter = container.begin();
	iter != container.end();
		++iter)
	{
		if ((*iter) != nullptr)
		{
			((foundation::IUnknown *)*iter)->AddRef();
		}
	}
}

template <class Container, class T>
void ContainerToArray(const Container& vector, UINT32 *size, T **pArray)
{
	CreateArrayFromContainer(vector, size, pArray);
}

template <class Container>
void ContainerToArray(const Container& vector, UINT32 *size, foundation::IInspectable ***ppArray)
{
	ContainerRefCountToArray(vector, size, ppArray);
}

template <class Container>
void ContainerToArray(const Container& vector, UINT32 *size, HSTRING **ppArray)
{
	CreateArrayFromContainer(vector, size, ppArray);
	for (UINT32 index = 0; index < *size; ++index)
	{
		HSTRING hstring;
		_pal_DuplicateString((*ppArray)[index], &hstring);
		(*ppArray)[index] = hstring;
	}
}

