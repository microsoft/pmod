/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CGTParser.h
****/
#include "CGTParser.h"

using namespace foundation;

ValuePtr CGTParser::readEntry(std::istream& is)
{
	char dataType, temp;
	is.get(dataType);

	switch (dataType)
	{
	case 'E':
		break;
	case 'B':
		is.get(temp);
		return ValuePtr(temp ? true : false);
	case 'b':
	{
		BYTE b;
		is.read((char *)&b, 1);
		return ValuePtr(b);
	}
	case 'I':
	{
		INT16 value;
		is.read((char *)&value, 2);
		return ValuePtr(value);
	}
	case 'S':
		return ValuePtr(readString(is));
	}

	//_throwError(L"Invalid data type:%c", dataType);
	return ValuePtr();
}

std::wstring CGTParser::readStringEntry(std::istream& is)
{
	auto value = readEntry(is);
	std::wstring s;
	THROW_IFR(value.GetValue(s));
	return s;
}

INT16 CGTParser::readIntEntry(std::istream& is)
{
	auto value = readEntry(is);
	INT16 intValue;
	THROW_IFR(value.GetValue(intValue));
	return intValue;
}

bool CGTParser::readBooleanEntry(std::istream& is)
{
	auto value = readEntry(is);
	bool boolValue;
	THROW_IFR(value.GetValue(boolValue));
	return boolValue;
}

BYTE CGTParser::readByteEntry(std::istream& is)
{
	auto value = readEntry(is);
	BYTE byteValue;
	THROW_IFR(value.GetValue(byteValue));
	return byteValue;
}

std::wstring CGTParser::readString(std::istream& is)
{
	std::wstring s;
	while (true)
	{
		char l, h;
		is.get(l);
		is.get(h);

		wchar_t w = ((wchar_t)h) << 8;
		w |= l;
		if (w == 0)
		{
			break;
		}
		s += w;
	}
	return s;
}

bool CGTParser::load(std::istream& is)
{
	char recordType;

	_header = readString(is);
	while (!is.eof()) {
		// Read record type & number of entries
		is.get(recordType);
		if (is.fail()) {
			if (is.eof()) {
				break;
			}
			else {
				return false;
			}
		}

		if (recordType != 77) {
			throw std::runtime_error("Record type is not supported");
			return false;
		}

		INT16 nEntries = 0;
		is.read((char *)&nEntries, 2);

		BYTE contentType = readByteEntry(is);
		switch (contentType)
		{
			case 'P':
			{
				_grammarInfo.Name = readStringEntry(is);
				_grammarInfo.Version = readStringEntry(is);
				_grammarInfo.Author = readStringEntry(is);
				_grammarInfo.About = readStringEntry(is);
				_caseSensitive = readBooleanEntry(is);
				auto startSymbol = readIntEntry(is);
			}
			break;
			case 'T':
			{
				auto numberOfSymbols = readIntEntry(is);
				auto numberOfCharacterSets = readIntEntry(is);
				auto numberOfRules = readIntEntry(is);
				auto numberOfDFA = readIntEntry(is);
				auto numberOfLALR = readIntEntry(is);

				_characterSetTable.resize(numberOfCharacterSets);
				_symbolTable.resize(numberOfSymbols);
				_ruleTable.resize(numberOfRules);
				_dfaStateTable.resize(numberOfDFA);
				_larlStateTable.resize(numberOfLALR);
			}
			break;
			case 'C':
			{
				auto index = readIntEntry(is);
				auto characterSet = readStringEntry(is);
				_characterSetTable[index] = characterSet;
			}
			break;
			case 'S':
			{
				auto index = readIntEntry(is);
				SymbolTable symbol;
				symbol.Name = readStringEntry(is);
				symbol.Type = (SymbolType)readIntEntry(is);

				_symbolTable[index] = symbol;
			}
			break;
			case 'R':
			{
				auto index = readIntEntry(is);
				Rule rule;
				rule.NonTerminal = readIntEntry(is);
				auto empty = readEntry(is);

				for (INT16 i = 0; i < nEntries - 4; i++)
				{
					rule.Symbols.push_back(readIntEntry(is));
				}
				_ruleTable[index] = rule;
			}
			break;
			case 'I':
			{
				_DFAInit = readIntEntry(is);
				_LALRInit = readIntEntry(is);
			}
			break;
			case 'D':
			{
				auto index = readIntEntry(is);
				DFAState state;
				state.Accept = readBooleanEntry(is);
				state.AcceptIndex = readIntEntry(is);
				auto empty = readEntry(is);

				for (INT16 i = 0; i < nEntries - 5; i+=3)
				{
					Edge edge;
					edge.CharacterSetIndex = readIntEntry(is);
					edge.TargetIndex = readIntEntry(is);
					empty = readEntry(is);
					state.Edges.push_back(edge);
				}
				_dfaStateTable[index] = state;
			}
			break;
			case 'L':
			{
				auto index = readIntEntry(is);
				auto empty = readEntry(is);

				LALRState state;
				for (INT16 i = 0; i < nEntries - 3; i += 4)
				{
					Action action;
					action.SymbolIndex = readIntEntry(is);
					action.ActionIndex = readIntEntry(is);
					action.TargetIndex = readIntEntry(is);
					empty = readEntry(is);
					state.Actions.push_back(action);
				}
				_larlStateTable[index] = state;
			}
			break;

			default:
				break;
		}
	}

	return true;
}
