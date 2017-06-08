/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CastHelper.h
****/
#pragma once

struct TQICast
{
	template <class T, class Q>
	static HRESULT CastTo(T *pT, Q **ppQ)
	{
		foundation_assert(pT);
		HRESULT hr = foundation::QueryWeakReference(pT, ppQ);
		foundation_assert(SUCCEEDED(hr));
		return hr;
	}
};

struct TStaticCast
{
	template <class T, class Q>
	static HRESULT CastTo(T *pT, Q **ppQ)
	{
		*ppQ = static_cast<Q *>(pT);
		return S_OK;
	}
};

struct TInspectableCast
{
	template <class T, class Q>
	static void CastTo(T *pT, Q **ppQ)
	{
		*ppQ = pT->CastToInspectable();
	}
};
