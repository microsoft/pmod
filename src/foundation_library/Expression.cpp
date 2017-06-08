/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Expression.cpp
****/

#include "pch.h"
#include "Expression.h"

#include <foundation/pv_util.h>
#include <foundation/library/logger_macros.h>
#include <stack>
#include <set>
#include <string>
#include <wctype.h>
#include <ctype.h>
#include <climits>

using namespace foundation;

enum _OperatorType
{
    OperatorType_None  = 0,
    OperatorType_not,
    OperatorType_mul,
    OperatorType_div,
    OperatorType_idiv,
    OperatorType_mod,
    OperatorType_add,
    OperatorType_sub,
    OperatorType_xor,
    OperatorType_band,
    OperatorType_bor,
    OperatorType_land,
    OperatorType_lor,
    OperatorType_nor,
    OperatorType_nand,
    OperatorType_iseq,
    OperatorType_ne,
    OperatorType_shl,
    OperatorType_shr,
    OperatorType_lt,
    OperatorType_gt,
    OperatorType_gte,
    OperatorType_lte,
};

struct OperatorTypeInfo
{
    _OperatorType   _operatorType;
    const CHAR_t    *_operator_token;
    int             _precedence;
};

static const OperatorTypeInfo _operatorTypeInfos[] =
{
    {OperatorType_not,U("!"), 20}, 
    {OperatorType_mul,U("*"), 19}, 
    {OperatorType_div,U("/"), 19}, 
    {OperatorType_idiv,U("\\"), 19}, 
    {OperatorType_mod,U("%"), 18},
    {OperatorType_shl,U("<<"), 17}, 
    {OperatorType_shr,U(">>"), 17},
    {OperatorType_sub, U("-"),16},
    {OperatorType_add,U("+"), 16},
    {OperatorType_xor,U("^"), 15},
    {OperatorType_band,U("&"), 15},
    {OperatorType_bor,U("|"), 15},
    {OperatorType_nor,U("!|"),15}, 
    {OperatorType_land,U("&&"),14},
    {OperatorType_lor,U("||"), 14},
    {OperatorType_nand,U("!&"), 15},
    {OperatorType_iseq, U("=="),15},
    {OperatorType_lt, U("<"),13},
    {OperatorType_gt,U(">"), 13},
    {OperatorType_gte,U(">="), 13},
    {OperatorType_ne, U("!="),13},
    {OperatorType_lte,U("<="), 13}
};

enum _TokenType
{
    TokenType_operator,
    TokenType_value_operand,
    TokenType_reference_operand,
    TokenType_parenthesis,
};

static bool isRealType(PropertyValuePtr& value)
{
    PropertyType type;
    value->get_Type(&type);
    return type==PropertyType_Single || type==PropertyType_Double;
}
static bool hasRealTypes(PropertyValuePtr& value1,PropertyValuePtr& value2)
{
    return isRealType(value1) || isRealType(value2);
}

static bool isSignedType(PropertyValuePtr& value)
{
    PropertyType type;
    value->get_Type(&type);
    return type==PropertyType_Int16 ||
        type==PropertyType_Int32 ||
        type==PropertyType_Int64;
}

static bool hasSignedTypes(PropertyValuePtr& value1,PropertyValuePtr& value2)
{
    return isSignedType(value1) || isSignedType(value2);
}
template <class T>
static T arithmeticOperator(_OperatorType operatorType,const T& value1,const T& value2)
{
    T result;
    switch (operatorType)
    {
    case OperatorType_add:
        result = value1 + value2;
        break;
    case OperatorType_sub:
        result = value1 - value2;
        break;
    case OperatorType_mul:
        result = value1 * value2;
        break;
    case OperatorType_div:
        result = value1 / value2;
        break;
    default:
        result = 0; // just to make the compiler happy
        foundation_assert(0);
        break;
    }
    return result;
}
//+-----------------------------------------------------------------------------
// Class:   CTokenBase
//
// Purpose: Base class for token parsing
//------------------------------------------------------------------------------
class CTokenBase
{
public:
    template <class T>
    T *As()
    {
        return static_cast<T *>(this);
    }
    _TokenType getType()
    {
        return _tokenType;
    }
    
    virtual ~CTokenBase()
    {
    }
protected:
    CTokenBase(_TokenType tokenType):
        _tokenType(tokenType)
    {
    }


private:
    _TokenType _tokenType;
};
//+-----------------------------------------------------------------------------
// Class:   CTokenOperator
//
// Purpose: Token operator class
//------------------------------------------------------------------------------
class CTokenOperator : public CTokenBase
{
public:
    CTokenOperator(int operatorTypeInfoIndex):
        CTokenBase(TokenType_operator),
        _operatorTypeInfoIndex(operatorTypeInfoIndex)
    {
    }

