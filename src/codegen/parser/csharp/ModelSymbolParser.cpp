/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelSymbolParser.cpp
****/
#include "ModelSymbolParser.h"
#include "ParserEngineUtil.h"
#include <assert.h>

const wchar_t *CTerminalReferenceSymbol::getReferenceValue()const
{
    return _symbolReference ? _symbolReference->Image.data():nullptr;
}

static void ParseType(Symbol *declareSymbol, CQualifiedReferenceSymbol& type)
{
	// parse Type
	Symbol* typeNode = nullptr;

	_SymbolVectorType childrenOfTypeType;
	CParserEngineUtil::selectNodes(declareSymbol, L"Type", childrenOfTypeType);
	for (_SymbolVectorType::const_iterator it = childrenOfTypeType.cbegin();
	it != childrenOfTypeType.cend();
		++it)
	{
		const bool isTypeWithinAttribute = (CParserEngineUtil::selectParentNode(*it, L"Attrib Opt") != nullptr);
		if (!isTypeWithinAttribute)
		{
			typeNode = *it;
			break;
		}
	}

	// this will support the case where we have Array types and the Qualified Id is duplicated.
	if (typeNode)
	{
		type.parse(typeNode);
	}
	else
	{
		type.parse(declareSymbol);
	}
	assert(type.hasReference());
}


void CQualifiedReferenceSymbol::parse(Symbol* pParentSymbol)
{
    setSymbolReference(CParserEngineUtil::selectSingleNode(pParentSymbol, L"Qualified ID"));
    
    if(this->hasReference())
    {
        Terminal *first = CParserEngineUtil::selectFirstTerminal(_symbolReference);
        assert(first);

        _identifier = (first->Image);

        _SymbolVectorType membersDecs;
        CParserEngineUtil::selectNodes(_symbolReference,L"MemberName",membersDecs);
        for(_SymbolVectorType::const_iterator iter = membersDecs.begin();
            iter != membersDecs.end();
            ++iter)
        {
            assert( (*iter)->Type == TERMINAL);
            _identifier += ((Terminal *)(*iter))->Image;
        }
    }
}

void CEnumItemReferenceSymbol::parse(Symbol *enumItemDec)
{
    this->parseAsIdentifier(enumItemDec);
    Symbol *decLiteral = CParserEngineUtil::selectSingleNode(enumItemDec,L"DecLiteral");
    if(decLiteral != nullptr)
    {
        _value.setSymbolReference((Terminal *)decLiteral);
    }
    else
    {
        Symbol *hexLiteral = CParserEngineUtil::selectSingleNode(enumItemDec, L"HexLiteral");
        if (hexLiteral != nullptr)
        {
            _value.setSymbolReference((Terminal *)hexLiteral);
        }
    }
}

void CEnumTypeReferenceSymbol::parse(Symbol *enumDeclareSymbol)
{
    this->parseAsIdentifier(enumDeclareSymbol);

    _SymbolVectorType enumItemDecs;
    CParserEngineUtil::selectNodes(enumDeclareSymbol,L"Enum Item Dec",enumItemDecs);

    for(_SymbolVectorType::const_iterator iter = enumItemDecs.begin();
        iter != enumItemDecs.end();
        ++iter)
    {
        CEnumItemReferenceSymbol *pEnumItemReferenceSymbol = new CEnumItemReferenceSymbol();
        pEnumItemReferenceSymbol->parse(*iter);
        _enumItems.push_back(pEnumItemReferenceSymbol);
    }

    // Enum attribute
    //Attrib Section
    Symbol *pAttributeSection = CParserEngineUtil::selectSingleNode(enumDeclareSymbol,L"Attrib Section");
    if(pAttributeSection)
    {
        CAttributeReferenceSymbol *pEnumAttribute = new CAttributeReferenceSymbol();
        pEnumAttribute->parseAttributeDeclare(pAttributeSection);
        _enumAttributes.push_back(pEnumAttribute);
    }

    // Interface attributes
    Symbol *headerSymbol = CParserEngineUtil::selectSingleNode(enumDeclareSymbol, L"Header");
    if (headerSymbol)
    {
        CAttributeReferenceSymbol::parse(headerSymbol, _enumAttributes);
    }

}

