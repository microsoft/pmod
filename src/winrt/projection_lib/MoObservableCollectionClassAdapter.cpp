/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoObservableCollectionClassAdapter.cpp
****/

#include "pch.h"

#include "MoObservableCollectionClassAdapter.h"

// Interface ICollectionModelProtected
HRESULT STDMETHODCALLTYPE CMoObservableCollectionClassAdapter::AddItemInternal( 
        IInspectable *item)
{
    return m_pInner->AppendItemInternal(item);
}
                        
HRESULT STDMETHODCALLTYPE CMoObservableCollectionClassAdapter::InsertItemInternal( 
        int nIndex,
        IInspectable *item)
{
    return m_pInner->InsertItemInternal((UINT32)nIndex,item);
}
                        
HRESULT STDMETHODCALLTYPE CMoObservableCollectionClassAdapter::ReplaceItemInternal( 
        int nIndex,
        IInspectable *item)
{
    return m_pInner->ReplaceItemInternal((UINT32)nIndex,item);
}

HRESULT STDMETHODCALLTYPE CMoObservableCollectionClassAdapter::InsertItems( 
        int nIndex,
        UINT32 size,
        IInspectable **items) 
{
    return m_pInner->InsertItems((UINT32)nIndex,size,items);
}
                        
HRESULT STDMETHODCALLTYPE CMoObservableCollectionClassAdapter::ReplaceItems( 
        int nIndex,
        UINT32 size,
        IInspectable **items)
{
    return m_pInner->ReplaceItems((UINT32)nIndex,size,items);
}
                        
HRESULT STDMETHODCALLTYPE CMoObservableCollectionClassAdapter::RemoveItems( 
        int nIndex,
        UINT32 count)
{
    return m_pInner->RemoveItems((UINT32)nIndex,count);
}

