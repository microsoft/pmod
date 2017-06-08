/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypeInfo.h
****/

#pragma once

#include <foundation/library/foundation_library.h>
#include <foundation/errors.h>
#include <foundation/dictionary_ptr.h>

#include <foundation/library/base_adapter_host.h>

#include <foundation/ctl/com_library.h>
#include <foundation/ctl/com_weak_reference_source.h>

#include <map>

#include "CastHelper.h"

struct ITypeInfoCacheProvider 
{
    virtual HRESULT GetTypeInfos(
        foundation::_TypeInfoFilter *pTypeInfoFilter,
        std::vector<foundation::IObjectTypeInfo *>& typeInfos) = 0;

    virtual HRESULT LookupTypeInfo(
        _In_ const IID& iidType,
        _Outptr_ foundation::IObjectTypeInfo **ppTypeInfo) = 0;

    virtual HRESULT CreateTypeInfo(
        _In_ const foundation::TypeInfo *pTypeInfo,
        _Outptr_ foundation::IObjectTypeInfo **ppTypeInfo) = 0;

    virtual HRESULT CreateTypeInfoClass(
        _In_ const IID&    iidType,
        bool allowDuplicate,
        _Outptr_ foundation::IObjectTypeInfo **ppTypeInfoInstance) = 0;

    virtual HRESULT InitializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pCreatedTypeInfo,
        _In_ const foundation::TypeInfo *pTypeInfo
        ) = 0;

    virtual HRESULT InitializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pCreatedTypeInfo,
        const foundation::DictionaryPtr& type_info_ptr
        ) = 0;

    virtual HRESULT SerializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pTypeInfo,
        foundation::DictionaryPtr& type_info_ptr
        ) = 0;

    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

class CTypeInfoCache
{
public:
    // get type infos with AddRef semantic
    static HRESULT GetTypeInfos(
        foundation::_TypeInfoFilter *pTypeInfoFilter,
        UINT32 *size,
        foundation::IObjectTypeInfo ***ppTypeInfos);

    // get type info with AddRef semantic
    static HRESULT GetTypeInfo(
        const IID& iidType,
        /*_COM_Outptr_result_maybenull_*/ foundation::IObjectTypeInfo **ppTypeInfo);

    // get type info using Weak Reference (Note: no AddRef will be called)
    static HRESULT LookupTypeInfo(
        const IID& iidType,
        /*_COM_Outptr_result_maybenull_*/ foundation::IObjectTypeInfo **ppTypeInfo);

    // get or create type info with weak reference semantic
    static HRESULT CreateOrGetTypeInfo(
        _In_ const foundation::TypeInfo *pTypeInfo,
        /*_COM_Outptr_*/ foundation::IObjectTypeInfo **ppTypeInfo);

    static HRESULT RegisterMultipleTypeInfos(_In_ const foundation::TypeInfo* typeInfoEntries[]);

    static void RegisterTypeInfoCacheProvider(
        UINT32 type_info_type,
        ITypeInfoCacheProvider *pTypeInfoProvider);

    static HRESULT RegisterMultipleTypeInfos(UINT32 size, foundation::IDictionary **ppDictionaryTypeInfo);

    static HRESULT SerializeTypeInfo(
        _In_ foundation::IObjectTypeInfo *pTypeInfo,
        foundation::IDictionary **ppDictionaryTypeInfo);

private:
    CTypeInfoCache();
    HRESULT GetTypeInfosInternal(
        foundation::_TypeInfoFilter *pTypeInfoFilter,
        UINT32 *size,
        foundation::IObjectTypeInfo ***ppTypeInfos);

    HRESULT LookupTypeInfoInternal(
        const IID& iidType,
        /*_COM_Outptr_result_maybenull_*/ foundation::IObjectTypeInfo **ppTypeInfo);

    HRESULT CreateOrGetTypeInfoInternal(
        _In_ const foundation::TypeInfo *pTypeInfo,
        /*_COM_Outptr_*/ foundation::IObjectTypeInfo **ppTypeInfo);

    HRESULT RegisterMultipleTypeInfosInternal(_In_ const foundation::TypeInfo* pTypeInfoEntries[]);
    HRESULT RegisterMultipleTypeInfosInternal(UINT32 size, foundation::IDictionary **ppDictionaryTypeInfo);

    HRESULT SerializeTypeInfoInternal(
        _In_ foundation::IObjectTypeInfo *pTypeInfo,
        foundation::IDictionary **ppDictionaryTypeInfo);

