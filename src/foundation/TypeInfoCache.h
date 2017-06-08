/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeInfoCache.h
****/

#pragma once

#include "FoundationUtilHelper.h"
#include "TypeInfo.h"
#include "CriticalSection.h"

#include <foundation/errors.h>
#include <foundation/library/foundation_library.h>
#include <foundation/library/disposable_util.h>
#include <map>
#include <assert.h>

#include <foundation/weak_reference.h>

#include "PtrSingleton.h"

#if DEBUG
#include "DebugUtil.h"
#endif

//------------------------------------------------------------------------------
// Class:   _TTypeInfoCache Template
//
// Purpose: Define a template class that allow caching of TypeModelInfo
//
//------------------------------------------------------------------------------
template <class TypeInfoClass,class TUnkCast = TInspectableCast>
class _TTypeInfoCache
{
public:
    typedef std::map<IID, TypeInfoClass *, foundation::library::GuidLessThan> _TypeInfoClassMapType;

    _TTypeInfoCache()
    {
        m_critsec.Init();
    }

    virtual ~_TTypeInfoCache()
    {
        CleanupInternal();
    }

    const typename _TTypeInfoCache<TypeInfoClass,TUnkCast>::_TypeInfoClassMapType& GetTypeModelInfoClassMap() const
    {
        return m_TypeInfos;
    }

    TypeInfoClass *LookupTypeByTypeId(UINT32 typeId)
    {
        foundation_lib::_CriticalSectionLock csl(m_critsec);

        for (typename _TypeInfoClassMapType::const_iterator iter = m_TypeInfos.begin();
            iter != m_TypeInfos.end();
            ++iter)
        {
            UINT32 _typeId;
            (*iter).second->GetTypeId(&_typeId);
            if(typeId == _typeId)
            {
                return (*iter).second;
            }
        }
        return nullptr;
    }

    TypeInfoClass *LookupTypeByName(LPCSTR_t pName)
    {
        foundation_lib::_CriticalSectionLock csl(m_critsec);

        for(typename _TypeInfoClassMapType::const_iterator iter = m_TypeInfos.begin();
            iter != m_TypeInfos.end();
            ++iter)
        {
            foundation::HStringPtr _name;
            (*iter).second->GetName(_name.GetAddressOf());
            if(_pal_strcmp(_pal_GetStringRawBuffer(_name,nullptr),pName)==0)
            {
                return (*iter).second;
            }
        }
        return nullptr;
    }

    TypeInfoClass *LookupTypeByNamespaceAndName(LPCSTR_t pNamespace,LPCSTR_t pName)
    {
        foundation_lib::_CriticalSectionLock csl(m_critsec);

        for (typename _TypeInfoClassMapType::const_iterator iter = m_TypeInfos.begin();
            iter != m_TypeInfos.end();
            ++iter)
        {
            foundation::string_t namespace_, name;
            (*iter).second->_GetNamespaceAndName(namespace_, name);
            if ((pNamespace == nullptr || (_pal_strcmp(pNamespace, namespace_.c_str()) == 0))
                && (_pal_strcmp(pName, name.c_str()) == 0))
            {
                return (*iter).second;
            }
        }
        return nullptr;
    }

    void GetTypeInfos(
        foundation::_TypeInfoFilter *pTypeInfoFilter,
        std::vector<TypeInfoClass *>& typeInfos
        )
    {
        foundation_lib::_CriticalSectionLock csl(m_critsec);

        for (typename _TypeInfoClassMapType::const_iterator iter = m_TypeInfos.begin();
            iter != m_TypeInfos.end();
            ++iter)
        {
            if (pTypeInfoFilter == nullptr ||
                FilterTypeInfo((*iter).second, pTypeInfoFilter))
            {
                typeInfos.push_back((*iter).second);
            }
        }
    }

    TypeInfoClass *LookupType(const IID& iid)
    {
        foundation_lib::_CriticalSectionLock csl(m_critsec);

        typename _TypeInfoClassMapType::iterator iter = m_TypeInfos.find(iid);
        if(iter != m_TypeInfos.end())
        {
            // Note we don't AddRef
            return iter->second;
        }
        return nullptr;
    }

