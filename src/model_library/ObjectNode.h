/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectNode.h
****/
#pragma once

#include "ObjectBase.h"

#include "InterfaceAdapterFactoryBase.h"
#include "InterfaceAdapterFactoryHost.h"
#include "ClassStatics.h"

// from public
#include <foundation/hstring_wrapper.h>
#include <foundation/library/logger_util.h>
#include <foundation/library/logger_macros.h>
#include <foundation/library/critical_section.h>
#include <foundation/library/node_impl.h>
#include <foundation/node_statics_util.h>
#include <foundation/type_info_statics_util.h>

#include <pmod/logger_type_info.h> 
#include <pmod/library/class_options.h>
#include <pmod/library/interfaces/object_node_class.h>

#if PMOD_LIB_LIBRARY_STATIC_LIB
// included to allow sub classes to enforce library instance if static lib is selected
EXTERN_C void _ModelLibraryInstanceInitializerStub();
#endif

typedef foundation::library::_NodeImplAdapterBase<
    foundation::_IObjectNode,
    pmod::library::IObjectNodeClass,
    &foundation::IID__IObjectNode> _ObjectNode_AdapterBase_type;

class _ObjectNodeClassAdapter :
    public _ObjectNode_AdapterBase_type
{
protected:
    foundation::library::ICriticalSection *get_lock() override
    {
        foundation::ComPtr<foundation::library::ICriticalSection> lock_ptr;
        IGNOREHR(this->GetInner()->GetCriticalSection(lock_ptr.GetAddressOf()));
        return lock_ptr;
    }

    void _OnParentChanged(foundation::library::_INode_impl *pParent) override
    {
        this->GetInner()->_OnParentChanged(pParent);
    }

    void _OnAncestorChanged(
        foundation::library::_INode_impl *pChild, 
        foundation::library::_INode_impl *pParent) override
    {
        this->GetInner()->_OnAncestorChanged(pChild,pParent);
    }

    HRESULT GetParentInternal(foundation::IObject **ppParent) override
    {
        return this->GetInner()->_GetParent(this->_GetParent(), ppParent);
    }

    void _SetParent(foundation::library::_INode_impl *pParent) override
    {
        this->GetInner()->_SetParent(this, pParent);
    }

};
//------------------------------------------------------------------------------
// Class:   ObjectNode
//
// Purpose: Template Base class for a Model with IObjectNode support
//
//------------------------------------------------------------------------------
template <
            class Model,
            class ModelTypeInfo,
            class ModelClass,
            class TDelegate,
            class TInitializeParameter,
            class TEventSource,
            class TEventHandler,
            class TSender,
            class TEventArgs,
            class TLogCategoryFactory,
            class TBASE = foundation::ctl::ComposableWeakRefSourceInspectableBase
        >
