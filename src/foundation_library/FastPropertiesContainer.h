/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FastPropertiesContainer.h
****/
#pragma once
#include <foundation/ctl/com_library.h>
#include <foundation/library/util_stl.h>
#include <foundation/library/interfaces/fast_properties_container_parameters.h>
#include <foundation/library/object_dispatch_impl.h>

#include <map>

extern const UINT32 Properties_Offset_Start;
//------------------------------------------------------------------------------
// Class:   CFastPropertiesContainer
// IPropertiesContainer 'fast' implementation for a Static container
//------------------------------------------------------------------------------
class CFastPropertiesContainer :
    public foundation::ctl::ComInspectableBase,
    public foundation::library::IFastPropertiesContainer
{
public:
    static HRESULT CreateInstance(
        const foundation::library::FastPropertiesContainerParameters *pParameters,
        foundation::library::IFastPropertiesContainer **ppPropertiesContainer);
    HRESULT _Initialize(
        const foundation::library::FastPropertiesContainerParameters* pParameters);

protected:
    CFastPropertiesContainer() :
        _propertiesSlotsAlloc(nullptr),
        _propertySlotPrefixSize(0),
        _isDelegateWeakRef(false),
        _pDelegate(nullptr)
    {
    }
    // Interface IPropertiesContainer
    STDMETHOD(HasProperty)(UINT32 propertyId, _Out_ bool *found);
    STDMETHOD(GetValue)(UINT32 propertyId, foundation::IInspectable **value);
    STDMETHOD(SetValue)(UINT32 propertyId, _In_opt_ foundation::IInspectable *value);
    STDMETHOD(RemoveProperty)(UINT32 propertyId);
    STDMETHOD(GetSize)(_Out_ UINT32* size);
    STDMETHOD(Dispose)()
    {
        return DisposeInternal();
    }

    // Interface IFastPropertiesContainer
    STDMETHOD(GetPropertType)(UINT32 propertyId,_Out_ foundation::PropertyType* pPropertyType);
    STDMETHOD(ReadCustomData)(UINT32 propertyId, _Inout_ LPVOID pCustomData);
    STDMETHOD(WriteCustomData)(UINT32 propertyId, _Inout_ LPVOID pCustomData);

    HRESULT QueryInterfaceImpl(REFIID iid, _COM_Outptr_ foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::library::IPropertiesContainer::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IPropertiesContainer *>(this);
        }
        else if(iid == foundation::library::IFastPropertiesContainer::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IFastPropertiesContainer *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;

    }

    void FinalRelease() override;
private:
    struct _PropertySlotCacheInfo : public foundation::library::_PropertySlotInfo {
        size_t  _offset;
    };

    inline static size_t ToPropertyIdIndex(UINT32 propertyId)
    {
        return (size_t)((propertyId % Properties_Offset_Start) - 1);
    }
    inline bool CheckBounds(size_t propertyIdIndex)
    {
        foundation_assert(propertyIdIndex >= 0);
        return _propertiesSlotCache->size() > propertyIdIndex;
    }
    HRESULT ClearPropertySlot(_PropertySlotCacheInfo & propertyCacheInfo);
    HRESULT DisposeInternal();

    LPVOID GetSlotPtr(const _PropertySlotCacheInfo & propertyCacheInfo);
    inline LPVOID GetCustomSlotPtr(const _PropertySlotCacheInfo & propertyCacheInfo);

    typedef foundation::library::_ObjectDispatchInfoCache<_PropertySlotCacheInfo> _CFastPropertiesDataInfoCache_BaseType;
    class CFastPropertiesDataInfoCache : public _CFastPropertiesDataInfoCache_BaseType
    {
    protected:
        HRESULT GetProperties(
            const foundation::ArrayRefCountWrapper<foundation::IPropertyInfo>& allPropertiesInfo,
            std::vector<foundation::ComPtr<foundation::IPropertyInfo>>& propertInfos) override;

        HRESULT AddPropertiesToCache(
            const std::vector<foundation::ComPtr<foundation::IPropertyInfo>>& propertInfos,
            std::shared_ptr<_PropertiesContainerType>& cache) override;

    };
    HRESULT GetCustomSlotPtr(UINT32 propertyId, LPVOID* ppCustomSlotPtr);
private:
    static CFastPropertiesDataInfoCache _propertySlotCacheMap;

    std::shared_ptr<CFastPropertiesDataInfoCache::_PropertiesContainerType>   _propertiesSlotCache;
    LPVOID                          _propertiesSlotsAlloc;
    UINT32                          _propertySlotPrefixSize;
    bool                            _isDelegateWeakRef;
    foundation::library::IFastPropertiesContainerDelegate *_pDelegate;
};
