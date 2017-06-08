/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ParserEngineUtil.h
****/
#pragma once

#include "SymbolParser.h"

#include <vector>
#include <queue>
#include <assert.h>

typedef std::vector<Symbol *> _SymbolVectorType;

class CParserEngineUtil
{
public:
	static SymbolResult *parseContent(const char *sourceContent, const char *grammarFilePath);
	static SymbolResult *parseFile(const char *sourceFilePath,const char *grammarFilePath);
    static void selectNodes(Symbol *next,const wchar_t *symbolName,_SymbolVectorType& nodes);
    static Symbol *selectSingleNode(Symbol *next,const wchar_t *symbolName);
    static Symbol *selectParentNode(Symbol *next,const wchar_t *symbolName);
    static Terminal *selectFirstTerminal(Symbol *next,const wchar_t *symbolName = nullptr);
    static Symbol *selectPath(Symbol *next, ...);
    static Symbol *selectPathQueue(Symbol *next,bool exact, std::queue<std::wstring> path);

    static const std::wstring& getTerminalImage(Symbol* symbol);
    static void getImage(Symbol* symbol, std::wstring& image);

private:
    static bool loadSource (const char *filename, std::string& source);
};


