/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionStatics.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <pmod/library/interfaces/observable_collection_statics.h>
#include <foundation/com_ptr.h>


class CObservableCollectionStatics :
    public foundation::ctl::ComInspectableBase,
    public pmod::library::IObservableCollectionStatics
{
 public:
     static pmod::library::IObservableCollectionStatics *GetInstance();

     static HRESULT LookupItemInternal(
         pmod::IObservableCollection *pCollection,
         foundation::IInspectable *pDefaultPropertyValue,
         foundation::IInspectable **ppItem);

protected:
    ~CObservableCollectionStatics();

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::library::IObservableCollectionStatics::GetIID())
        {
            *ppInterface = static_cast<pmod::library::IObservableCollectionStatics *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // IObservableCollectionStatics Interface
    STDMETHOD(LookupItem)(
        pmod::IObservableCollection *pCollection,
        foundation::IInspectable *pDefaultPropertyValue,
        foundation::IInspectable **ppItem);
};


