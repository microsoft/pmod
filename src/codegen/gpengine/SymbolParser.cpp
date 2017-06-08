/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SymbolParser.cpp
****/
#include "SymbolParser.h"
#include <assert.h>

enum Actions { ACTION_SHIFT = 1, ACTION_REDUCE = 2, ACTION_GOTO = 3, ACTION_ACCEPT = 4 };
enum Reductions { REDUCTION_OK, REDUCTION_ERROR, REDUCTION_TEXT_ACCEPTED, REDUCTION_SIMPLIFIED };

Symbol::Symbol()
	: Parent(nullptr), Trimmed(false)
{
//	printf("->%p ", this);
}

Symbol::~Symbol()
{
//	printf("<-%p ", this);
}

NonTerminal::~NonTerminal()
{
}

std::vector<std::wstring> SymbolParser::getPossibleTokens(const CGTParser& cgtParser, int index)
{
	std::vector<std::wstring> tokenVector;
	for (size_t i = 0; i < cgtParser.getLARLStateTable()[index].Actions.size(); i++) {
		int j = cgtParser.getLARLStateTable()[index].Actions[i].SymbolIndex;
		if (cgtParser.getSymbolTable()[j].Type == TERMINAL) {
			std::wstring tokenName = cgtParser.getSymbolTable()[j].Name;
			tokenVector.push_back(tokenName);
		}
	}
	return tokenVector;
}

bool SymbolParser::getNextAction(const CGTParser& cgtParser, int symbolIndex, int index, Action& action) {
	auto state = cgtParser.getLARLStateTable()[index];
	for(auto iter = state.Actions.cbegin();iter != state.Actions.cend();++iter)
	{
		if ((*iter).SymbolIndex == symbolIndex) 
		{
			action = *iter;
			return true;
		}
	}

	return false;
}

