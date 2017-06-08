/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SchemaSerializer.h
****/
#ifndef _SCHEMA_SERIALIZER_DEFINED_
#define _SCHEMA_SERIALIZER_DEFINED_

#include <iostream>

namespace schema
{
    class CModelSchema;
}

class CSchemaSerializer
{
public:
    // Json Serialize/Deserialize a Model Schema
    static void serializeJson(const schema::CModelSchema& modelSchema, std::ostream& os);
    static schema::CModelSchema *deserializeJson(std::istream& is);

};

#endif
