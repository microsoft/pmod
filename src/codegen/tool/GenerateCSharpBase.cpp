/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCSharpBase.cpp
****/

#include "GenerateCSharpBase.h"
#include "guidhelper.h"

std::wstring CGenerateCSharpBase::toCSharpIIDType(GUID iidType)
{
    return ToWideString(iidType);
}

std::wstring CGenerateCSharpBase::toCSharpType(foundation::PropertyType propertyType)
{
    std::wstring csharpType;

    switch(propertyType & 0xff)
    {
    case foundation::PropertyType::PropertyType_Empty:
        csharpType = L"VoidModelType";
        break;
    case foundation::PropertyType::PropertyType_UInt8:
        csharpType = L"byte";
        break;
    case foundation::PropertyType::PropertyType_Int16:
        csharpType = L"short";
        break;
    case foundation::PropertyType::PropertyType_UInt16:
        csharpType = L"ushort";
        break;
    case foundation::PropertyType::PropertyType_Int32:
        csharpType = L"int";
        break;
    case foundation::PropertyType::PropertyType_UInt32:
        csharpType = L"uint";
        break;
    case foundation::PropertyType::PropertyType_Int64:
        csharpType = L"long";
        break;
    case foundation::PropertyType::PropertyType_UInt64:
        csharpType = L"ulong";
        break;
    case foundation::PropertyType::PropertyType_Single:
        csharpType = L"float";
        break;
    case foundation::PropertyType::PropertyType_Double:
        csharpType = L"double";
        break;
    case foundation::PropertyType::PropertyType_Char16:
        csharpType = L"char";
        break;
    case foundation::PropertyType::PropertyType_Boolean:
        csharpType = L"bool";
        break;
    case foundation::PropertyType::PropertyType_String:
        csharpType = L"string";
        break;
    case foundation::PropertyType::PropertyType_Inspectable:
        csharpType = L"object";
        break;
    case foundation::PropertyType::PropertyType_Guid:
        csharpType = L"Guid";
        break;
    case foundation::PropertyType::PropertyType_Point:
    case foundation::PropertyType::PropertyType_Size:
    case foundation::PropertyType::PropertyType_Rect:
    default:
        assert(false);
    }
    return toPropertyType(propertyType,csharpType.c_str(),L"[]");
}


void CGenerateCSharpBase::emitEnum(
    std::wostream& os,
    const schema::CEnumTypeInfo *pEnumTypeInfo)
{
    os << "\tpublic enum " << pEnumTypeInfo->getName() << std::endl;
    os << "\t{" << std::endl;

    // Iterate Enums
    for (schema::_EnumInfoVectorType::const_iterator iter = pEnumTypeInfo->getEnums().begin();
        iter != pEnumTypeInfo->getEnums().end();
        ++iter)
    {
        // generate enum
        if (iter != pEnumTypeInfo->getEnums().begin())
        {
            os << "," << std::endl;
        }
        os << "\t\t" << std::get<0>(*iter) << " = ";
        UINT32 enum_value = std::get<1>(*iter);
        if (enum_value & 0x80000000)
        {
            os << "unchecked((int)" << enum_value << ")";
        }
        else
        {
            os << enum_value;
        }
    }

    os << std::endl << "\t}" << std::endl;
}
