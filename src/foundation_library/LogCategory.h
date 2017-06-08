/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LogCategory.h
****/

#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/library/base_adapter_host.h>

#include <foundation/library/com_ptr_singleton.h>

#include <map>
#include <set>

#pragma once

class CEnumTypeInfo;
class CLogger;
struct _GetTypeFunc
{
    virtual bool operator()(IID* pType) const = 0;
};
//------------------------------------------------------------------------------
// Class:   CLogCategory
//
// Purpose: Implements the ILogCategory Interface
//
//------------------------------------------------------------------------------
class CLogCategory:
    public foundation::library::_DefaultAdapterHost
    <
        foundation::ILogCategory,
        foundation::ctl::ImplementsInspectable
        <
        foundation::ILogCategory,
        &foundation::IID_ILogCategory
        >
    >
{
public:
    static const foundation::string_t _CategoryPostfix;

    static inline CLogCategory *ToLogCategoryClass(foundation::ComPtr<foundation::ILogCategory> &spLogCategory)
    {
        return static_cast<CLogCategory *>(static_cast<ILogCategory *>(spLogCategory));
    }
    static inline CLogCategory *ToLogCategoryClass(foundation::ILogCategory *pLogCategory)
    {
        return static_cast<CLogCategory *>(pLogCategory);
    }
    inline foundation::ILogCategory *AsInterface() const
    {
        return static_cast<foundation::ILogCategory *>((CLogCategory *)this);
    }

    static bool IsEnabled();

    static HRESULT CreateInstance(
        CLogCategory *pParent,
        UINT32      categoryTypeId,
        CLogCategory **ppLogCategory);

    static HRESULT GetOrCreateCategory(
        _In_opt_ CLogCategory *pRootCategory,
        UINT32 size,
        _In_ UINT32 *pCategoryTypeId,
        CLogCategory **ppLogCategory);

    bool IsLevelEnabled(
        foundation::LoggingLevel level,
        UINT32 messageEnumValue,
        const _GetTypeFunc& funcGetType) const
    {
        if(!IsEnabled())
        {
            // when class enabled Logging is disabled for any category 
            return false;
        }
        return IsLevelEnabledInternal(level,messageEnumValue,funcGetType);
    }

    bool IsLevelEnabledInternal(
        foundation::LoggingLevel level,
        UINT32 messageEnumValue,
        const _GetTypeFunc& funcGetType) const
    {
        // check message filtering
        if ((messageEnumValue != (UINT32)-1)
            && this->HasFilterMessages()
            && !this->IsMessageIncluded(messageEnumValue))
        {
            // category has the Logging level but will filter the message
            return false;
        }

        const CLogCategory *pLogCategory = this;

        while (pLogCategory)
        {
            if (pLogCategory->_level >= level ||
                (pLogCategory->_messageLevels.size() && pLogCategory->IsLevelEnabledForMessage(messageEnumValue, level)))
            {
                // filter by IID
                IID iid;
                return
                    (
                    !pLogCategory->HasFilterTypes()
                    || !funcGetType(&iid)
                    || pLogCategory->IsTypeIncluded(iid)
                    );
            }
            pLogCategory = pLogCategory->_pParent;
        }
        return false;
    }

    struct _NoType : _GetTypeFunc
    {
        bool operator()(IID* pType) const
        {
            UNREFERENCED_PARAMETER(pType);
            return false;
        }
    };
    
    inline bool IsInfoEnabled(UINT32 messageEnumValue = (UINT32)-1) const
    {
        return IsLevelEnabled(foundation::LoggingLevel::Info, messageEnumValue, _NoType());
    }
    
    inline bool IsDebugEnabled(UINT32 messageEnumValue = (UINT32)-1) const
    {
        return IsLevelEnabled(foundation::LoggingLevel::Debug, messageEnumValue, _NoType());
    }
    
    inline bool IsInfoEnabled(UINT32 messageEnumValue, const _GetTypeFunc& func) const
    {
        return IsLevelEnabled(foundation::LoggingLevel::Info, messageEnumValue, func);
    }
    
    inline bool IsDebugEnabled(UINT32 messageEnumValue, const _GetTypeFunc& func) const
    {
        return IsLevelEnabled(foundation::LoggingLevel::Debug, messageEnumValue, func);
    }

    inline bool IsTraceEnabled(UINT32 messageEnumValue, const _GetTypeFunc& func = _NoType()) const
    {
        return IsLevelEnabled(foundation::LoggingLevel::Trace, messageEnumValue, func);
    }

    bool IsLevelEnabled(
        foundation::LoggingLevel level,
        UINT32 messageEnumValue,
        GUID *pForType) const;

protected:
    friend class CLogger;
    CLogCategory();
    ~CLogCategory();

    // Properties
    STDMETHOD(GetTypeInfo)(foundation::IEnumTypeInfo **ppValue);
    STDMETHOD(GetParent)(foundation::ILogCategory **ppLogCategory);
    STDMETHOD(GetSubCategories)(UINT32 *size, _Outptr_ ILogCategory ***ppLogCategories);
    STDMETHOD(GetLevel)(foundation::LoggingLevel* level);
    STDMETHOD(SetLevel)(foundation::LoggingLevel level);
    // methods.
    STDMETHOD(Configure)(foundation::IDictionary *pConfigParameters);
    STDMETHOD(AddMessageFilter)(UINT32 messageEnumValue);
    STDMETHOD(RemoveMessageFilter)(UINT32 messageEnumValue);
    STDMETHOD (AddTypeFilter)(GUID iidType);
    STDMETHOD (RemoveTypeFilter)(GUID iidType);
    STDMETHOD(SetMessageLevel)(UINT32 messageEnumValue, foundation::LoggingLevel level);

    STDMETHOD(IsLevelEnabled)(foundation::LoggingLevel level, UINT32 messageEnumValue, GUID *piidType, bool *isEnabled);
private:
    bool HasFilterTypes() const
    {
        return _filterTypes.size() > 0;
    }
    bool HasFilterMessages() const
    {
        return _filterMessages.size() > 0;
    }
    bool IsTypeIncluded(GUID iidType) const
    {
        return _filterTypes.find(iidType) != _filterTypes.end();
    }
    bool IsMessageIncluded(UINT32 messageEnumValue) const
    {
        return _filterMessages.find(messageEnumValue) != _filterMessages.end();
    }
    bool IsLevelEnabledForMessage(UINT32 messageEnumValue, foundation::LoggingLevel level) const
    {
        _MessageLevelMapType::const_iterator iter = _messageLevels.find(messageEnumValue);
        if (iter != _messageLevels.end())
        {
            return (*iter).second >= level;
        }
        return false;
    }

    bool IsCategoryNameEquals(LPCSTR_t categoryName);
    CLogCategory *LookupCategoryByName(LPCSTR_t categoryName);
    CLogCategory *LookupCategory(UINT32 categoryTypeId);
    void AddSubCategory(UINT32 categoryTypeId,CLogCategory *subCategory);

    HRESULT GetMessageEnumValue(HSTRING messageId, foundation::IEnumValue **ppEnumValue);

    static HRESULT CreateInstanceInternal(
        CLogCategory *pParent,
        CEnumTypeInfo *pEnumTypeInfo,
        UINT32      categoryTypeId,
        CLogCategory **ppLogCategory);

    static foundation::library::_ComPtrSingleton<ILogCategory> _rootInstance;
    static CLogCategory     *GetRootCategory();
private:
    typedef std::map<UINT32, foundation::ComPtr<foundation::ILogCategory> > _CategoryMapType;
    typedef std::map<UINT32, foundation::LoggingLevel> _MessageLevelMapType;

    CLogCategory                    *_pParent;
    foundation::LoggingLevel         _level;
    std::set<GUID, foundation::library::GuidLessThan>  _filterTypes;
    std::set<UINT32>                _filterMessages;
    _MessageLevelMapType            _messageLevels;

    _CategoryMapType    _subCategories;
    CEnumTypeInfo       *_pEnumTypeInfo;
};
