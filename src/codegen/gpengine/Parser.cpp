/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Parser.cpp
****/
#include "Parser.h"

enum Actions { ACTION_SHIFT = 1, ACTION_REDUCE = 2, ACTION_GOTO = 3, ACTION_ACCEPT = 4 };
enum Reductions { REDUCTION_OK, REDUCTION_ERROR, REDUCTION_TEXT_ACCEPTED, REDUCTION_SIMPLIFIED };

bool Parser::scan(const CGTParser& cgtParser, const std::wstring& text) {

	bool commentBlock = false;
	bool commentLine = false;
	int currentLine = 1, currentCol = 1, tokenBeginCol = 1;

	std::wstring tmpImageStr;
	wchar_t currentChar;
	int currentState = cgtParser.getDFAInit();
	int i = 0;

	bool run = true;

	while (run) {
		if (text[i] == 0) {
			// Check if Comment Block not ended
			if (commentBlock) {
				// Set error and return
				ParserError error;
				error.Type = ERROR_SCAN;
				error.Value = END_COMMENT_NOT_FOUND;
				error.Line = currentLine;
				error.Column = tokenBeginCol;

				_errors.push_back(error);
				return false;
			}
			else {
				run = false;
			}
		}
		if (text[i] == L'\n') {
			currentLine++;
			currentCol = 1;
			if (commentLine) {
				commentLine = false;
				//  i++;
				if (text[i] == 0) {
					break;
				}
			}
		}

		if (!cgtParser.isCaseSensitive()) {
			currentChar = (wchar_t)towlower(text[i]);
		}
		else {
			currentChar = (wchar_t)text[i];
		}

		if (!commentLine) {

			tmpImageStr += text[i];

			// Check which is the next state
			bool changedState = false;
			for (size_t e = 0; e <  cgtParser.getDFAStateTable()[currentState].Edges.size(); e++) {

				auto edge = cgtParser.getDFAStateTable()[currentState].Edges[e];
				int index = edge.CharacterSetIndex;
				// Check if the current character matchs one of the edge string
				auto edgeString = cgtParser.getCharacterSetTable()[index];
				int s = 0;
				while (edgeString[s] != 0) {
					if (currentChar == edgeString[s]) {
						currentState = edge.TargetIndex;
						changedState = true;
						break;
					}
					s++;
				}
				if (changedState) break;

			}

			if (!changedState) {

				// Check if it is an Accept state
				if (cgtParser.getDFAStateTable()[currentState].Accept) {
					i--;
					currentCol--;
					// If the current character was a \n we have to decrement the line
					// counter
					if (text[i + 1] == L'\n') {
						currentLine--;
					}

					// Check for type of symbol
					int index2 = cgtParser.getDFAStateTable()[currentState].AcceptIndex;
					switch (cgtParser.getSymbolTable()[index2].Type) {
						// Non terminal symbol
					case 0:
						// Cannot be
						break;

						// Terminal symbol
					case 1:
						if (!commentBlock) {

							Token token;
							token.Symbol = cgtParser.getSymbolTable()[index2].Name;
							token.Image = tmpImageStr.substr(0, tmpImageStr.length() - 1);

							token.SymbolIndex = index2;
							token.Kind = TERMINAL;

							token.Line = currentLine;
							token.Column = tokenBeginCol;

							_tokens.push_back(token);
						}
						tokenBeginCol = currentCol;
						break;

						// Whitespaces
					case 2:
						// Just dont do anything or generate error for token not accepted
						tokenBeginCol = currentCol;
						break;

						// End symbol
					case 3:
						wprintf(L"EOF SYMBOL");
						break;

						// Comment Start
					case 4:
						commentBlock = true;
						break;

						// Comment End
					case 5:
						commentBlock = false;
						break;

					case 6:
						commentLine = true;
						break;
					}
					currentState = cgtParser.getDFAInit();
					//imgIndex = 0;
					tmpImageStr.clear();
				}
				else {
					if (!commentBlock) {
						// Set error and return

						ParserError error;
						error.Type = ERROR_SCAN;
						error.Value = UNKNOWN_TOKEN;
						error.Line = currentLine;
						error.Column = tokenBeginCol;

						_errors.push_back(error);

						currentState = cgtParser.getDFAInit();
						//imgIndex = 0;
						tmpImageStr.clear();
						return false;
					}
					tokenBeginCol = currentCol;
				}
			}

		}
		i++;
		currentCol++;
	}

	// Generate EOF token
	Token token;
	token.Symbol = L"EOF";
	token.Image = L"EOF";

	token.Kind = NON_TERMINAL;

	token.Line = currentLine;
	token.Column = currentCol;

	_tokens.push_back(token);

	return true;
}