    static CTypeInfoCache& GetInstance();
private:
    typedef std::map<UINT32, ITypeInfoCacheProvider *> _TypeInfoCacheProvider_MapType;
    _TypeInfoCacheProvider_MapType _typeInfoCaches;
};

template <class T, const UINT32 _type_info_type, class TypeInfoClass_Create,class TCast = TStaticCast >
class CTypeInfoCacheProvider_Base :
    public ITypeInfoCacheProvider
{
public:
    CTypeInfoCacheProvider_Base(bool registerTypeInfo = true)
    {
        if (registerTypeInfo)
        {
            // register myself into the Type Info Cache
            CTypeInfoCache::RegisterTypeInfoCacheProvider(_type_info_type, this);
        }
    }

protected:
    typedef typename T::TypeInfoClass_Type TypeInfoClass;

    // interface ITypeInfoCacheProvider
    HRESULT GetTypeInfos(
        foundation::_TypeInfoFilter *pTypeInfoFilter,
        std::vector<foundation::IObjectTypeInfo *>& typeInfos) override
    {
        std::vector<TypeInfoClass *> typeInfos_;
        T::GetTypeInfos(pTypeInfoFilter,typeInfos_);
        for (typename std::vector<TypeInfoClass *>::const_iterator iter = typeInfos_.begin();
            iter != typeInfos_.end();
            ++iter)
        {
            foundation::IObjectTypeInfo *pObjectTypeInfo;
            TCast::CastTo(*iter, &pObjectTypeInfo);
            typeInfos.push_back(pObjectTypeInfo);
        }
        return S_OK;
    }

    HRESULT LookupTypeInfo(
        _In_ const IID& iidType,
        _Outptr_ foundation::IObjectTypeInfo **ppTypeInfo) override
    {
        TypeInfoClass *pTypeInfoClass = T::LookupType(iidType);
        if (pTypeInfoClass == nullptr)
        {
            return foundation::E_TYPE_INFO_NOT_REGISTERED;
        }
        return TCast::CastTo(pTypeInfoClass, ppTypeInfo);
    }

    HRESULT CreateTypeInfoClass(
        _In_ const IID&    iidType,
        bool allowDuplicate,
        _Outptr_ foundation::IObjectTypeInfo **ppTypeInfoClass) override
    {
        TypeInfoClass_Create *pCreateTypeInfoClass = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<TypeInfoClass_Create>::CreateInstance(&pCreateTypeInfoClass));
        // Now Cache this new created Type Info
        T::AddToCache(iidType, pCreateTypeInfoClass, allowDuplicate);

        // return pp
        *ppTypeInfoClass = pCreateTypeInfoClass;
        return S_OK;
    }

    void Lock() override
    {
        T::Lock();
    }

    void Unlock() override
    {
        T::Unlock();
    }
};

template <class T, const UINT32 _type_info_type>
class CTypeInfoCacheProvider :
    public CTypeInfoCacheProvider_Base<T, _type_info_type, typename T::TypeInfoClass_Type>
{
public:
    CTypeInfoCacheProvider(bool registerTypeInfo = true):
        CTypeInfoCacheProvider_Base<T,_type_info_type,TypeInfoClass>(registerTypeInfo)
    {}

private:
    typedef typename T::TypeInfoClass_Type TypeInfoClass;

    // interface ITypeInfoCacheProvider
    HRESULT CreateTypeInfo(
        _In_ const foundation::TypeInfo *pTypeInfo,
        _Outptr_ foundation::IObjectTypeInfo **ppTypeInfo) override
    {
        typedef typename TypeInfoClass::_TypeInfo_Type _TypeInfo_Type;
        TypeInfoClass *pTypeInfoClass = nullptr;
        IFR_ASSERT(TypeInfoClass::CreateInstance(static_cast<const _TypeInfo_Type *>(pTypeInfo), &pTypeInfoClass));
        *ppTypeInfo = pTypeInfoClass;
        return S_OK;
    }

    HRESULT InitializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pCreatedTypeInfo,
        _In_ const foundation::TypeInfo *pTypeInfo
        ) override
    {
        foundation_assert(pCreatedTypeInfo);
        foundation_assert(pTypeInfo);

        typedef typename TypeInfoClass::_TypeInfo_Type _TypeInfo_Type;
        TypeInfoClass *pTypeInfoClass = static_cast<TypeInfoClass *>(pCreatedTypeInfo);
        IFR_ASSERT(pTypeInfoClass->_Initialize(
            static_cast<const _TypeInfo_Type *>(pTypeInfo)));
        return S_OK;
    }

    HRESULT InitializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pCreatedTypeInfo,
        const foundation::DictionaryPtr& type_info_ptr
        ) override
    {
        foundation_assert(pCreatedTypeInfo);

        TypeInfoClass *pTypeInfoClass = static_cast<TypeInfoClass *>(pCreatedTypeInfo);
        IFR_ASSERT(pTypeInfoClass->_DeserializeTypeInfo(type_info_ptr));
        return S_OK;
    }

    HRESULT SerializeTypeInfoClass(
        _In_ foundation::IObjectTypeInfo *pTypeInfo,
        foundation::DictionaryPtr& type_info_ptr
        ) override
    {
        foundation_assert(pTypeInfo);

        TypeInfoClass *pTypeInfoClass = static_cast<TypeInfoClass *>(pTypeInfo);
        IFR_ASSERT(pTypeInfoClass->_SerializeTypeInfo(type_info_ptr));
        return S_OK;
    }

};

