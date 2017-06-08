/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:property_value.h
****/
#pragma once
#include <foundation/com_interfaces.h>

#if defined(_WINDOWS_RUNTIME)
#include <windows.foundation.h>
namespace foundation {
    // for windows runtime support re define the foundation interfaces under our namespace
    typedef ABI::Windows::Foundation::IPropertyValue        IPropertyValue;
    typedef ABI::Windows::Foundation::IPropertyValueStatics IPropertyValueStatics;
    
}
#else

namespace foundation {

    extern FOUNDATION_API const IID IID_IPropertyValue;
    extern FOUNDATION_API const IID IID_IPropertyValueStatics;

#define foundation_IID_IPropertyValue           foundation::IID_IPropertyValue

// unsupported PAL Types
struct Point
{
};
struct Size
{
};
struct Rect
{
};

struct IPropertyValue : public IInspectable
{
public:
    virtual HRESULT STDMETHODCALLTYPE get_Type( 
        PropertyType *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE get_IsNumericScalar( 
        boolean *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetUInt8( 
        BYTE *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetInt16( 
        INT16 *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetUInt16( 
        UINT16 *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetInt32( 
        INT32 *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetUInt32( 
        UINT32 *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetInt64( 
        INT64 *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetUInt64( 
        UINT64 *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetSingle( 
        FLOAT *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetDouble( 
       DOUBLE *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetChar16( 
        WCHAR *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetBoolean( 
        boolean *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetString( 
       HSTRING *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetGuid( 
        GUID *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetDateTime( 
        DateTime *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetTimeSpan( 
        TimeSpan *value) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPoint( 
        Point *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetSize( 
        Size *value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetRect( 
        Rect *value) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetUInt8Array( 
        UINT32 *__valueSize,
        BYTE **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetInt16Array( 
        UINT32 *__valueSize,
        INT16 **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetUInt16Array( 
        UINT32 *__valueSize,
         UINT16 **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetInt32Array( 
        UINT32 *__valueSize,
        INT32 **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetUInt32Array( 
        UINT32 *__valueSize,
        UINT32 **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetInt64Array( 
        UINT32 *__valueSize,
        INT64 **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetUInt64Array( 
        UINT32 *__valueSize,
        UINT64 **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetSingleArray( 
        UINT32 *__valueSize,
        FLOAT **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetDoubleArray( 
        UINT32 *__valueSize,
        DOUBLE **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetChar16Array( 
        UINT32 *__valueSize,
        WCHAR **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetBooleanArray( 
        UINT32 *__valueSize,
        boolean **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetStringArray( 
        UINT32 *__valueSize,
        HSTRING **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetInspectableArray( 
        UINT32 *__valueSize,
        IInspectable ***value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetGuidArray( 
        UINT32 *__valueSize,
        GUID **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetDateTimeArray( 
        UINT32 *__valueSize,
        DateTime **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetTimeSpanArray( 
        UINT32 *__valueSize,
        TimeSpan **value) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetPointArray( 
        UINT32 *__valueSize,
        Point **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetSizeArray( 
        UINT32 *__valueSize,
        Size **value) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE GetRectArray( 
        UINT32 *__valueSize,
        Rect **value) = 0;
};

struct IPropertyValueStatics : public IInspectable
{
    static const IID& GetIID()
    {
        return IID_IPropertyValueStatics;
    }

    virtual HRESULT STDMETHODCALLTYPE CreateEmpty( 
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateUInt8( 
        BYTE value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateInt16( 
        INT16 value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateUInt16( 
        UINT16 value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateInt32( 
        INT32 value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateUInt32( 
        UINT32 value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateInt64( 
        INT64 value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateUInt64( 
        UINT64 value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateSingle( 
        FLOAT value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateDouble( 
        DOUBLE value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateChar16( 
        WCHAR value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateBoolean( 
        boolean value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateString( 
        HSTRING value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateInspectable( 
        IInspectable *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateGuid( 
        GUID value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateDateTime( 
        DateTime value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateTimeSpan( 
        TimeSpan value,
        IInspectable **propertyValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE CreatePoint( 
        Point value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateSize( 
        Size value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateRect( 
        Rect value,
        IInspectable **propertyValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE CreateUInt8Array( 
        UINT32 __valueSize,
        BYTE *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateInt16Array( 
        UINT32 __valueSize,
        INT16 *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateUInt16Array( 
        UINT32 __valueSize,
        UINT16 *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateInt32Array( 
        UINT32 __valueSize,
        INT32 *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateUInt32Array( 
        UINT32 __valueSize,
        UINT32 *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateInt64Array( 
        UINT32 __valueSize,
        INT64 *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateUInt64Array( 
        UINT32 __valueSize,
        UINT64 *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateSingleArray( 
        UINT32 __valueSize,
        FLOAT *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateDoubleArray( 
        UINT32 __valueSize,
        DOUBLE *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateChar16Array( 
        UINT32 __valueSize,
        WCHAR *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateBooleanArray( 
        UINT32 __valueSize,
        boolean *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateStringArray( 
        UINT32 __valueSize,
        HSTRING *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateInspectableArray( 
        UINT32 __valueSize,
        IInspectable **value,
        IInspectable **propertyValue) = 0;

    virtual HRESULT STDMETHODCALLTYPE CreateGuidArray( 
        UINT32 __valueSize,
        GUID *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateDateTimeArray( 
        UINT32 __valueSize,
        DateTime *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateTimeSpanArray( 
        UINT32 __valueSize,
        TimeSpan *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreatePointArray( 
        UINT32 __valueSize,
        Point *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateSizeArray( 
        UINT32 __valueSize,
        Size *value,
        IInspectable **propertyValue) = 0;
                    
    virtual HRESULT STDMETHODCALLTYPE CreateRectArray( 
        UINT32 __valueSize,
        Rect *value,
        IInspectable **propertyValue) = 0;
                     
};

}

#endif