Symbol *SymbolParser::getNextReduction(
	const std::vector<Token>& tokens,
	const CGTParser& cgtParser,
	bool trimReduction,
	bool reportOnlyOneError,
	std::vector<Symbol *>& symbolStack,
	std::vector<Symbol *>& allSymbols)
{
	NonTerminal  *newNonTerminal;
	Terminal     *newTerminal;

	int index;
	int action, target;

	for (; tokenIterator != tokens.cend(); ++tokenIterator) {
		// Get next action
		Action actionObj;
		bool found = getNextAction(cgtParser, (*tokenIterator).SymbolIndex, _currentState, actionObj);

		// Save this tokens information for the error system.
		auto tok = (*tokenIterator);
		_lastTerminal.SymbolName = tok.Symbol;
		_lastTerminal.Image = tok.Image;
		_lastTerminal.Line = tok.Line;
		_lastTerminal.Column = tok.Column;

		if (!found)
		{
			// Generate ERROR & recover pushing expected symbol in the stack
			// RECOVERING IS IN THE TODO LIST!
			// FOR THAT WE NEED A MECHANISM TO "ESTIMATE" THE NEXT TOKEN
			// Or use Burke-Fisher recovering algorithm

			// Create a symbol traceback vector.
			std::vector <Symbol*> traceback;
			std::vector <Symbol*> tmptokvector = symbolStack;

			//std::reverse_copy(tmptokvector.begin(), tmptokvector.end(), traceback.begin());
			for (auto iter2 = tmptokvector.cbegin();
				iter2 != tmptokvector.cend();
				++iter2)
			{
				traceback.insert(traceback.begin(), *iter2);
			}

			std::vector <std::wstring> expectedTokens = getPossibleTokens(cgtParser, _currentState);
			// Add the error to the Error class.
			ParserError error;
			error.Type = ERROR_PARSE;
			error.Value = UNEXPECTED_TOKEN;
			error.Line = tok.Line;
			error.Column = tok.Column;
			//error.Reduction = prevReduction;
			//error.LastTerminal = lastTerminal;
			_errors.push_back(error);

			if (reportOnlyOneError) {
				reductionResult = REDUCTION_ERROR;
				return nullptr;
			}

		}
		else {
			action = actionObj.ActionIndex;
			target = actionObj.TargetIndex;

#if defined(DEBUG_LALR) 
			wprintf(L"Action: %d\n", action);
#endif
			switch (action) {
				/*
				Pushes current token into the stack
				*/
			case ACTION_SHIFT:
#if defined(DEBUG_LALR) 
			{
				wprintf(L"Shifting: ");
				wprintf(tokens[tokenIndex]->symbol);
				wprintf(L"\nGo to state:%d\n\n", target);
			}
#endif
			// Push current token on the stack
			_currentState = target;

			_currentLine = tok.Line;
			_currentCol = tok.Column;

			//tokens[tokenIndex]->state = currentState;

			// Create a terminal symbol and push it on the stack
			newTerminal = new Terminal();
			newTerminal->Type = TERMINAL;
			allSymbols.push_back(newTerminal);

			newTerminal->SymbolName = tok.Symbol;
			newTerminal->Image = tok.Image;
			newTerminal->SymbolIndex = tok.SymbolIndex;
			newTerminal->State = _currentState;
			newTerminal->Line = tok.Line;
			newTerminal->Column = tok.Column;

			_currentLine = tok.Line;
			_currentCol = tok.Column;

			symbolStack.push_back(newTerminal);
			break;

			/*
			Creates a new reduction. Pops all the terminals and non terminals
			for this rule and pushes the most left non terminal.
			*/
			case ACTION_REDUCE:
#if defined(DEBUG_LALR) 
			{
				wprintf(L"Reducing...\n");
			}
#endif       
			// Create a new Non Terminal (to represent this reduction)
			newNonTerminal = new NonTerminal();
			newNonTerminal->Type = NON_TERMINAL;

			allSymbols.push_back(newNonTerminal);

			index = cgtParser.getRuleTable()[target].NonTerminal;

			newNonTerminal->SymbolIndex = index;
			newNonTerminal->SymbolName = cgtParser.getSymbolTable()[index].Name;
			newNonTerminal->Line = _currentLine;
			newNonTerminal->Column = _currentCol;

			// If the rule has only a nonterminal then we dont create a reduction
			// node for this rule in the tree since its not usefull.
			// User can decide to simplify this by enabling the trimming
			if ((cgtParser.getRuleTable()[target].Symbols.size() == 1) &&
				(cgtParser.getSymbolTable()[cgtParser.getRuleTable()[target].Symbols[0]].Type ==
					NON_TERMINAL) && trimReduction) {
				trim = true;
				newNonTerminal->Trimmed = true;
			}
			else {
				newNonTerminal->Trimmed = false;
				trim = false;
			}

#if defined(DEBUG_LALR) 
			{
				wprintf(symbolTable->symbols[ruleTable->rules[target].nonTerminal].name);
				wprintf(L" = ");
			}
#endif       

			// pop from the stack the tokens for the reduced rule
			// and store them in the reduction
			for (size_t i = 0; i < cgtParser.getRuleTable()[target].Symbols.size(); i++) {
				Symbol *s = symbolStack.back();

				// If the symbol is trimmed we just pick up its children
				if (s->Trimmed) {
					assert(s->Type == NON_TERMINAL);
					NonTerminal *trimmedNT = (NonTerminal*)s;

					assert(trimmedNT->Childrens.size() == 1);
					s = trimmedNT->Childrens[0];
					//newNonTerminal->children.push_front (trimmedNT->children[0]);
				}
				//  else {
				//	newNonTerminal->children.push_front (s);
				//}
				newNonTerminal->Childrens.push_front(s);
				s->Parent = newNonTerminal;

				symbolStack.pop_back();
			}

#if defined(DEBUG_LALR) 
			{
				for (i = 0; i < ruleTable->rules[target].symbols.size(); i++) {
					int symIndex;
					if (!trim) {
						symIndex = newNonTerminal->children[i]->symbolIndex;
						wprintf(symbolTable->symbols[symIndex].name);
					}
					wprintf(L" ");
				}
				wprintf(L"\n");
			}
#endif       

			// Perform GOTO
#if defined(DEBUG_LALR) 
			{
				wprintf(L"state: %d index: %d\n", symbolStack.top()->state,
					newNonTerminal->symbolIndex);
			}
#endif       
			found = getNextAction(cgtParser, newNonTerminal->SymbolIndex, symbolStack.back()->State, actionObj);

			if (found && (actionObj.ActionIndex == ACTION_GOTO)) {
#if defined(DEBUG_LALR)
				wprintf(L"Go to state: %d\n\n", actObj->target);
#endif       

				_currentState = actionObj.TargetIndex;
				newNonTerminal->State = _currentState;

				// Push the reduced nonterminal in the stack
				symbolStack.push_back(newNonTerminal);
			}
			else {
				wprintf(L"Internal Error!!\n");
				reductionResult = REDUCTION_ERROR;
				return NULL;
			}

			/*
			if (trim) {
			reductionResult = REDUCTION_SIMPLIFIED;
			//   return NULL;
			return newNonTerminal;
			} else {
			reductionResult = REDUCTION_OK;
			return newNonTerminal;
			}
			*/
			return newNonTerminal;

			// This Action can never happen...
			case ACTION_GOTO:
				wprintf(L"Goto: %d", target);
				_currentState = target;
				break;

			case ACTION_ACCEPT:
				reductionResult = REDUCTION_TEXT_ACCEPTED;
#if defined(DEBUG_LALR) 
				wprintf(L"text parsed succesfully\n");
#endif       
				return nullptr;
				break;
			}
		}

	}

	reductionResult = REDUCTION_ERROR;
	return nullptr;
}

SymbolResult *SymbolParser::parse(
	const std::vector <Token> &tokens,
	const CGTParser& cgtParser,
	bool trimReductions,
	bool reportOnlyOneError)
{
	auto result = new SymbolResult();

	std::vector<Symbol *> symbolStack;

	Symbol *startReduction;
	startReduction = new Symbol();
	startReduction->Parent = nullptr;
	startReduction->State = cgtParser.getLALRInit();

	result->_symbols.push_back(startReduction);
	symbolStack.push_back(startReduction);

	_currentState = cgtParser.getLALRInit();
	trim = false;
	tokenIterator = tokens.cbegin();

	Symbol *reduction;
	_prevReduction = nullptr;
	while (true) {
		reduction = getNextReduction(tokens, cgtParser, trimReductions, reportOnlyOneError, symbolStack, result->_symbols);
		if ((reduction == nullptr) && ((reductionResult == REDUCTION_ERROR) ||
			(reductionResult == REDUCTION_TEXT_ACCEPTED))) {
			break;
		}
		else if (reduction) {
			_prevReduction = reduction;
		}
	}

	if (reductionResult == REDUCTION_TEXT_ACCEPTED) {
		result->_result = _prevReduction;
	}

	return result;
}

SymbolResult::~SymbolResult()
{
	for(auto iter = _symbols.cbegin();iter != _symbols.cend();++iter)
	{
		delete *iter;
	}
	_symbols.clear();
}
