/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValue.cpp
****/

#include <foundation/interfaces/property_value.h>

#include <foundation/macros.h>
#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>

#define FACILITY_PROPERTY_VALUE 0xF8

#define MAKE_PROPERTY_VALUE_ERROR(err)  MAKE_HRESULT(SEVERITY_ERROR, FACILITY_PROPERTY_VALUE,  err)

// PropertyValue Error codes
#define PV_E_WRONG_TYPE               (MAKE_PROPERTY_VALUE_ERROR(0x100))
#define PV_E_CAST_FAILED              (MAKE_PROPERTY_VALUE_ERROR(0x101))

namespace foundation {
    const IID IID_IPropertyValue =
    { 0x4BD682DD, 0x7554, 0x40E9, { 0x9A, 0x9B, 0x82, 0x65, 0x4E, 0xDE, 0x7E, 0x62 } };

    const IID IID_IPropertyValueStatics =
    { 0x629BDBC8, 0xD932, 0x4FF4, { 0x96, 0xB9, 0x8D, 0x96, 0xC5, 0xC1, 0xE8, 0x58 } };

}
using namespace foundation;

// Base structure for all the PropertyValue storage
struct tagPropertyValueBase
{
    UINT16 _propertyType;

    PropertyType GetType()
    {
        return (PropertyType)_propertyType;
    }

    void SetType(PropertyType type)
    {
        _propertyType = (UINT16)type;
    }
    template <class T>
    HRESULT GetNumericValueAs(T *)
    {
        return PV_E_CAST_FAILED;
    }

    // WCHAR 
    void CopyToChar16(WCHAR *)
    {
        foundation_assert(false);
    }

    // Boolean 
    void CopyToBoolean(boolean *)
    {
        foundation_assert(false);
    }
    // String 
    HRESULT CopyToString(HSTRING *)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }
    // GUID 
    void CopyToGuid(GUID *)
    {
        foundation_assert(false);
    }
    // DateTime 
    HRESULT CopyToDateTime(foundation::DateTime *)
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }
};

// A 1 byte storage
struct tagPropertyValue8 : public tagPropertyValueBase
{
    union 
    {
        UINT8   uint8Value;
        bool    booleanValue:1;
    };

    template <class T>
    HRESULT GetNumericValueAs(T *pValue)
    {
        T value;
        switch(_propertyType)
        {
        case PropertyType_UInt8:
            value = (T)uint8Value;
            break;
        default:
            return PV_E_CAST_FAILED;
        }
        *pValue = value;
        return S_OK;
    }

    // Boolean
    void SetBoolean(boolean value)
    {
        booleanValue = value ? true:false;
    }
    void CopyToBoolean(boolean *value)
    {
        *value = booleanValue;
    }
};

// A 2 bytes storage
struct tagPropertyValue16 : public tagPropertyValueBase
{
    union 
    {
        INT16   int16Value;
        UINT16  uint16Value;
        WCHAR   wCharValue;
    };

    template <class T>
    HRESULT GetNumericValueAs(T *pValue)
    {
        T value;
        switch(_propertyType)
        {
        case PropertyType_Int16:
            value = (T)int16Value;
            if((INT16)value != int16Value)
            {
                return PV_E_CAST_FAILED;
            }
            break;
        case PropertyType_UInt16:
            value = (T)uint16Value;
            if((UINT16)value != uint16Value)
            {
                return PV_E_CAST_FAILED;
            }
            break;
        default:
            return PV_E_CAST_FAILED;
        }
        *pValue = value;
        return S_OK;
    }

    // WCHAR 
    void CopyToChar16(WCHAR *c)
    {
        *c = wCharValue;
    }
};

// A 4 bytes storage
struct tagPropertyValue32 : public tagPropertyValueBase
{
    union 
    {
        INT32   int32Value;
        UINT32  uint32Value;
        FLOAT   floatValue;
    };

    template <class T>
    HRESULT GetNumericValueAs(T *pValue)
    {
        T value;
        switch(_propertyType)
        {
        case PropertyType_Int32:
            value = (T)int32Value;
            if((INT32)value != int32Value)
            {
                return PV_E_CAST_FAILED;
            }
            break;
        case PropertyType_UInt32:
            value = (T)uint32Value;
            if((UINT32)value != uint32Value)
            {
                return PV_E_CAST_FAILED;
            }
            break;
        case PropertyType_Single:
            value = (T)floatValue;
            if((FLOAT)value != floatValue)
            {
                return PV_E_CAST_FAILED;
            }
            break;
        default:
            return PV_E_CAST_FAILED;
        }
        *pValue = value;
        return S_OK;
    }
};

