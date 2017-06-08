/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:persistant_storage_service_util.h
****/
#pragma once

#include "interfaces/persistant_storage_service.h"
#include <foundation/node_statics_util.h>
#include <foundation/type_info_statics_util.h>
#include <foundation/object_dispatch_util.h>
#include "object_dispatch_class.h"

#include <stdarg.h>
#include <vector>

namespace foundation
{
    namespace library
    {
		// ISetPropertyCallback Interface
		BEGIN_DECLARE_INTERFACE(ISetPropertyCallback, foundation::IUnknown, FOUNDATION_API)
			STDMETHOD(Invoke)(_In_ UINT32 propertyId, _In_opt_ foundation::IInspectable *pValue) = 0;
		END_DECLARE_INTERFACE()

        class persistant_storage_service_util
        {
        public:
            static HRESULT GetPersistantStorageService(
                _In_ foundation::IObject *pObjectNode,
                _COM_Outptr_ IPersistantStorageService **ppPersistantStorageService)
            {
                foundation::ComPtr<IPersistantStorageService> persistant_storage_service_ptr;
                return QueryObjectService(
                    pObjectNode,
                    IPersistantStorageService::GetIID(),
                    ppPersistantStorageService);
            }

            static HRESULT RegisterTypeProperties(
                _In_ IPersistantStorageService *pPersistantStorageService,
                REFIID iidType,
                const std::vector<UINT32>& properties,
                foundation::PropertyType keyPropertyType)
            {
                foundation::ComPtr<IObjectDispatchInfo> type_info_ptr;
                IFR_ASSERT(foundation::GetTypeInfoAs(iidType, type_info_ptr.GetAddressOf()));

                return pPersistantStorageService->RegisterTypeProperties(
                    type_info_ptr,
                    (UINT32)properties.size(),
                    (UINT32 *)&properties.front(),
                    keyPropertyType);
            }

            static HRESULT RegisterTypeProperties(
                _In_ IPersistantStorageService *pPersistantStorageService,
                REFIID iidType,
                foundation::PropertyType keyPropertyType,
                UINT32 propertyIdStart,
                ...)
            {
                va_list marker;
                va_start(marker, propertyIdStart);
                std::vector<UINT32> properties;
                CreateProperties(properties, propertyIdStart, marker);

                return RegisterTypeProperties(
                    pPersistantStorageService,
                    iidType,
                    properties,
                    keyPropertyType);
            }

            static HRESULT RegisterTypeProperties(
                _In_ IPersistantStorageService *pPersistantStorageService,
                REFIID iidType,
                UINT32 *pProperties,
                foundation::PropertyType keyPropertyType)
            {
                std::vector<UINT32> properties;
                CreateProperties(properties, pProperties);

                return RegisterTypeProperties(
                    pPersistantStorageService,
                    iidType,
                    properties,
                    keyPropertyType);
            }

            static HRESULT UpdateProperties(
                _In_ IObjectDispatch *pPropertyData,
                UINT32 size_properties,
                _In_ UINT32 *pProperties,
                _In_ foundation::IInspectable *pKeyValue)
            {
                foundation_assert(pPropertyData != nullptr);

                foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> properties_id_array;
                _IFR_(pPropertyData->GetProperties(
                    foundation::pv_util::CreateValue(size_properties, pProperties),
                    properties_id_array.GetSizeAddressOf(),
                    properties_id_array.GetBufferAddressOf()));

                foundation::ComPtr<library::IPersistantStorageService> persistant_storage_service_ptr;
                IFR_ASSERT(GetPersistantStorageService(
                    pPropertyData,
                    persistant_storage_service_ptr.GetAddressOf()));

                return persistant_storage_service_ptr->UpdateProperties(
                    GetPropertyDataInfo(pPropertyData),
                    properties_id_array.GetSize(),
                    (foundation::IPropertyIdPair **)properties_id_array.GetBuffer(),
                    pKeyValue);
            }

            static HRESULT LoadProperties(
                _In_ IPersistantStorageService *pPersistantStorageService,
                REFIID iidType,
                _In_ foundation::IInspectable *pKeyValue,
                std::vector<foundation::ComPtr<foundation::IPropertyIdPair>> &loaded_properties,
                UINT32 propertyIdStart,
                ...)
            {
                va_list marker;
                va_start(marker, propertyIdStart);
                std::vector<UINT32> properties;
                CreateProperties(properties, propertyIdStart, marker);

                foundation::ComPtr<IObjectDispatchInfo> type_info_ptr;
                IFR_ASSERT(foundation::GetTypeInfoAs(iidType, type_info_ptr.GetAddressOf()));

                foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> properties_id_array;

                _IFR_(pPersistantStorageService->LoadProperties(
                    type_info_ptr,
                    (UINT32)properties.size(),
                    &properties.front(),
                    pKeyValue,
                    properties_id_array.GetSizeAddressOf(),
                    properties_id_array.GetBufferAddressOf()));
                // copy to our output vector
                properties_id_array.CopyToVector(loaded_properties);
                return S_OK;
            }

