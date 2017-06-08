/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SchemaParser.h
****/
#ifndef _CSHARP_PARSER_DEFINED_
#define _CSHARP_PARSER_DEFINED_

#include "ToolUtil.h"

#include <foundation/library/dictionary_class_ptr.h> 
namespace schema
{
    class CModelSchema;
}
class CSchemaParser
{
public:
    static bool checkFileDependencies(
        int _fh,
        const char *pModelSchemaPath,
        const _ToolParameterType& parameters);

    static schema::CModelSchema *parse(
        const char *pModelSchemaPath,
        const _ToolParameterType& parameters,
        foundation::library::DictionaryClassPtr& other_attributes);
};

#endif
