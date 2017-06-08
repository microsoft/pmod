/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PtrSingleton.h
****/
#pragma once

#include <pal/library/pal_once_util.h>

template <class T>
class PtrSingleton
{
public:
	PtrSingleton() :
		_pInstance()
	{}
	~PtrSingleton()
	{
		delete _pInstance;
		_pInstance = nullptr;
	}

	T *GetInstance()
	{
		pal::library::pal_CallOnce_t(
			this->_initOnce,
			[=]()
		{
			this->_pInstance = new T();
			foundation_assert(this->_pInstance != nullptr);
			return S_OK;
		});
		return this->_pInstance;
	}
	T *_Get()
	{
		return _pInstance;
	}
private:
	pal::library::PalOnceFlag    _initOnce;
	T*                _pInstance;
};

template <class T>
T *_GetPtrInstance()
{
	static PtrSingleton<T> _PtrInstance;

	return _PtrInstance.GetInstance();
}
