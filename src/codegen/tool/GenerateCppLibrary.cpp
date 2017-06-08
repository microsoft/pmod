/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCppLibrary.cpp
****/

#include "GenerateCppLibrary.h"


// Format template for Internal Command Model
// {0} = Type Info Model (lowercase)
// {1} = Property Name
// {2} = Core namespace
// {3} = CommmandType Name
// {4} = Execute section
// {5} = CanExecute section
// {6} = prefix class name
// {7} = parameter SAL annotation
// {8} = base class name template
const wchar_t *_commandInternalTemplate_Template =
L"\
template <class T>\n\
class {6}{0}{1}Command:\n\
public {2}::library::{8}\n\
{\n\
public:\n\
\tstatic const IID& GetType()\n\
\t{\n\
\t    return {3};\n\
\t}\n\
\ttemplate <class TInstance,class TOuter>\n\
\tstatic HRESULT CreateInstance(_In_ TOuter *pT, _COM_Outptr_ TInstance **ppNewInstance)\n\
\t{\n\
\t    return foundation::library::CreateObjectClass(GetType(),pmod::library::CommandOptions::None, ppNewInstance, pT);\n\
\t}\n\
protected:\n\
\tSTDMETHOD(OnExecute)({7} foundation::IInspectable *parameter,foundation::IInspectable **ppResult) override\n\
\t{\n\
\t\tHRESULT hr = S_OK;\n\
\t\tUNREFERENCED_PARAMETER(parameter);\n\
\t\tT *pT = this->get_target();\n\
\t\tif (pT == nullptr) return E_UNEXPECTED;\n\
{4}\n\
\t\treturn hr;\n\
\t}\n\
{5}\n\
};\n\
\n\
";

// Format template for Internal CanExecute sectionCommand Model
// {0} = CanExecute section
const wchar_t *_commandCanExecuteInternalTemplate_Template = 
L"\
\tSTDMETHOD(OnCanExecute)({1} foundation::IInspectable *parameter, _Out_ bool *pValue) override\n\
\t{\n\
\t\tHRESULT hr = S_OK;\n\
\t\tUNREFERENCED_PARAMETER(parameter);\n\
\t\t*pValue = false;\n\
\t\tT *pT = this->get_target();\n\
\t\tif (pT == nullptr) return S_FALSE;\n\
{0}\n\
\t\treturn hr;\n\
\t}\n\
";

// {0} = Lower case type name
static const wchar_t* _createPropertyValue_Template =
L"foundation::pv_util::Create{0}Value";

// Temaplate for the OnGetValue array case when the property is a raw container.
// {0} = Model type name camel case
// {1} = Property name camel case
// {2} = Property type declaration
// {3} = Case Invoker
// {4} = Property Type
static const wchar_t* _arrayRawTypeGetPropertyCaseStatement_Template =
L"\t\t\tcase {0}::Property_{1}:\n\
\t\t\t{\n\
\t\t\t\t{2}\n\
\t\t\t\tIFR_ASSERT(this->GetOuter()->{3}(value.GetSizeAddressOf(), value.GetBufferAddressOf()));\n\
\t\t\t\t*ppValue = foundation::CreateValue(value.GetSize(), ({4})value.GetBuffer()).Detach();\n\
\t\t\t\treturn S_OK; \n\
\t\t\t}";


// {0} = Model type name camel case
// {1} = Property name camel case
// {2} = Property type declaration
// {3} = Case Invoker
// {4} = Property Type
static const wchar_t* _arrayTypeSetPropertyCaseStatement_Template =
L"\t\t\tcase {0}::Property_{1}:\n\
\t\t\t{\n\
\t\t\t\t{2}\n\
\t\t\t\tIFR_ASSERT(foundation::pv_util::GetValue(pValue, value.GetSizeAddressOf(), value.GetBufferAddressOf()));\n\
\t\t\t\treturn this->GetOuter()->{3}(value.GetSize(), ({4})value.GetBuffer());\n\
\t\t\t}";

// Temaplate for the OnGetValue case when the property is a raw container.
// {0} = Model type name camel casefget
// {1} = Property name camel case
// {2} = Property type declaration
// {3} = Property name lower case
// {4} = inspectable conversion soure code
// {5} = address of local property var
static const wchar_t* _rawTypeGetPropertyCaseStatement_Template =
L"\t\t\tcase {0}::Property_{1}:\n\
\t\t\t{\n\
\t\t\t\t{2} value;\n\
\t\t\t\tIFR_ASSERT(this->GetOuter()->{3}({5}));\n\
\t\t\t\treturn {4}; \n\
\t\t\t}";

