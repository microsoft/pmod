/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_node_ptr.h
****/
#pragma once

#include <foundation/object_ptr.h>

namespace pmod
{

template <
    class T, 
    class TPublicInterface
>
class _ObjectNodePtrWrapper : public foundation::_ObjectPtrWrapper<T, TPublicInterface>
{
public:
    template <class TAncestorInterface>
    HRESULT GetAncestor(_COM_Outptr_result_maybenull_ TAncestorInterface **ppAncestor)
    {
        return foundation::GetObjectAncestor(
			this->GetClassInterface(), 
			ppAncestor);
    }

    template <class TService>
    HRESULT QueryService(
        REFIID guidService,
        _COM_Outptr_ TService** ppService)
    {
        return foundation::QueryObjectService(
            this->GetClassInterface(),
            guidService,
            ppService);
    }
};

}