// A 64 bytes storage
struct tagPropertyValue64 : public tagPropertyValueBase
{
    union 
    {
        INT64   int64Value;
        UINT64  uint64Value;
        DOUBLE  doubleValue;

        HSTRING hStringValue;
        foundation::DateTime dateTime;

        LPVOID  pBufferArray;
    };

    ~tagPropertyValue64()
    {
        if(_propertyType == PropertyType_String)
        {
            _pal_DeleteString(hStringValue);
        }
    }

    template <class T>
    HRESULT GetNumericValueAs(T *pValue)
    {
        T value;
        switch(_propertyType)
        {
        case PropertyType_Int64:
            value = (T)int64Value;
            if((INT64)value != int64Value)
            {
                return PV_E_CAST_FAILED;
            }
            break;
        case PropertyType_UInt64:
            value = (T)uint64Value;
            if((UINT64)value != uint64Value)
            {
                return PV_E_CAST_FAILED;
            }
            break;
        case PropertyType_Double:
            value = (T)doubleValue;
            if((DOUBLE)value != doubleValue)
            {
                return PV_E_CAST_FAILED;
            }
            break;
        default:
            return PV_E_CAST_FAILED;
        }
        *pValue = value;
        return S_OK;
    }

    // String
    HRESULT CopyToString(HSTRING *value)
    {
        return _pal_DuplicateString(hStringValue,value);
    }
    
    // Date/Time support
    void SetDateTime(foundation::DateTime dt)
    {
        dateTime = dt;
    }
    HRESULT CopyToDateTime(foundation::DateTime *dt)
    {
        *dt = dateTime;
        return S_OK;
    }

};

// A Guid storage
struct tagGuidValue : public tagPropertyValueBase
{
    GUID    guidValue;

    void CopyToGuid(GUID *value)
    {
        *value = guidValue;
    }
};

#define IFTYPE(type,expectedType)   \
    if(type != expectedType)        \
    {                               \
        return PV_E_WRONG_TYPE;     \
    }                               \


//------------------------------------------------------------------------------
// Class:  TPropertyValue
//
// Purpose: Implements IPropertyValue for non Win8 platforms
//
//------------------------------------------------------------------------------
template <class TPropertyValueStorage>
class TPropertyValue : public foundation::ctl::ImplementsInspectable<IPropertyValue, &foundation_IID_IPropertyValue>
{
public:
    template <class T>
    static HRESULT CreateValue(T& value,IInspectable **ppValue)
    {
        TPropertyValue<TPropertyValueStorage> *pPropertyValue = nullptr;
        IFR_ASSERT(CreateInstance(&pPropertyValue));
        IFR_ASSERT(pPropertyValue->SetValue(value));
        *ppValue = static_cast<foundation::ctl::ComInspectableBase *>(pPropertyValue);
        return S_OK;
    }

