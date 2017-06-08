/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GoldenParserTest.cpp
****/
#include "pch.h"

#include "CGTParser.h"
#include "Parser.h"
#include "SymbolParser.h"
#include "ExePath.h"
#include "ParserEngineUtil.h"
#include "ModelSymbolParser.h"
#include <foundation/library/string_util.h>

#include <fstream>

using namespace foundation;
using namespace foundation::library;
using namespace std;


class CGoldenParserTest : public CUnitTestBase
{
	const wchar_t *InterfaceTest =
L"\
    public interface IMyModel\n\
    {\n\
        string Property1 { get; }\n\
        void Method1(int index);\n\
	}\n\
";

protected:
    void Test()
    {
		std::string exePath = ExePath();
		string_util::find_and_replace(exePath, std::string("\\"), std::string("/"));

		ifstream is;
		is.open(exePath + "/CSharp_2.0_r7.cgt", ifstream::in | ifstream::binary);

		CGTParser cgtParser;
		cgtParser.load(is);

		Parser parser;
		parser.scan(cgtParser, InterfaceTest);

		SymbolParser symbolParser;
		auto result = symbolParser.parse(parser.getTokens(), cgtParser, true, true);
		auto rdcSymbol = result->getSymbolResult();
		delete result;
	}

	void TestModel()
	{
		std::string exePath = ExePath();
		string_util::find_and_replace(exePath, std::string("\\"), std::string("/"));
		exePath += "/CSharp_2.0_r7.cgt";

		auto result = CParserEngineUtil::parseContent(to_utf8_string(InterfaceTest).data(), exePath.data());
		auto model = CModelSchemaSymbols::parse(L"", result);
		UT_VERIFY_BOOL(model->getInterfaceTypes().size() > 0);

		delete model;
	}

public:
    BeginTestMethodMap(CGoldenParserTest)
		TestMethod(Test)
		TestMethod(TestModel)
	EndTestMethodMap()

};
ImplementTestClass(CGoldenParserTest);


