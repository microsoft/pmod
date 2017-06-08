/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoSourceModelDelegateAdapter.h
****/

#pragma once

#include "MoBaseDelegateAdapter.h"
#include <foundation/com_ptr.h>

using namespace ABI::Microsoft::PropertyModel;
//------------------------------------------------------------------------------
// Class:   CMoSourceModelBaseDelegateAdapterBase
//
// Purpose: Implementation for ISourceModelBaseOverrides
//
//------------------------------------------------------------------------------
template <class TBASE>
class CMoSourceModelBaseDelegateAdapterBase:
    public TBASE
{
protected:
    STDMETHOD( OnSourceAttached )()
    {
        if(IsOverrideComposed())
        {
            ISourceModelBaseOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));

            IFR(pOverrides->OnSourceAttached());
        }
        return S_OK;    
    }

    STDMETHOD( OnSourceDetached )()
    {
        if(IsOverrideComposed())
        {
            ISourceModelBaseOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));

            IFR(pOverrides->OnSourceDetached());
        }
        return S_OK;    
    }
};

//------------------------------------------------------------------------------
// Class:   CMoSourceModelBaseDelegateAdapter
//
// Purpose: Implementation of ISourceModelDelegateBase<> Interface Adapter
//
//------------------------------------------------------------------------------
template <class TDelegate,class TEventHandler,class TModel>
class CMoSourceModelBaseDelegateAdapter:
    public CMoSourceModelBaseDelegateAdapterBase
        <
            CMoDelegeteBaseAdapter<TDelegate,TEventHandler>
        >
{
};

//------------------------------------------------------------------------------
// Class:   CMoSourceModelDelegateAdapter
//
// Purpose: Implementation of ISourceModelDelegateBase<> Interface Adapter
//
//------------------------------------------------------------------------------
template <class TDelegate,class TEventHandler,class TModel>
class CMoSourceModelDelegateAdapter:
    public CMoSourceModelBaseDelegateAdapter<TDelegate,TEventHandler,TModel>
{
protected:
    STDMETHOD( OnSourceChanged )(_In_ TModel *pObject)
    {
        if(IsOverrideComposed())
        {
            ISourceModelOverrides *pOverrides;
            IFR(QueryOverrides(pOverrides));
            foundation::ComPtr<IObject> spSource;
            if (pObject != nullptr)
            {
                IFR(foundation::QueryInterface(pObject, spSource.GetAddressOf()));
            }
            IFR(pOverrides->OnSourceChanged(spSource));
        }
        return S_OK;
    }
};

//------------------------------------------------------------------------------
// Class:   CMoMultiSourceModelDelegateAdapter
//
// Purpose: Implementation of IMultiSourceModelDelegateBase<> Interface Adapter
//
//------------------------------------------------------------------------------
template <class TDelegate,class TEventHandler,class TModel>
class CMoMultiSourceModelDelegateAdapter:
    public CMoSourceModelBaseDelegateAdapter<TDelegate,TEventHandler,TModel>
{
protected:
};
