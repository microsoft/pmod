/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:source_model_ptr.h
****/
#pragma once

#include <pmod/interfaces/source_model.h>

namespace pmod
{

template <class T, class TPublicInterface>
class SourceModelBasePtrWrapper
{
public:
    HRESULT is_attached(bool *pValue)
    {
        return this->GetSourceInterface()->GetIsAttached(pValue);
    }

    bool IsAttached()
    {
        bool isAttached;
        IGNOREHR(this->GetSourceInterface()->GetIsAttached(&isAttached));
        return isAttached;
    }

    HRESULT SetForceAttached(bool newValue)
    {
        return this->GetSourceInterface()->SetForceAttached(newValue);
    }
protected:
    TPublicInterface *GetSourceInterface()
    {
        T *pThis = static_cast<T *>(this);
        return pThis->GetSourceInterface();
    }
};

template <class T>
class SourceModelPtrWrapper : public SourceModelBasePtrWrapper<T, ISourceModel>
{
public:
    HRESULT GetSource(foundation::IObject **ppObject)
    {
        return this->GetSourceInterface()->GetSource(ppObject);
    }
    HRESULT SetSource(foundation::IObject *pObject)
    {
        return this->GetSourceInterface()->SetSource(pObject);
    }
};

}

