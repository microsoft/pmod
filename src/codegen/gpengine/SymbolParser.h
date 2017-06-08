/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SymbolParser.h
****/
#pragma once

#include <deque>
#include <memory>
#include "Parser.h"

struct Symbol 
{
	Symbol();
	virtual ~Symbol();

	SymbolType Type;
	Symbol *Parent;
	std::wstring SymbolName;

	int SymbolIndex;
	int State;
	int Line, Column;

	bool Trimmed;
};


struct Terminal : public Symbol
{
	std::wstring Image;
};

struct NonTerminal : public Symbol
{
	virtual ~NonTerminal();
	std::deque<Symbol *> Childrens;
};

class SymbolParser;
class SymbolResult
{
public:
	~SymbolResult();

	Symbol *getSymbolResult() { return _result; }

	friend class SymbolParser;
private:
	SymbolResult() :
		_result(nullptr)
	{}
private:
	Symbol *_result;
	std::vector<Symbol *> _symbols;
};

class SymbolParser
{
public:
	SymbolResult *parse(
		const std::vector <Token> &tokens,
		const CGTParser& cgtParser,
		bool trimReductions,
		bool reportOnlyOneError);

	const std::vector<ParserError>& getErrors() const {
		return _errors;
	}
private:
	static std::vector<std::wstring> getPossibleTokens(const CGTParser& cgtParser, int index);
	static bool getNextAction(const CGTParser& cgtParser, int symbolIndex, int index, Action& action);
	Symbol *getNextReduction(
		const std::vector<Token>& tokens,
		const CGTParser& cgtParser,
		bool trimReduction,
		bool reportOnlyOneError,
		std::vector<Symbol *>& symbolStack,
		std::vector<Symbol *>& allSymbols);

private:
	int _currentState;
	int _currentLine, _currentCol;

	Symbol       *_prevReduction; // last correctly reduced non terminal
	Terminal      _lastTerminal;   // last parsed symbol

	int reductionResult;
	bool trim;
	std::vector<ParserError> _errors;
	std::vector<Token>::const_iterator tokenIterator;
};