// Temaplate for the OnSetValue case when the property is a raw container.
// {0} = Model type name camel case
// {1} = Property name camel case
// {2} = Property var type declaration
// {3} = Property name lower case
// {4} = address of local property var
static const wchar_t* _rawTypeSetPropertyCaseStatement_Template =
L"\t\t\tcase {0}::Property_{1}:\n\
\t\t\t{\n\
\t\t\t\t{2} value;\n\
\t\t\t\tIFR_ASSERT(foundation::pv_util::GetValue(pValue, {4}));\n\
\t\t\t\treturn this->GetOuter()->{3}(value);\n\
\t\t\t}\n";

// Temaplate for the Get/Set property
// {0} = Type name camel case
// {1} = Property name camel case
// {2} = Impl Get/Set
static const wchar_t* _objectGetSetPropertyCaseStatement_Template =
L"\t\t\tcase {0}::Property_{1}:\n\
\t\t\t{\n\
{2}\n\
\t\t\t}";

// Template for the GetObject
// {0} = Get Invoker
// {1} = Property type name
static const wchar_t* _implGetObject_Template =
L"\
\t\t\t\treturn this->GetOuter()->{0}(({1} **)ppValue);\
";

// Template for the SetObject
// {0} = Set Invoker
// {1} = Property type name
static const wchar_t* _implSetObject_Template =
L"\
\t\t\t\treturn this->GetOuter()->{0}(({1} *)pValue);\n\
";

CGenerateCppLibrary::CGenerateCppLibrary() :
    _useAsyncOperationClassPtr(true)
{
}

void CGenerateCppLibrary::emitTypeVarDeclare(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    const wchar_t *pVarName)
{
    if (pPropertyTypeInfo->getModelType())
    {
        if (pPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum)
        {
            // Enum Type Info generation
            this->emitFullName(os, pPropertyTypeInfo->getModelType());
        }
        else
        {
            os << "foundation::ComPtr<";
            emitCoreModelTypeInfo(os, pPropertyTypeInfo->getModelType()->getModelType());
            os << ">";
        }
    }
    else
    {
        if (pPropertyTypeInfo->getPropertyType() & 0x400)
        {
            emitDeclareArrayWrapperType(os, pPropertyTypeInfo);
        }
        else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Inspectable)
        {
            os << "foundation::InspectablePtr";
        }
        else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_String)
        {
            os << "foundation::HStringPtr";
        }
        else
        {
            os << toCoreType(pPropertyTypeInfo->getPropertyType());
        }
    }
    os << " " << pVarName << ";" << std::endl;
}

void CGenerateCppLibrary::emitTypeVarAddress(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    const wchar_t *pVarName)
{
    if (pPropertyTypeInfo->getPropertyType() & 0x400)
    {
        os << pVarName << ".GetSizeAddressOf(), " << pVarName << ".GetBufferAddressOf()";
        return;
    }

    const foundation::PropertyType propType = pPropertyTypeInfo->getPropertyType();

    bool isSmartPtr =
        (pPropertyTypeInfo->getModelType() && pPropertyTypeInfo->getModelType()->getModelType() != schema::ModelTypeInfoType_Enum)
        || propType == foundation::PropertyType_Inspectable
        || propType == foundation::PropertyType_String;
    if (!isSmartPtr)
    {
        os << "&";
    }
    os << pVarName;
    if (isSmartPtr)
    {
        os << ".GetAddressOf()";
    }
}

void CGenerateCppLibrary::emitDelegateTypeVarDeclare(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    const wchar_t *pVarName)
{
    const schema::CTypeInfo *pModelTypeInfo = pPropertyTypeInfo->getModelType();
    schema::ModelTypeInfoType modelType = pModelTypeInfo ?
        pModelTypeInfo->getModelType() : schema::ModelTypeInfoType_None;

    if (modelType == schema::ModelTypeInfoType_ObjectDispatch ||
        modelType == schema::ModelTypeInfoType_ObservableObject ||
        modelType == schema::ModelTypeInfoType_ViewModel)
    {
        std::wstring typeInfoNamespace = this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
        os << FormatW(L"foundation::ComPtr<{0}::{1}> {2};",
            typeInfoNamespace.c_str(),
            pModelTypeInfo->getName().c_str(),
            pVarName,
            nullptr) << std::endl;
    }
    else
    {
        emitTypeVarDeclare(os, pPropertyTypeInfo, pVarName);
    }
}

void CGenerateCppLibrary::emitDelegateTypeVarAddress(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    const wchar_t *pVarName)
{
    emitTypeVarAddress(os, pPropertyTypeInfo, pVarName);
}