    OperatorTypeInfo getOperatorTypeInfo()
    {
        return _operatorTypeInfos[_operatorTypeInfoIndex];
    }

private:
    int _operatorTypeInfoIndex;
};

//+-----------------------------------------------------------------------------
// Class:   CTokenValue
//
// Purpose: Token Value class
//------------------------------------------------------------------------------
class CTokenValue : public CTokenBase
{
public:
    template <class TValue>
    static PropertyValuePtr create_property_value(TValue value)
    {
        return pv_util::ToPropertyValuePtr(pv_util::CreateValue(value));
    }

    CTokenValue(PropertyValuePtr& value) :
        CTokenBase(TokenType_value_operand),
        _value(value)
    {
    }
    template <class TValue>
    CTokenValue(TValue value) :
        CTokenBase(TokenType_value_operand)
    {
        _value = create_property_value(value);
    }

    void getValue(PropertyValuePtr& value)
    {
        value = _value;
    }
private:
    PropertyValuePtr _value;
};

//+-----------------------------------------------------------------------------
// Class:   CTokenReferenceValue
//
// Purpose: Token Index class
//------------------------------------------------------------------------------
class CTokenReferenceValue : public CTokenBase
{
public:
    CTokenReferenceValue(const CHAR_t *referenceToken):
        CTokenBase(TokenType_reference_operand),
        _referenceToken(referenceToken)
    {
    }
    // Return Index of binding reference
    const CHAR_t * getReferenceToken()
    {
        return _referenceToken.c_str();
    }
private:
    foundation::string_t _referenceToken;
};
//+-----------------------------------------------------------------------------
// Class:   CTokenParenthesis
//
// Purpose: Token Parenthesis class
//------------------------------------------------------------------------------
class CTokenParenthesis : public CTokenBase
{
public:
    CTokenParenthesis(bool isLeft):
        CTokenBase(TokenType_parenthesis),
        _isLeft(isLeft)
    {
    }


    bool IsLeft()
    {
        return _isLeft;
    }
private:
    bool _isLeft;
};

static bool IsLeftParenthesis(CTokenBase *pTokenBase)
{
    return pTokenBase->getType() ==TokenType_parenthesis && pTokenBase->As<CTokenParenthesis>()->IsLeft();
}

__unused_attribute__
static bool IsRightParenthesis(CTokenBase *pTokenBase)
{
    return pTokenBase->getType() ==TokenType_parenthesis && !pTokenBase->As<CTokenParenthesis>()->IsLeft();
}