    void AddToCache(const IID& iid,TypeInfoClass *pTypeInfoClass,bool allowDuplicate = false)
    {
        foundation_lib::_CriticalSectionLock csl(m_critsec);
        if (allowDuplicate)
        {
            TypeInfoClass *pCurrentTypeInfoClass = LookupType(iid);
            // replace or insert the type info
            m_TypeInfos[iid] = pTypeInfoClass;
            if(pCurrentTypeInfoClass != nullptr)
            {
                // release last existing TypeInfo reference
                foundation::IUnknown *pUnk = CastToUnknown(pCurrentTypeInfoClass);
                // dispose first the type info 
                foundation::library::DisposeIf(pUnk);
                // release
                pUnk->Release();
            }
        }
        else
        {
#if DEBUG
            foundation_assert(LookupType(iid) == nullptr);
#endif
            m_TypeInfos.insert(typename _TypeInfoClassMapType::value_type(iid, pTypeInfoClass));
        }
    }

    void Clean()
    {
        CleanupInternal();
    }
    // manually lock/unlock
    void Lock()
    {
        m_critsec.Lock();
    }
    void Unlock()
    {
        m_critsec.Unlock();
    }
protected:

    // Note:
    // This method will browse the tree and Release the 'TypeInfoClass' reference
    void CleanupInternal()
    {
#if defined(DEBUG)
        std::vector<foundation::WeakReferencePtr> typeInfosWeakRef;
        typeInfosWeakRef.resize(m_TypeInfos.size());
#endif

        // Final release of Type Model Info references
        for (typename _TypeInfoClassMapType::iterator itType = m_TypeInfos.begin();
             itType != m_TypeInfos.end(); itType++)
        {
            foundation::IUnknown *pUnk = CastToUnknown(itType->second);
#if defined(DEBUG)
            foundation::AsWeakRef(typeInfosWeakRef[std::distance(m_TypeInfos.begin(), itType)], pUnk);
#endif
            pUnk->Release();

        }
#if defined(DEBUG)
        if ((GetDebugUtilOptionsFlagsInternal() & foundation::library::_OptionTrackTypeInfoLeaks) != 0)
        {
            // Assert if there are existing reference to the 'TypeInfoClass'
            for (std::vector<foundation::WeakReferencePtr>::const_iterator iter = typeInfosWeakRef.begin();
                iter != typeInfosWeakRef.end(); iter++)
            {
                foundation::IInspectable *pStrongRef = nullptr;
                foundation::ResolveWeakRefWeak(*iter, pStrongRef);
                assert(pStrongRef == nullptr);
            }
        }
#endif

        // Ensure we remove any created Type Model Info
        // Please note the use of 'swap' to ensure after the 
        // call, every mem allocated by the Map is gone
        _TypeInfoClassMapType().swap(m_TypeInfos);
    }
private:
    static foundation::IUnknown *CastToUnknown(_In_ TypeInfoClass *pTypeInfoClass)
    {
        foundation::IUnknown *pTypeInfoClassUnk = nullptr;
        TUnkCast::CastTo(pTypeInfoClass, &pTypeInfoClassUnk);
        foundation_assert(pTypeInfoClassUnk);
        return pTypeInfoClassUnk;
    }
    static bool FilterTypeInfo(
        _In_ TypeInfoClass *pTypeInfoClass,
        _In_ foundation::_TypeInfoFilter *pTypeInfoFilter)
    {
        foundation_assert(pTypeInfoClass != nullptr);
        foundation_assert(pTypeInfoFilter != nullptr);

        foundation::IObjectTypeInfo *pObjectTypeInfo = foundation::CastTo<foundation::IObjectTypeInfo>(CastToUnknown(pTypeInfoClass));
        if (pObjectTypeInfo == nullptr)
        {
            foundation_assert(false);
            return false;
        }

        if (pTypeInfoFilter->_NamePrefix != nullptr &&
            _pal_strlen(pTypeInfoFilter->_NamePrefix) != 0)
        {
            UINT32 len = static_cast<UINT32>(_pal_strlen(pTypeInfoFilter->_NamePrefix));

            foundation::HStringPtr name;
            pObjectTypeInfo->GetName(name.GetAddressOf());

            if ((name.GetLength() < len) ||
                (_pal_strncmp(name.GetRawBuffer(),pTypeInfoFilter->_NamePrefix,len) != 0))
            {
                return false;
            }
        }
        if (pTypeInfoFilter->_Type != foundation_GUID_NULL)
        {
            foundation::ComPtr<foundation::IUnknown> unk_ptr;
            if (FAILED(foundation::QueryInterface(
                pObjectTypeInfo,
                pTypeInfoFilter->_Type, 
                unk_ptr.GetAddressOf())))
            {
                return false;
            }
        }
        if (!pTypeInfoFilter->_IncludeReserved)
        {
            UINT32 typeId;
            pObjectTypeInfo->GetTypeId(&typeId);

            if (typeId >= foundation::_ReservedTypeId_Base ||
                typeId < 0x500)
            {
                return false;
            }
        }
        return true;
    }

private:
    foundation_lib::CAutoDeleteCriticalSection          m_critsec;
    // When using the std::map we need to delay the mem allocator
    _TypeInfoClassMapType m_TypeInfos;
};