    static HRESULT CreateValue(IInspectable **ppValue)
    {
        TPropertyValue<TPropertyValueStorage> *pPropertyValue = nullptr;
        IFR_ASSERT(CreateInstance(&pPropertyValue));
        *ppValue = static_cast<foundation::ctl::ComInspectableBase *>(pPropertyValue);
        return S_OK;
    }
protected:
    template <class T>
    static HRESULT CreateInstance(T **ppPropertyValue)
    {
        foundation::ComPtr< foundation::ctl::ComInspectableObject<T> > spPropertyValue;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<T>::CreateInstance(spPropertyValue.GetAddressOf()));
        *ppPropertyValue = spPropertyValue.Detach();
        return S_OK;
    }

    HRESULT SetValue(UINT8 uint8Value)
    {
        this->SetType(PropertyType_UInt8);
        m_value.uint8Value = uint8Value;
        return S_OK;
    }

    HRESULT SetValue(INT16 int16Value)
    {
        this->SetType(PropertyType_Int16);
        m_value.int16Value = int16Value;
        return S_OK;
    }
    HRESULT SetValue(UINT16 uint16Value)
    {
        this->SetType(PropertyType_UInt16);
        m_value.uint16Value = uint16Value;
        return S_OK;
    }
    HRESULT SetValue(INT32 int32Value)
    {
        this->SetType(PropertyType_Int32);
        m_value.int32Value = int32Value;
        return S_OK;
    }
    HRESULT SetValue(UINT32 uint32Value)
    {
        this->SetType(PropertyType_UInt32);
        m_value.uint32Value = uint32Value;
        return S_OK;
    }
    HRESULT SetValue(INT64 int64Value)
    {
        this->SetType(PropertyType_Int64);
        m_value.int64Value = int64Value;
        return S_OK;
    }
    HRESULT SetValue(UINT64 uint64Value)
    {
        this->SetType(PropertyType_UInt64);
        m_value.uint64Value = uint64Value;
        return S_OK;
    }
    HRESULT SetValue(FLOAT floatValue)
    {
        this->SetType(PropertyType_Single);
        m_value.floatValue = floatValue;
        return S_OK;
    }
    HRESULT SetValue(DOUBLE doubleValue)
    {
        this->SetType(PropertyType_Double);
        m_value.doubleValue = doubleValue;
        return S_OK;
    }
    HRESULT SetValue(WCHAR wCharValue)
    {
        this->SetType(PropertyType_Char16);
        m_value.wCharValue = wCharValue;
        return S_OK;
    }
    HRESULT SetValue(bool booleanValue)
    {
        this->SetType(PropertyType_Boolean);
        m_value.SetBoolean( booleanValue);
        return S_OK;
    }
    HRESULT SetValue(HSTRING hString)
    {
        this->SetType(PropertyType_String);
        return _pal_DuplicateString(hString,&m_value.hStringValue);
    }

    HRESULT SetValue(GUID guidValue)
    {
        this->SetType(PropertyType_Guid);
        m_value.guidValue = guidValue;
        return S_OK;
    }
    HRESULT SetValue(foundation::DateTime dateTime)
    {
        this->SetType(PropertyType_DateTime);
        m_value.SetDateTime( dateTime);
        return S_OK;
    }