void CGenerateCppLibrary::emitDelegateTypeVarAssign(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    const wchar_t *pVarName,
    const wchar_t *pVarNameIn)
{
    os << "IFR(" << getFoundationNamespace() << "::pv_util::GetValue(" << pVarNameIn << ", ";
    emitDelegateTypeVarAddress(os, pPropertyTypeInfo, pVarName);
    os << "));" << std::endl;
}

void CGenerateCppLibrary::emitDelegateTypeVarReturn(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    const wchar_t *pVarName,
    const wchar_t *pVarNameOut)
{
    UNREFERENCED_PARAMETER(pPropertyTypeInfo);
    os << "*" << pVarNameOut << " = " << getFoundationNamespace() << "::CreateValue(" << pVarName << ").Detach(); " << std::endl;
}

void CGenerateCppLibrary::emitPropertyCommandHandler(
    std::wostream& os,
    const schema::CPropertyInfo*pPropertyInfo,
    const wchar_t *base_class_name,
    const wchar_t *prefix_class_name)
{
    assert(pPropertyInfo->getModelType());
    assert(pPropertyInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Command);

    const schema::CCommandTypeInfo *pCommandModelInfo = (const schema::CCommandTypeInfo *)pPropertyInfo->getModelType();
    assert(pCommandModelInfo);

    foundation::PropertyType paramType = foundation::PropertyType_Empty;
    if (pCommandModelInfo->getParameterTypeInfo() != nullptr)
    {
        paramType = pCommandModelInfo->getParameterTypeInfo()->getPropertyType();
    }

    foundation::PropertyType resultType = foundation::PropertyType_Empty;
    if (pCommandModelInfo->getResultTypeInfo() != nullptr)
    {
        resultType = pCommandModelInfo->getResultTypeInfo()->getPropertyType();
    }

    const bool hasParameter = (paramType != foundation::PropertyType_Empty);
    const bool isArrayParam = ((paramType & 0x400) != 0);
    const bool hasResult = (resultType  != foundation::PropertyType_Empty);
    const bool isArrayResult = ((resultType & 0x400) != 0);
    const bool isAsync = pCommandModelInfo->getIsAsync();
    const bool canExecuteNeeded = pCommandModelInfo->getIsUnknownState();

    std::wstring commandNameCallback = pPropertyInfo->getName();
    if (isAsync)
    {
        commandNameCallback += L"Async";
    }

    std::wostringstream osParameterDeclare;
    if (hasParameter)
    {
        osParameterDeclare << "\t\t";
        emitDelegateTypeVarDeclare(osParameterDeclare, pCommandModelInfo->getParameterTypeInfo(), L"_parameter");
        osParameterDeclare << "\t\t";
        emitDelegateTypeVarAssign(
            osParameterDeclare,
            pCommandModelInfo->getParameterTypeInfo(),
            L"_parameter",
            L"parameter"
            );
    }

    std::wostringstream osExecute;
    std::wostringstream osCanExecute;

    osExecute << osParameterDeclare.str();
    osCanExecute << osParameterDeclare.str();

    if (hasResult && !isAsync)
    {
        osExecute << "\t\t";
        emitDelegateTypeVarDeclare(osExecute,pCommandModelInfo->getResultTypeInfo(),L"_result");
    }
    osExecute << "\t\t";
    osExecute << "hr = pT->Execute" << commandNameCallback << "(";

    osCanExecute << "\t\t";
    osCanExecute << "hr = pT->CanExecute" << commandNameCallback << "(";

    if (hasParameter)
    {
        if (isArrayParam)
        {
            osExecute << "_parameter.size(), _parameter.GetRawBuffer()";
            osCanExecute << "_parameter.size(), _parameter.GetRawBuffer(), ";
        }
        else
        {
            osExecute << "_parameter";
            osCanExecute << "_parameter, ";
        }
    }

    if ((hasResult || isAsync) && hasParameter)
    {
        osExecute << ", ";
    }

    if (isAsync)
    {
        osExecute << "reinterpret_cast<" << getFoundationNamespace() << "::library::IAsyncOperationClass*>(*ppResult)";
    }
    else if (hasResult)
    {
        emitDelegateTypeVarAddress(osExecute,pCommandModelInfo->getResultTypeInfo(),L"_result");
    }

    osExecute << ");" << std::endl;
    osCanExecute << "pValue);" << std::endl;

    if (hasResult && !isAsync)
    {
        osExecute << "\t\t";
        osExecute << "if (SUCCEEDED(hr))" << std::endl;
        osExecute << "\t\t{" << std::endl << "\t\t\t";
        emitDelegateTypeVarReturn(osExecute, pCommandModelInfo->getResultTypeInfo(), L"_result", L"ppResult");
        osExecute << "\t\t}" << std::endl;
    }

    std::wstring parentTypeInfoName = toTypeInfoName(pPropertyInfo->getParent().getName());

    std::wstring parentTypeInfoNameCodeGen = this->toCodeGenTypeName(parentTypeInfoName);
    std::wstring propertyNameCodeGen = this->toCodeGenTypeName(pPropertyInfo->getName());

    std::wstring commandModelType;
    if (!hasParameter && !hasResult)
    {
        commandModelType = FormatW(
            isAsync ? L"{0}::IID_ISimpleAsyncCommand" : L"{0}::IID_ISimpleCommand",
            this->getCoreNamespace(),
            nullptr);
    }
    else if (isObjectObjectCommand(pCommandModelInfo))
    {
        commandModelType = FormatW(
            isAsync ? L"{0}::IID_IAsyncCommand" : L"{0}::IID_ICommandModel",
            this->getCoreNamespace(),
            nullptr);
    }
    else
    {
        commandModelType = FormatW(
            L"{0}::{1}::IIDType",
            this->getNamespace().data(),
            getModelTypeInfoIncName(pCommandModelInfo).c_str(),
            nullptr);
    }

    os << FormatW(_commandInternalTemplate_Template,
        parentTypeInfoNameCodeGen.c_str(),
        propertyNameCodeGen.c_str(),
        getCoreNamespace(),
        commandModelType.data(),
        osExecute.str().data(),
        canExecuteNeeded ? 
            FormatW(_commandCanExecuteInternalTemplate_Template, 
                    osCanExecute.str().c_str(), 
                    hasParameter ? L"_In_" : L"_In_opt_",
                    nullptr).c_str() :
                    L"",
        prefix_class_name,
        hasParameter ? L"_In_" : L"_In_opt_",
        base_class_name,
        nullptr);
}