void CTerminalReferenceSymbol::parseAsIdentifier(Symbol *symbol,bool isOpt)
{
    Symbol *identifier = CParserEngineUtil::selectSingleNode(symbol,L"Identifier");
    if(!isOpt || identifier)
    {
        assert(identifier != nullptr);
        assert(identifier->Type == TERMINAL);
        this->_symbolReference = (Terminal *)identifier;
    }
}

void CStructFieldReferenceSymbol::parse(Symbol *fieldDeclareSymbol)
{
	// parse property identifier
	Symbol *pVarDeclare = CParserEngineUtil::selectSingleNode(fieldDeclareSymbol, L"Variable Declarator");
	assert(pVarDeclare != nullptr);
	this->parseAsIdentifier(pVarDeclare);
	ParseType(fieldDeclareSymbol, _type);
}

void CStructReferenceSymbol::parse(Symbol *structTypeDeclareSymbol)
{
	this->parseAsIdentifier(structTypeDeclareSymbol);

	_SymbolVectorType fieldDeclares;
	CParserEngineUtil::selectNodes(structTypeDeclareSymbol, L"Field Dec", fieldDeclares);

	for (_SymbolVectorType::const_iterator iter = fieldDeclares.begin();
	iter != fieldDeclares.end();
		++iter)
	{
		CStructFieldReferenceSymbol *pFieldReferenceSymbol = new CStructFieldReferenceSymbol();
		pFieldReferenceSymbol->parse((*iter));
		_fields.push_back(pFieldReferenceSymbol);
	}
}

void CAttributeExpressionSymbol::parse(Symbol *atributeExpressionDeclare)
{
    Symbol *validId = CParserEngineUtil::selectSingleNode(atributeExpressionDeclare,L"Valid ID");

    this->parseAsIdentifier(validId);

    Symbol *literal = CParserEngineUtil::selectSingleNode(atributeExpressionDeclare,L"Literal");
    if(literal != nullptr)
    {
        _value = new CTerminalReferenceSymbol(CParserEngineUtil::selectFirstTerminal(literal));
    }
    else
    {
        Symbol *primaryExp = CParserEngineUtil::selectSingleNode(atributeExpressionDeclare,L"Primary Exp");
        _value = new CQualifiedReferenceSymbol(primaryExp);
    }

}

void CAttributeReferenceSymbol::parseAttributeDeclare(Symbol *attributeDeclareSymbol)
{
    Symbol *qualifiedId = CParserEngineUtil::selectSingleNode(attributeDeclareSymbol,L"Qualified ID");
    assert(qualifiedId != nullptr);
    this->parseAsIdentifier(qualifiedId);

     _SymbolVectorType expressionDeclares;
    CParserEngineUtil::selectNodes(attributeDeclareSymbol,L"Expression",expressionDeclares);

    for(_SymbolVectorType::const_iterator iter = expressionDeclares.begin();
        iter != expressionDeclares.end();
        ++iter)
    {
        CAttributeExpressionSymbol *pAttributeExpressionSymbol = new CAttributeExpressionSymbol();
        pAttributeExpressionSymbol->parse((*iter));
        _attributeExpressions.push_back(pAttributeExpressionSymbol);
    }
}

void CAttributeReferenceSymbol::parse(Symbol *symbol,std::vector<CAttributeReferenceSymbol *>& attributes)
{
    _SymbolVectorType attributeDeclares;
    CParserEngineUtil::selectNodes(symbol,L"Attribute",attributeDeclares);
    for(_SymbolVectorType::const_iterator iter = attributeDeclares.begin();
        iter != attributeDeclares.end();
        ++iter)
    {
        CAttributeReferenceSymbol *pAttributeReferenceSymbol = new CAttributeReferenceSymbol();
        pAttributeReferenceSymbol->parseAttributeDeclare(*iter);
        attributes.push_back(pAttributeReferenceSymbol);
    }
}

void CEventReferenceSymbol::parse(Symbol *eventDeclareSymbol)
{
    // parse property identifier
    this->parseAsIdentifier(eventDeclareSymbol);


    // Property attributes
    CAttributeReferenceSymbol::parse(eventDeclareSymbol,_eventAttributes);
}