//------------------------------------------------------------------------------
// Class:   _TStaticTypeInfoCache Template
//
// Purpose: Define a template class that allow caching of a model core type
//
//------------------------------------------------------------------------------

template <
    class T, 
    class TypeInfoClass, 
    class TUnkCast = TInspectableCast>
class _TStaticTypeInfoCache
{
public:
    typedef TypeInfoClass TypeInfoClass_Type;
    typedef _TTypeInfoCache<TypeInfoClass,TUnkCast> _TypeModelInfoCacheType;

    static bool IsTypeSupported(const IID& iid)
    {
        return LookupType(iid) != nullptr;
    }

    static void GetTypeInfos(
        foundation::_TypeInfoFilter *pTypeInfoFilter,
        std::vector<TypeInfoClass *>& typeInfos)
    {
        return InternalGetInstance().GetTypeInfos(pTypeInfoFilter, typeInfos);
    }

    static TypeInfoClass *LookupType(const IID& iid)
    {
        return InternalGetInstance().LookupType(iid);
    }

    static TypeInfoClass *LookupTypeByTypeId(UINT32 typeId)
    {
        return InternalGetInstance().LookupTypeByTypeId(typeId);
    }

    static TypeInfoClass *LookupTypeByName(LPCSTR_t pName)
    {
        return InternalGetInstance().LookupTypeByName(pName);
    }

    static TypeInfoClass *LookupTypeByNamespaceAndName(LPCSTR_t pNamespace, LPCSTR_t pName)
    {
        return InternalGetInstance().LookupTypeByNamespaceAndName(pNamespace, pName);
    }

    static void AddToCache(const IID& iid, TypeInfoClass *pTypeModelInfo, bool allowDuplicate = false)
    {
        InternalGetInstance().AddToCache(iid, pTypeModelInfo, allowDuplicate);
    }

    static void Cleanup()
    {
        InternalGetInstance().Clean();
    }

    static void Lock()
    {
        InternalGetInstance().Lock();
    }
    static void Unlock()
    {
        InternalGetInstance().Unlock();
    }

protected:
    static _TypeModelInfoCacheType& InternalGetInstance()
    {
        return T::GetInstance();
    }
private:
};

//------------------------------------------------------------------------------
// Class:   CStaticTypeInfoCache Template
//
// Purpose: template derived from _TStaticTypeInfoCache to allow a typedef
//
//------------------------------------------------------------------------------
template <class TypeInfoClass, class TUnkCast = TInspectableCast>
class CStaticTypeInfoCache :
    public _TStaticTypeInfoCache < CStaticTypeInfoCache<TypeInfoClass, TUnkCast>, TypeInfoClass, TUnkCast >
{
public:
    static typename CStaticTypeInfoCache::_TypeModelInfoCacheType& GetInstance()
    {
        static PtrSingleton<typename CStaticTypeInfoCache::_TypeModelInfoCacheType> instance;
        return *instance.GetInstance();
    }
};
