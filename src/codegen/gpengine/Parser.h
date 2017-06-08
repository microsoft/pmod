/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Parser.h
****/
#pragma once

#include "CGTParser.h"

struct Token 
{
	Token():
		SymbolIndex(0),
		Kind(0),
		Line(-1),
		Column(-1)
	{}

	std::wstring Symbol;
	std::wstring Image;

	int SymbolIndex;
	int Line;
	int Column;

	//int State;
	int Kind;
};

enum ErrorType {
	ERROR_PARSE,
	ERROR_SCAN
};

enum ErrorValue
{
	END_COMMENT_NOT_FOUND, // end of comment not found
	UNKNOWN_TOKEN,
	UNEXPECTED_TOKEN
};

struct ParserError
{
	ErrorType Type;
	ErrorValue Value;

	SymbolTable Reduction;
	int Line;
	int Column;
	std::vector<std::wstring> expected;
	std::vector<SymbolTable>  Traceback;
	//Terminal LastTerminal;
};

class Parser
{
public:
	bool scan(const CGTParser& cgtParser,const std::wstring& text);
	const std::vector<Token>& getTokens() const {
		return _tokens;
	}
	const std::vector<ParserError>& getErrors() const {
		return _errors;
	}
private:
	std::vector<ParserError> _errors;
	std::vector<Token> _tokens;
};