std::wstring CGenerateCppLibrary::emitPropertyUtilCreateMethod(
    const schema::CPropertyInfo*pPropertyInfo)
{
    foundation::PropertyType propertyType = pPropertyInfo->getPropertyType();
    std::wstring type = L"";

    switch (propertyType & 0xff)
    {
    case foundation::PropertyType::PropertyType_UInt8:
        type = L"UInt8";
        break;
    case foundation::PropertyType::PropertyType_Int16:
        type = L"Int16";
        break;
    case foundation::PropertyType::PropertyType_UInt16:
        type = L"UInt16";
        break;
    case foundation::PropertyType::PropertyType_Int32:
        type = L"Int32";
        break;
    case foundation::PropertyType::PropertyType_UInt32:
        type = L"UInt32";
        break;
    case foundation::PropertyType::PropertyType_Int64:
        type = L"Int64";
        break;
    case foundation::PropertyType::PropertyType_UInt64:
        type = L"UInt64";
        break;
    case foundation::PropertyType::PropertyType_Single:
        type = L"Single";
        break;
    case foundation::PropertyType::PropertyType_Double:
        type = L"Double";
        break;
    case foundation::PropertyType::PropertyType_Char16:
        type = L"Char16";
        break;
    case foundation::PropertyType::PropertyType_Boolean:
        type = L"Boolean";
        break;
    case foundation::PropertyType::PropertyType_String:
        type = L"String";
        break;
    case foundation::PropertyType::PropertyType_Inspectable:
        type = L"Inspectable";
        break;
    case foundation::PropertyType::PropertyType_DateTime:
        type = L"DateTime";
        break;
    case foundation::PropertyType::PropertyType_TimeSpan:
        assert(false);
        break;
    case foundation::PropertyType::PropertyType_Guid:
        type = L"Guid";
        break;
    case foundation::PropertyType::PropertyType_Point:
    case foundation::PropertyType::PropertyType_Size:
    case foundation::PropertyType::PropertyType_Rect:
    default:
        assert(false);
    }

    if (propertyType & 0x400)
    {
        type.append(L"Array");
    }
    return type;
}