void CPropertyReferenceSymbol::parse(Symbol *propertyDeclareSymbol)
{
    // parse property identifier
    this->parseAsIdentifier(propertyDeclareSymbol);

    // parse Type
	ParseType(propertyDeclareSymbol, _type);

    // Interface Accessors
    Symbol *interfaceAccessors = CParserEngineUtil::selectSingleNode(propertyDeclareSymbol,L"Interface Accessors");
    assert(interfaceAccessors != nullptr);
    _hasGet = CParserEngineUtil::selectSingleNode(interfaceAccessors,L"get") != nullptr;
    _hasSet = CParserEngineUtil::selectSingleNode(interfaceAccessors,L"set") != nullptr;

    // Property attributes
    CAttributeReferenceSymbol::parse(propertyDeclareSymbol,_propertyAttributes);
}

void CFormalReferenceSymbol::parse(Symbol *formalParameterDec)
{
    // parse parameter name
    this->parseAsIdentifier(formalParameterDec);

    _parameterType.parse(formalParameterDec);
}

void CMethodReferenceSymbol::parse(Symbol *methodDeclareSymbol)
{
    // parse parameter name
    this->parseAsIdentifier(methodDeclareSymbol);

    // parse Type   Itegral type first
    Symbol * typeNode = CParserEngineUtil::selectPath(methodDeclareSymbol,L"Interface Method Dec", L"Qualified ID",nullptr);
   
    // check for array types
    if (typeNode == nullptr)
    {
        typeNode = CParserEngineUtil::selectPath(methodDeclareSymbol,L"Interface Method Dec",L"Type",L"Qualified ID",nullptr);
    }

    // There was no valid type found; should never happen.
    if (typeNode == nullptr)
    {
        // we shuld never get here!!
        assert(false);
    }

    // this will support the case where we have Array types and the Qualified Id is duplicated.
    // We need to check for Other Type first if this exist it will contain the return value for the method.
    // If is not there it comes in the regular place that it comes for the general case Type.
    if (typeNode != nullptr)
    {
        _resultType.parse(typeNode);
    }

    _SymbolVectorType formalParametersDeclare;
    CParserEngineUtil::selectNodes(methodDeclareSymbol,L"Formal Param",formalParametersDeclare);

    for(_SymbolVectorType::const_iterator iter = formalParametersDeclare.begin();
        iter != formalParametersDeclare.end();
        ++iter)
    {
        CFormalReferenceSymbol *pFormalReferenceSymbol = new CFormalReferenceSymbol();
        pFormalReferenceSymbol->parse((*iter));
        _parameters.push_back(pFormalReferenceSymbol);
    }

    // Method attributes
    CAttributeReferenceSymbol::parse(methodDeclareSymbol,_methodAttributes);
}

void CInterfaceReferenceSymbol::parse(Symbol *interfaceTypeDeclareSymbol)
{
    this->parseAsIdentifier(interfaceTypeDeclareSymbol);

    Symbol *interfaceBaseOpt = CParserEngineUtil::selectSingleNode(interfaceTypeDeclareSymbol,L"Interface Base Opt");
    assert(interfaceBaseOpt != nullptr);

    _baseTypeName.parse(interfaceBaseOpt);

    _SymbolVectorType eventDeclares;
    CParserEngineUtil::selectNodes(interfaceTypeDeclareSymbol,L"Interface Event Dec",eventDeclares);

    for(_SymbolVectorType::const_iterator iter = eventDeclares.begin();
        iter != eventDeclares.end();
        ++iter)
    {
        CEventReferenceSymbol *pEventReferenceSymbol = new CEventReferenceSymbol();
        pEventReferenceSymbol->parse((*iter));
        _events.push_back(pEventReferenceSymbol);
    }

    _SymbolVectorType propertyDeclares;
    CParserEngineUtil::selectNodes(interfaceTypeDeclareSymbol,L"Interface Property Dec",propertyDeclares);

    for(_SymbolVectorType::const_iterator iter = propertyDeclares.begin();
        iter != propertyDeclares.end();
        ++iter)
    {
        CPropertyReferenceSymbol *pPropertyReferenceSymbol = new CPropertyReferenceSymbol();
        pPropertyReferenceSymbol->parse((*iter));
        _properties.push_back(pPropertyReferenceSymbol);
    }

    _SymbolVectorType methodDeclares;
    CParserEngineUtil::selectNodes(interfaceTypeDeclareSymbol,L"Interface Method Dec",methodDeclares);

    for(_SymbolVectorType::const_iterator iter = methodDeclares.begin();
        iter != methodDeclares.end();
        ++iter)
    {
        CMethodReferenceSymbol *pMethodReferenceSymbol = new CMethodReferenceSymbol();
        pMethodReferenceSymbol->parse((*iter));
        _methods.push_back(pMethodReferenceSymbol);
    }

    // Interface attributes
    Symbol *headerSymbol = CParserEngineUtil::selectSingleNode(interfaceTypeDeclareSymbol,L"Header");
    if(headerSymbol)
    {
        CAttributeReferenceSymbol::parse(headerSymbol,_interfaceAttributes);
    }
}

