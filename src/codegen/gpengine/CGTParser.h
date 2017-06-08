/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CGTParser.h
****/
#pragma once
#include <foundation/value_ptr.h>
#include <iostream> 
#include <vector> 

enum SymbolType { TERMINAL = 1, NON_TERMINAL = 0 };
struct SymbolTable
{
	std::wstring Name;
	SymbolType Type;
};

struct Rule
{
	int NonTerminal;
	std::vector<int> Symbols;
};

struct Edge
{
	int CharacterSetIndex;
	int TargetIndex;
};

struct DFAState
{
	bool Accept;
	int AcceptIndex;
	std::vector<Edge> Edges;
};

struct Action
{
	int SymbolIndex;
	int ActionIndex;
	int TargetIndex;
};

struct LALRState
{
	std::vector<Action> Actions;
};

struct GrammarInfo
{
	std::wstring Name;
	std::wstring Version;
	std::wstring Author;
	std::wstring About;
};

class CGTParser
{
public:
	bool load(std::istream& is);
	const GrammarInfo&getGrammarInfo() const { return _grammarInfo; }
	int getDFAInit() const { return _DFAInit; }
	int getLALRInit() const { return _LALRInit; }
	bool isCaseSensitive() const { return _caseSensitive; }
	const std::vector<DFAState>& getDFAStateTable() const { return _dfaStateTable; }
	const std::vector<LALRState>& getLARLStateTable() const { return _larlStateTable; }
	const std::vector<std::wstring>& getCharacterSetTable() const { return _characterSetTable; }
	const std::vector<SymbolTable>& getSymbolTable() const { return _symbolTable; }
	const std::vector<Rule>& getRuleTable() const { return _ruleTable; }

private:
	static foundation::ValuePtr readEntry(std::istream& is);
	static std::wstring readString(std::istream& is);
	static std::wstring readStringEntry(std::istream& is);
	static INT16 readIntEntry(std::istream& is);
	static bool readBooleanEntry(std::istream& is);
	static BYTE readByteEntry(std::istream& is);

private:
	GrammarInfo _grammarInfo;
	bool _caseSensitive;
	int _DFAInit;
	int _LALRInit;
	std::wstring _header;
	std::vector<std::wstring> _characterSetTable;
	std::vector<SymbolTable> _symbolTable;
	std::vector<Rule> _ruleTable;
	std::vector<DFAState> _dfaStateTable;
	std::vector<LALRState> _larlStateTable;
};

