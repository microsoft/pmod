/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SerializeUtil.h
****/
#pragma once

#include <iostream>
#include <string>
#include <assert.h>
#include <foundation/pal.h>
#include <foundation/string_t.h>

class binary_serializer
{
public :
    binary_serializer(std::ostream& os)
        :_os(os)
    {
    }
    std::ostream& get_ostream() { return _os; }

    template <class T>
    void serializeValue(T value)
    {
        _os.write((const char *)&value,sizeof(T));
    }

    template <class T>
    void serializeArray(UINT32 size,const T* pValue)
    {
        serializeValue(size);
        while(size--)
        {
            serializeValue(*pValue++);
        }
    }

    void serializeByte(UINT8 intValue)
    {
        serializeValue(intValue);
    }
    void serializeInt16(INT16 intValue)
    {
        return serializeValue(intValue);
    }
    void serializeUInt16(UINT16 intValue)
    {
        return serializeValue(intValue);
    }
    void serializeInt(INT32 intValue)
    {
        return serializeValue(intValue);
    }
    void serializeUInt(UINT32 uintValue)
    {
        return serializeValue(uintValue);
    }
    void serializeInt64(INT64 uintValue)
    {
        return serializeValue(uintValue);
    }
    void serializeUInt64(UINT64 uintValue)
    {
        return serializeValue(uintValue);
    }
    void serializeChar16(WCHAR value)
    {
        return serializeValue(value);
    }
    void serializeSingle(FLOAT floatValue)
    {
        return serializeValue(floatValue);
    }
    void serializeDouble(DOUBLE doubleValue)
    {
        return serializeValue(doubleValue);
    }
    void serializeBoolean(bool boolValue)
    {
        return serializeValue(boolValue);
    }
    void serializeIID(const IID& iid)
    {
        return serializeValue(iid);
    }

    void serializeString(const foundation::string_t& s)
    {
#ifdef __UTF16_STRINGS
        serializeWideString(s);
#else
        serializeUtf8String(s);
#endif
    }

    void serializeUtf8String(const std::string& str)
    {
        serializeUInt((UINT32)str.size());
        _os.write((const char *)str.data(),str.size() * sizeof(char));
    }

    void serializeString(const char *p, UINT32 size)
    {
        serializeUInt((UINT32)size);
        _os.write(p,size * sizeof(char));
    }

    void serializeString(const wchar_t *p,UINT32 size)
    {
        serializeWideString(p,size);
    }
    void serializeWideString(const std::wstring& str)
    {
        serializeWideString(str.c_str(),(UINT32)str.size());
    }
    void serializeWideString(const wchar_t *p,UINT32 size)
    {
        serializeUInt((UINT32)size);
        _os.write((const char *)p,size * sizeof(wchar_t));
    }
    void serializeSize(size_t size)
    {
        return serializeValue(size);
    }
private:
    std::ostream& _os;
};

class binary_deserializer
{
public :
    binary_deserializer(std::istream& is)
        :_is(is)
    {
    }
    std::istream& get_istream() { return _is; }

    template <class T>
    T deserializeValue()
    {
        T value;
        _is.read((char *)&value,sizeof(T));
        return value;
    }

    template <class T>
    void deserializeArray(UINT32 *pSize,T **ppValues)
    {
        assert(pSize);
        assert(ppValues);

        UINT32 size = deserializeUInt();
        T *pBuffer = (T*)_pal_MemAlloc(size*sizeof(T*));
        if (!pBuffer)
        {
            return;
        }
#pragma warning ( disable: 6386 )
        for(UINT32 index = 0;index < size; ++index)
        {
            pBuffer[index] = deserializeValue<T>();
        }
        *ppValues = pBuffer;
        *pSize = size;
    }

    UINT8 deserializeByte()
    {
        return deserializeValue<UINT8>();
    }

    INT16 deserializeInt16()
    {
        return deserializeValue<INT16>();
    }

    UINT16 deserializeUInt16()
    {
        return deserializeValue<INT16>();
    }

    INT32 deserializeInt()
    {
        return deserializeValue<INT32>();
    }

    UINT32 deserializeUInt()
    {
        return deserializeValue<UINT32>();
    }

    INT64 deserializeInt64()
    {
        return deserializeValue<INT64>();
    }

    UINT64 deserializeUInt64()
    {
        return deserializeValue<UINT64>();
    }

    size_t deserializeSize()
    {
        return deserializeValue<size_t>();
    }

    WCHAR deserializeChar16()
    {
        return deserializeValue<WCHAR>();
    }

    FLOAT deserializeSingle()
    {
        return deserializeValue<FLOAT>();
    }

    DOUBLE deserializeDouble()
    {
        return deserializeValue<DOUBLE>();
    }

    bool deserializeBoolean()
    {
        return deserializeValue<bool>();
    }

    IID deserializeIID()
    {
        return deserializeValue<IID>();
    }

    foundation::string_t deserializeString()
    {
#ifdef __UTF16_STRINGS
        return deserializeWideString();
#else
        return deserializeUtf8String();
#endif
    }

    std::string deserializeUtf8String()
    {
        size_t size = deserializeUInt();
        std::string buffer;
        buffer.reserve(size + 1);
        _is.read((char *)buffer.data(),size * sizeof(char));
        return std::string(buffer.data(),size);
    }

    std::wstring deserializeWideString()
    {
        size_t size = deserializeUInt();
        std::wstring buffer;
        buffer.reserve(size + 1);
        _is.read((char *)buffer.data(),size * sizeof(wchar_t));
        return std::wstring(buffer.data(),size);
    }
private:
    std::istream& _is;
};

