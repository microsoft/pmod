/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:observable_statics_util.h
****/
#pragma once

#include <foundation/assert.h>
#include <vector>
#include <stdarg.h>

#include "interfaces/observable_object_statics.h"
#include <foundation/boxing_util.h>
#include <foundation/com_ptr.h>

namespace pmod
{
    namespace library
    {
        static IObservableObjectStatics *get_instance()
        {
            return GetModelLibraryFactory<IObservableObjectStatics>(U("pmod.ObservableObjectStatics"));
        }

        // Get the 'Source' from an ISourceModel interface
        // retun E_NOINTERFACE if pUnk does not implement it
        inline static HRESULT GetSource(
            _In_ foundation::IUnknown *pUnk, 
            _COM_Outptr_result_maybenull_ foundation::IObject **ppSource)
        {
            return get_instance()->GetSource(pUnk,ppSource);
        }

        inline static HRESULT try_get_source(
            _In_ foundation::IUnknown *pUnk, 
            _COM_Outptr_result_maybenull_ foundation::IObject **ppSource)
        {
            HRESULT hr = GetSource(pUnk,ppSource);
            if(hr == E_NOINTERFACE)
            {
                *ppSource = nullptr;
                // don't fail since we are trying
                return S_FALSE;
            }
            IFHR_ASSERT(hr);
            return S_OK;
        }

        inline static HRESULT get_source_of_type(
            _In_ foundation::IUnknown *pUnk,
            _In_ REFIID iidInstanceType,
            _In_ foundation::InstanceTypeOptions options,
            _In_ bool isModelClassType,
            _COM_Outptr_result_maybenull_ foundation::IObject **ppSource)
        {
            return get_instance()->GetSourceOfType(pUnk,iidInstanceType,options,isModelClassType,ppSource);
        }

        // Create a std::vector<UINT32> from a property ptr
        inline static void CreateProperties(
            std::vector<UINT32>& properties,
            _In_ UINT32 *pProperties)
        {
            UINT32 propertyId;
            while ((propertyId = *pProperties) != (UINT32)-1)
            {
                properties.push_back(propertyId);
                ++pProperties;
            }
        }
        // Create a std::vector<UINT32> from a propertyId marker
        inline static void CreateProperties(
            std::vector<UINT32>& properties,
            _In_ UINT32 propertyIdStart,
            _In_ va_list marker)
        {

            UINT32 propertyId = propertyIdStart;
            while (propertyId != (UINT32)-1)
            {
                properties.push_back(propertyId);
                // go to next property id
                propertyId = va_arg(marker, int);
            }
        }

        // Create a property path from a marker
        inline static foundation::InspectablePtr CreatePropertyPath(
            _In_ UINT32 propertyIdStart, 
            _In_ va_list marker)
        {
            std::vector<UINT32> propertyPath;
            CreateProperties(propertyPath, propertyIdStart, marker);
            return foundation::CreateValueFromVector(propertyPath);
        }

        template <class T>
        static HRESULT GetPropertyValue(
            _In_ IObservableObject *pObservableObject,
            _Outptr_ T* pValue,
            _In_ UINT32 propertyIdStart, 
            ... )
        {
            IFCPTR_ASSERT(pObservableObject);
            IFCPTR_ASSERT(pValue);
            *pValue = T();

            va_list marker;
            va_start( marker, propertyIdStart );     /* Initialize variable arguments. */

            foundation::InspectablePtr path = CreatePropertyPath(propertyIdStart, marker);

            foundation::InspectablePtr spValue;
            HRESULT hr = get_instance()->GetPropertyValue(pObservableObject, path, spValue.GetAddressOf());
            if(SUCCEEDED(hr) && spValue != nullptr)
            {
                return foundation::pv_util::GetValue(spValue, pValue);
            }
            else
            {
                hr = S_FALSE;
            }
            va_end( marker );              /* Reset variable arguments.      */

            return hr;
        }

        template <class T>
        static HRESULT GetPropertyValue(
            _In_ IObservableObject *pObservableObject,
            _In_ LPCSTR_t propertyPath,
            _Out_ T* pValue)
        {
            IFCPTR_ASSERT(pObservableObject);
            IFCPTR_ASSERT(propertyPath);
            IFCPTR_ASSERT(pValue);
            *pValue = T();

            foundation::InspectablePtr path = foundation::pv_util::CreateValue(propertyPath);
            foundation::InspectablePtr spValue;
            _IFR_(get_instance()->GetPropertyValue(pObservableObject, path, spValue.GetAddressOf()));
            if(spValue != nullptr)
            {
                return foundation::pv_util::GetValue(spValue, pValue);
            }
            else
            {
                return S_FALSE;
            }
        }

        inline static HRESULT set_property_value(
            _In_ IObservableObject *pObservableObject,
            _In_ LPCSTR_t propertyPath,
            _In_ foundation::IInspectable* pValue)
        {
            IFCPTR_ASSERT(pObservableObject);
            IFCPTR_ASSERT(propertyPath);

            foundation::InspectablePtr path = foundation::pv_util::CreateValue(propertyPath);
            return get_instance()->SetPropertyValue(pObservableObject,path,pValue);
        }

    };

}
