/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:node_impl.h
****/
#pragma once
#include <foundation/interfaces/node.h>
#include <foundation/ctl/com_library.h>
#include "critical_section.h"
#include "adapter_base.h"
#include "interfaces/node_impl.h"

#include <set>
#include <vector>

namespace foundation {
    
    namespace library {

        template <
            class TNodeInterface,
            class TBASE
        >
        class _NodeImplBase :
            public TBASE,
            public _INode_impl
        {
        public:
            typedef typename TNodeInterface::_Node_Type _Node_Type;

            void SetParent(
                _In_opt_ _INode_impl *pT)
            {
                NoRefCountFoundationCriticalSection cs(this->get_lock());

                SetParentInternal(pT);
            }

            void ClearNodeTree()
            {
                for (_Child_container_type::const_iterator iter = _childs.begin();
                iter != _childs.end();
                    ++iter)
                {
                    (*iter)->_ClearParent();
                }
                SetParentInternal(nullptr);

                for (_Child_container_type::const_iterator iter = _childs.begin();
                iter != _childs.end();
                    ++iter)
                {
                    (*iter)->_NotifyParentChanged(nullptr);
                }
                _childs.clear();
            }

        protected:
            virtual HRESULT GetParentInternal(_Node_Type **ppParent)
            {
                _INode_impl *pParent = this->_GetParent();
                return foundation::QueryInterfaceIf(pParent, ppParent);
            }

            virtual HRESULT GetChildsInternal(UINT32 *size, _Node_Type ***ppChilds)
            {
                std::vector<_Node_Type *> vec_childs;
                vec_childs.resize(_childs.size());
                size_t index = 0;
                for (_Child_container_type::const_iterator iter = _childs.begin();
                iter != _childs.end();
                    ++iter)
                {
                    IFR_ASSERT(foundation::QueryWeakReference(
                        (*iter), 
                        &vec_childs.at(index++)));
                }
                return RefCountCopyTo(
                    (UINT32)vec_childs.size(),
                    index ? &vec_childs.front():nullptr,
                    size,
                    ppChilds);
            }

        protected:
            _NodeImplBase() :
                _pParent(nullptr)
            {}
            ~_NodeImplBase()
            {
                foundation_assert(_pParent == nullptr);
                foundation_assert(_childs.size() == 0);
            }
            virtual ICriticalSection *get_lock()
            {
                return nullptr;
            }
        protected:
            void _OnParentChanged(_INode_impl *pParent) override
            {}
            void _OnAncestorChanged(_INode_impl *pChild, _INode_impl *pParent) override
            {}

            _INode_impl *_GetParent() override
            {
                return _pParent;
            }

            void _SetParent(_INode_impl *pParent) override
            {
                this->SetParent(pParent);
            }

            void _RemoveChild(_INode_impl *pChild) override
            {
                NoRefCountFoundationCriticalSection cs(this->get_lock());
                _childs.erase(pChild);
            }

            void _AddChild(_INode_impl *pChild) override
            {
                NoRefCountFoundationCriticalSection cs(this->get_lock());
                _childs.insert(pChild);
            }

            void _ClearParent() override
            {
                NoRefCountFoundationCriticalSection cs(this->get_lock());
                _pParent = nullptr;
            }

            bool _IsChildContained(_INode_impl *pChild) override
            {
                NoRefCountFoundationCriticalSection cs(this->get_lock());

                for (_Child_container_type::const_iterator iter = _childs.begin();
                iter != _childs.end();
                    ++iter)
                {
                    if ((*iter) == pChild || (*iter)->_IsChildContained(pChild))
                    {
                        return true;
                    }
                }
                return false;
            }

            void _NotifyParentChanged(_In_opt_ _INode_impl *pParent)
            {
                NoRefCountFoundationCriticalSection cs(this->get_lock());
                _Notify_parent_changed(pParent);
            }

            void SetParentInternal(
                _In_opt_ _INode_impl *pParent)
            {
                if (_pParent == pParent)
                {
                    return;
                }

                if (_pParent != nullptr)
                {
                    _pParent->_RemoveChild(this);
                }
                this->_pParent = pParent;

                if (pParent)
                {
                    pParent->_AddChild(this);
                }
                _Notify_parent_changed(pParent);
            }

            void _Notify_parent_changed(_In_opt_ _INode_impl *pParent)
            {
                this->_OnParentChanged(pParent);
                _RaiseAncestorChanged(this, pParent);
            }

            void _RaiseAncestorChanged(
                _INode_impl *pChild,
                _INode_impl *pParent)
            {
                for (_Child_container_type::const_iterator iter = _childs.begin();
                iter != _childs.end();
                    ++iter)
                {
                    (*iter)->_OnAncestorChanged(pChild, pParent);
                    (*iter)->_RaiseAncestorChanged(pChild, pParent);
                }
            }

        private:
            typedef std::set<_INode_impl *> _Child_container_type;

            _INode_impl *_pParent;
            _Child_container_type _childs;
        };

        template <
            class TNodeInterface,
            class TBASE = foundation::ctl::ComInspectableBase
        >
        class _NodeImplComBase :
            public _NodeImplBase<TNodeInterface, TBASE>,
            public TNodeInterface
        {
        public:
            typedef typename TNodeInterface::_Node_Type _Node_Type;

            STDMETHOD(GetParent)(_Node_Type **ppParent)
            {
                return this->GetParentInternal(ppParent);
            }
            STDMETHOD(GetChilds)(UINT32 *size, _Node_Type ***ppChilds)
            {
                return this->GetChildsInternal(size, ppChilds);
            }

        protected:
            void FinalRelease() override
            {
                this->ClearNodeTree();
                TBASE::FinalRelease();
            }

            HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
            {
                if (iid == TNodeInterface::GetIID())
                {
                    *ppInterface = static_cast<TNodeInterface *>(this);
                }
                else if (iid == _INode_impl::GetIID())
                {
                    *ppInterface = static_cast<_INode_impl *>(this);
                }
                else
                {
                    return TBASE::QueryInterfaceImpl(iid, ppInterface);
                }
                return S_OK;
            }
        };

        template <class TNodeInterface, class TInnerInterface,const IID* piid>
        class _NodeImplAdapterBase :
            public _NodeImplBase<TNodeInterface, AdapterBase<TInnerInterface, TNodeInterface, piid>>
        {
        public:
            typedef typename TNodeInterface::_Node_Type _Node_Type;

            STDMETHOD(GetParent)(_Node_Type **ppParent)
            {
                return this->GetParentInternal(ppParent);
            }
            STDMETHOD(GetChilds)(UINT32 *size, _Node_Type ***ppChilds)
            {
                return this->GetChildsInternal(size, ppChilds);
            }
        protected:
            typedef AdapterBase<TInnerInterface, TNodeInterface, piid> _Base_type;
            // Interface IInterfaceAdapter
            STDMETHOD(QueryAdapterInterface)(REFIID iid, IUnknown **ppInterface)  override
            {
                if (iid == _INode_impl::GetIID())
                {
                    *ppInterface = static_cast<_INode_impl *>(this);
                }
                else
                {
                    return _Base_type::QueryAdapterInterface(iid, ppInterface);
                }
                return S_OK;
            }

            void FinalRelease() override
            {
                this->ClearNodeTree();
                ctl::ComInspectableBase::FinalRelease();
            }
        };

    }
}