static int isOperator(foundation::string_t op)
{
    const foundation::string_t _operatorToken = U("!<>=*/%+-^&|\\");

    size_t i = 0, oplen = 0;
    // scan as long as it is a valid operator
    // an operator might  have not just one symbol to represent it

    while (i < op.length())
    {
        if (_operatorToken.find(op[i]) == _operatorToken.npos)
            break;
        oplen++;
        i++;
    }

    // no valid symbol in operator!s
    if (!oplen)
        return -1;

    // identify what operator that is and return its index
    // 1 = highest , N = lowest
    // -1 = invalid operator
    foundation::string_t token = op.substr(0, oplen);
    for (i=0; i < sizeof(_operatorTypeInfos)/sizeof(OperatorTypeInfo); i++)
    {
        if (token == _operatorTypeInfos[i]._operator_token)
        {
            return (int)i;
        }
    }
    return -1;
}
//+-----------------------------------------------------------------------------
// Method:   ExpressionToRpn
//
// Purpose: Convert an expression to RPN (reverse pollack notation) format
//------------------------------------------------------------------------------
static HRESULT ExpressionToRpn(
    const foundation::string_t& expression,
    _TokenBaseVectorType &rpn,
    std::set<foundation::string_t> &references)
{
    const string_t _false_keyword(U("false"));
    const string_t _true_keyword(U("true"));

    std::stack<CTokenBase *> st;

    for (size_t i=0; i < expression.length();i++)
    {
        CHAR_t nextToken = expression[i];

        // skip white space
        if (iswspace(nextToken))
        {
            continue;
        }

        // push left parenthesis
        else if (nextToken == U('('))
        {
            st.push(new CTokenParenthesis(true));
            continue;
        }

        // flush all stack till matching the left-parenthesis
        else if (nextToken == U(')'))
        {
            for (;;)
            {
                // could not match left-parenthesis
                if (st.empty())
                return E_UNEXPECTED;

                CTokenBase *pTopTokenBase = st.top();
                st.pop();
                if (IsLeftParenthesis(pTopTokenBase))
                {
                    delete pTopTokenBase;
                    break;
                }
                rpn.push_back(pTopTokenBase);
            }
            continue;
        }
        // is this an operator?
        int  operatorTypeInfoIndex = isOperator(expression.substr(i));
        // an operand
        if (operatorTypeInfoIndex == -1)
        {
            size_t pos = foundation::string_t::npos;
            if(nextToken == U('{'))
            {
                pos = expression.find(U('}'),i);
                if(pos == expression.npos)
                {
                    return E_UNEXPECTED;
                }
                foundation::string_t referenceToken = expression.substr(i+1, pos - i -1 );
                if (referenceToken.length() ==0)
                {
                    return E_UNEXPECTED;
                }
                references.insert(referenceToken);
                rpn.push_back(new CTokenReferenceValue(referenceToken.data()));
            }
            else if(nextToken == U('\''))
            {
                // lookup for end of string
                pos = expression.find(U('\''),i + 1);
                if(pos == expression.npos)
                {
                    return E_UNEXPECTED;
                }
                // construct a string
                PropertyValuePtr stringValue = 
                    pv_util::ToPropertyValuePtr
                    (
                    pv_util::CreateValue(expression.substr(i+1, pos - i -1 ).c_str())
                    );
                rpn.push_back(new CTokenValue(stringValue));
            }
            else if (expression.compare(0, _false_keyword.length(),_false_keyword) == 0)
            {
                pos += _false_keyword.length();
                rpn.push_back(new CTokenValue(false));
            }
            else if (expression.compare(0, _true_keyword.length(), _true_keyword) == 0)
            {
                pos += _true_keyword.length();
                rpn.push_back(new CTokenValue(true));
            }
            else
            {
                foundation::string_t value_str;

                // attempt to find the end token of a value
                pos = i;
                bool isDouble = false;
                bool isHex = false;
                bool isSigned = false;
                while(pos < expression.size())
                {
                    CHAR_t c = expression[pos];
                    if(c == U('x'))
                    {
                        if(pos > 0 && expression[pos-1]==U('0'))
                        {
                            isHex = true;
                            value_str = U("");
                        }
                    }
                    else if(c == '-')
                    {
                        isSigned = true;
                        value_str += c;
                    }
                    else if(c == '.')
                    {
                        isDouble = true;
                        value_str += c;
                    }
                    else if(isxdigit(c))
                    {
                        value_str += c;
                    }
                    else
                    {
                        break;
                    }
                    ++pos;
                }
                --pos; // adjust the value termination position

                if(value_str.length() > 0)
                {
                    PropertyValuePtr value;

                    const CHAR_t *pValue = value_str.c_str();
                    CHAR_t *pEndPtr = nullptr;
                    // attempt to convert the value
                    if (isDouble)
                    {
                        // TODO: we need to PAL wcstod call
                        double d = _pal_strtod(pValue,&pEndPtr);
                        value = CTokenValue::create_property_value(d);
                    }
                    else
                    {
                        int base =  isHex ? 16:10;
                        if(isSigned)
                        {
                            long val = _pal_strtol(pValue, &pEndPtr, base);
                            if (val != LONG_MAX)
                            {
                                value = CTokenValue::create_property_value((INT32)val);
                            }
                            else
                            {
                                // attempt 64 bit
                                value = CTokenValue::create_property_value(_pal_strtoll(pValue, &pEndPtr, base));
                            }
                        }
                        else
                        {
                            unsigned long val = _pal_strtoul(pValue,&pEndPtr,base);
                            if (val != ULONG_MAX)
                            {
                                value = CTokenValue::create_property_value((UINT32)val);
                            }
                            else
                            {
                                // attempt 64 bit
                                value = CTokenValue::create_property_value(_pal_strtoull(pValue, &pEndPtr, base));
                            }
                        }
                    }
                    if ((size_t)(pEndPtr - pValue) != value_str.size())
                    {
                        return E_UNEXPECTED; // error in conversion
                    }
                    rpn.push_back(new CTokenValue(value));
                }
                else
                {
                    return E_UNEXPECTED; // zero length 
                }
            }
            foundation_assert(pos != std::string::npos);
            i = pos;
            continue;
        }
        // is an operator
        else
        {
            // expression is empty or last operand an operator
            //if (rpn.empty() || (isOperator(token) > 0))
            //{
            //rpn.append(SEP + "0");
            //}

            // get precedence
            int topPrecedence = 0;

            // get current operator
            i += _pal_strlen(_operatorTypeInfos[operatorTypeInfoIndex]._operator_token) -1;
            for (;;)
            {
                CTokenBase *pTopTokenBase = nullptr;
                // get top's precedence
                if (!st.empty())
                {
                    pTopTokenBase = st.top();
                    if (pTopTokenBase->getType() != TokenType_operator)
                        topPrecedence = 1; // give a low priority if operator not ok!
                    else
                        topPrecedence = pTopTokenBase->As<CTokenOperator>()->getOperatorTypeInfo()._precedence;
                }

                if (st.empty() || IsLeftParenthesis(st.top()) || _operatorTypeInfos[operatorTypeInfoIndex]._precedence > topPrecedence)
                {
                    st.push(new CTokenOperator(operatorTypeInfoIndex));
                    break;
                }
                // operator has lower precedence then pop it
                else
                {
                    st.pop();
                    foundation_assert(pTopTokenBase);
                    rpn.push_back(pTopTokenBase);
                }
            }
            continue;
        }
 
    }

    for (;;)
    {
        if (st.empty())
            break;
        CTokenBase *pTopTokenBase = st.top();
        st.pop();
        if (!IsLeftParenthesis(pTopTokenBase))
        {
            rpn.push_back(pTopTokenBase);
        }
        else
        {
            delete pTopTokenBase;
            return E_UNEXPECTED;
        }
    }
    return S_OK;
}

