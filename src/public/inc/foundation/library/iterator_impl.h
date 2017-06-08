/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:iterator_impl.h
****/
#pragma once
#include <foundation/interfaces/iterator.h>

namespace foundation {
    namespace library {

        //------------------------------------------------------------------------------
        // Class:   _IteratorImpl
        // foundation::IIterator_impl<IInspectable*> implementation using std::iterator
        //------------------------------------------------------------------------------
        template <class Container, class T, class TConverter, class TBASE = ctl::ComInspectableBase>
        class _IteratorImpl :
            public TBASE,
            public IIterator_impl<T>
        {
        public:
            HRESULT _Initialize(
                _In_ foundation::IUnknown *pContainerHost,
                const Container& items)
            {
                foundation_assert(pContainerHost);
                this->_pContainerHost = pContainerHost;
                this->_items = &items;
                this->_iterator = items.begin();
                // add ref
                pContainerHost->AddRef();
                return S_OK;
            }
        protected:
            _IteratorImpl() :
                _pContainerHost(nullptr),
                _items(nullptr)
            {}
            void FinalRelease()
            {
                // release our items container
                RELEASE_INTERFACE(_pContainerHost);
            }
            STDMETHOD(GetCurrent)(_Outptr_ T *current)
            {
                IFCPTR_ASSERT(current);
                IFR_ASSERT(TConverter::ToValue((*_iterator), current));
                return S_OK;
            }
            STDMETHOD(GetHasCurrent)(_Outptr_ bool *hasCurrent)
            {
                IFCPTR_ASSERT(hasCurrent);
                *hasCurrent = _iterator != _items->end();
                return S_OK;
            }
            STDMETHOD(MoveNext)(_Outptr_ bool *hasCurrent)
            {
                IFCPTR_ASSERT(hasCurrent);
                ++_iterator;
                *hasCurrent = _iterator != _items->end();
                return S_OK;
            }
        private:
            typedef typename Container::const_iterator _TIterator_Type;
            foundation::IUnknown    *_pContainerHost;
            const Container*        _items;
            _TIterator_Type         _iterator;
        };
    }
}

