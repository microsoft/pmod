/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:enumerable_impl.h
****/
#pragma once

#include "base_object_impl.h"
#include "iterator_impl.h"
#include "foundation_adapter_base.h"
#include <foundation/boxing_util.h>
#include <foundation/interfaces/enumerable.h>
#include <vector>

namespace foundation {

    namespace library {

        template 
        <
            class T,
            class TInterface,
            class TConverter,
            class TComBase = foundation::ctl::ComInspectableBase
        >
        class _VectorEnumerableImplBase :
            public _ObjectImplBase
            <
                TInterface,
                IEnumerable,
                TComBase
            >
        {
        public:
            std::vector<T>& GetItems() { return _items; }

            typedef typename TInterface::_Item_Type _Item_type;
            STDMETHOD(First)(_COM_Outptr_ foundation::IIterator_impl<_Item_type> **first)
            {
                typedef _IteratorImpl<std::vector<T>, _Item_type, TConverter> _Iterator_type;
                return ctl::inspectable::CreateInstanceWithInitialize<_Iterator_type>(
                    first,
                    this->CastToInspectable(),
                    _items);
            }

        private:
            std::vector<T> _items;
        };

        typedef _VectorEnumerableImplBase<INT32, IEnumerable_Int32, _CastConverter> EnumerableVectorImplInt32;
        typedef _VectorEnumerableImplBase<InspectablePtr, IEnumerable, _InspectableConverter> EnumerableVectorImplInspectable;

        typedef _ObjectWithTypeImplBase<EnumerableVectorImplInspectable, IID_IEnumerable> EnumerableVectorImpl;

        inline HRESULT CreateEnumerableVectorImpl(const IID& iidType, EnumerableVectorImpl **pp)
        {
            return ctl::inspectable::CreateInstanceWithInitialize(pp, &iidType,false);
        }
    }
}