std::wstring CGenerateCppLibrary::toNativePropertyType(
    foundation::PropertyType propertyType)
{
    std::wstring type = L"";
    std::wstring ptrIndirection = L"*";

    switch (propertyType & 0xff)
    {
    case foundation::PropertyType::PropertyType_UInt8:
        type = L"BYTE";
        break;
    case foundation::PropertyType::PropertyType_Int16:
        type = L"INT16";
        break;
    case foundation::PropertyType::PropertyType_UInt16:
        type = L"UINT16";
        break;
    case foundation::PropertyType::PropertyType_Int32:
        type = L"INT32";
        break;
    case foundation::PropertyType::PropertyType_UInt32:
        type = L"UINT32";
        break;
    case foundation::PropertyType::PropertyType_Int64:
        type = L"INT64";
        break;
    case foundation::PropertyType::PropertyType_UInt64:
        type = L"UINT64";
        break;
    case foundation::PropertyType::PropertyType_Single:
        type = L"FLOAT";
        break;
    case foundation::PropertyType::PropertyType_Double:
        type = L"DOUBLE";
        break;
    case foundation::PropertyType::PropertyType_Char16:
        type = L"WCHAR";
        break;
    case foundation::PropertyType::PropertyType_Boolean:
        type = L"bool";
        break;
    case foundation::PropertyType::PropertyType_String:
        type = L"HSTRING";
        break;
    case foundation::PropertyType::PropertyType_Inspectable:
        ptrIndirection = L"**";
        type = L"foundation::IInspectable";
        break;
    case foundation::PropertyType::PropertyType_DateTime:
        type = L"foundation::DateTime";
        break;
    case foundation::PropertyType::PropertyType_TimeSpan:
        type = L"foundation::TimeSpan";
        break;
    case foundation::PropertyType::PropertyType_Guid:
        type = L"GUID";
        break;
    case foundation::PropertyType::PropertyType_Point:
    case foundation::PropertyType::PropertyType_Size:
    case foundation::PropertyType::PropertyType_Rect:
    default:
        assert(false);
    }
    return toPropertyType(propertyType, type.c_str(), ptrIndirection.c_str());
}

std::wstring CGenerateCppLibrary::toNativePropertyType(const schema::CPropertyInfo *pPropertyInfo)
{
    std::wostringstream enumType;
    if (pPropertyInfo->getModelType() &&
        pPropertyInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum)
    {
        // Enum Type Info generation
        this->emitFullName(enumType, pPropertyInfo->getModelType());
        if (isPropertyTypeInfoArray(pPropertyInfo))
        {
            enumType << " *";
        }
        return enumType.str();
    }

    foundation::PropertyType propType = pPropertyInfo->getPropertyType();
    return toNativePropertyType(propType);
}

void CGenerateCppLibrary::emitGetSetValuePropertyCases(
    std::wostream *pGetValueCases,
    std::wostream *pSetValueCases,
    const schema::CPropertyInfo*pPropertyInfo)
{
    bool isEnum = (pPropertyInfo->getFlags() & PropertyFlagType_IsEnum) != 0;
    foundation::PropertyType propType = pPropertyInfo->getPropertyType();

    std::wstring parentInfoName = toTypeInfoName(pPropertyInfo->getParent().getName());

    std::wstring getPropertyCaseInvoker = getPropertyCaseInvoke(pPropertyInfo, true);
    std::wstring setPropertyCaseInvoker = getPropertyCaseInvoke(pPropertyInfo, false);

    std::wstring propertyModelTypeName = toNativePropertyType(pPropertyInfo);
    std::wstring propertyInfoName = pPropertyInfo->getName();

    if (pPropertyInfo->getPropertyType() & 0x400) // if Array type case
    {

        std::wstring propertyUtilCreateMethod = FormatW(
            _createPropertyValue_Template,
            emitPropertyUtilCreateMethod(pPropertyInfo).data(),
            nullptr);
        std::wostringstream typeDeclare;
        emitArrayWrapperType(typeDeclare, pPropertyInfo, L"value");

        if (pGetValueCases != nullptr)
        {
            *pGetValueCases << FormatW(_arrayRawTypeGetPropertyCaseStatement_Template,
                parentInfoName.data(),
                propertyInfoName.data(),
                typeDeclare.str().data(),
                getPropertyCaseInvoker.data(),
                propertyModelTypeName.data(),
                nullptr) << std::endl;
        }

        if (pSetValueCases != nullptr &&  !pPropertyInfo->isParent())
        {
            std::wostringstream typeDeclareSet;
            emitPropertyTypeInfo(typeDeclareSet, pPropertyInfo);
            *pSetValueCases << FormatW(_arrayTypeSetPropertyCaseStatement_Template,
                parentInfoName.data(),
                propertyInfoName.data(),
                typeDeclare.str().data(),
                setPropertyCaseInvoker.data(),
                typeDeclareSet.str().data(),
                nullptr) << std::endl;
        }

    }
    // generate the get value cases
    else if (propType != foundation::PropertyType_Inspectable)
    {
        std::wstring returnAsInspectable;

        if (isEnum)
        {
            returnAsInspectable = FormatW(L"{0}::GetEnumValue({1}::{2}::TypeId,(UINT32)value,reinterpret_cast<{0}::IEnumValue **>(ppValue))",
                getFoundationNamespace(),
                this->getNamespaceLookup(pPropertyInfo->getModelType()).c_str(),
                toTypeInfoName((const schema::CEnumTypeInfo *)pPropertyInfo->getModelType()).c_str(),
                nullptr);

        }
        else
        {
            returnAsInspectable = FormatW(
                L"{0}(value, ppValue)",
                FormatW(_createPropertyValue_Template, emitPropertyUtilCreateMethod(pPropertyInfo).data(), nullptr).c_str(),
                nullptr);
        }

        std::wstring varPropertyTypeName =
            propType == foundation::PropertyType_String ?
            L"foundation::HStringPtr" : propertyModelTypeName;
        std::wstring varAddressOfPropertyTypeName =
            propType == foundation::PropertyType_String ?
            L"value.GetAddressOf()" : L"&value";

        if (pGetValueCases != nullptr)
        {
            *pGetValueCases << FormatW(_rawTypeGetPropertyCaseStatement_Template,
                parentInfoName.data(),
                propertyInfoName.data(),
                varPropertyTypeName.data(),
                getPropertyCaseInvoker.data(),
                returnAsInspectable.data(),
                varAddressOfPropertyTypeName.data(),
                nullptr) << std::endl;
        }
        if (pSetValueCases != nullptr && !pPropertyInfo->isParent())
        {
            *pSetValueCases << FormatW(_rawTypeSetPropertyCaseStatement_Template,
                parentInfoName.data(),
                propertyInfoName.data(),
                varPropertyTypeName.data(),
                setPropertyCaseInvoker.data(),
                varAddressOfPropertyTypeName.data(),
                nullptr) << std::endl;
        }
    }
    else // Object case
    {
		if (pPropertyInfo->getModelType())
		{
			propertyModelTypeName = toPropertyTypeName(pPropertyInfo);
		}

		if (pGetValueCases != nullptr)
		{
			*pGetValueCases << FormatW(_objectGetSetPropertyCaseStatement_Template,
				parentInfoName.data(),
				propertyInfoName.data(),
				getPropertyCaseObjectInvokeImpl(pPropertyInfo, propertyModelTypeName, true).data(),
				nullptr) << std::endl;
		}
		if (pSetValueCases != nullptr && !pPropertyInfo->isParent())
		{
			*pSetValueCases << FormatW(_objectGetSetPropertyCaseStatement_Template,
				parentInfoName.data(),
				propertyInfoName.data(),
				getPropertyCaseObjectInvokeImpl(pPropertyInfo, propertyModelTypeName, false).data(),
				nullptr) << std::endl;
		}
	}
}