//+-----------------------------------------------------------------------------
// Method:   EvaluateRpnExpression
//
// Purpose: Evaluate a RPN expression with new binding values
//------------------------------------------------------------------------------
static HRESULT EvaluateRpnExpression(
    const _TokenBaseVectorType &rpn,
    _In_ IResolveTokenDelegate *pResolveDelegate,
    _COM_Outptr_result_maybenull_ IPropertyValue **ppValue)
{
    IFCPTR_ASSERT(pResolveDelegate);
    IFCPTR_ASSERT(ppValue);
    *ppValue = nullptr;

    std::stack<PropertyValuePtr> st;

    for(_TokenBaseVectorType::const_iterator iter = rpn.begin();
        iter != rpn.end();
        ++iter)
    {
        CTokenBase *pTokenBase = (*iter);

        if(pTokenBase->getType() == TokenType_value_operand)
        {
            PropertyValuePtr value;
            pTokenBase->As<CTokenValue>()->getValue(value);
            st.push(value);
        }
        else if(pTokenBase->getType() == TokenType_reference_operand)
        {
            const CHAR_t *referenceToken = pTokenBase->As<CTokenReferenceValue>()->getReferenceToken();
            PropertyValuePtr value;

            IFR(pResolveDelegate->Invoke(referenceToken,value.GetAddressOf()));
            st.push(value);
        }
        else
        {
            _OperatorType operatorType = pTokenBase->As<CTokenOperator>()->getOperatorTypeInfo()._operatorType;
            // is an operator
            PropertyValuePtr operandRight = st.top();
            st.pop();

            PropertyValuePtr result;
            if(operatorType == OperatorType_not)
            {
                boolean boolValue;
                IFR(operandRight->GetBoolean(&boolValue));

                result = pv_util::ToPropertyValuePtr(pv_util::CreateValue((bool)(boolValue ? false:true)));
            }
            else
            {
                PropertyValuePtr operandLeft = st.top();
                if (!st.empty())
                {
                    operandLeft = st.top();
                    st.pop();
                } 
                else
                    return E_UNEXPECTED;
                // evaluate operator
                switch(operatorType)
                {
                case OperatorType_land:
                    {
                        boolean boolValueLeft,boolValueRight;
                        IFR(operandLeft->GetBoolean(&boolValueLeft));
                        IFR(operandRight->GetBoolean(&boolValueRight));
                        result = pv_util::ToPropertyValuePtr(pv_util::CreateValue((bool)(boolValueLeft && boolValueRight)));
                    }
                    break;
                case OperatorType_lor:
                    {
                        boolean boolValueLeft,boolValueRight;
                        IFR(operandLeft->GetBoolean(&boolValueLeft));
                        IFR(operandRight->GetBoolean(&boolValueRight));
                        result = pv_util::ToPropertyValuePtr(pv_util::CreateValue((bool)(boolValueLeft || boolValueRight)));
                    }
                    break;
                case OperatorType_iseq:
                case OperatorType_gt:
                case OperatorType_gte:
                case OperatorType_lt:
                case OperatorType_lte:
                case OperatorType_ne:
                    {
                        int cmp = pv_util::CompareInspectableValues(operandLeft,operandRight);
                        bool cond = false;
                        if(operatorType == OperatorType_iseq)
                        {
                            cond = (cmp == 0);
                        }
                        else if(operatorType == OperatorType_gt)
                        {
                            cond = (cmp > 0);
                        }
                        else if(operatorType == OperatorType_gte)
                        {
                            cond = (cmp > 0) || (cmp == 0);
                        }
                        else if(operatorType == OperatorType_lt)
                        {
                            cond = (cmp < 0);
                        }
                        else if(operatorType == OperatorType_lte)
                        {
                            cond = (cmp < 0) || (cmp == 0);
                        }
                        else if(operatorType == OperatorType_ne)
                        {
                            cond = cmp != 0;
                        }
                        result = pv_util::ToPropertyValuePtr(pv_util::CreateValue(cond));
                    }
                    break;
                case OperatorType_band:
                case OperatorType_bor:
                case OperatorType_shl:
                case OperatorType_shr:
                case OperatorType_xor:
                    {
                        UINT32 left,right;
                        IFR(operandLeft->GetUInt32(&left));
                        IFR(operandRight->GetUInt32(&right));
                        UINT32 resultVal = 0;
                        if(operatorType == OperatorType_band)
                        {
                            resultVal = left & right;
                        }
                        else if(operatorType == OperatorType_bor)
                        {
                            resultVal = left | right;
                        }
                        else if(operatorType == OperatorType_shl)
                        {
                            resultVal = left << right;
                        }
                        else if(operatorType == OperatorType_shr)
                        {
                            resultVal = left >> right;
                        }
                        else if(operatorType == OperatorType_xor)
                        {
                            resultVal = left ^ right;
                        }
                        result = pv_util::ToPropertyValuePtr(pv_util::CreateValue(resultVal));
                    }
                    break;
                case OperatorType_add:
                case OperatorType_sub:
                case OperatorType_mul:
                case OperatorType_div:
                case OperatorType_idiv:
                    {
                        if(hasRealTypes(operandLeft,operandRight))
                        {
                            DOUBLE left,right;
                            IFR(operandLeft->GetDouble(&left));
                            IFR(operandRight->GetDouble(&right));
                            result = pv_util::ToPropertyValuePtr(pv_util::CreateValue(arithmeticOperator(operatorType,left,right)));
                        }
                        else if(operatorType==OperatorType_sub || hasSignedTypes(operandLeft,operandRight))
                        {
                            INT32 left,right;
                            IFR(operandLeft->GetInt32(&left));
                            IFR(operandRight->GetInt32(&right));
                            result = pv_util::ToPropertyValuePtr(pv_util::CreateValue(arithmeticOperator(operatorType,left,right)));
                        }
                        else
                        {
                            UINT32 left,right;
                            IFR(operandLeft->GetUInt32(&left));
                            IFR(operandRight->GetUInt32(&right));
                            result = pv_util::ToPropertyValuePtr(pv_util::CreateValue(arithmeticOperator(operatorType,left,right)));
                        }
                    }
                    break;
                default:
                    return E_NOTIMPL;
                }
            }
            // push result
            st.push(result);
        }
    }
    PropertyValuePtr result = st.top();
    st.pop();
    if (!st.empty())
        return E_UNEXPECTED;
    return result.CopyTo(ppValue);
}


