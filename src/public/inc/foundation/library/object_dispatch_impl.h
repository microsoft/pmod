/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_dispatch_impl.h
****/
#pragma once

#include "base_object_impl.h"
#include "key_value_pair_impl.h"
#include "util_stl.h"
#include "critical_section_lock.h"
#include <foundation/boxing_util.h>
#include <foundation/interfaces/object_dispatch.h>
#include <foundation/interfaces/object_dispatch_info.h>
#include <map>
#include <vector>

namespace foundation {

    namespace library {

        template <class T>
        class _ObjectDispatchHelperBase
        {
        protected:
            HRESULT GetPropertyInternal(UINT32 propertyId, foundation::IInspectable **ppValue)
            {
                return E_NOTIMPL;
            }
            HRESULT SetPropertyInternal(UINT32 propertyId, foundation::IInspectable *pValue)
            {
                return E_NOTIMPL;
            }

            inline T *GetOuter()
            {
                return static_cast<T *>(this);
            }
        };

        template <class T>
        class _MethodHandlerBase
        {
        protected:
            HRESULT InvokeMethodInternal(UINT32 methodId, _In_ UINT32 size, _In_ foundation::IInspectable **parameters, foundation::IInspectable **ppResult)
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }
            inline T *GetOuter()
            {
                return static_cast<T *>(this);
            }
        };

        static HRESULT CreatePropertyIdPairImpl(
            UINT32 propertyId,
            _In_opt_ foundation::IInspectable *pValue,
            _COM_Outptr_ IPropertyIdPair **ppPropertyIdPair)
        {
            typedef _InspectableKeyValuePair<IPropertyIdPair> _Kvp_type;
            return CreateKeyValuePair<_Kvp_type>(propertyId, pValue, ppPropertyIdPair);
        }

        static HRESULT _GetProperties(
            _In_ IObjectDispatchInfo *pPropertyDataInfo,
            _In_ IObjectDispatch *pPropertyData,
            void *_Context,
            bool(*_IncludePropertyCallback)(void *_Context, UINT32 propertyId),
            foundation::IInspectable *pProperties,
            UINT32* size,
            IPropertyIdPair ***ppProperties)
        {
            // returned properties
            std::vector<foundation::ComPtr<IPropertyIdPair> > properties;

            foundation::PropertyType type = foundation::PropertyType_Empty;
            if (pProperties != nullptr)
            {
                foundation::pv_util::GetType(pProperties, &type);
            }

            if ((type & 0x400) != 0)
            {
                foundation::UInt32ArrayWrapper array_properties;
                IFR_ASSERT(foundation::pv_util::GetUInt32Array(
                    pProperties,
                    array_properties.GetSizeAddressOf(),
                    array_properties.GetBufferAddressOf()));

                for (UINT32 index = 0; index < array_properties.GetSize(); ++index)
                {
                    foundation::InspectablePtr spValue;
                    _IFR_(pPropertyData->GetProperty(array_properties[index], spValue.GetAddressOf()));

                    foundation::ComPtr<IPropertyIdPair> spPropertyIdPair;
                    IFR_ASSERT(CreatePropertyIdPairImpl(
                        array_properties[index],
                        spValue,
                        spPropertyIdPair.GetAddressOf()));

                    properties.push_back(spPropertyIdPair);
                }
            }
            else
            {
                UINT32 propertiesFlag;
                if (pProperties == nullptr)
                {
                    propertiesFlag = static_cast<UINT32>(InstanceFlags::All);
                }
                else
                {
                    IFR_ASSERT(foundation::pv_util::GetUInt32Value(pProperties, &propertiesFlag));
                }

                foundation::ArrayRefCountWrapper<IPropertyInfo> propertiesInfo;
                IFR_ASSERT(pPropertyDataInfo->GetProperties(
                    propertiesFlag,
                    propertiesInfo.GetSizeAddressOf(),
                    propertiesInfo.GetBufferAddressOf()));

                for (UINT32 index = 0; index < propertiesInfo.GetSize(); ++index)
                {
                    foundation::InspectablePtr spValue;
                    UINT32 propertyId;
                    IFR_ASSERT(propertiesInfo[index]->GetId(&propertyId));

                    // skip properties not on the container
                    if (_IncludePropertyCallback != nullptr &&
                        !(*_IncludePropertyCallback)(_Context, propertyId))
                    {
                        continue;
                    }

                    _IFR_(pPropertyData->GetProperty(propertyId, spValue.GetAddressOf()));

                    foundation::ComPtr<IPropertyIdPair> spPropertyIdPair;
                    IFR_ASSERT(CreatePropertyIdPairImpl(propertyId, spValue, spPropertyIdPair.GetAddressOf()));

                    properties.push_back(spPropertyIdPair);
                }
            }
            // copy now to public API
            return RefCountCopyTo(properties, size, ppProperties);
        }