class ObjectNode :
    public CInterfaceAdapterFactoryBase
    <
        _ObjectBase<Model, ModelClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, ModelTypeInfo, TLogCategoryFactory, TBASE>,
        foundation::library::IInterfaceAdapterFactoryHost
    >
{
private:
    // this Model Type
    typedef ObjectNode<Model, ModelTypeInfo, ModelClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, TLogCategoryFactory, TBASE>  _This_type;

    typedef _ObjectBase<Model, ModelClass, TDelegate, TInitializeParameter, TEventSource, TEventHandler, TSender, TEventArgs, ModelTypeInfo, TLogCategoryFactory, TBASE> _ObjectBase_type;

    // Our Base Model template
    typedef CInterfaceAdapterFactoryBase
    <
        _ObjectBase_type,
        foundation::library::IInterfaceAdapterFactoryHost
    > _BaseType;

public:

    HRESULT _Initialize(const TInitializeParameter *pInitializeParameter)
    {
        IFR_ASSERT(_BaseType::_Initialize(pInitializeParameter));

        m_NotifyInModelChanges = IsOptionEnabled(
            pInitializeParameter->m_options,
            pmod::library::CommonOptions_NotifyInModelChanges);

        m_UseHierarchyHostAdapterFactory = IsOptionEnabled(
            pInitializeParameter->m_options,
            pmod::library::CommonOptions_UseHierarchyHostAdapterFactory);

        m_VerifyTypeInfoInternal = IsOptionEnabled(
            pInitializeParameter->m_options,
            pmod::library::CommonOptions_VerifyTypeInfoInternal);

        return S_OK;
    }


    HRESULT _QueryCreateAdapterInterfaceImpl(
        foundation::IInspectable* pOuter,
        foundation::IInspectable* pInner,
        REFIID iid,
        REFIID iidHostType,
        foundation::IInspectable **ppModeAdapterNewInstance) override
    {
        foundation_assert(pOuter != nullptr);

        HRESULT hr = E_NOINTERFACE;

        if(m_UseHierarchyHostAdapterFactory)
        {
            // next block will attempt to resolve the Interface Adapter Factory on 
            // the model hierarchy
            foundation::IUnknown *pInnerUnk = pInner;
            foundation::library::IInterfaceAdapterFactoryHost *pModelAdapterFactoryHost = this;
            while(pModelAdapterFactoryHost)
            {
                hr = pModelAdapterFactoryHost->QueryCreateInterfaceAdapter(
                    pOuter,
                    pInner,
                    iidHostType,
                    iid,
                    ppModeAdapterNewInstance);
                if(hr == E_NOINTERFACE)
                {
                    foundation::IObject *pObjectNode = nullptr;
                    foundation::QueryWeakReference(pInnerUnk,&pObjectNode);
                    foundation_assert(pObjectNode);

                    foundation::ComPtr<foundation::IObject> spParentNode;
                    foundation::GetObjectParent(pObjectNode, spParentNode.GetAddressOf());

                    pModelAdapterFactoryHost = nullptr;
                    if(spParentNode != nullptr)
                    {
                        foundation::QueryWeakReference(spParentNode,&pModelAdapterFactoryHost);
                        pInnerUnk = pModelAdapterFactoryHost;
                    }
                }
                else
                {
                    // either SUCCEED or FAILURE
                    break;
                }
            }
        }
        else
        {
            // if no hierarchy we can attempt a local attempt on our 'CInterfaceAdapterFactoryBase'
            hr = this->QueryCreateAdapterInterfaceInternal(
                pOuter,
                pInner,
                iidHostType,
                iid,
                ppModeAdapterNewInstance);
        }

        if(hr == E_NOINTERFACE)
        {
            return this->_BaseType::_QueryCreateAdapterInterfaceImpl(
                pOuter,
                pInner,
                iid,
                iidHostType,
                ppModeAdapterNewInstance);
        }
        return hr;
    }


protected:
    ObjectNode():
          m_InModelChanges(false),
          m_NotifyInModelChanges(false),
          m_UseHierarchyHostAdapterFactory(false),
          m_VerifyTypeInfoInternal(false)
    {
    }
#ifdef DEBUG
    ~ObjectNode()
    {
    }
#endif

    bool IsInModelChanges() { return m_InModelChanges; }
    bool IsVerifyTypeInfoInternal() { return m_VerifyTypeInfoInternal; }

    inline foundation::library::ICriticalSection *GetNodeCriticalSection()
    {
        return this->GetCriticalSectionInterface();
    }

    // _FoundationBase overrides
    HRESULT DisposeInternal() override
    {
        this->ClearNodeTree();

        return S_OK;
    }

    // FoundationBase Override
    void OnFinalRelease() override
    {
        // NOTE:  this code was migrated from pre_final_release 'deprecated'
        // override. This mean the code below can't add references during 
        // any parent/child notification

        // Cleanup parent/child relationship
        this->ClearNodeTree();

        _BaseType::OnFinalRelease();
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        HRESULT hr = S_OK;
        if (iid == pmod::library::IObjectNodeClass::GetIID())
        {
            *ppInterface = static_cast<pmod::library::IObjectNodeClass *>(this);
        }
        else if (iid == foundation::_IObjectNode::GetIID())
        {
            return this->GetOrCreateObjectNodeAdapter(reinterpret_cast<foundation::_IObjectNode **>(ppInterface));
        }
        else if (iid == foundation::library::_INode_impl::GetIID())
        {
            foundation::_IObjectNode *pObjectNode;
            IFR_ASSERT(this->GetOrCreateObjectNodeAdapter(&pObjectNode));
            _ObjectNodeClassAdapter *pObjectNode_adapter = static_cast<_ObjectNodeClassAdapter *>(pObjectNode);
            *ppInterface = static_cast<foundation::library::_INode_impl *>(pObjectNode_adapter);
        }
        else
        {
            hr = _BaseType::QueryInterfaceImpl(iid,ppInterface);
        }
        return hr;
    }

    // IObjectNodeClass Interface
    STDMETHOD(RuntimeInitialize)()
    {
        if(this->m_pDelegate)
        {
            IFR(this->m_pDelegate->OnRuntimeInitialize());
        }        
        return S_OK;
    }

    STDMETHOD(_OnParentChanged)(_In_opt_ foundation::library::_INode_impl *pParent) override
    {
        UNREFERENCED_PARAMETER(pParent);
        return S_OK;
    }

    STDMETHOD(_OnAncestorChanged)(
        _In_ foundation::library::_INode_impl *pChild,
        _In_opt_ foundation::library::_INode_impl *pParent) override
    {
        UNREFERENCED_PARAMETER(pChild);
        UNREFERENCED_PARAMETER(pParent);
        return S_OK;
    }

    // Virtual methods to Override


    virtual HRESULT OnFireNotifyInModelChangesChanged(bool newValue)
    {
        UNREFERENCED_PARAMETER(newValue);
        return S_OK;
    }

    virtual HRESULT OnBeginModelChangesInternal(pmod::library::BatchModelChangesOptions options)
    {
        UNREFERENCED_PARAMETER(options);
        return S_OK;
    }

    virtual HRESULT OnEndModelChangesInternal(pmod::library::EndModelChangesOptions options)
    {
        UNREFERENCED_PARAMETER(options);
        return S_OK;
    }

    STDMETHOD(BeginModelChanges)(_In_ pmod::library::BatchModelChangesOptions options)
    {
        this->LogObject(foundation::LoggingLevel::Debug, pmod::ObjectNode_Category_BeginModelChanges_Id);

        if(m_InModelChanges)
        {
            return E_UNEXPECTED;
        }
        m_InModelChanges = true;
        HRESULT hr = this->OnBeginModelChangesInternal(options);
        if(FAILED(hr))
        {
            m_InModelChanges = false;
        }
        IFHR(hr);
        if(m_NotifyInModelChanges)
        {
            this->OnFireNotifyInModelChangesChanged(true);
        }
        return S_OK;
    }

    STDMETHOD(EndModelChanges)(_In_ pmod::library::EndModelChangesOptions options)
    {
        this->LogObject(foundation::LoggingLevel::Debug, pmod::ObjectNode_Category_EndModelChanges_Id);

        if (!m_InModelChanges)
        {
            return E_UNEXPECTED;
        }
        m_InModelChanges = false;
        IGNOREHR(this->OnEndModelChangesInternal(options));
        if(m_NotifyInModelChanges)
        {
            this->OnFireNotifyInModelChangesChanged(false);
        }
        return S_OK;
    }

    STDMETHOD(GetIsInModelChanges)(_Outptr_ bool *pValue)
    {
        IFCPTR_ASSERT(pValue);
        *pValue = m_InModelChanges;
        return S_OK;
    }

    STDMETHOD(GetLocalWorkingDispatcher)(_In_ foundation::library::IDispatcher **ppWorkingDispatcher)
    {
        IFCPTR_ASSERT(ppWorkingDispatcher);
        return m_spWorkingDispatcher.CopyTo(ppWorkingDispatcher);
    }

    STDMETHOD(SetLocalWorkingDispatcher)(_Outptr_ foundation::library::IDispatcher *pWorkingDispatcher)
    {
        m_spWorkingDispatcher = pWorkingDispatcher;
        return S_OK;
    }

    STDMETHOD(GetWorkingDispatcher)(_In_ foundation::library::IDispatcher **ppWorkingDispatcher)
    {
        IFCPTR_ASSERT(ppWorkingDispatcher);
        *ppWorkingDispatcher = GetWorkingDispatcherInternal();
        if (*ppWorkingDispatcher)
        {
            (*ppWorkingDispatcher)->AddRef();
        }
        return S_OK;
    }

    STDMETHOD(SetParentInternal)(foundation::IObject *pParent) override
    {
        foundation::ComPtr<foundation::library::_INode_impl> spParentNodeImpl;
        IFR_ASSERT(foundation::QueryInterfaceIf(pParent, spParentNodeImpl.GetAddressOf()));

        return SetParentNodeImpl(spParentNodeImpl);
    }

    STDMETHOD(_GetParent)(foundation::library::_INode_impl *pParent, foundation::IObject **ppParent)
    {
        return foundation::QueryInterfaceIf(pParent, ppParent);
    }

    STDMETHOD(_SetParent)(
        foundation::library::_INode_impl *pThisNode, 
        foundation::library::_INode_impl *pParentNodeImpl) override
    {
        if (pParentNodeImpl != nullptr &&
            this->IsLevelEnabled(
            foundation::LoggingLevel::Trace,
                pmod::ObjectNode_Category_SetParent_Id))
        {
            foundation::IObject *pParentObject;
            foundation::QueryWeakReference(pParentNodeImpl,&pParentObject);
            foundation_assert(pParentObject != nullptr);
            UINT32 parentUniqueId;
            pParentObject->GetUniqueId(&parentUniqueId);
            IID iidParentType;
            foundation::GetObjectType(pParentObject,iidParentType);

            foundation::logger_util::Log(
                CLogger::GetLogger(),
                this->AsInspectable(),
                foundation::LoggingLevel::Trace,
                // Message Enum Value
                pmod::ObjectNode_Category_SetParent_Id,
                // Log Category
                TLogCategoryFactory::GetLogCategory()->AsInterface(),
                pmod::SetParent_LogRecordEnum::TypeId,
                // Child UniqueId
                pmod::SetParent_LogRecord::ChildUniqueId,
                foundation::pv_util::CreateValue((UINT32)this->m_UniqueId).Get(),
                // Child  TypeId
                pmod::SetParent_LogRecord::ChildTypeId,
                foundation::pv_util::CreateValue(this->GetTypeInfo()).Get(),
                // Parent UniqueId
                pmod::SetParent_LogRecord::ParentUniqueId,
                foundation::pv_util::CreateValue(parentUniqueId).Get(),
                // Parent  TypeId
                pmod::SetParent_LogRecord::ParentTypeId,
                foundation::pv_util::CreateValue(iidParentType).Get(),
                -1);
        }
        else if (pParentNodeImpl == nullptr &&
            this->IsLevelEnabled(
            foundation::LoggingLevel::Trace,
                pmod::ObjectNode_Category_ClearParent_Id))
        {
            foundation::logger_util::Log(
                CLogger::GetLogger(),
                this->AsInspectable(),
                foundation::LoggingLevel::Trace,
                // Message Enum Value
                pmod::ObjectNode_Category_ClearParent_Id,
                // Log Category
                TLogCategoryFactory::GetLogCategory()->AsInterface(),
                foundation::Object_LogRecordEnum::TypeId,
                // UniqueId
                foundation::Object_LogRecord::UniqueId,
                foundation::pv_util::CreateValue(this->m_UniqueId).Get(),
                //  TypeId
                foundation::Object_LogRecord::TypeId,
                foundation::pv_util::CreateValue(this->GetTypeInfo()).Get(),
                -1);
        }
        // perform the parent changes
        _ObjectNodeClassAdapter *pObjectNode_adapter = static_cast<_ObjectNodeClassAdapter *>(pThisNode);
        pObjectNode_adapter->SetParent(pParentNodeImpl);
        return S_OK;
    }

    HRESULT GetOrCreateObjectNodeAdapter(foundation::_IObjectNode **pp)
    {
        return this->template GetOrCreateAdapter<_ObjectNodeClassAdapter>(
            this->CastToInspectable(),
            pp);
    }

    HRESULT SetParentNodeImpl(_In_opt_ foundation::library::_INode_impl *pParent)
    {
        foundation::_IObjectNode *pObjectNode;
        IFR_ASSERT(this->GetOrCreateObjectNodeAdapter(&pObjectNode));
        _ObjectNodeClassAdapter *pObjectNode_adapter = static_cast<_ObjectNodeClassAdapter *>(pObjectNode);
        pObjectNode_adapter->SetParent(pParent);
        return S_OK;
    }

    HRESULT SetAsParentNodeImpl(
        _In_opt_ foundation::IInspectable *pInspectable)
    {
        if (pInspectable == nullptr)
        {
            return S_OK;
        }
        foundation_assert(!this->IsFinalRelease());

        if (this->IsFinalRelease())
        {
            _IFR_(E_UNEXPECTED);
        }
        foundation::library::_INode_impl *pNode_impl = nullptr;
        foundation::QueryWeakReference(pInspectable, &pNode_impl);

        if (pNode_impl == nullptr)
        {
            return S_FALSE;
        }

        foundation::_IObjectNode *pThisObjectNode;
        IFR_ASSERT(this->GetOrCreateObjectNodeAdapter(&pThisObjectNode));
        foundation::library::_INode_impl *pThisNode_impl = 
            static_cast<foundation::library::_INode_impl *>(static_cast<_ObjectNodeClassAdapter *>(pThisObjectNode));

        if (
            // not myself
            pNode_impl != pThisNode_impl &&
            // Parent was null
            pNode_impl->_GetParent() == nullptr &&
            // the Target Node does not contain me as a child
            !pNode_impl->_IsChildContained(pThisNode_impl)
            )
        {
            pNode_impl->_SetParent(pThisNode_impl);
        }
        return S_OK;
    }

    HRESULT GetParentNodeImpl(foundation::library::_INode_impl **ppParent)
    {
        foundation::library::_INode_impl *pThisNode_impl = nullptr;
        IFR_ASSERT(this->QueryAdaptersInternal(&pThisNode_impl));
        if (pThisNode_impl != nullptr)
        {
            *ppParent = pThisNode_impl->_GetParent();
        }
        else
        {
            *ppParent = nullptr;
        }
        return S_OK;
    }

    void ClearNodeTree()
    {
        foundation::library::_INode_impl *pThisNode_impl = nullptr;
        IGNOREHR(this->QueryAdaptersInternal(&pThisNode_impl));
        if (pThisNode_impl != nullptr)
        {
            _ObjectNodeClassAdapter *pObjectNode_adapter = static_cast<_ObjectNodeClassAdapter *>(pThisNode_impl);
            pObjectNode_adapter->ClearNodeTree();
        }
    }

    HRESULT GetParentNode(foundation::IObject **ppParent)
    {
        foundation::library::_INode_impl *pParentNode_impl;
        IFR_ASSERT(this->GetParentNodeImpl(&pParentNode_impl));
        if (pParentNode_impl != nullptr)
        {
            IFR_ASSERT(foundation::QueryWeakReference(pParentNode_impl, ppParent));
        }
        else
        {
            *ppParent = nullptr;
        }
        return S_OK;
    }

    HRESULT ClearParentNodeImpl(_In_opt_ foundation::IInspectable *pInspectable)
    {
        if (pInspectable == nullptr)
        {
            return S_OK;
        }
        foundation::library::_INode_impl *pThisNode_impl = nullptr;
        IFR_ASSERT(this->QueryAdaptersInternal(&pThisNode_impl));

        if (pThisNode_impl == nullptr)
        {
            return S_FALSE;
        }
        foundation::library::_INode_impl *pNode_impl = nullptr;
        foundation::QueryWeakReference(pInspectable,&pNode_impl);

        if (pNode_impl != nullptr && (pNode_impl->_GetParent() == pThisNode_impl))
        {
            pNode_impl->_SetParent(nullptr);
        }
        return S_OK;
    }

public:
    foundation::library::IDispatcher *_GetEventDispatcher() override
    {
        foundation::IObject *pObjectNode = this;

        while (pObjectNode != nullptr)
        {
            foundation::ComPtr<pmod::library::IObjectNodeClass> spObjectNodeClass;
            foundation::QueryInterface(pObjectNode, spObjectNodeClass.GetAddressOf());
            if (spObjectNodeClass != nullptr)
            {
                foundation::ComPtr<foundation::library::IDispatcher> spEventDispatcher;
                spObjectNodeClass->GetEventDispatcher(spEventDispatcher.GetAddressOf());
                if (spEventDispatcher != nullptr)
                {
                    return spEventDispatcher;
                }
            }
            foundation::ComPtr<foundation::IObject> spParentNode;
            foundation::GetObjectParent(pObjectNode, spParentNode.GetAddressOf());
            pObjectNode = spParentNode;
        }
        // return Instance Event Dispatcher
        return CClassStatics::GetInstanceInternal()->GetEventDispatcher();
    }

    foundation::library::IDispatcher *GetWorkingDispatcherInternal()
    {
        foundation::IObject *pObjectNode = this;

        while(pObjectNode != nullptr)
        {
            foundation::ComPtr<pmod::library::IObjectNodeClass> spObjectNodeClass;
            foundation::QueryInterface(pObjectNode, spObjectNodeClass.GetAddressOf());
            if(spObjectNodeClass != nullptr)
            {
                foundation::ComPtr<foundation::library::IDispatcher> spWorkingDispatcher;
                spObjectNodeClass->GetLocalWorkingDispatcher(spWorkingDispatcher.GetAddressOf());
                if (spWorkingDispatcher != nullptr)
                {
                    return spWorkingDispatcher;
                }
            }
            foundation::ComPtr<foundation::IObject> spParentNode;
            foundation::GetObjectParent(pObjectNode, spParentNode.GetAddressOf());
            pObjectNode = spParentNode;
        }
        // return Instance Working Dispatcher
        return CClassStatics::GetInstanceInternal()->GetWorkingDispatcher();
    }

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObjectBase(const CHAR_t *pTypeName)
    {
        ULONG refCnt;
        foundation::IUnknown *pControllingUnk = nullptr;
        if(this->IsComposed())
        {
            foundation::QueryWeakReference(
                this->AsInspectable(),
                foundation_IID_IUnknown,
                &pControllingUnk);
            foundation_assert(pControllingUnk);
            pControllingUnk->AddRef();
            refCnt = pControllingUnk->Release();
        }
        else
        {
            refCnt = this->GetRefCount();
        }

        _traceMessage(U("Type:%s UniqueId:%d this:0x%04x pControllingUnk:0x%04x RefCount:%d"),pTypeName,this->m_UniqueId,this,pControllingUnk,refCnt);
    }

    static void DumpHierarchy(foundation::IObject * pParent)
    {
        int cCount = 0;
        HRESULT hr = S_OK;
        foundation::IObject *pNextParent = pParent;
        if (!pNextParent)
        {
            _traceMessage(U("---No Parent---"));
        }
        else
        {
            while(pNextParent)
            {
                pmod::IObservableObject *pObservableObject = nullptr;
                pmod::IObservableCollection *pObservableCollection = nullptr;
                if(SUCCEEDED(foundation::QueryWeakReference(pNextParent,&pObservableCollection)) && pObservableCollection)
                {
                    // Ancestor found as a collection
                    UINT32 uniqueId = 0;
                    pObservableCollection->GetUniqueId(&uniqueId);
                    _traceMessage(U("IObservableCollection(ID=%d)->"), uniqueId);
                }
                else if(SUCCEEDED(foundation::QueryWeakReference(pNextParent,&pObservableObject)) && pObservableObject)
                {
                    // ancestor found as a PropertyModel
                    UINT32 uniqueId = 0;
                    pObservableObject->GetUniqueId(&uniqueId);
                    _traceMessage(U("IObservableObject(ID=%d)->"), uniqueId);
                }
                cCount++;
                // next Parent to found

                foundation::ComPtr<foundation::IObject> spNextParent;
                hr = foundation::GetObjectParent(pNextParent, spNextParent.GetAddressOf());
                if (FAILED(hr))
                {
                    break;
                }
                pNextParent = spNextParent;
            }
        }
        _traceMessage(U(" Depth:%d"), cCount);
    }

#endif

private:
    bool                    m_InModelChanges:1;
    bool                    m_NotifyInModelChanges:1;
    bool                    m_UseHierarchyHostAdapterFactory:1;
    bool                    m_VerifyTypeInfoInternal:1;
    foundation::ComPtr<foundation::library::IDispatcher> m_spWorkingDispatcher;
};

