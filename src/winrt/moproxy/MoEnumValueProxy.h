/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoEnumValueProxy.h
****/

#pragma once

#include <foundation/assert.h>

#include "Microsoft.PropertyModel.h"
#include "MoAdapterProxyInterface.h"
#include <pmod/library/model_library.h>
#include "MoCoreProxyFactory.h"
#include "MoObservableObjectProxy.h"

//------------------------------------------------------------------------------
// Class:   CMoEnumValueProxy
//
// Purpose: Implementation of pmod::IEnumValue
//          Interface using the Modern Interface
//------------------------------------------------------------------------------
class CMoEnumValueProxy :
    public TMoObjectProxy
    <
        foundation::IEnumValue,
        ABI::Microsoft::PropertyModel::IEnumValue
    >,
    public ABI::Windows::Foundation::IPropertyValue
{
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, IUnknown **ppInterface) override
    {
        if (iid == __uuidof(ABI::Windows::Foundation::IPropertyValue) )
        {
            *ppInterface = static_cast<ABI::Windows::Foundation::IPropertyValue *>(this);
        }
        else if (iid == IObject::GetIID())
        {
            *ppInterface = static_cast<IObject *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
    HRESULT GetRuntimeClassNameImpl(_Outptr_ HSTRING *pClassName) override
    {
        return this->m_pSourceInterface->GetRuntimeClassName(pClassName);
    }

    // Interface pmod::IEnumValue
    STDMETHOD (GetValue)(UINT32 *value)
    {
        return this->m_pSourceInterface->get_Value(value);
    }

    STDMETHOD (ToString)(HSTRING *value)
    {
        return this->m_pSourceInterface->ToString(value);
    }

    // IPropertyValue
    HRESULT STDMETHODCALLTYPE get_Type( 
        ABI::Windows::Foundation::PropertyType *value) override
    {
        *value = ABI::Windows::Foundation::PropertyType_UInt32;
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE get_IsNumericScalar( 
        boolean *value) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetUInt8( 
        BYTE *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt16( 
        INT16 *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt16( 
        UINT16 *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt32( 
        INT32 *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt32( 
        UINT32 *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt64( 
        INT64 *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt64( 
        UINT64 *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetSingle( 
        FLOAT *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetDouble( 
       DOUBLE *value) override
    {
        return GetEnumValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetChar16( 
        WCHAR *value) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetBoolean( 
        boolean *value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetString( 
       HSTRING *value) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetGuid( 
        GUID *value) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetDateTime( 
        ABI::Windows::Foundation::DateTime *value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetTimeSpan( 
        ABI::Windows::Foundation::TimeSpan *value) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetPoint( 
         __RPC__out ABI::Windows::Foundation::Point *value)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetSize( 
        ABI::Windows::Foundation::Size *value)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetRect( 
        ABI::Windows::Foundation::Rect *value)
    {
        return E_NOTIMPL;
    }
                                        
    HRESULT STDMETHODCALLTYPE GetUInt8Array( 
        UINT32 *__valueSize,
        BYTE **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt16Array( 
        UINT32 *__valueSize,
        INT16 **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt16Array( 
        UINT32 *__valueSize,
         UINT16 **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt32Array( 
        UINT32 *__valueSize,
        INT32 **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt32Array( 
        UINT32 *__valueSize,
        UINT32 **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt64Array( 
        UINT32 *__valueSize,
        INT64 **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt64Array( 
        UINT32 *__valueSize,
        UINT64 **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetSingleArray( 
        UINT32 *__valueSize,
        FLOAT **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetDoubleArray( 
        UINT32 *__valueSize,
        DOUBLE **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetChar16Array( 
        UINT32 *__valueSize,
        WCHAR **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetBooleanArray( 
        UINT32 *__valueSize,
        boolean **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetStringArray( 
        UINT32 *__valueSize,
        HSTRING **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInspectableArray( 
        UINT32 *__valueSize,
        IInspectable ***value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetGuidArray( 
        UINT32 *__valueSize,
        GUID **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetDateTimeArray( 
        UINT32 *__valueSize,
        ABI::Windows::Foundation::DateTime **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetTimeSpanArray( 
        UINT32 *__valueSize,
        ABI::Windows::Foundation::TimeSpan **value) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetPointArray( 
        UINT32 *__valueSize,
        ABI::Windows::Foundation::Point **value)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetSizeArray( 
        UINT32 *__valueSize,
        ABI::Windows::Foundation::Size **value)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetRectArray( 
        UINT32 *__valueSize,
        ABI::Windows::Foundation::Rect **value) 
    {
        return E_NOTIMPL;
    }
private:
    template <class T>
    HRESULT GetEnumValueAs(T *pValue)
    {
        UINT32 enumValue;
        this->GetValue(&enumValue);

        *pValue = (T)enumValue;
        if((UINT32)(*pValue) != enumValue)
        {
            return E_FAIL;
        }
        return S_OK;
    }
};