            static HRESULT LoadProperties(
				_In_ IObjectDispatch *pPropertyData,
				_In_ ISetPropertyCallback *pSetPropertyCallback,
                UINT32 size_properties,
                _In_opt_ UINT32 *pProperties,
                _In_ foundation::IInspectable *pKeyValue)
            {
                foundation_assert(pPropertyData != nullptr);
				foundation_assert(pSetPropertyCallback != nullptr);

                foundation::ComPtr<library::IPersistantStorageService> persistant_storage_service_ptr;
                IFR_ASSERT(GetPersistantStorageService(
                    pPropertyData,
                    persistant_storage_service_ptr.GetAddressOf()));

                foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> properties_id_array;

                _IFR_(persistant_storage_service_ptr->LoadProperties(
                    GetPropertyDataInfo(pPropertyData),
                    size_properties,
                    pProperties,
                    pKeyValue,
                    properties_id_array.GetSizeAddressOf(),
                    properties_id_array.GetBufferAddressOf()));

                for (UINT32 index = 0; index < properties_id_array.GetSize(); ++index)
                {
                    UINT32 propertyId;
                    IFR_ASSERT(properties_id_array[index]->GetKey(&propertyId));
                    foundation::InspectablePtr valuePtr;
                    properties_id_array[index]->GetValue(valuePtr.GetAddressOf());

                    if (!foundation::pv_util::IsValueEmpty(valuePtr))
                    {
                        _IFR_(pSetPropertyCallback->Invoke(propertyId, valuePtr));
                    }
                }
                return S_OK;
            }

            template <typename TKeyValue, typename TPropertyValue>
            static HRESULT LoadProperty(
                _In_ IPersistantStorageService *pPersistantStorageService,
                REFIID iidType,
                UINT32 propertyId,
                const TKeyValue& keyValue,
                TPropertyValue &propertyValue)
            {
                std::vector<foundation::ComPtr<foundation::IPropertyIdPair>> loaded_properties;
                _IFR_(LoadProperties(
                    pPersistantStorageService,
                    iidType,
                    foundation::CreateValue(keyValue).Get(),
                    loaded_properties,
                    propertyId,
                    (UINT32)-1
                    ));
                foundation::InspectablePtr valuePtr;
                _IFR_(loaded_properties[0]->GetValue(valuePtr.GetAddressOf()));
                _IFR_(foundation::GetValue(valuePtr, propertyValue));
                return S_OK;
            }

            template <typename TKeyValue, typename TPropertyValue>
            static HRESULT UpdateProperty(
                _In_ IPersistantStorageService *pPersistantStorageService,
                REFIID iidType,
                UINT32 propertyId,
                const TPropertyValue &propertyValue,
                const TKeyValue& keyValue)
            {
                foundation::ComPtr<foundation::IPropertyIdPair> property_id_pair_ptr;
                IFR_ASSERT(CreatePropertyIdPair(
                    propertyId, 
                    foundation::CreateValue(propertyValue),
                    property_id_pair_ptr.GetAddressOf()));

                foundation::ComPtr<IObjectDispatchInfo> type_info_ptr;
                IFR_ASSERT(foundation::GetTypeInfoAs(iidType, type_info_ptr.GetAddressOf()));

                _IFR_(pPersistantStorageService->UpdateProperties(
                    type_info_ptr,
                    1,
                    property_id_pair_ptr.GetAddressOfPtr(),
                    foundation::CreateValue(keyValue).Get()));
                return S_OK;
            }

            static HRESULT UpdateAllProperties(
                _In_ IObjectDispatch *pPropertyData,
                _In_ foundation::IInspectable *pKeyValue)
            {
                foundation::UInt32ArrayWrapper properties;
                _IFR_(GetTypeProperties(pPropertyData, properties));

                return UpdateProperties(
                    pPropertyData,
                    (UINT32)properties.GetSize(),
                    (UINT32 *)properties.GetBuffer(),
                    pKeyValue);
            }

            template <class TKeyValue>
            static HRESULT UpdateAllProperties_t(
                _In_ IObjectDispatch *pPropertyData,
                const TKeyValue& key_value)
            {
                return UpdateAllProperties(pPropertyData,foundation::CreateValue(key_value));
            }

            static HRESULT LoadAllProperties(
				_In_ IObjectDispatch *pPropertyData,
                _In_ ISetPropertyCallback *pSetPropertyCallback,
                _In_  foundation::IInspectable *pKeyValue)
            {
                foundation::UInt32ArrayWrapper properties;
                _IFR_(GetTypeProperties(pPropertyData, properties));

                return LoadProperties(
					pPropertyData,
					pSetPropertyCallback,
                    (UINT32)properties.GetSize(),
                    (UINT32 *)properties.GetBuffer(),
                    pKeyValue);
            }

            template <class TKeyValue>
            static HRESULT LoadAllProperties_t(const TKeyValue& key_value)
            {
                return LoadAllProperties(foundation::CreateValue(key_value));
            }

        private:
            static HRESULT GetTypeProperties(
                _In_ IObjectDispatch *pPropertyData,
                foundation::UInt32ArrayWrapper& properties)
            {
                foundation::ComPtr<library::IPersistantStorageService> persistant_storage_service_ptr;
                IFR_ASSERT(GetPersistantStorageService(
                    pPropertyData,
                    persistant_storage_service_ptr.GetAddressOf()));
                _IFR_(persistant_storage_service_ptr->GetTypeProperties(
                    GetPropertyDataInfo(pPropertyData),
                    properties.GetSizeAddressOf(),
                    properties.GetBufferAddressOf()));
                return S_OK;
            }

            static IObjectDispatchInfo *GetPropertyDataInfo(
                _In_ IObjectDispatch *pPropertyData)
            {
                foundation::ComPtr<IObjectDispatchInfo> property_data_info_ptr;

                foundation::GetObjectTypeInfo(
                    pPropertyData,
					property_data_info_ptr.GetAddressOf());
                foundation_assert(property_data_info_ptr != nullptr);
                return property_data_info_ptr;
            }
        };
    }
}


