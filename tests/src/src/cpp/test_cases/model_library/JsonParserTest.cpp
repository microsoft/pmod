/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JsonParserTest.cpp
****/
#include "pch.h"

#include <foundation/library/json_statics_util.h>
#include <foundation/dictionary_ptr.h>

using namespace foundation;

class CJsonParser : public CUnitTestBase
{
protected:
    void Test()
    {
const char *_Json1= "\
{\n\
\"prop1\":\"hello\",\n\
\"prop2\":100,\n\
\"prop3\":7.5\n\
}\n\
";
        DictionaryPtr jsonObject;
        UT_VERIFY_HR(json_util::Parse_t(_Json1, nullptr, jsonObject));

        FLOAT floatValue;
        UT_VERIFY_HR(jsonObject.GetValue(U("prop3"), floatValue));
        UT_VERIFY_BOOL(floatValue == 7.5);
    }

public:
    BeginTestMethodMap(CJsonParser)
        TestMethod(Test)
    EndTestMethodMap()

};
ImplementTestClass(CJsonParser);


