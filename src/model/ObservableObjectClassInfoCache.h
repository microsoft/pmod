/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectClassInfoCache.h
****/

#pragma once
#include "TypeInfoCache.h"
#include "ObservableObjectInfo.h"


//------------------------------------------------------------------------------
// Class:   CPropertyModelClassInfoCache 
//
// Purpose: Define a class that allow caching of ObservableObjectInfo
//
//------------------------------------------------------------------------------

class CPropertyModelClassInfoCache:
    public _TStaticTypeInfoCache<CPropertyModelClassInfoCache,CObservableObjectInfo,TStaticCast>
{
public:
    static CPropertyModelClassInfoCache::_TypeModelInfoCacheType& GetInstance();

    // Note : this function does not add a ref 
    static HRESULT GetOrCreatePropertyModelInfo(
        _In_ const pmod::ObservableObjectTypeInfo *pmi,
        _Outptr_ CObservableObjectInfo **ppPropertyModelInfo);


    // register a property model info in my cache
    static HRESULT RegisterPropertyModelInfo(_In_ const pmod::ObservableObjectTypeInfo *pmi);
    static HRESULT RegisterPropertyModelInfo(_In_ CPropertyModelInfoClass *pPropertyModelInfo, _In_ bool isViewModel);

    // AddInstrinsicProperties
    static HRESULT AddInstrinsicProperties(
        _In_ CObservableObjectInfo *pPropertyModelInfo,
        _In_ bool isPropertyViewModel
        );

    static bool ContainsTypeInfo(_In_ const IID& iidType);

private:

    static HRESULT CreatePropertyModelInfoInternal(
        _In_ const pmod::ObservableObjectTypeInfo *pmi,
        _Outptr_ CObservableObjectInfo **pValue);

    static HRESULT CreatePropertyModelInfoInternal(
        _In_ const pmod::ObservableObjectTypeInfo *pPropertyModelInfo,
        _In_ CObservableObjectInfo *pBaseType,
        _Outptr_ CObservableObjectInfo **pValue);
};
