/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelSymbolParser.h
****/
#pragma once

#include <string>
#include <assert.h>
#include <memory>
#include "CodeGenUtil.h"

struct Symbol;
struct Terminal;
class SymbolResult;

class CModelSchemaSymbols;

struct IReferenceSymbol
{
    virtual ~IReferenceSymbol() {}

    virtual Symbol *getSymbolReference() const = 0;
    virtual const wchar_t *getReferenceValue() const = 0;
};

template <class T>
class CReferenceSymbol : public IReferenceSymbol
{
protected:
    CReferenceSymbol():
        _symbolReference(nullptr)
    {
    }

public:
    // interface IReferenceSymbol
    Symbol *getSymbolReference() const
    {
        return _symbolReference;
    }

    void setSymbolReference(T* symbolReference)
    {
        _symbolReference = symbolReference;
    }
    bool hasReference() const { return _symbolReference != nullptr; }
protected:
    T* _symbolReference;
};

class CTerminalReferenceSymbol : public CReferenceSymbol<Terminal>
{
public:
    CTerminalReferenceSymbol()
    {
    }
    CTerminalReferenceSymbol(Terminal *symbolReference)
    {
        _symbolReference = symbolReference;
    }

    const wchar_t *getReferenceValue() const;
    void parseAsIdentifier(Symbol *symbol,bool isOpt = false);
};

class CQualifiedReferenceSymbol : public CReferenceSymbol<Symbol>
{
public:
    CQualifiedReferenceSymbol()
    {
    }
    CQualifiedReferenceSymbol(Symbol* qualifiedIdParent)
    {
        this->parse(qualifiedIdParent);
    }

    const wchar_t *getReferenceValue() const
    {
        assert(_symbolReference);
        return _identifier.data();
    }

    void parse(Symbol* qualifiedIdParent);

private:
    std::wstring    _identifier;
};

class CAttributeExpressionSymbol : public CTerminalReferenceSymbol
{
public:
    CAttributeExpressionSymbol():
        _value(nullptr)
    {
    }
    ~CAttributeExpressionSymbol()
    {
        delete _value;
    }

    const IReferenceSymbol& getReferenceSymbol() 
    {
        assert(_value != nullptr);
        return *_value;
    }
    void parse(Symbol *atributeExpressionDeclare);

protected:
    IReferenceSymbol *_value;
};

class CAttributeReferenceSymbol : public CTerminalReferenceSymbol
{
public:
    static void parse(Symbol *symbol,std::vector<CAttributeReferenceSymbol *>& attributes);

    const std::vector<CAttributeExpressionSymbol *>& getAttributeExpressions() { return _attributeExpressions; }
    void parseAttributeDeclare(Symbol *symbol);

private:
    AutoDeleteVector<CAttributeExpressionSymbol *> _attributeExpressions;
};

class CStructFieldReferenceSymbol : public CTerminalReferenceSymbol
{
public:
	const CQualifiedReferenceSymbol& getType() { return _type; }

protected:
	friend class CStructReferenceSymbol;
	void parse(Symbol *propertyDeclareSymbol);
private:
	CQualifiedReferenceSymbol    _type;
};

class CStructReferenceSymbol : public CTerminalReferenceSymbol
{
public:
	const std::vector<CStructFieldReferenceSymbol *>& getFields() { return _fields; }

private:
	friend class CModelSchemaSymbols;

	void parse(Symbol *structTypeDeclareSymbol);
private:
	AutoDeleteVector<CStructFieldReferenceSymbol *> _fields;
};

class CEventReferenceSymbol : public CTerminalReferenceSymbol
{
public:
    const std::vector<CAttributeReferenceSymbol *>& getAttributes() { return _eventAttributes; }

protected:
    friend class CInterfaceReferenceSymbol;
    void parse(Symbol *eventDeclareSymbol);
private:
    AutoDeleteVector<CAttributeReferenceSymbol *> _eventAttributes;
};

class CPropertyReferenceSymbol : public CTerminalReferenceSymbol
{
public:
    const CQualifiedReferenceSymbol& getType() { return _type;}
    const std::vector<CAttributeReferenceSymbol *>& getAttributes() { return _propertyAttributes; }

    bool hasGet() { return _hasGet; }
    bool hasSet() { return _hasSet; }
protected:
    friend class CInterfaceReferenceSymbol;
    void parse(Symbol *propertyDeclareSymbol);
private:
    CQualifiedReferenceSymbol    _type;
    AutoDeleteVector<CAttributeReferenceSymbol *> _propertyAttributes;
    bool                        _hasGet;
    bool                        _hasSet;
};