std::wstring CGenerateCppLibrary::getPropertyCaseInvoke(const schema::CPropertyInfo*pPropertyInfo, bool isGet)
{
    return (isGet ? L"Get":L"Set") + pPropertyInfo->getName();
}

std::wstring CGenerateCppLibrary::getPropertyCaseObjectInvokeImpl(
	const schema::CPropertyInfo*pPropertyInfo,
	const std::wstring& propertyModelTypeName,
	bool isGet)
{
	return FormatW(isGet ? _implGetObject_Template : _implSetObject_Template,
		getPropertyCaseInvoke(pPropertyInfo, isGet).data(),
		propertyModelTypeName.data(),
		nullptr);
}


void CGenerateCppLibrary::emitDeclarePropertyTypeInfo(
	std::wostream& os,
	const schema::CPropertyTypeInfo*pPropertyTypeInfo,
	bool isOutput,
	int parameterIndex)
{
	if (pPropertyTypeInfo->getPropertyType() & 0x400)
	{
		os << "UINT32 ";
		if (isOutput)
		{
			os << "*";
		}
		os << "size";
		if (parameterIndex)
		{
			os << "_" << parameterIndex;
		}
		os << ",";
	}
	emitWrapperPropertyTypeInfo(os, pPropertyTypeInfo, isOutput);
}

void CGenerateCppLibrary::emitWrapperPropertyTypeInfo(
	std::wostream& os,
	const schema::CPropertyTypeInfo*pPropertyTypeInfo,
	bool isOutput)
{
	if (pPropertyTypeInfo->getModelType())
	{
		const schema::CTypeInfo *pModelTypeInfo = pPropertyTypeInfo->getModelType();

		if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Enum)
		{
			// Enum Type Info generation
			this->emitFullName(os, pModelTypeInfo);
			if (isOutput)
			{
				os << "*";
			}
		}
		else
		{
			os << toPropertyTypeName(pPropertyTypeInfo);
			os << " *";
			if (isOutput)
			{
				os << "*";
			}
		}
		// if array add indirection
		if (pPropertyTypeInfo->getPropertyType() & 0x400)
		{
			os << "*";
		}
	}
	else
	{
		os << toCoreType(pPropertyTypeInfo->getPropertyType());
		if (isOutput)
		{
			os << "*";
		}
	}
}

