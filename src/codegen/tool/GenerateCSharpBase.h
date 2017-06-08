/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCSharpBase.h
****/
#pragma once

#include "CodeGenToolBase.h"

class CGenerateCSharpBase : public CCodeGenToolBase
{
protected:
    static std::wstring toCSharpType(foundation::PropertyType propertyType);
    static std::wstring toCSharpIIDType(GUID iidType);
    static void emitEnum(
        std::wostream& os,
        const schema::CEnumTypeInfo *pEnumTypeInfo);

};


