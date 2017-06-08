/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EnumValue.h
****/

#pragma once

#include <foundation/ctl/com_library.h>
#include <foundation/ctl/com_weak_reference_source.h>
#include <foundation/library/base_adapter_host.h>

#include <map>

//------------------------------------------------------------------------------
// Class:   CEnumValue
//
// Purpose: Define a class that Implements the IEnumValue interface
//
//------------------------------------------------------------------------------

typedef foundation::library::_DefaultAdapterHost<foundation::IEnumValue, foundation::ctl::ComWeakReferenceSourceInspectableBase> _CEnumValue_BaseType;
class CEnumValue :
    public _CEnumValue_BaseType,
    public foundation::IEnumValue,
    public foundation::IPropertyValue
{
public:
    static HRESULT CreateInstance(
        _In_ foundation::IEnumTypeInfo *pEnumTypeInfo,
        _In_ UINT32 enumValue,
        _Outptr_ CEnumValue **ppEnumValueClass);

protected:
    CEnumValue():
        _enumValue(0)
    {
    }

    HRESULT Initialize(
        foundation::IEnumTypeInfo *pEnumTypeInfo,
        UINT32 enumValue);

    // IObject Interface
    STDMETHOD(GetTypeInfo)(foundation::IObjectTypeInfo** value) override;
    STDMETHOD(GetUniqueId)(UINT32 *uniqueId) override;
    STDMETHOD(GetProcessId)(UINT32 *processId) override;

    // IEnumValue Interface
    STDMETHOD (GetValue)(UINT32 *value) override;
    STDMETHOD (ToString)(HSTRING *value) override;

    // Override ComInspectableBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override;

#if defined(_WINDOWS_RUNTIME)
    HRESULT GetRuntimeClassNameImpl(_Outptr_ HSTRING *pClassName) override;
#endif

    // IPropertyValue
    HRESULT STDMETHODCALLTYPE get_Type( 
        foundation::PropertyType *value) override;
                    
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
        *value = _enumValue;
        return S_OK;
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
        foundation::DateTime *value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetTimeSpan( 
        foundation::TimeSpan *value) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetPoint( 
         __RPC__out foundation::Point *value)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetSize( 
        foundation::Size *value)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetRect( 
        foundation::Rect *value)
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
        foundation::IInspectable ***value) override
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
        foundation::DateTime **value) override
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetTimeSpanArray( 
        UINT32 *__valueSize,
        foundation::TimeSpan **value) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE GetPointArray( 
        UINT32 *__valueSize,
        foundation::Point **value)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetSizeArray( 
        UINT32 *__valueSize,
        foundation::Size **value)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE GetRectArray( 
        UINT32 *__valueSize,
        foundation::Rect **value) 
    {
        return E_NOTIMPL;
    }
private:
    template <class T>
    HRESULT GetEnumValueAs(T *pValue)
    {
        *pValue = (T)_enumValue;
        if((UINT32)(*pValue) != _enumValue)
        {
            return E_FAIL;
        }
        return S_OK;
    }
private:
    UINT32 _enumValue;
    foundation::ComPtr<foundation::IEnumTypeInfo> m_EnumTypeInfo;
};