void CGenerateCppLibrary::emitModelPropertyTypeInfo(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    bool isOutput,
    int parameterIndex)
{
    const schema::CTypeInfo *pModelTypeInfo = pPropertyTypeInfo->getModelType();
    schema::ModelTypeInfoType modelType = pModelTypeInfo ?
        pModelTypeInfo->getModelType() : schema::ModelTypeInfoType_None;

    if (!isPropertyTypeInfoArray(pPropertyTypeInfo) &&
        (modelType == schema::ModelTypeInfoType_ObjectDispatch ||
            modelType == schema::ModelTypeInfoType_ObservableObject ||
            modelType == schema::ModelTypeInfoType_ViewModel))
    {
        std::wstring typeInfoNamespace = this->getNativeNamespaceLookup(pModelTypeInfo->getParent().getNamespace());

        os << FormatW(L"{0}::{1} *",
            typeInfoNamespace.c_str(),
            pModelTypeInfo->getName().c_str(),
            nullptr);
        if (isOutput)
        {
            os << "*";
        }

    }
    else
    {
        emitDeclarePropertyTypeInfo(os, pPropertyTypeInfo, isOutput, parameterIndex);
    }
}

void CGenerateCppLibrary::emitAsyncOperationClassDeclare(std::wostream& os)
{
    if (_useAsyncOperationClassPtr)
    {
        os << "const ";
    }
    os << getFoundationNamespace() << "::";
    if (_useAsyncOperationClassPtr)
    {
        os << "AsyncOperationClassPtr& AsyncOperationClassPtr";
    }
    else
    {
        os << "library::IAsyncOperationClass *pAsyncOperation";
    }
}

bool CGenerateCppLibrary::isUseWrapperDelegatePropertyTypeInfo(const schema::CPropertyTypeInfo* pPropertyTypeInfo)
{
    const schema::CTypeInfo *pModelTypeInfo = pPropertyTypeInfo->getModelType();
    schema::ModelTypeInfoType modelType = pModelTypeInfo ?
        pModelTypeInfo->getModelType() : schema::ModelTypeInfoType_None;

    return (
        modelType == schema::ModelTypeInfoType_ObjectDispatch ||
        modelType == schema::ModelTypeInfoType_ObservableObject ||
        modelType == schema::ModelTypeInfoType_ViewModel);
}