extern LPCSTR_t _TypeInfo_Type_Key;
extern LPCSTR_t _TypeInfo_BaseType_Key;
extern LPCSTR_t _TypeInfo_TypeId_Key;
extern LPCSTR_t _TypeInfo_IIdType_Key;
extern LPCSTR_t _TypeInfo_Name_Key;

BEGIN_DECLARE_INTERFACE(IObjectTypeInfoInternal, foundation::IUnknown,FOUNDATION_API)
    STDMETHOD(GetTypeInfo)(foundation::TypeInfo **ppTypeInfo) = 0;
END_DECLARE_INTERFACE()

inline GUID& _DuplicateIIDType(const GUID& iid)
{
    GUID *pType = new GUID;
    memcpy(pType, &iid, sizeof(GUID));
    return *pType;
}

HRESULT _DeserializeTypeInfo(
	const foundation::DictionaryPtr& type_info_ptr,
	_In_ foundation::TypeInfo *pTypeInfo);

HRESULT _SerializeTypeInfo(
	_In_ const foundation::TypeInfo *pTypeInfo,
	foundation::DictionaryPtr& type_info_ptr);

template <class T,class _TypeInfo = foundation::TypeInfo,class TBaseType = T>
class CTypeInfo :
    public foundation::library::_DefaultAdapterHost<foundation::IObjectTypeInfo, foundation::ctl::ComWeakReferenceSourceInspectableBase>,
    public T,
    public IObjectTypeInfoInternal
{
private:
    typedef foundation::library::_DefaultAdapterHost<foundation::IObjectTypeInfo, foundation::ctl::ComWeakReferenceSourceInspectableBase> _TBASE;
public:
    typedef T _ITypeInfo_Type;
    typedef _TypeInfo _TypeInfo_Type;

    TBaseType *GetBaseType() { return m_BaseType; }
    const _TypeInfo *GetTypeInfo() { return m_pTypeInfo; }

protected:
    CTypeInfo():
        m_BaseType(nullptr),
        m_pTypeInfo(nullptr),
        m_bDeleteTypeInfo(false)
    {
    }
public:
    HRESULT _InitializeTypeInfo(const _TypeInfo *pTypeInfo)
    {
        foundation_assert(pTypeInfo);
        m_pTypeInfo = pTypeInfo;

        return S_OK;
    }

    void SetDeleteTypeInfo(bool newValue)
    {
        m_bDeleteTypeInfo = newValue;
    }

    HRESULT _DeserializeTypeInfo(const foundation::DictionaryPtr& type_info_ptr)
    {
        _TypeInfo *pTypeInfo = new _TypeInfo;
		IFR_ASSERT(::_DeserializeTypeInfo(type_info_ptr, pTypeInfo));

        // assign to our members with delete type info on the dtor
        m_pTypeInfo = pTypeInfo;
        m_bDeleteTypeInfo = true;
        // resolve the base type
        if (type_info_ptr.HasKey(_TypeInfo_BaseType_Key))
        {
            IID baseType;
            type_info_ptr.GetValue(_TypeInfo_BaseType_Key, baseType);

            foundation::IObjectTypeInfo *pBaseTypeInfo = nullptr;
            CTypeInfoCache::LookupTypeInfo(baseType, &pBaseTypeInfo);
            foundation_assert(pBaseTypeInfo != nullptr);
            // assign to strong reference member
            this->SetBaseType(static_cast<TBaseType *>(pBaseTypeInfo));
        }
        return S_OK;
    }

    HRESULT _SerializeTypeInfo(foundation::DictionaryPtr& type_info_ptr)
    {
		IFR_ASSERT(::_SerializeTypeInfo(m_pTypeInfo, type_info_ptr));

        if (m_BaseType != nullptr)
        {
            IID baseType;
            static_cast<foundation::IObjectTypeInfo *>(m_BaseType)->GetType(&baseType);
            type_info_ptr.SetValue(_TypeInfo_BaseType_Key,baseType);
        }
        return S_OK;
    }

    // IObjectTypeInfoInternal Interface
    STDMETHOD(GetTypeInfo)(foundation::TypeInfo **ppTypeInfo)
    {
        IFCPTR_ASSERT(ppTypeInfo);
        IFCPTR_ASSERT(m_pTypeInfo);
        *ppTypeInfo = const_cast<_TypeInfo *>(m_pTypeInfo);
        return S_OK;
    }

    // IObjectTypeInfo Interface
    STDMETHOD(GetBaseType)(_COM_Outptr_result_maybenull_ foundation::IObjectTypeInfo **ppModelTypeInfo)
    {
        *ppModelTypeInfo = nullptr;
        if(m_BaseType != nullptr)
        {
            *ppModelTypeInfo = static_cast<foundation::IObjectTypeInfo *>(m_BaseType);
            (*ppModelTypeInfo)->AddRef();
            return S_OK;
        }
        return S_OK;
    }

    STDMETHOD (GetName)(_Outptr_ HSTRING* typeName)
    {
        IFCPTR_ASSERT(m_pTypeInfo);
        return CreateStringHelper(m_pTypeInfo->m_name,typeName);
    }

    STDMETHOD(GetTypeId)(UINT32* typeId)
    {
        IFCPTR_ASSERT(m_pTypeInfo);
        * typeId = m_pTypeInfo->m_type_id;
        return S_OK;
    }

    STDMETHOD (GetType)(_Out_ IID *iidType)
    {
        IFCPTR_ASSERT(m_pTypeInfo);
        *iidType = m_pTypeInfo->get_iid_type();
        return S_OK;
    }

    STDMETHOD(GetIsRuntimeCreated)(_Out_ bool *pValue)
    {
        *pValue = m_bDeleteTypeInfo;
        return S_OK;
    }

    void _GetNamespaceAndName(
        foundation::string_t& namespace_,
        foundation::string_t& name)
    {
        foundation::string_t fullName = m_pTypeInfo->m_name;
        size_t pos;
        if ((pos = fullName.find_last_of(U('.'))) != foundation::string_t::npos)
        {
            namespace_ = fullName.substr(0, pos);
            name = fullName.substr(pos + 1);
        }
        else
        {
            namespace_.clear();
            name = fullName;
        }
    }
protected:

    // Override ComInspectableBase
    void FinalRelease() override
    {
        if (m_bDeleteTypeInfo && m_pTypeInfo)
        {
            // free duplicated LPCSTR_t
            _pal_MemFree((LPVOID)m_pTypeInfo->m_name);

            // free duplicated GUID
            delete m_pTypeInfo->m_p_iid_type;
            // now the dynamic type info structure
            delete m_pTypeInfo;

            m_pTypeInfo = nullptr;
        }
        RELEASE_INTERFACE(m_BaseType);
    }

    void SetBaseType(TBaseType* pBaseType)
    {
        foundation_assert(m_BaseType == nullptr);
        m_BaseType = pBaseType;
        if (pBaseType)
        {
            pBaseType->AddRef();
        }
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::IObjectTypeInfo::GetIID())
        {
            *ppInterface = static_cast<foundation::IObjectTypeInfo *>(this);
        }
        else if (iid == IObjectTypeInfoInternal::GetIID())
        {
            *ppInterface = static_cast<IObjectTypeInfoInternal *>(this);
        }
        else if (iid == T::GetIID())
        {
            *ppInterface = static_cast<T *>(this);
        }
        else
        {
            return _TBASE::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
private:
	static HRESULT _DeserializeTypeInfo(
		const foundation::DictionaryPtr& type_info_ptr,
		_In_ foundation::IObjectTypeInfo *pTypeInfo);

private:
    TBaseType*                  m_BaseType;
protected:
    const _TypeInfo*            m_pTypeInfo;   // Reference to Model Type Info structure
    bool                        m_bDeleteTypeInfo;  // if delete Type Info on Final Release

};