protected:
    TPropertyValue()
    {
        memset(&m_value,0,sizeof(m_value));
        SetType(PropertyType_Empty);
    }

    PropertyType GetType()
    { 
        return m_value.GetType(); 
    }

    void SetType(PropertyType type) 
    { 
        m_value.SetType(type);
    }

    ~TPropertyValue()
    {
    }


    HRESULT STDMETHODCALLTYPE get_Type( 
        PropertyType *value) override
    {
        *value = GetType();
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
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt16( 
        INT16 *value) override
    {
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt16( 
        UINT16 *value) override
    {
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt32( 
        INT32 *value) override
    {
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt32( 
        UINT32 *value) override
    {
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt64( 
        INT64 *value) override
    {
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt64( 
        UINT64 *value) override
    {
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetSingle( 
        FLOAT *value) override
    {
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetDouble( 
       DOUBLE *value) override
    {
        return m_value.GetNumericValueAs(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetChar16( 
        WCHAR *value) override
    {
        IFTYPE(m_value.GetType(),PropertyType_Char16)
        m_value.CopyToChar16(value);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetBoolean( 
        boolean *value) override
    {
        IFTYPE(m_value.GetType(),PropertyType_Boolean)
        m_value.CopyToBoolean(value);
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE GetString( 
       HSTRING *value) override
    {
        if(m_value.GetType() == PropertyType_Empty)
        {
            *value = nullptr;
            return S_OK;
        }
        IFTYPE(m_value.GetType(),PropertyType_String)
        return m_value.CopyToString(value);
    }

    HRESULT STDMETHODCALLTYPE GetGuid( 
        GUID *value) override
    {
        IFTYPE(m_value.GetType(),PropertyType_Guid)
        m_value.CopyToGuid(value); 
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetDateTime( 
        DateTime *value) override
    {
        IFTYPE(m_value.GetType(),PropertyType_DateTime)
        return m_value.CopyToDateTime(value);
    }
                    
    HRESULT STDMETHODCALLTYPE GetTimeSpan( 
        TimeSpan *value) override
    {
        return PV_E_WRONG_TYPE;
    }

    HRESULT STDMETHODCALLTYPE GetPoint( 
         __RPC__out foundation::Point *value)
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetSize( 
        foundation::Size *value)
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetRect( 
        foundation::Rect *value)
    {
        return PV_E_WRONG_TYPE;
    }
                                        
    HRESULT STDMETHODCALLTYPE GetUInt8Array( 
        UINT32 *__valueSize,
        BYTE **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt16Array( 
        UINT32 *__valueSize,
        INT16 **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt16Array( 
        UINT32 *__valueSize,
         UINT16 **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt32Array( 
        UINT32 *__valueSize,
        INT32 **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt32Array( 
        UINT32 *__valueSize,
        UINT32 **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt64Array( 
        UINT32 *__valueSize,
        INT64 **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt64Array( 
        UINT32 *__valueSize,
        UINT64 **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetSingleArray( 
        UINT32 *__valueSize,
        FLOAT **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetDoubleArray( 
        UINT32 *__valueSize,
        DOUBLE **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetChar16Array( 
        UINT32 *__valueSize,
        WCHAR **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetBooleanArray( 
        UINT32 *__valueSize,
        boolean **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetStringArray( 
        UINT32 *__valueSize,
        HSTRING **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInspectableArray( 
        UINT32 *__valueSize,
        IInspectable ***value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetGuidArray( 
        UINT32 *__valueSize,
        GUID **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetDateTimeArray( 
        UINT32 *__valueSize,
        DateTime **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetTimeSpanArray( 
        UINT32 *__valueSize,
        TimeSpan **value) override
    {
        return PV_E_WRONG_TYPE;
    }

    HRESULT STDMETHODCALLTYPE GetPointArray( 
        UINT32 *__valueSize,
        foundation::Point **value)
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetSizeArray( 
        UINT32 *__valueSize,
        foundation::Size **value)
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetRectArray( 
        UINT32 *__valueSize,
        foundation::Rect **value) 
    {
        return PV_E_WRONG_TYPE;
    }
private:
    bool IsNumericType(PropertyType propertyType)
    {
        return m_value.propertyType >= PropertyType_UInt8 && m_value.propertyType <= PropertyType_Char16;
    }
protected:
    TPropertyValueStorage m_value;
};

typedef TPropertyValue<tagPropertyValueBase> CPropertyValueEmpty;
typedef TPropertyValue<tagPropertyValue8> CPropertyValue8;
typedef TPropertyValue<tagPropertyValue16> CPropertyValue16;
typedef TPropertyValue<tagPropertyValue32> CPropertyValue32;
typedef TPropertyValue<tagPropertyValue64> CPropertyValue64;
typedef TPropertyValue<tagGuidValue> CPropertyValueGuid;

//------------------------------------------------------------------------------
// Class:   CPropertyArray
//
// Purpose: Implements Array support for IPropertyValue for non Win8 platforms
//
//------------------------------------------------------------------------------
class CPropertyArray : public CPropertyValue64
{
public:
    template <class T>
    static HRESULT CreateArray(UINT32 size,T* pValues,IInspectable **ppValue)
    {
        CPropertyArray *pPropertyArray = nullptr;
        IFR_ASSERT(CreateInstance(&pPropertyArray));
        IFR_ASSERT(pPropertyArray->SetArray(size, pValues));
        *ppValue = static_cast<foundation::ctl::ComInspectableBase *>(pPropertyArray);
        return S_OK;
    }
    template <class T>
    static HRESULT CreateArray(PropertyType type,UINT32 size, T* pValues, IInspectable **ppValue)
    {
        CPropertyArray *pPropertyArray = nullptr;
        IFR_ASSERT(CreateInstance(&pPropertyArray));
        IFR_ASSERT(pPropertyArray->SetArrayInternal(type,size,pValues));
        *ppValue = static_cast<foundation::ctl::ComInspectableBase *>(pPropertyArray);
        return S_OK;
    }

private:
    HRESULT SetArray(UINT32 size,IInspectable **pValues)
    {
        SetArrayInternal(PropertyType_InspectableArray,size,pValues);
        while(size--)
        {
            if(*pValues)
            {
                (*pValues)->AddRef();
            }
            ++pValues;
        }
        return S_OK;
    }

    HRESULT SetArray(UINT32 size,HSTRING *pValues)
    {
        SetArrayInternal(PropertyType_StringArray,size,pValues);
        HSTRING *pHStringBuffer = (HSTRING *)this->m_value.pBufferArray;
        while(size--)
        {
            HSTRING hString;
            _pal_DuplicateString(*pHStringBuffer,&hString);
            *pHStringBuffer = hString;
            // next
            ++pHStringBuffer;
        }
        return S_OK;
    }
    
    HRESULT SetArray(UINT32 size,UINT8 *pValues)
    {
        SetArrayInternal(PropertyType_UInt8Array,size,pValues);
        return S_OK;
    }
    
    HRESULT SetArray(UINT32 size,UINT16 *pValues)
    {
        SetArrayInternal(PropertyType_UInt16Array,size,pValues);
        return S_OK;
    }
    
    HRESULT SetArray(UINT32 size,INT16 *pValues)
    {
        SetArrayInternal(PropertyType_Int16Array,size,pValues);
        return S_OK;
    }

    HRESULT SetArray(UINT32 size,INT32 *pValues)
    {
        SetArrayInternal(PropertyType_Int32Array,size,pValues);
        return S_OK;
    }
    
    HRESULT SetArray(UINT32 size,INT64 *pValues)
    {
        SetArrayInternal(PropertyType_Int64Array,size,pValues);
        return S_OK;
    }


    HRESULT SetArray(UINT32 size,UINT32 *pValues)
    {
        SetArrayInternal(PropertyType_UInt32Array,size,pValues);
        return S_OK;
    }

    HRESULT SetArray(UINT32 size,UINT64 *pValues)
    {
        SetArrayInternal(PropertyType_UInt64Array,size,pValues);
        return S_OK;
    }
    
    HRESULT SetArray(UINT32 size,DOUBLE *pValues)
    {
        SetArrayInternal(PropertyType_DoubleArray,size,pValues);
        return S_OK;
    }
    
    HRESULT SetArray(UINT32 size,FLOAT *pValues)
    {
        SetArrayInternal(PropertyType_SingleArray,size,pValues);
        return S_OK;
    }
    
    HRESULT SetArray(UINT32 size,WCHAR *pValues)
    {
        SetArrayInternal(PropertyType_Char16Array,size,pValues);
        return S_OK;
    }
    
#if !defined(_WINDOWS)
    HRESULT SetArray(UINT32 size, boolean *pValues)
    {
        SetArrayInternal(PropertyType_BooleanArray,size,pValues);
        return S_OK;
    }
#endif

    template <class T>
    HRESULT SetArrayInternal(PropertyType type,UINT32 size,T *pValues)
    {
        ULONG allocSize = sizeof(T)*size;

        LPVOID pBufferArray = _pal_MemAlloc(allocSize);
		IFCPTR_ASSERT(pBufferArray);

        memcpy (pBufferArray, (LPVOID)pValues, allocSize );
        // assign type/buffer/size
        this->SetType(type);
        this->m_size = size;
        this->m_value.pBufferArray = pBufferArray;
        return S_OK;
    }

    template <class T>
    HRESULT GetArray(UINT32 *size,T **ppValues)
    {
        ULONG allocSize = sizeof(T)*this->m_size;
        LPVOID pBufferArray = _pal_MemAlloc(allocSize);
		IFCPTR_ASSERT(pBufferArray);
		memcpy (pBufferArray, this->m_value.pBufferArray, allocSize );

        *size = this->m_size;
        *ppValues = (T *)pBufferArray;
        return S_OK;
    }

protected:
    ~CPropertyArray()
    {
        if(GetType() == PropertyType_StringArray)
        {
            HSTRING *pHString = (HSTRING *)this->m_value.pBufferArray;
            UINT32 size = this->m_size;
            while(size --)
            {
                _pal_DeleteString(*pHString);
                ++pHString;
            }
        }
        else if(GetType() == PropertyType_InspectableArray)
        {
            IInspectable **ppInspectable = (IInspectable **)this->m_value.pBufferArray;
            UINT32 size = this->m_size;
            while(size --)
            {
                if(*ppInspectable) 
                {
                    (*ppInspectable)->Release();
                }
                ++ppInspectable;
            }
        }
       _pal_MemFree(this->m_value.pBufferArray);
    }

    HRESULT STDMETHODCALLTYPE GetUInt8Array( 
        UINT32 *__valueSize,
        BYTE **value) override
    {
        IFTYPE(this->GetType(),PropertyType_UInt8Array)
        IFR_ASSERT(GetArray(__valueSize,value));
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt16Array( 
        UINT32 *__valueSize,
        INT16 **value) override
    {
        IFTYPE(this->GetType(),PropertyType_Int16Array)
        IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt16Array( 
        UINT32 *__valueSize,
         UINT16 **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt32Array( 
        UINT32 *__valueSize,
        INT32 **value) override
    {
        IFTYPE(this->GetType(),PropertyType_Int32Array)
        IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt32Array( 
        UINT32 *__valueSize,
        UINT32 **value) override
    {
        IFTYPE(this->GetType(),PropertyType_UInt32Array)
        IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInt64Array( 
        UINT32 *__valueSize,
        INT64 **value) override
    {
        IFTYPE(this->GetType(),PropertyType_Int64Array)
            IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;    }
                    
    HRESULT STDMETHODCALLTYPE GetUInt64Array( 
        UINT32 *__valueSize,
        UINT64 **value) override
    {
        IFTYPE(this->GetType(),PropertyType_UInt64Array)
            IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;   
    }
                    
    HRESULT STDMETHODCALLTYPE GetSingleArray( 
        UINT32 *__valueSize,
        FLOAT **value) override
    {
        IFTYPE(this->GetType(),PropertyType_SingleArray)
            IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;   
    }
                    
    HRESULT STDMETHODCALLTYPE GetDoubleArray( 
        UINT32 *__valueSize,
        DOUBLE **value) override
    {
        IFTYPE(this->GetType(),PropertyType_DoubleArray)
            IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;   
    }
                    
    HRESULT STDMETHODCALLTYPE GetChar16Array( 
        UINT32 *__valueSize,
        WCHAR **value) override
    {
        IFTYPE(this->GetType(),PropertyType_Char16Array)
            IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;   
    }
                    
    HRESULT STDMETHODCALLTYPE GetBooleanArray( 
        UINT32 *__valueSize,
        boolean **value) override
    {
        IFTYPE(this->GetType(),PropertyType_BooleanArray)
            IFR_ASSERT(GetArray(__valueSize, value));
        return S_OK;   
    }
                    
    HRESULT STDMETHODCALLTYPE GetStringArray( 
        UINT32 *__valueSize,
        HSTRING **ppValues) override
    {
        IFTYPE(this->GetType(),PropertyType_StringArray)

            IFR_ASSERT(GetArray(__valueSize, ppValues));
        for(UINT32 index = 0;index < *__valueSize; ++index)
        {
            HSTRING *phString = (*ppValues + index);
            HSTRING hString;
            IFR_ASSERT(_pal_DuplicateString(*phString, &hString));
            *phString = hString;
        }
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE GetInspectableArray( 
        UINT32 *__valueSize,
        IInspectable ***ppValues) override
    {
        IFTYPE(this->GetType(),PropertyType_InspectableArray)

            IFR_ASSERT(GetArray(__valueSize, ppValues));
        for(UINT32 index = 0;index < *__valueSize; ++index)
        {
            IInspectable *pItem = (*ppValues)[index];
            if(pItem)
            {
                pItem->AddRef();
            }
        }
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE GetGuidArray( 
        UINT32 *__valueSize,
        GUID **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetDateTimeArray( 
        UINT32 *__valueSize,
        DateTime **value) override
    {
        return PV_E_WRONG_TYPE;
    }
                    
    HRESULT STDMETHODCALLTYPE GetTimeSpanArray( 
        UINT32 *__valueSize,
        TimeSpan **value) override
    {
        return PV_E_WRONG_TYPE;
    }
private:
    UINT32 m_size;
};

//------------------------------------------------------------------------------
// Class:   CPropertyValueStatics
//
// Purpose: Implements IPropertyValueStatics for non Win8 platforms
//
//------------------------------------------------------------------------------
class CPropertyValueStatics : public foundation::ctl::ImplementsInspectable<IPropertyValueStatics, &foundation_IID_IPropertyValue>
{
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::IID_IPropertyValueStatics)
        {
            *ppInterface = static_cast<foundation::IPropertyValueStatics *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

     HRESULT STDMETHODCALLTYPE CreateEmpty( 
        IInspectable **propertyValue) override
    {
        return CPropertyValueEmpty::CreateValue(propertyValue);
    }

    HRESULT STDMETHODCALLTYPE CreateUInt8( 
        BYTE value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue8::CreateValue(value,propertyValue);
    }

    HRESULT STDMETHODCALLTYPE CreateInt16( 
        INT16 value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue16::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateUInt16( 
        UINT16 value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue16::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateInt32( 
        INT32 value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue32::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateUInt32( 
        UINT32 value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue32::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateInt64( 
        INT64 value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue64::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateUInt64( 
        UINT64 value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue64::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateSingle( 
        FLOAT value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue32::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateDouble( 
        DOUBLE value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue64::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateChar16( 
        WCHAR value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue16::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateBoolean( 
        boolean value,
        IInspectable **propertyValue) override
    {
        bool _value = value ? true : false;
        return CPropertyValue8::CreateValue(_value, propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateString( 
        HSTRING value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue64::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateInspectable( 
        IInspectable *value,
        IInspectable **propertyValue) override
    {
        *propertyValue = value;
        if(value)
        {
            (*propertyValue)->AddRef();
        }
        return S_OK;
    }
                    
    HRESULT STDMETHODCALLTYPE CreateGuid( 
        GUID value,
        IInspectable **propertyValue) override
    {
        return CPropertyValueGuid::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateDateTime( 
        foundation::DateTime value,
        IInspectable **propertyValue) override
    {
        return CPropertyValue64::CreateValue(value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateTimeSpan( 
        foundation::TimeSpan value,
         IInspectable **propertyValue) override
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE CreatePoint( 
        foundation::Point value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
                   
    HRESULT STDMETHODCALLTYPE CreateSize( 
        foundation::Size value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
                    
    HRESULT STDMETHODCALLTYPE CreateRect( 
        foundation::Rect value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
                                        
    HRESULT STDMETHODCALLTYPE CreateUInt8Array( 
        UINT32 __valueSize,
        BYTE *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateInt16Array( 
        UINT32 __valueSize,
        INT16 *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateUInt16Array( 
        UINT32 __valueSize,
        UINT16 *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateInt32Array( 
        UINT32 __valueSize,
        INT32 *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateUInt32Array( 
        UINT32 __valueSize,
        UINT32 *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateInt64Array( 
        UINT32 __valueSize,
        INT64 *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateUInt64Array( 
        UINT32 __valueSize,
        UINT64 *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateSingleArray( 
        UINT32 __valueSize,
        FLOAT *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateDoubleArray( 
        UINT32 __valueSize,
        DOUBLE *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateChar16Array( 
        UINT32 __valueSize,
        WCHAR *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateBooleanArray( 
        UINT32 __valueSize,
        boolean *value,
        IInspectable **propertyValue) override
    {
#if defined(_WINDOWS)
        return CPropertyArray::CreateArray(PropertyType_BooleanArray,__valueSize, value, propertyValue);
#else
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
#endif
    }
                    
    HRESULT STDMETHODCALLTYPE CreateStringArray( 
        UINT32 __valueSize,
        HSTRING *value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
                    
    HRESULT STDMETHODCALLTYPE CreateInspectableArray( 
        UINT32 __valueSize,
        IInspectable **value,
        IInspectable **propertyValue) override
    {
        return CPropertyArray::CreateArray(__valueSize,value,propertyValue);
    }
    
    HRESULT STDMETHODCALLTYPE CreateGuidArray( 
        UINT32 __valueSize,
        GUID *value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
                    
    virtual HRESULT STDMETHODCALLTYPE CreateDateTimeArray( 
        UINT32 __valueSize,
        foundation::DateTime *value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
                    
    virtual HRESULT STDMETHODCALLTYPE CreateTimeSpanArray( 
        UINT32 __valueSize,
        foundation::TimeSpan *value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
                    
    virtual HRESULT STDMETHODCALLTYPE CreatePointArray( 
        UINT32 __valueSize,
        foundation::Point *value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
                    
    virtual HRESULT STDMETHODCALLTYPE CreateSizeArray( 
        UINT32 __valueSize,
        foundation::Size *value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
                    
    virtual HRESULT STDMETHODCALLTYPE CreateRectArray( 
        UINT32 __valueSize,
        foundation::Rect *value,
        IInspectable **propertyValue)
    {
        return E_NOTIMPL;
    }
};

EXTERN_C HRESULT CreatePropertyValueStaticsInternal(IPropertyValueStatics **ppPropertyValueStatics)
{
    foundation::ComPtr< foundation::ctl::ComInspectableObject<CPropertyValueStatics> > spPropertyValueStatics;
    IFR_ASSERT(foundation::ctl::ComInspectableObject<CPropertyValueStatics>::CreateInstance(spPropertyValueStatics.GetAddressOf()));
    *ppPropertyValueStatics = spPropertyValueStatics.Detach();
    return S_OK;
}
