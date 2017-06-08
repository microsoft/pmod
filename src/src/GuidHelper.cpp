/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GuidHelper.cpp
****/

#include "guidhelper.h"

void SwapUInt16(UINT8 *p)
{
    UINT16* p_dest = reinterpret_cast< UINT16* >(p);
    UINT16 const* const p_src = reinterpret_cast< UINT16 const* >(p);
    *p_dest = (*p_src >> 8) | (*p_src << 8);
}

void SwapUInt32(UINT8 *p)
{
    UINT32* p_dest = reinterpret_cast< UINT32* >(p);
    UINT32 const* const p_src = reinterpret_cast< UINT32 const* >(p);
    *p_dest = (*p_src >> 24) | ((*p_src & 0x00ff0000) >> 8) | ((*p_src & 0x0000ff00) << 8) | (*p_src << 24);
}

static void ToHexBytes(std::string& os,UINT8 *&pIIdType,unsigned int nbytes)
{
    const char * hex = "0123456789abcdef";

    while(nbytes--)
    {
        os += hex[(*pIIdType>>4)&0xF];
        os += hex[(*pIIdType)&0xF];
        ++pIIdType;
    }
}

static UINT8 FromHexByte(char c)
{
    if(c>= '0' && c<='9')
    {
        return c - '0';
    }
    else if(c>= 'A' && c<='F')
    {
        return (c - 'A') + 10;

    }
    else if(c>= 'a' && c<='f')
    {
        return (c - 'a') + 10;

    }
    return 0xff;
}

static bool FromHexBytes(
    const char *&p_cstr,
    UINT8 *pIIdType,
    unsigned int nbytes)
{
    UINT8 hex;
    while(nbytes--)
    {
        if ((hex = FromHexByte(*p_cstr)) == 0xff)
        {
            return false;
        }
        UINT8 byte = hex << 4;
        ++p_cstr;
        if ((hex = FromHexByte(*p_cstr)) == 0xff)
        {
            return false;
        }
        byte += hex;
        ++p_cstr;
        *pIIdType = byte;
        ++pIIdType;
    }
    return true;
}

std::string ToString(GUID iidType)
{
    UINT8 *pIIdType = (UINT8 *)&iidType;

    std::string os;

    // first 4 bytes
    SwapUInt32(pIIdType);
    ToHexBytes(os,pIIdType,4);
    os += "-";
    // next 2 bytes
    SwapUInt16(pIIdType);
    ToHexBytes(os,pIIdType,2);
    os += "-";
    // next 2 bytes
    SwapUInt16(pIIdType);
    ToHexBytes(os,pIIdType,2);
    os += "-";
    // next 2 bytes
    ToHexBytes(os,pIIdType,2);
    os += "-";
    // rest 6 bytes
    ToHexBytes(os,pIIdType,6);

    return os;
}

bool FromString(const char *pIIDStr,IID& iid)
{
    UINT8 *pIIdType = (UINT8 *)&iid;
    const char *p_cstr = pIIDStr;

    // skip first character
    if (*p_cstr == '{')
        ++p_cstr;
    // first 4 bytes
    if (!FromHexBytes(p_cstr, pIIdType, 4))
    {
        return false;
    }
    SwapUInt32(pIIdType);
    pIIdType +=4;
    ++p_cstr;
    // next 2 bytes
    if(!FromHexBytes(p_cstr,pIIdType,2))
    {
        return false;
    }
    SwapUInt16(pIIdType);
    pIIdType +=2;
    ++p_cstr;
    // next 2 bytes
    if(!FromHexBytes(p_cstr,pIIdType,2))
    {
        return false;
    }
    SwapUInt16(pIIdType);
    pIIdType +=2;
    ++p_cstr;
    // next 2 bytes
    if(!FromHexBytes(p_cstr,pIIdType,2))
    {
        return false;
    }
    pIIdType += 2;
    ++p_cstr;
    // rest 6 bytes
    if(!FromHexBytes(p_cstr,pIIdType,6))
    {
        return false;
    }

    return true;
}

