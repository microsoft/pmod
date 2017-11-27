/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ParserEngineUtil.cpp
****/
#include "ParserEngineUtil.h"
#include "CodeGenUtil.h"
#include <iostream>
#include <stdlib.h>

#include <assert.h>
#include <string>
#include <cstdarg>
#include <fstream>

// from GP Engine
#include "CGTParser.h"
#include "Parser.h"

#include <foundation/library/string_util.h>

using namespace std;
using namespace foundation::library;


bool CParserEngineUtil::loadSource(const char *filename, string& source)
{
    assert(source.length() == 0);

    ifstream ifs;
    ifs.open(filename, ifstream::in | ifstream::binary);

    if (ifs.bad() || !ifs.is_open())
    {
        return false;
    }

    ifs.seekg(0, ios::end);
    try
    {
        source.reserve(size_t(ifs.tellg()));
    }
    catch (bad_alloc)
    {
        return false;
    }

    ifs.seekg(0, ios::beg);
    source.assign(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>());

    // detect BOM marker
    if (source.length() >= 3 && source[0] == (char)0xEF || source[1] == (char)0xBB || source[2] == (char)0xBF)
    {
        source = source.substr(3);
    }

    return true;
}

SymbolResult *CParserEngineUtil::parseFile(const char *sourceFilePath, const char *grammarFilePath)
{
	// Load source file
	std::string source;
	if (!loadSource(sourceFilePath, source))
	{
		_throwError(L"Error loading source file:%S", sourceFilePath);
	}

	return parseContent(source.data(), grammarFilePath);
}

SymbolResult* CParserEngineUtil::parseContent(const char *sourceContent, const char *grammarFilePath)
{
	CGTParser cgtParser;

	ifstream is;
	is.open(grammarFilePath, ifstream::in | ifstream::binary);

	if (is.fail())
	{
		_throwError(L"Could not open gramma file:%S", grammarFilePath);
	}

    // Load grammar file
    if (!cgtParser.load (is))
    {
        _throwError(L"Load Grammar Failed:%S", grammarFilePath);
    }

    // Scan the source in search of tokens
	Parser parser;
	parser.scan(cgtParser, foundation::to_pal_wstring(sourceContent).get());
    
    // If there are errors report them
    if (parser.getErrors().size() > 0)
    {
        for (auto iter = parser.getErrors().cbegin(); iter != parser.getErrors().cend();++iter)
        {
			
            cout << "Line:" << (*iter).Line << " Column:" << (*iter).Column;
        }
        _throwError(L"Scan Errors reported");
    }
 
    // Get the tokens to feed the LALR machine with them
	SymbolParser symbolParser;

    // Parse the tokens
    auto result = symbolParser.parse (parser.getTokens(), cgtParser, true, true);
   
    if (symbolParser.getErrors().size() > 0)
    {
		for (auto iter = symbolParser.getErrors().cbegin(); iter != symbolParser.getErrors().cend(); ++iter)
		{

			cout << "Line:" << (*iter).Line << " Column:" << (*iter).Column;
		}
		_throwError(L"Parse Errors reported");
        return nullptr;
    }
  
    // Note : uncomment for debugging reduction tree
    //lalr->printReductionTree(rdc, 0);
    return result;
}

void CParserEngineUtil::selectNodes(Symbol* next, const wchar_t* symbolName, _SymbolVectorType& nodes)
{
    if (wcscmp(next->SymbolName.c_str(), symbolName) == 0)
    {
        nodes.push_back(next);
        return;
    }

    if (next->Type == NON_TERMINAL)
    {
        NonTerminal* nonTerminal = (NonTerminal*)next;
        for (auto iter = nonTerminal->Childrens.cbegin();
             iter != nonTerminal->Childrens.cend();
             ++iter) 
        {
            selectNodes(*iter, symbolName, nodes);
        }
    }
}