void CModelSchemaSymbols::parseInternal(Symbol *modelRdc)
{
    _SymbolVectorType enumDeclares;
    CParserEngineUtil::selectNodes(modelRdc,L"Enum Decl",enumDeclares);

    for(_SymbolVectorType::const_iterator iter = enumDeclares.begin();
        iter != enumDeclares.end();
        ++iter)
    {
        CEnumTypeReferenceSymbol *pEnumTypeReferenceSymbol = new CEnumTypeReferenceSymbol();
        pEnumTypeReferenceSymbol->parse((*iter));
        _enumTypes.push_back(pEnumTypeReferenceSymbol);
    }

	_SymbolVectorType structTypeDeclares;
	CParserEngineUtil::selectNodes(modelRdc, L"Struct Decl", structTypeDeclares);
	for (_SymbolVectorType::const_iterator iter = structTypeDeclares.begin();
	iter != structTypeDeclares.end();
		++iter)
	{
		CStructReferenceSymbol *pStructReferenceSymbol = new CStructReferenceSymbol();
		pStructReferenceSymbol->parse((*iter));
		_structTypes.push_back(pStructReferenceSymbol);
	}

    _SymbolVectorType interfaceTypeDeclares;
    CParserEngineUtil::selectNodes(modelRdc,L"Interface Decl",interfaceTypeDeclares);

    for(_SymbolVectorType::const_iterator iter = interfaceTypeDeclares.begin();
        iter != interfaceTypeDeclares.end();
        ++iter)
    {
        CInterfaceReferenceSymbol *pInterfaceReferenceSymbol = new CInterfaceReferenceSymbol();
        pInterfaceReferenceSymbol->parse((*iter));
        _interfaceTypes.push_back(pInterfaceReferenceSymbol);
    }
}

CModelSchemaSymbols *CModelSchemaSymbols::parse(const wchar_t *pModelFilePath, SymbolResult *pModelRdcResult)
{
    CModelSchemaSymbols *pModelSchemaSymbols = new CModelSchemaSymbols(pModelFilePath, pModelRdcResult);
    pModelSchemaSymbols->parseInternal(pModelRdcResult->getSymbolResult());
    return pModelSchemaSymbols;
}

CModelSchemaSymbols::~CModelSchemaSymbols()
{
}

CInterfaceReferenceSymbol* CModelSchemaSymbols::lookupInterface(const wchar_t *interfaceName)
{
    for(std::vector<CInterfaceReferenceSymbol *>::const_iterator 
        iter = _interfaceTypes.begin();
        iter != _interfaceTypes.end();
        ++iter)
    {
        if(wcscmp((*iter)->getReferenceValue(),interfaceName) == 0)
        {
            return *iter;
        }
    }
    return nullptr;
}

CInterfaceReferenceSymbol* CModelSchemaSymbols::lookupInterface(
        const std::vector<CModelSchemaSymbols *>&modelSchemaSymbols,
        const wchar_t *interfaceName)
{
    for(std::vector<CModelSchemaSymbols *>::const_iterator 
        iter = modelSchemaSymbols.begin();
        iter != modelSchemaSymbols.end();
        ++iter)
    {
        CInterfaceReferenceSymbol *pInterfaceReferenceSymbol = (*iter)->lookupInterface(interfaceName);
        if(pInterfaceReferenceSymbol)
        {
            return pInterfaceReferenceSymbol;
        }
    }
    return nullptr;
}

bool CModelSchemaSymbols::isTypeArray(const IReferenceSymbol& referenceSymbol)
{
    Symbol *pTypeSymbol = CParserEngineUtil::selectParentNode(referenceSymbol.getSymbolReference(), L"Type");
    return  pTypeSymbol ? CParserEngineUtil::selectSingleNode(pTypeSymbol, L"Rank Specifier") != nullptr : false;
}