void CGenerateCppLibrary::emitMethodHandler(
    std::wostream& osMethodHandler,
    std::wostream& osAbstractMethods,
    const schema::CMethodInfo*pMethodInfo,
    bool isPublic)
{
    bool hasResult = pMethodInfo->getResultType() && pMethodInfo->getResultType()->getPropertyType() != foundation::PropertyType_Empty;

    osMethodHandler << "\t\t\tcase " << toTypeInfoName(pMethodInfo->getParent().getName()) << "::Method_" << pMethodInfo->getName() << ":" << std::endl;
    osMethodHandler << "\t\t\t{" << std::endl;
    // Emit macro parameters check
    osMethodHandler << "\t\t\t\t";
    if (pMethodInfo->getParameters().size() == 0)
    {
        osMethodHandler << "IFCEXPECT(parameters==nullptr || size==0);";
    }
    else
    {
        osMethodHandler << "IFCEXPECT(size==" << pMethodInfo->getParameters().size() << ");";
    }
    osMethodHandler << std::endl;
    // Emit result holder
    if (hasResult && !pMethodInfo->getIsAsync())
    {
        osMethodHandler << "\t\t\t\t";
        emitModelTypeVarDeclare(osMethodHandler, pMethodInfo->getResultType(), L"result");
    }

    std::wstring invokeInstance;
    std::wstring methodNameCallback = this->getMethodCallback(pMethodInfo, invokeInstance);

    osAbstractMethods << "\tvirtual HRESULT " << methodNameCallback << "(";

    std::wostringstream osClassMethodInvoke;
    osClassMethodInvoke << "\t\t\t\t";
    osClassMethodInvoke << "hr = " << invokeInstance << "->" << methodNameCallback << "(";

    int parameterIndex = 1;
    for (schema::_MethodParamIteratorType::const_iterator iterParam = pMethodInfo->getParameters().begin();
    iterParam != pMethodInfo->getParameters().end();
        ++iterParam)
    {
        bool isParameterArray = ((*iterParam)->getPropertyType() & 0x400) != 0;
        // Emit Virtual abstract params
        if (iterParam != pMethodInfo->getParameters().begin())
        {
            osAbstractMethods << ",";
            osClassMethodInvoke << ",";
        }
        emitModelPropertyTypeInfo(osAbstractMethods, (*iterParam), false, parameterIndex);

        osAbstractMethods << " " << (*iterParam)->getParameterName();

        // Emit Var declares
        osMethodHandler << "\t\t\t\t";
        std::wstring varName = L"_" + (*iterParam)->getParameterName();
        emitModelTypeVarDeclare(osMethodHandler, (*iterParam), varName.data());
        osMethodHandler << std::endl;
        osMethodHandler << "\t\t\t\t";

        osMethodHandler << "IFHR(" << getFoundationNamespace() << "::pv_util::GetValue(parameters[";
        osMethodHandler << (int)(iterParam - pMethodInfo->getParameters().begin());
        osMethodHandler << "],";
        emitDelegateTypeVarAddress(osMethodHandler, *iterParam, varName.data());

        osMethodHandler << "));" << std::endl;

        // Add to Class Invoke method
        if (isParameterArray)
        {
            osClassMethodInvoke << varName << ".GetSize(),";
            // use Cast to avoid (const) prefix on GetBuffer
            osClassMethodInvoke << "(";
            emitPropertyTypeInfo(osClassMethodInvoke, (*iterParam));
            osClassMethodInvoke << ")";
        }
        osClassMethodInvoke << varName;
        if (isParameterArray)
        {
            osClassMethodInvoke << ".GetBuffer()";
        }
        ++parameterIndex;
    }

    if ((hasResult || pMethodInfo->getIsAsync()) && pMethodInfo->getParameters().size())
    {
        osAbstractMethods << ",";
        osClassMethodInvoke << ",";
    }

    bool isResultArray = (pMethodInfo->getResultType()->getPropertyType() & 0x400) != 0;

    if (pMethodInfo->getIsAsync())
    {
        if (isPublic)
        {
            auto asyncResultType = toPropertyTypeInfoAsync(pMethodInfo->getResultType(), true);
            osAbstractMethods << asyncResultType << "**ppResult";
            osClassMethodInvoke << "reinterpret_cast<" << asyncResultType << "**>(*ppResult)";
        }
        else
        {
            emitAsyncOperationClassDeclare(osAbstractMethods);
            osClassMethodInvoke << "reinterpret_cast<" << getFoundationNamespace() << "::library::IAsyncOperationClass *>(*ppResult)";
        }
    }
    else if (hasResult)
    {

        emitModelPropertyTypeInfo(osAbstractMethods, pMethodInfo->getResultType(), true);

        osAbstractMethods << " pResult";

        emitDelegateTypeVarAddress(osClassMethodInvoke, pMethodInfo->getResultType(), L"result");
    }

    osAbstractMethods << ")\n"
        << "\t{\n"
        << "\t\tfoundation_assert(false);\n"
        << "\t\treturn E_NOTIMPL;\n"
        << "\t}\n" << std::endl;

    osClassMethodInvoke << ");" << std::endl;

    osMethodHandler << osClassMethodInvoke.str().data();
    if (hasResult && !pMethodInfo->getIsAsync())
    {

        osMethodHandler << "\t\t\t\t";
        osMethodHandler << "if(SUCCEEDED(hr))" << std::endl;
        osMethodHandler << "\t\t\t\t";
        osMethodHandler << "{" << std::endl;
        osMethodHandler << "\t\t\t\t\t";

        if (isUseWrapperDelegatePropertyTypeInfo(pMethodInfo->getResultType()))
        {
            if (isResultArray)
            {
                osMethodHandler << "*ppResult = foundation::CreateValue(result).Detach();" << std::endl;
            }
            else
            {
                osMethodHandler << "*ppResult = result.Detach();" << std::endl;
            }
        }
        else
        {
            osMethodHandler << "*ppResult = " << getFoundationNamespace() << "::CreateValue(";
            if (isResultArray)
            {
                osMethodHandler << "result.GetSize(),";
                // use Cast to avoid (const) prefix on GetBuffer
                osMethodHandler << "(";
                emitPropertyTypeInfo(osMethodHandler, (pMethodInfo->getResultType()));
                osMethodHandler << ")";
                osMethodHandler << "result.GetBuffer()";
            }
            else
            {
                osMethodHandler << "result";
            }
            osMethodHandler << ").Detach();" << std::endl;
        }
        osMethodHandler << "\t\t\t\t";
        osMethodHandler << "}" << std::endl;
    }

    osMethodHandler << "\t\t\t}" << std::endl;
    osMethodHandler << "\t\t\tbreak;" << std::endl;
}