        template <
            class TInterface,
            class TComBase = foundation::ctl::ComInspectableBase
        >
        class _ObjectDispatchImplBase :
            public _ObjectImplBase<
                TInterface,
                IObjectDispatch,
                TComBase>
        {
        public:
            template <class TValue>
            HRESULT SetProperty(UINT32 propertyId, TValue value)
            {
                return this->SetProperty(propertyId, CreateValue(value));
            }

            template <class T>
            HRESULT GetProperty(UINT32 propertyId, T& value)
            {
                InspectablePtr valuePtr;
                _IFR_(this->GetProperty(propertyId, valuePtr.GetAddressOf()));
                return foundation::GetValue(valuePtr, value);
            }

            // Interface IObjectDispatch
            STDMETHOD(GetProperty)(UINT32 propertyId, foundation::IInspectable** value)
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }

            STDMETHOD(SetProperty)(UINT32 propertyId, foundation::IInspectable *value)
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }

            STDMETHOD(GetProperties)(foundation::IInspectable* pProperties, UINT32 *size, foundation::IPropertyIdPair ***ppProperties)
            {
                foundation::ComPtr<foundation::IObjectDispatchInfo> propertyDataInfoPtr;
                IFR_ASSERT(foundation::GetObjectTypeInfo(this, propertyDataInfoPtr.GetAddressOf()));
                return _GetProperties(
                    propertyDataInfoPtr,
                    static_cast<foundation::IObjectDispatch *>(this),
                    nullptr,
                    nullptr,
                    pProperties,
                    size,
                    ppProperties);
            }
        };

        struct _PropertySlotInfo
        {
            UINT32                   _index;
            UINT32                   _propertyId;
            foundation::PropertyType _propertyType;
            UINT32                  _flags;
            IID                     _iidType;
        };

        template <class TPropertySlotInfo = _PropertySlotInfo>
        class _ObjectDispatchInfoCache
        {
        public:
            typedef std::vector<TPropertySlotInfo> _PropertiesContainerType;
            HRESULT GetCache(
                const IID& iidType,
                std::shared_ptr<_PropertiesContainerType>& cache)
            {
                ComPtr<IObjectDispatchInfo> propertyDataInfoPtr;
                _IFR_(GetTypeInfoAs(iidType, propertyDataInfoPtr.GetAddressOf()));
                return GetCache(propertyDataInfoPtr, cache);
            }

            HRESULT GetCache(
                _In_ IObjectDispatchInfo *pPropertyDataInfo,
                std::shared_ptr<_PropertiesContainerType>& cache)
            {

                CriticalSectionLock lock(cs);

                foundation_assert(pPropertyDataInfo);

                IID iidType;
                pPropertyDataInfo->GetType(&iidType);
                typename _PropertyDataInfoType::iterator iter = _cache.find(iidType);
                if (iter != _cache.end())
                {
                    cache = iter->second;
                    return S_OK;
                }

                ArrayRefCountWrapper<IPropertyInfo> allPropertiesInfo;
                IFR_ASSERT(pPropertyDataInfo->GetProperties(
                    static_cast<UINT32>(InstanceFlags::All),
                    allPropertiesInfo.GetSizeAddressOf(),
                    allPropertiesInfo.GetBufferAddressOf()));

                std::vector<ComPtr<IPropertyInfo>> propertInfos;
                IFR_ASSERT(GetProperties(allPropertiesInfo, propertInfos));

                cache = std::make_shared<_PropertiesContainerType>();
                cache->resize((size_t)propertInfos.size());

                IFR_ASSERT(AddPropertiesToCache(propertInfos, cache));

                // add to our cache
                _cache[iidType] = cache;
                return S_OK;
            }
        protected:
            virtual HRESULT GetProperties(
                const ArrayRefCountWrapper<IPropertyInfo>& allPropertiesInfo,
                std::vector<ComPtr<IPropertyInfo>>& propertInfos)
            {
                for (UINT32 index = 0; index < allPropertiesInfo.GetSize(); ++index)
                {
                    propertInfos.push_back(allPropertiesInfo[index]);
                }
                return S_OK;
            }

            virtual HRESULT AddPropertiesToCache(
                const std::vector<ComPtr<IPropertyInfo>>& propertInfos,
                std::shared_ptr<_PropertiesContainerType>& cache)
            {
                for (std::vector<ComPtr<IPropertyInfo>>::const_iterator iter = propertInfos.begin();
                iter != propertInfos.end();
                    ++iter)
                {
                    UINT32 index = (UINT32)(iter - propertInfos.begin());
                    TPropertySlotInfo& propertySlotInfo = cache->at(index);
                    propertySlotInfo._index = index;

                    (*iter)->GetId(&propertySlotInfo._propertyId);
                    (*iter)->GetPropertyType(&propertySlotInfo._propertyType);
                    (*iter)->GetFlags(&propertySlotInfo._flags);

                    ComPtr<foundation::IObjectTypeInfo> objectTypeInfoPtr;
                    (*iter)->GetTypeInfo(objectTypeInfoPtr.GetAddressOf());

                    if (objectTypeInfoPtr != nullptr)
                    {
                        objectTypeInfoPtr->GetType(&propertySlotInfo._iidType);
                    }
                    else
                    {
                        propertySlotInfo._iidType = GUID_NULL;
                    }
                }
                return S_OK;
            }
        private:
            typedef std::map<
                IID,
                std::shared_ptr<_PropertiesContainerType>,
                GuidLessThan
            > _PropertyDataInfoType;

            pal::library::CriticalSection cs;
            _PropertyDataInfoType _cache;
        };

        class ObjectDispatchContainerImpl
        {
        private:
            typedef _ObjectDispatchInfoCache<>::_PropertiesContainerType _Properties_type;
        protected:
            HRESULT _Initialize_container(const IID& iidType)
            {
                IFR_ASSERT(get_cache_internal(iidType, this->_propertiesContainerInfo));
                _containerData.resize(this->_propertiesContainerInfo->size());
                // fill default values
                for (_Properties_type::const_iterator iter = this->_propertiesContainerInfo->begin();
                    iter != this->_propertiesContainerInfo->end();
                    ++iter)
                {
                    pv_util::CreateDefaultValue(
                        iter->_propertyType,
                        _containerData[iter->_index].GetAddressOf());
                }
                return S_OK;
            }

            HRESULT GetPropertyValue(UINT32 propertyId, foundation::IInspectable** value)
            {
                UINT32 index = propertyId - 1;
                if (index >= _containerData.size())
                {
                    return foundation::Errors::E_PROPERTY_NOT_DEFINED;
                }
                return _containerData[index].CopyTo(value);
            }

            HRESULT SetPropertyValue(UINT32 propertyId, foundation::IInspectable *value)
            {
                UINT32 index = propertyId - 1;
                if (index >= _containerData.size())
                {
                    return Errors::E_PROPERTY_NOT_DEFINED;
                }
                InspectablePtr valuePtr(value);
                const _PropertySlotInfo& propertyInfo = _propertiesContainerInfo->operator[](index);
                if (propertyInfo._propertyType == PropertyType_Inspectable)
                {
                    if (value != nullptr && !IsEqualIID(propertyInfo._iidType, foundation_GUID_NULL))
                    {
                        ComPtr<IObject> objectPtr;
                        _IFR_(foundation::QueryInterface(value, objectPtr.GetAddressOf()));
                        bool result = false;
                        IsInstanceOfType(objectPtr, propertyInfo._iidType, result);
                        if (!result)
                        {
                            return Errors::E_WRONG_TYPE;
                        }
                    }
                }
                else
                {
                    _IFR_(pv_util::ChangeType(propertyInfo._propertyType,valuePtr.GetAddressOfPtr()));
                }
                _containerData[index] = valuePtr;
                return S_OK;
            }
        private:
            HRESULT get_cache_internal(const IID& iidType,
                std::shared_ptr<_Properties_type>& cache)
            {
                static _ObjectDispatchInfoCache<> _property_data_info_cache;
                return _property_data_info_cache.GetCache(iidType, cache);
            }
        private:

            std::shared_ptr<_Properties_type> _propertiesContainerInfo;

            typedef std::vector<InspectablePtr> _Container_type;
            _Container_type _containerData;
        };

        typedef _ObjectWithTypeImplBase<_ObjectDispatchImplBase<IObjectDispatch>, IID_IObjectDispatch> _ObjectDispatchImplBaseType;
        class ObjectDispatchImpl :
            public _ObjectDispatchImplBaseType,
            protected ObjectDispatchContainerImpl
        {
        public:
            HRESULT _Initialize(const IID& iidType, bool isFreeThreaded)
            {
                _IFR_(_ObjectDispatchImplBaseType::_Initialize(&iidType, isFreeThreaded));
                _IFR_(ObjectDispatchContainerImpl::_Initialize_container(iidType));
                return S_OK;
            }

            STDMETHOD(GetProperty)(UINT32 propertyId, foundation::IInspectable** value)
            {
                NoRefCountFoundationCriticalSection cs(this->_GetCriticalSection());
                return this->GetPropertyValue(propertyId, value);
            }

            STDMETHOD(SetProperty)(UINT32 propertyId, foundation::IInspectable *value)
            {
                NoRefCountFoundationCriticalSection cs(this->_GetCriticalSection());
                return this->SetPropertyValue(propertyId, value);
            }

            STDMETHOD(InvokeMethod)(UINT32 methodId, _In_ UINT32 size, _In_opt_ foundation::IInspectable **parameters, _COM_Outptr_result_maybenull_ foundation::IInspectable **ppResult)
            {
                foundation_assert(false);
                return E_NOTIMPL;
            }

        };

        static inline HRESULT CreateObjectDispatchImpl(ObjectDispatchImpl **pp, const IID& iidType, bool isFreeThreaded = false)
        {
            return ctl::inspectable::CreateInstanceWithInitialize(pp, iidType, isFreeThreaded);
        }

    }
}