HRESULT CExpression::_Initialize(LPCSTR_t expression)
{
    std::set<foundation::string_t> tokenReferences;
    IFR(ExpressionToRpn(expression,m_rpn,tokenReferences));

    _CreateArrayFromContainer(
            tokenReferences,
			[](std::set<foundation::string_t>::const_iterator &iter) {
				HSTRING hstring;
				_pal_CreateString((*iter).c_str(), (UINT32)(*iter).size(), &hstring);
				return hstring;
			},
            m_TokenReferences.GetSizeAddressOf(),
            m_TokenReferences.GetBufferAddressOf());
    return S_OK;
}

STDMETHODIMP CExpression::GetReferenceTokens(UINT32 *size, HSTRING **ppReferenceTokens)
{
    m_TokenReferences.CopyTo(size,ppReferenceTokens);
    return S_OK;
}

STDMETHODIMP CExpression::Evaluate(foundation::IResolveTokenDelegate *pResolveDelegate,
                                   foundation::IPropertyValue ** ppValue)
{
    IFCPTR(pResolveDelegate);
    IFCPTR(ppValue);

    IFR(EvaluateRpnExpression(m_rpn,pResolveDelegate,ppValue));
    return S_OK;
}

HRESULT CExpression::CreateInstance(
    _In_ LPCSTR_t expression,
    _COM_Outptr_ IExpression **ppExpression)
{
    return ctl::inspectable::CreateInstanceWithInitialize<CExpression>(ppExpression, expression);
}