class CFormalReferenceSymbol : public CTerminalReferenceSymbol
{
public:
    const CQualifiedReferenceSymbol& getParameterType() { return _parameterType;}
private:
    friend class CMethodReferenceSymbol;
    void parse(Symbol *formalParameterDec);
    
    CQualifiedReferenceSymbol    _parameterType;
};

class CMethodReferenceSymbol : public CTerminalReferenceSymbol
{
public:
    const CQualifiedReferenceSymbol& getResultType() { return _resultType;}
    const std::vector<CFormalReferenceSymbol *>& getParameters() { return _parameters; }
    const std::vector<CAttributeReferenceSymbol *>& getAttributes() { return _methodAttributes; }

protected:
    friend class CInterfaceReferenceSymbol;
    void parse(Symbol *methodDeclareSymbol);
private:
    CQualifiedReferenceSymbol    _resultType;
    AutoDeleteVector<CFormalReferenceSymbol *> _parameters;
    AutoDeleteVector<CAttributeReferenceSymbol *> _methodAttributes;
};

class CEnumItemReferenceSymbol : public CTerminalReferenceSymbol
{
public:
    const CTerminalReferenceSymbol& getValue() { return _value; }
private:
    friend class CEnumTypeReferenceSymbol;

    void parse(Symbol *enumItemDec);
private:
    CTerminalReferenceSymbol    _value;
};

class CEnumTypeReferenceSymbol : public CTerminalReferenceSymbol
{
public:
	CEnumTypeReferenceSymbol()
	{
	}

	~CEnumTypeReferenceSymbol()
	{
	}
    const std::vector<CEnumItemReferenceSymbol *>& getEnumItems() { return _enumItems; }
    const std::vector<CAttributeReferenceSymbol *>& getAttributes() { return _enumAttributes; }
private:
    friend class CModelSchemaSymbols;

    void parse(Symbol *enumDeclareSymbol);
private:
    AutoDeleteVector<CAttributeReferenceSymbol *> _enumAttributes;
    AutoDeleteVector<CEnumItemReferenceSymbol *> _enumItems;
};

class CInterfaceReferenceSymbol : public CTerminalReferenceSymbol
{
public:
    const CQualifiedReferenceSymbol& getBaseType() { return _baseTypeName; }
    const std::vector<CEventReferenceSymbol *>& getEvents() { return _events; }
    const std::vector<CPropertyReferenceSymbol *>& getProperties() { return _properties; }
    const std::vector<CMethodReferenceSymbol *>& getMethods() { return _methods; }
    const std::vector<CAttributeReferenceSymbol *>& getAttributes() { return _interfaceAttributes; }

private:
    friend class CModelSchemaSymbols;

    void parse(Symbol *interfaceTypeDeclareSymbol);
private:
    AutoDeleteVector<CAttributeReferenceSymbol *> _interfaceAttributes;
    CQualifiedReferenceSymbol _baseTypeName;
    AutoDeleteVector<CEventReferenceSymbol *> _events;
    AutoDeleteVector<CPropertyReferenceSymbol *> _properties;
    AutoDeleteVector<CMethodReferenceSymbol *>  _methods;   
};

class CModelSchemaSymbols
{
public:
    ~CModelSchemaSymbols();

    const std::wstring& getModelFilePath() const { return _modelFilePath; }

    static bool isTypeArray(const IReferenceSymbol& referenceSymbol);

    static CModelSchemaSymbols *parse(const wchar_t *pModelFilePath, SymbolResult *pModelRdcResult);

	const std::vector<CStructReferenceSymbol *>& getStructTypes() { return _structTypes; }
	const std::vector<CEnumTypeReferenceSymbol *>& getEnumTypes() { return _enumTypes; }
    const std::vector<CInterfaceReferenceSymbol *>& getInterfaceTypes() { return _interfaceTypes; }
    
    CInterfaceReferenceSymbol* lookupInterface(const wchar_t *interfaceName);

    static  CInterfaceReferenceSymbol* lookupInterface(
        const std::vector<CModelSchemaSymbols *>&modelSchemaSymbols,
        const wchar_t *interfaceName);
private:
    CModelSchemaSymbols(const wchar_t *pModelFilePath, SymbolResult *pModelRdcResult):
        _modelFilePath(pModelFilePath),
		_modelRdcResult(pModelRdcResult)
    {
    }
    void parseInternal(Symbol *modelRdc);

private:
    std::wstring                                _modelFilePath;
	std::shared_ptr<SymbolResult>               _modelRdcResult;
    AutoDeleteVector<CEnumTypeReferenceSymbol *> _enumTypes;
	AutoDeleteVector<CStructReferenceSymbol *> _structTypes;
	AutoDeleteVector<CInterfaceReferenceSymbol *> _interfaceTypes;
};