Terminal *CParserEngineUtil::selectFirstTerminal(Symbol *next,const wchar_t *symbolName)
{
    if (next->Type == TERMINAL && (symbolName==nullptr || next->SymbolName==symbolName) ) {
        return (Terminal *)next;
    }
    if (next->Type == NON_TERMINAL) {
        NonTerminal *nonTerminal = (NonTerminal*) next; 
        for (unsigned int i=0; i < nonTerminal->Childrens.size(); i++) 
        {
            Terminal *terminal = selectFirstTerminal(nonTerminal->Childrens[i],symbolName);
            if(terminal != nullptr)
            {
                return terminal;
            }
        }
    }
    return nullptr;
}

Symbol *CParserEngineUtil::selectParentNode(Symbol *next, const wchar_t *symbolName)
{
    assert(next);

    Symbol *parent;
    while((parent = next->Parent) != nullptr && parent != next)
    {
        if(parent->SymbolName == symbolName)
        {
            return parent;
        }
        next = parent;
    }
    return nullptr;
}

Symbol *CParserEngineUtil::selectSingleNode(Symbol *next,const wchar_t *symbolName)
{
    if (next->SymbolName == symbolName)
    {
        return next;
    }

    if (next->Type == NON_TERMINAL) {
        NonTerminal *nonTerminal = (NonTerminal*) next; 
        for (unsigned int i=0; i < nonTerminal->Childrens.size(); i++) 
        {
            Symbol *child = nonTerminal->Childrens[i];
            if(child->SymbolName == symbolName)
            {
                return child;
            }
        }

        // recursion
        for (unsigned int i=0; i < nonTerminal->Childrens.size(); i++)
        {
            Symbol *node = selectSingleNode(nonTerminal->Childrens[i],symbolName);
            if(node != nullptr)
            {
                return node;
            }
        }
    }

    return nullptr;
}

Symbol *CParserEngineUtil::selectPath(Symbol *next, ...)
{
    va_list arguments; 
   
    va_start(arguments, next);

    std::queue<std::wstring> path;
    wchar_t* nodeName;
    
    //loop until the nullptr in the arguments.
    while ((nodeName = va_arg(arguments, wchar_t*)))
    {
        path.push(std::wstring(nodeName));
    }
    // clean arguments list
    va_end(arguments);

    // start recursive function.
    return selectPathQueue(next, true ,path);
    
}
Symbol *CParserEngineUtil::selectPathQueue(Symbol *next,bool keepSearching, std::queue<std::wstring> path)
{
    // Validate the Queue is not empty.
    if (path.empty())
    {
        return nullptr;
    }

    //inspect first node in the path
    std::wstring name = path.front();

    //Check if we are doing exact match in the case of fail
    if (next->SymbolName != name &&!keepSearching)
    {
        return nullptr;
    }

    // Match name and full path
    if (next->SymbolName == name && path.size() == 1)
    {
        return next;
    }
    //match name and not full path
    else 
    {
        // we matched the item we need to pop and enforce the next item with exact level
        if (next->SymbolName == name)
        {
            path.pop();
            keepSearching = false;
        }
        
        // loop the childs of the element.
        if (next->Type == NON_TERMINAL) {
            NonTerminal *nonTerminal = (NonTerminal*)next;
            for (unsigned int i = 0; i < nonTerminal->Childrens.size(); i++)
            {                
                // recursive call.
                Symbol *child = selectPathQueue(nonTerminal->Childrens[i], keepSearching, path);
                if (child != nullptr)
                {
                    return child;
                }
            }
        }
    }
    return nullptr;
}

const std::wstring& CParserEngineUtil::getTerminalImage(Symbol *symbol)
{    
    assert(symbol->Type == TERMINAL);
    return ((Terminal *)symbol)->Image;
}

void CParserEngineUtil::getImage(Symbol* symbol, std::wstring& image)
{
    if (symbol->Type == TERMINAL)
    {
        image += static_cast<Terminal*>(symbol)->Image;
        return;
    }

    auto nonTerminal = static_cast<NonTerminal*>(symbol);
    for (auto it = nonTerminal->Childrens.cbegin();
        it != nonTerminal->Childrens.cend();
        ++it)
    {
        getImage(*it, image);
    }
}
