/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateTypeInfo.cpp
****/

#include "GenerateTypeInfo.h"
#include "guidhelper.h"

// Format template for Model Inc Header File
// {0} = API
const wchar_t *_modelInc_Visibility_Template = 
L"\n\
#if defined(_WINDOWS) || defined __CYGWIN__ \n\
\n\
#if defined({0}_EXPORT) \n\
#define {0} __declspec(dllexport) \n\
#elif defined({0}_IMPORT) \n\
#define {0} __declspec(dllimport) \n\
#else \n\
#define {0}  \n\
#endif \n\
\n\
#else \n\
#define {0} __attribute__ ((visibility (\"default\"))) \n\
#endif \n\
\n\
";

// {0} = API
// {1} = Register Entry Point
const wchar_t *_modelInc_Register_Declare_Template =
L"\n\
\t// Register Model Types entry point\n\
\t{0} HRESULT STDAPICALLTYPE {1}(void);\n\
";

const wchar_t *_pmodHeadersInc_Template =
L"\n\
#include <pmod/interfaces.h> \n\
#include <pmod/type_info_declare.h> \n\
";

const wchar_t *_enumTypesInc_Template =
L"\n\
#include <foundation/pal.h> \n\
namespace foundation {\n\
struct EnumTypeInfo;\n\
}\n\
";

// {0} = A name of the dedicated enums file
const wchar_t *_enumsHeaderInc_Template =
L"\n\
#if !defined(DO_NOT_INCLUDE_GENERATED_ENUMS)\n\
#include \"{0}\"\n\
#endif\n\
";


// Format template for Model Inc Header File
// {0} = Namespace Declare
// {1} = Header content
// {2} = Namespace End
const wchar_t *_enumsInc_Template =
L"\
#pragma once\n\
\n\
{0}\n\
{1}\n\
{2}\n\
";

const wchar_t *_enumValueUniqueInc_Template =
L"\
    const UINT32 {0}_{1}_Id = 0x{2};\n\
";

// Format template for Model Inc Header File
// {0} = Visibility API
// {1} = Namespace Declare
// {2} = register declare
// {3} = Header content
// {4} = Header include
// {5} = Namespace End
const wchar_t *_typeInfoInc_Template =
L"\n\
#pragma once\n\
\n\
{4}\n\
{0}\n\
{1}\n\
{2}\n\
{3}\n\
{5}\n\
";

// Format template for Model Src File
// {0} = Model Inc Header
// {1} = Foundation Namespace
// {2} = Type Info Data
// {3} = Type Info Entries
// {4} = Model Inc Files
// {5} = Namespace prefix
// {6} = Register Entry Point
// {7} = Model namespace
const wchar_t *_typeInfoSrc_Template =
L"\n\
#include \"{0}\" \n\
#include <pmod/library/type_info_factory_util.h>\n\
#include <foundation/library/foundation_library.h>\n\
#include <pmod/library/model_library.h>\n\
\n\
{4}\n\
\n\
{2}\n\
\n\
static const {1}::TypeInfo* _typeInfoEntries [] = {\n\
{3}\n\
    nullptr\n\
};\n\
\n\
HRESULT  STDAPICALLTYPE {5}{6}(void)\n\
{\n\
    return {7}::library::RegisterMultipleTypeInfos(_typeInfoEntries);\n\
}\n\
";

const wchar_t *_typeInfoIIDsSrc_Template =
L"\n\
#include \"{0}\" \n\
\n\
{1}\n\
";

// Format template for Enum Info header
// {0} = Name
// {1} = API
// {2} = Foundation Namespace
// {3} = Type Id
// {4} = Guid str
const wchar_t *_enumInfoHeaderTemplate =
L"\
\t// uuid({4})\n\
\tstruct {0}\n\
\t{\n\
\t\tstatic const UINT32 TypeId = 0x{3};\n\
\t\tstatic {1} const IID IIDType;\n\
\t\tstatic {1} const {2}::EnumTypeInfo EnumTypeInfo;\n\
\t\tstatic const LPCSTR_t Names[];\n\
\t\tstatic const UINT32  Values[];\n\
\t};\n\
\n\
";

// Format template for Enum Info Source 
// {0} = Name
// {1} = Full Name
// {2} = Namespace
// {3} = Foundation Namespace
// {4} = true/false is Enum is Flags
// {5} = BaseType
const wchar_t *_enumInfoSourceTemplate =
L"\n\
const {3}::EnumTypeInfo {2}::{0}::EnumTypeInfo(\n\
\tTypeId,\n\
\tIIDType,\n\
\tU(\"{1}\"),\n\
\t{4},\n\
\t&Names[0],\n\
\t&Values[0],\n\
\t{5}\n\
\t);\n\
\n\
";

// Format template for Property Data Info header
// {0} = Name
// {1} = API
// {2} = Namespace
// {3} = Type Id
// {4} = Guid str
// {5} = Content
const wchar_t *_propertyDataInfoHeaderTemplate =
L"\
\t// uuid({4})\n\
\tstruct {0}\n\
\t{\n\
\t\tstatic const UINT32 TypeId = 0x{3};\n\
\t\tstatic {1} const IID IIDType;\n\
\t\tstatic {1} const {2}::ObjectDispatchTypeInfo ObjectDispatchInfo;\n\
\t\tstatic const {2}::PropertyInfo PropertyInfos[];\n\
{5}\n\
{6}\n\
\t};\n\
\n\
";

// Format template for Property Model Info header
// {0} = Name
// {1} = API
// {2} = Core Namespace
// {3} = ObservableObjectTypeInfo type
// {4} = EventInfo support
// {5} = Type Id
// {6} = Content
// {7} = Guid str
// {8} = MethodInfos support
// {9} = Foundation Namespace
const wchar_t *_propertyModelInfoHeaderTemplate =
L"\
\t// uuid({7})\n\
\tstruct {0}\n\
\t{\n\
\t\tstatic const UINT32 TypeId = 0x{5};\n\
\t\tstatic {1} const IID IIDType;\n\
\t\tstatic {1} const {2}::{3} ObservableObjectInfo;\n\
\t\tstatic const {9}::PropertyInfo PropertyInfos[];\n\
{8}\n\
\t\t{4}\n\
{6}\n\
\t};\n\
\n\
";

// Format template for Command Model Info header
// {0} = Name
// {1} = API
// {2} = Core Namespace
// {3} = Type Id
// {4} = Guid str
// {5} = Foundation Namespace
const wchar_t *_commandModelInfoHeaderTemplate =
L"\
\t// uuid({4})\n\
\tstruct {0}\n\
\t{\n\
\t\tstatic const UINT32 TypeId = 0x{3};\n\
\t\tstatic {1} const IID IIDType;\n\
\t\tstatic {1} const {2}::CommandTypeInfo CommandTypeInfo;\n\
\t\tstatic const {5}::PropertyTypeInfo ParameterTypeInfo;\n\
\t\tstatic const {5}::PropertyTypeInfo ResultTypeInfo;\n\
\t};\n\
\n\
";

// Format template for Collection Model Info header
// {0} = Name
// {1} = API
// {2} = Core Namespace
// {3} = Type Id
// {4} = Guid str
// {5} = Foundation Namespace
// {6} = Type Info Name
const wchar_t *_enumerationTypeInfoHeaderTemplate =
L"\
\t// uuid({4})\n\
\tstruct {0}\n\
\t{\n\
\t\tstatic const UINT32 TypeId = 0x{3};\n\
\t\tstatic {1} const IID IIDType;\n\
\t\tstatic {1} const {5}::EnumerableTypeInfo {6}TypeInfo;\n\
\t\tstatic const {5}::PropertyTypeInfo ItemTypeInfo;\n\
\t};\n\
\n\
";

// Format template for Command Model Source 
// {0} = Name
// {1} = Full Name
// {2} = Namespace
// {3} = Core Namespace
// {4} = IsAsync
const wchar_t *_commandModelInfoSourceTemplate = 
L"\n\
const {3}::CommandTypeInfo {2}::{0}::CommandTypeInfo(\n\
\tTypeId,\n\
\tIIDType,\n\
\tU(\"{1}\"),\n\
\t&ParameterTypeInfo,\n\
\t&ResultTypeInfo,\n\
\t{4}\n\
\t);\n\
\n\
";

// Format template for Async Operation Model Info header
// {0} = Name
// {1} = API
// {2} = Foundation Namespace
// {3} = Type Id
// {4} = Guid str
const wchar_t *_asynOperationModelInfoHeaderTemplate =
L"\
\t// uuid({4})\n\
\tstruct {0}\n\
\t{\n\
\t\tstatic const UINT32 TypeId = 0x{3};\n\
\t\tstatic {1} const IID IIDType;\n\
\t\tstatic {1} const {2}::AsyncOperationTypeInfo AsyncOperationInfo;\n\
\t\tstatic const {2}::PropertyTypeInfo ResultTypeInfo;\n\
\t};\n\
\n\
";

// Format template for Collection Model Source 
// {0} = Name
// {1} = Full Name
// {2} = Namespace
// {3} = public API Namespace
// {4} = Type Info name
// {5} = Foundation Namespace
const wchar_t *_enumerationTypeInfoSourceTemplate =
L"\n\
const {5}::EnumerableTypeInfo {2}::{0}::{4}TypeInfo(\n\
\tTypeId,\n\
\tIIDType,\n\
\tU(\"{1}\"),\n\
\t&ItemTypeInfo,\n\
\t{3}::TypeInfo_{4}\n\
\t);\n\
\n\
";

// Format template for Async Operation Model Source 
// {0} = Name
// {1} = Full Name
// {2} = Namespace
// {3} = Foundation Namespace
const wchar_t *_asyncOperationModelInfoSourceTemplate = 
L"\n\
const {3}::AsyncOperationTypeInfo {2}::{0}::AsyncOperationInfo(\n\
\tTypeId,\n\
\tIIDType,\n\
\tU(\"{1}\"),\n\
\t&ResultTypeInfo\n\
\t);\n\
\n\
";

void CGenerateTypeInfo::emitPropertiesInfo(
	const wchar_t *typeInfoName,
    const schema::CObjectDispatchInfo *pPropertyDataInfo)
{
	_osModelSrc << "const " << getFoundationNamespace() << "::PropertyInfo " << this->getNativeNamespace() << "::" << typeInfoName << "::PropertyInfos[] = {" << std::endl;

	// Iterate Properties
	for (auto iter = pPropertyDataInfo->getProperties().begin();
	iter != pPropertyDataInfo->getProperties().end();
		++iter)
	{
		_osModelSrc << "\t" << getFoundationNamespace() << "::PropertyInfo(Property_";
		_osModelSrc << (*iter)->getName() << ",U(\"" << (*iter)->getName() << "\"),";
		_osModelSrc << toPropertyTypeDesc((*iter)->getPropertyType());

		_osModelSrc << ", 0x" << std::hex << (*iter)->getFlags();

		// Get OptionFlags from Schema.        
		UINT32 optionFlags = (*iter)->getOptionFlags();
		if (optionFlags != 0)
		{
			optionFlags = optionFlags << 16;
			_osModelSrc << " + 0x" << optionFlags;
		}

		_osModelSrc << std::dec << ",";

		if ((*iter)->getModelType())
		{
			emitModelTypeInfoRef(_osModelSrc, (*iter)->getModelType());
		}
		else
		{
			_osModelSrc << "nullptr";
		}
		_osModelSrc << ")," << std::endl;
	}

	_osModelSrc << "\t" << getFoundationNamespace() << "::PropertyInfo()" << std::endl;
	_osModelSrc << "};" << std::endl << std::endl;
}

void CGenerateTypeInfo::emitMethodsInfo(
    const std::wstring& typeInfoName,
    const schema::CObjectDispatchInfo *pPropertyDataInfo)
{
    std::wostringstream osMethodInfo;

    osMethodInfo << "const " << getFoundationNamespace() << "::MethodInfo " << this->getNativeNamespace() << "::" << typeInfoName << "::MethodInfos[] = {" << std::endl;
    // Iterate Methods
    for (schema::_MethodIteratorType::const_iterator iter = pPropertyDataInfo->getMethods().begin();
        iter != pPropertyDataInfo->getMethods().end();
        ++iter)
    {

        // generate Param Array
        std::wstring methodParams = typeInfoName + L"_" + (*iter)->getName() + L"_Params";
        _osModelSrc << "static " << getFoundationNamespace() << "::MethodParamInfo " << methodParams << "[] = {" << std::endl;
        for (schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            _osModelSrc << "\t" << getFoundationNamespace() << "::MethodParamInfo(";
            _osModelSrc << "U(\"" << (*iterParam)->getParameterName() << "\"),";
            _osModelSrc << toPropertyTypeDesc((*iterParam)->getPropertyType());
            _osModelSrc << ",";
            if ((*iterParam)->getModelType())
            {
                emitModelTypeInfoRef(_osModelSrc, (*iterParam)->getModelType());
            }
            else
            {
                _osModelSrc << "nullptr";
            }
            _osModelSrc << ")," << std::endl;
        }
        _osModelSrc << "\t" << getFoundationNamespace() << "::MethodParamInfo()" << std::endl;
        _osModelSrc << "};" << std::endl;
        // generate Result Type Info
        std::wstring resultTypeInfoName = typeInfoName + L"_" + (*iter)->getName() + L"_Result";
        emitPropertyTypeInfo(_osModelSrc, L"static", resultTypeInfoName.data(), (*iter)->getResultType());

        // Emit Method Info
        osMethodInfo << "\t" << getFoundationNamespace() << "::MethodInfo(Method_";
        osMethodInfo << (*iter)->getName() << ",U(\"" << (*iter)->getName() << "\"),";
        osMethodInfo << methodParams << ",&" << resultTypeInfoName << ",";
        osMethodInfo << ((*iter)->getIsAsync() ? "true" : "false");
        osMethodInfo << ")," << std::endl;
    }
    osMethodInfo << "\t" << getFoundationNamespace() << "::MethodInfo()" << std::endl;
    osMethodInfo << "};" << std::endl << std::endl;
    _osModelSrc << osMethodInfo.str();
}

void CGenerateTypeInfo::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
    _modelApi = getStringParameter(parameters, L"modelapi", L"");
    _isTypeInfoPrivate = getBoolParamater(parameters, L"private", false) ;
    _generateEnumPrefixValues = getBoolParamater(parameters, L"enumPrefixValues", true);
    _generateEnumUnderscoreValues = getBoolParamater(parameters, L"enumUnderscoreValues",true);
    _generateEnumUniqueValues = getBoolParamater(parameters, L"enumUniqueValues", false);
	_generateEnumClass = getBoolParamater(parameters, L"enumClass", true);
    _isEnumTypes = getBoolParamater(parameters, L"enumTypes", false);
}

void CGenerateTypeInfo::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "modelapi:API keyword if export/import is supported" << std::endl;
    os << "enumsInc: Enums header file" << std::endl;
    os << "modelInc: Type Info header file" << std::endl;
    os << "modelSrc: Type Info source file" << std::endl;
    os << "modelIncReferences: list of Type Info Ref header files" << std::endl;
    os << "enumUnderscoreValues: if generate '_' on enum values" << std::endl;
    os << "enumPrefixValues: if generate prefix name on enum values" << std::endl;
    os << "enumTypes: if only enum types are being defined" << std::endl;
    os << "registerEntryPoint: registration entry point" << std::endl;
}

void CGenerateTypeInfo::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring enumsIncPath = getStringParameter(parameters, L"enumsInc", L"");
    std::wstring modelIncPath = getStringParameter(parameters, L"modelInc", nullptr);
    std::wstring modelSrcPath = getStringParameter(parameters, L"modelSrc", nullptr);
    std::wstring registerEntryPoint = getStringParameter(parameters, L"registerEntryPoint", L"RegisterTypeInfos");

    std::wstring modelIncStr;
    std::wstring includesAndForwards(_isEnumTypes ? _enumTypesInc_Template : _pmodHeadersInc_Template);

    if (!enumsIncPath.empty())
    {
        // generate a separate header for enums
        std::wstring enumsIncContent = FormatW(_enumsInc_Template,
            toNativeNamespaceDeclare(this->getNativeNamespace()).c_str(),
            _osEnumsInc.str().c_str(),
            toNativeNamespaceEnd(this->getNativeNamespace()).c_str(),
            nullptr);

        std::wofstream fileEnumsIncHeader(ToUtf8String(enumsIncPath.data()), std::ios_base::out);
        fileEnumsIncHeader << enumsIncContent;
        fileEnumsIncHeader.close();

        modelIncStr = _osModelInc.str();

        includesAndForwards += FormatW(_enumsHeaderInc_Template, enumsIncPath.substr(enumsIncPath.find_last_of(L":\\/") + 1).c_str(), nullptr);
    }
    else
    {
        // embed the enums into model inc.
        modelIncStr = FormatW(L"{0}\n{1}", _osEnumsInc.str().c_str(), _osModelInc.str().c_str(), nullptr);
    }

    std::wstring modelIncContent = FormatW(_typeInfoInc_Template,
        _isTypeInfoPrivate ? L"" : FormatW(_modelInc_Visibility_Template, _modelApi.data(), nullptr).data(),
        toNativeNamespaceDeclare(this->getNativeNamespace()).c_str(),
        _isTypeInfoPrivate ? L"" : FormatW(_modelInc_Register_Declare_Template, _modelApi.data(), registerEntryPoint.c_str(), nullptr).data(),
        modelIncStr.data(),
        includesAndForwards.c_str(),
        toNativeNamespaceEnd(this->getNativeNamespace()).c_str(),
        nullptr);

    // generate Model Header File
    std::wofstream fileModelIncHeader(ToUtf8String(modelIncPath.data()), std::ios_base::out);
    fileModelIncHeader << modelIncContent;
    fileModelIncHeader.close();

    std::wostringstream osModelIncReferences;
    emitIncludeReferences(osModelIncReferences, parameters, L"modelIncReferences");

    // generate TypeInfo IIds Source File
    std::wstring typeInfoIIDsSrcPath = modelSrcPath;

    size_t pos = typeInfoIIDsSrcPath.find_last_of(L"\\/");
    pos = typeInfoIIDsSrcPath.find(L".",pos);
    if (pos == std::wstring::npos)
    {
        throw std::wstring(L"expected extension");
    }
    typeInfoIIDsSrcPath.insert(pos, L"IIDs");
    std::wstring typeInfoIIDsSrcContent = FormatW(_typeInfoIIDsSrc_Template,
        getFilePath(modelIncPath).data(),   // {0}
        _osTypeInfoIIDsSrc.str().data(),
        nullptr);

    std::wofstream fileTypeInfoIIDs(ToUtf8String(typeInfoIIDsSrcPath.data()), std::ios_base::out);
    fileTypeInfoIIDs << typeInfoIIDsSrcContent;
    fileTypeInfoIIDs.close();

    // generate TypeInfo Source File
    std::wstring modelSrcContent = FormatW(_typeInfoSrc_Template,
        getFilePath(typeInfoIIDsSrcPath).data(),   // {0}
		getFoundationNamespace(),                 // {1}
        _osModelSrc.str().data(),           // {2}
        _osModelTypeInfoEntries.str().data(),   // {3}
        osModelIncReferences.str().data(),      // {4}
        _isTypeInfoPrivate ? L"" : (this->getNativeNamespace() + L"::").data(),                   // {5}
        registerEntryPoint.c_str(),
		getCoreNamespace(),                 // {1}
		nullptr);

    std::wofstream fileModelSrcHeader(ToUtf8String(modelSrcPath.data()),std::ios_base::out);
    fileModelSrcHeader << modelSrcContent;
    fileModelSrcHeader.close();
}

void CGenerateTypeInfo::codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo)
{
    std::wostringstream osEnumsUniqueInc;
    //////////////////////////// Emit Model Header Info ///////////////////////////////////////
    _osEnumsInc << "\tenum " << (_generateEnumClass ? "class ":"") << pEnumTypeInfo->getName().data() << std::endl;

    _osEnumsInc << "\t{" << std::endl;

    for(schema::_EnumInfoVectorType::const_iterator iter = pEnumTypeInfo->getEnums().begin();
        iter != pEnumTypeInfo->getEnums().end();
        ++iter)
    {
        _osEnumsInc << "\t\t";
        if (!_generateEnumClass && _generateEnumPrefixValues)
        {
            _osEnumsInc << pEnumTypeInfo->getName().c_str();
            if (_generateEnumUnderscoreValues)
            {
                _osEnumsInc << '_';
            }
        }
        _osEnumsInc << std::get<0>(*iter) << " = ";

		UINT32 enumValue = std::get<1>(*iter);
		if (_generateEnumClass && (enumValue > 0x80000000))
		{
			_osEnumsInc << "(int)";
		}

        if (std::get<2>(*iter) == schema::hexadecimal)
        {
            _osEnumsInc << "0x" << std::hex;
        }

        _osEnumsInc << enumValue << ',' << std::endl;
        // return back to default dec
        _osEnumsInc << std::dec;

        if (_generateEnumUniqueValues)
        {
            osEnumsUniqueInc << FormatW(_enumValueUniqueInc_Template,
                pEnumTypeInfo->getName().c_str(),
                std::get<0>(*iter).c_str(),
                ToHexString((pEnumTypeInfo->getId() << 16) + std::get<1>(*iter)).c_str(),
                nullptr);
        }
    }

    _osEnumsInc << "\t};" << std::endl;
    _osEnumsInc << std::endl;

    if (_generateEnumUniqueValues)
    {
        _osEnumsInc << osEnumsUniqueInc.str() << std::endl;
    }

	std::wstring typeInfoName = toTypeInfoName(pEnumTypeInfo);
	_osModelInc << FormatW(_enumInfoHeaderTemplate,
		typeInfoName.data(),
		_modelApi.data(),
		getFoundationNamespace(),
        ToHexString(pEnumTypeInfo->getId()).data(),
        ToWideString(pEnumTypeInfo->getType()).c_str(),
		nullptr);

    //////////////////////////// Emit Type Source Info ///////////////////////////////////////
    // generate Source Content
    _osTypeInfoIIDsSrc << "\n\nconst IID " << this->getNativeNamespace() << "::" << typeInfoName << "::IIDType = ";
    formatIIDType(_osTypeInfoIIDsSrc,pEnumTypeInfo->getType());
    _osTypeInfoIIDsSrc << std::endl;

    std::wostringstream osEnumBaseType;
    if (pEnumTypeInfo->getBaseType() != nullptr)
    {
        emitModelTypeInfoRef(osEnumBaseType, pEnumTypeInfo->getBaseType());
    }
    else
    {
        osEnumBaseType << "nullptr";
    }

	_osModelSrc << FormatW(_enumInfoSourceTemplate,
		typeInfoName.data(),
        toNamespaceFullName(getNamespace(), pEnumTypeInfo->getName()).c_str(),
		getNativeNamespace().data(),
		getFoundationNamespace(),
		pEnumTypeInfo->isFlags() ? L"true":L"false",
        osEnumBaseType.str().data(),
		nullptr);
	
    // Iterate for Names
	_osModelSrc << "const LPCSTR_t " << getNativeNamespace() << "::" << typeInfoName << "::Names[] = {"  << std::endl;

    // Generate names
	for(schema::_EnumInfoVectorType::const_iterator iter = pEnumTypeInfo->getEnums().begin();
        iter != pEnumTypeInfo->getEnums().end();
        ++iter)
    {
        _osModelSrc << '\t' << "U(\"" << std::get<0>(*iter) << "\")" << "," << std::endl;
    }

	_osModelSrc << "\t" << "nullptr" << std::endl << "};"  << std::endl;
	// Iterate for Values
	_osModelSrc << "\nconst UINT32 " << getNativeNamespace() << "::" << typeInfoName << "::Values[] = {"  << std::endl;

    // Generate values
	for(schema::_EnumInfoVectorType::const_iterator iter = pEnumTypeInfo->getEnums().begin();
        iter != pEnumTypeInfo->getEnums().end();
        ++iter)
    {
        _osModelSrc << '\t';

        if (std::get<2>(*iter) == schema::hexadecimal)
        {
            _osModelSrc << "0x" << std::hex;
        }

        _osModelSrc << std::get<1>(*iter) << "," << std::endl;
        // return back to default dec
        _osModelSrc << std::dec;
    }

	_osModelSrc << "\t(UINT32)-1\n};"  << std::endl;

	// register type info
	_osModelTypeInfoEntries << "\t&" << this->getNativeNamespace() << "::" << typeInfoName << "::EnumTypeInfo," << std::endl;
}

void CGenerateTypeInfo::codeGenerate(const schema::CObjectDispatchInfo *pPropertyDataInfo)
{
	std::wostringstream osTypeInc;

	// Iterate Properties
	for (schema::_PropertyIteratorType::const_iterator iter = pPropertyDataInfo->getProperties().begin();
	iter != pPropertyDataInfo->getProperties().end();
		++iter)
	{
		osTypeInc << "\t\tstatic const int Property_" << (*iter)->getName() << " = " << (*iter)->getId() << ";" << std::endl;
	}
    // Iterate Methods
    for (schema::_MethodIteratorType::const_iterator iter = pPropertyDataInfo->getMethods().begin();
        iter != pPropertyDataInfo->getMethods().end();
        ++iter)
    {
        osTypeInc << "\t\tstatic const int Method_" << (*iter)->getName() << " = " << (*iter)->getId();
        osTypeInc << ";" << std::endl;
    }

	std::wstring typeInfoName = toTypeInfoName(pPropertyDataInfo->getName());

	std::wstring typeInfoHeader = FormatW(_propertyDataInfoHeaderTemplate,
		typeInfoName.data(),
		_modelApi.data(),
		getFoundationNamespace(),
		ToHexString(pPropertyDataInfo->getId()).data(),
		ToWideString(pPropertyDataInfo->getType()).c_str(),
        pPropertyDataInfo->getMethods().size() ? FormatW(L"\t\tstatic const {0}::MethodInfo  MethodInfos[];", getFoundationNamespace(), nullptr).data() : L"",
		osTypeInc.str().data(),
        nullptr);

	_osModelInc << typeInfoHeader;

	// Iterate Properties
	emitPropertiesInfo(typeInfoName.c_str(), pPropertyDataInfo);

    if (pPropertyDataInfo->getMethods().size())
    {
        emitMethodsInfo(typeInfoName, pPropertyDataInfo);
    }

	//////////////////////////// Emit Model Source Info ///////////////////////////////////////

	// generate Source Content
    _osTypeInfoIIDsSrc << "const IID " << this->getNativeNamespace() << "::" << typeInfoName << "::IIDType = ";
	formatIIDType(_osTypeInfoIIDsSrc, pPropertyDataInfo->getType());
    _osTypeInfoIIDsSrc << std::endl;

	// emit type info
	_osModelSrc << "const " << getFoundationNamespace() << "::ObjectDispatchTypeInfo" << " " << this->getNativeNamespace() << "::" << typeInfoName << "::ObjectDispatchInfo(" << std::endl;
	_osModelSrc << '\t' << "TypeId," << std::endl;
	_osModelSrc << '\t' << "IIDType," << std::endl;
	_osModelSrc << '\t' << "U(\"" << pPropertyDataInfo->getFullName() << "\")," << std::endl;
	_osModelSrc << '\t' << "PropertyInfos," << std::endl;
    _osModelSrc << '\t' << (pPropertyDataInfo->getMethods().size() ? "MethodInfos" : "nullptr") << "," << std::endl;
    _osModelSrc << '\t' << "nullptr";

	_osModelSrc << std::endl;
	_osModelSrc << ");" << std::endl;
	_osModelSrc << std::endl;

}

void CGenerateTypeInfo::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    //////////////////////////// Emit Model Header Info ///////////////////////////////////////
    bool isViewModel = pPropertyModelInfo->getModelType() == schema::ModelTypeInfoType_ViewModel;

    std::wostringstream osTypeInc;

    // Iterate Events
    for(schema::_EventIteratorType::const_iterator iter = pPropertyModelInfo->getEvents().begin();
        iter != pPropertyModelInfo->getEvents().end();
        ++iter)
    {
        osTypeInc << "\t\tstatic const int Event_" << (*iter)->getName() << " = ";
        osTypeInc << (*iter)->getId() << ";" << std::endl;
    }

    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        osTypeInc << "\t\tstatic const int Property_" << (*iter)->getName() << " = ";
        if((*iter)->isParent())
        {
            osTypeInc << getCoreNamespace() << "::Property_Parent";
        }
        else
        {
            osTypeInc << (*iter)->getId();
        }
        if(isViewModel && !(*iter)->isParent())
        {
            osTypeInc << " + 0x1000";
        }
        osTypeInc << ";" << std::endl;
    }
    // Iterate Methods
    for(schema::_MethodIteratorType::const_iterator iter = pPropertyModelInfo->getMethods().begin();
        iter != pPropertyModelInfo->getMethods().end();
        ++iter)
    {
        osTypeInc << "\t\tstatic const int Method_" << (*iter)->getName() << " = " << (*iter)->getId();
        if(isViewModel)
        {
            osTypeInc << " + 0x1000";
        }
        osTypeInc << ";" << std::endl;
    }

    std::wstring typeInfoName = toTypeInfoName(pPropertyModelInfo->getName());

    std::wstring propertyModelInfoStruct = isViewModel ? 
        L"ViewModelTypeInfo":
        L"ObservableObjectTypeInfo";

    std::wstring typeInfoHeader = FormatW(_propertyModelInfoHeaderTemplate,
        typeInfoName.data(),
        _modelApi.data(),
        getCoreNamespace(),
        propertyModelInfoStruct.data(),
        pPropertyModelInfo->getEvents().size() ? FormatW(L"\t\tstatic const {0}::EventInfo EventInfo[];", getCoreNamespace(), nullptr).data() : L"",
        ToHexString(pPropertyModelInfo->getId()).data(),
        osTypeInc.str().data(),
        ToWideString(pPropertyModelInfo->getType()).c_str(),
        pPropertyModelInfo->getMethods().size() ? FormatW(L"\t\tstatic const {0}::MethodInfo  MethodInfos[];", getFoundationNamespace(), nullptr).data() : L"",
		getFoundationNamespace(),
		nullptr);

    _osModelInc << typeInfoHeader;

    //////////////////////////// Emit Model Source Info ///////////////////////////////////////

    // generate Source Content
    _osTypeInfoIIDsSrc << "const IID " << this->getNativeNamespace() << "::" << typeInfoName << "::IIDType = ";
    formatIIDType(_osTypeInfoIIDsSrc,pPropertyModelInfo->getType());
    _osTypeInfoIIDsSrc << std::endl;

    // emit type info
    _osModelSrc << "const " << getCoreNamespace() << "::" << propertyModelInfoStruct << " " << this->getNativeNamespace() << "::" << typeInfoName << "::ObservableObjectInfo(" << std::endl;
    _osModelSrc << '\t' << "TypeId," << std::endl;
    _osModelSrc << '\t' << "IIDType," << std::endl;
    _osModelSrc << '\t' << "U(\"" << pPropertyModelInfo->getFullName() << "\")," << std::endl;
    _osModelSrc << '\t' << "PropertyInfos," << std::endl;
    _osModelSrc << '\t' << (pPropertyModelInfo->getMethods().size() ? "MethodInfos":"nullptr") << "," << std::endl;
    if(pPropertyModelInfo->getEvents().size())
    {
        _osModelSrc << '\t' << "EventInfo," << std::endl;
    }
    else
    {
        _osModelSrc << '\t' << "nullptr," << std::endl;
    }

    if(pPropertyModelInfo->getBaseType())
    {
        _osModelSrc << "\t";
        emitModelTypeInfoRef(_osModelSrc,pPropertyModelInfo->getBaseType());
    }
    else
    {
        _osModelSrc << '\t' << "nullptr";
    }
    if(isViewModel)
    {
        const schema::CPropertyViewModelInfo *pPropertyViewModelInfo = (const schema::CPropertyViewModelInfo *)pPropertyModelInfo;
        _osModelSrc << "," << std::endl;
        _osModelSrc << "\t";
        // native namespace using lookup
        emitModelTypeInfoRef(_osModelSrc,pPropertyViewModelInfo->getSourceType());

        _osModelSrc << "," << std::endl;
        _osModelSrc << '\t' << (pPropertyViewModelInfo->getIsInherited() ? "true":"false");
    }

    _osModelSrc << std::endl;
    _osModelSrc << ");" << std::endl;
    _osModelSrc << std::endl;

    if(pPropertyModelInfo->getEvents().size())
    {
        std::wostringstream osEventInfo;

        osEventInfo << "const " << getCoreNamespace() << "::EventInfo " << this->getNativeNamespace() << "::" << typeInfoName << "::EventInfo[] = {" << std::endl;
        // Iterate Events
        for(schema::_EventIteratorType::const_iterator iter = pPropertyModelInfo->getEvents().begin();
            iter != pPropertyModelInfo->getEvents().end();
            ++iter)
        {
            // generate EventArgs Type Info
            std::wstring eventArgsTypeInfoName = typeInfoName + L"_" + (*iter)->getName() + L"_EventArgs";
            emitPropertyTypeInfo(_osModelSrc,L"static",eventArgsTypeInfoName.data(),(*iter)->getEventArgsType());

            osEventInfo << "\t" << getCoreNamespace() << "::EventInfo(Event_";
            osEventInfo << (*iter)->getName() << ",U(\"" << (*iter)->getName() << "\"),&" << eventArgsTypeInfoName;
            osEventInfo << ")," << std::endl;
        }
        osEventInfo << "\t" << getCoreNamespace() << "::EventInfo()" << std::endl;
        osEventInfo << "};" << std::endl << std::endl;
        
        _osModelSrc << std::endl << osEventInfo.str();

    }

    // Iterate Properties
	emitPropertiesInfo(typeInfoName.c_str(), pPropertyModelInfo);

    if (pPropertyModelInfo->getMethods().size())
    {
        emitMethodsInfo(typeInfoName, pPropertyModelInfo);
    }

    _osModelSrc << std::endl;
}

void CGenerateTypeInfo::codeGenerate(const schema::CEnumerableTypeInfo *pEnumerationTypeInfo)
{
    if(pEnumerationTypeInfo->getItemTypeInfo()->getModelType() == nullptr)
    {
        // the Item Type on the schema is a supported type on the 'Core' library so we don't need to declare it
        return;
    }
    schema::ModelTypeInfoType typeInfoType = pEnumerationTypeInfo->getModelType();

    std::wstring collectionModelInfoNameInc = getModelTypeInfoIncName(pEnumerationTypeInfo);

    std::wstring typeInfoName;
    if (typeInfoType == schema::ModelTypeInfoType_Enumerable)
    {
        typeInfoName = L"Enumerable";
    }
    else if (typeInfoType == schema::ModelTypeInfoType_ObservableCollection)
    {
        typeInfoName = L"Collection";
    }
    else
    {
        typeInfoName = L"List";
    }

    std::wstring typeInfoHeader = FormatW(_enumerationTypeInfoHeaderTemplate,
        collectionModelInfoNameInc.c_str(),
        _modelApi.c_str(),
        getCoreNamespace(),
        ToHexString(pEnumerationTypeInfo->getId()).c_str(),
        ToWideString(pEnumerationTypeInfo->getType()).c_str(),
        getFoundationNamespace(),
        typeInfoName.c_str(),
        nullptr);

    _osModelInc << typeInfoHeader;

    // generate Source Content
    _osTypeInfoIIDsSrc << "const IID " << this->getNativeNamespace() << "::" << collectionModelInfoNameInc << "::IIDType = ";
    formatIIDType(_osTypeInfoIIDsSrc,pEnumerationTypeInfo->getType());
    _osTypeInfoIIDsSrc << std::endl;

    _osModelSrc << FormatW(_enumerationTypeInfoSourceTemplate,
           collectionModelInfoNameInc.data(),
           pEnumerationTypeInfo->getFullName().data(),
           this->getNativeNamespace().data(),
            typeInfoType == schema::ModelTypeInfoType_Enumerable ? getFoundationNamespace():getCoreNamespace(),
           typeInfoName.c_str(),
           getFoundationNamespace(),
           nullptr
           );

    // Emit ItemTypeInfo
    emitPropertyTypeInfo(
        _osModelSrc,
        L"ItemTypeInfo",
        pEnumerationTypeInfo,
        pEnumerationTypeInfo->getItemTypeInfo());
    _osModelSrc<< std::endl;
}

void CGenerateTypeInfo::codeGenerate(const schema::CCommandTypeInfo *pCommandModelInfo)
{
    if (isSimpleCommand(pCommandModelInfo))
    {
        // no need to define type info for Simple Commands:
        // pmod::ISimpleCommand/pmod::ISimpleAsyncCommand
        return;
    }

    if (isObjectObjectCommand(pCommandModelInfo))
    {
        // no need to define type info for object/object commands
        // fallback to pmod::ICommand/pmod::IAsyncCommand
        return;
    }

    std::wstring commandModelInfoNameInc = getModelTypeInfoIncName(pCommandModelInfo);

    std::wstring typeInfoHeader = FormatW(_commandModelInfoHeaderTemplate,
        commandModelInfoNameInc.data(),
        _modelApi.data(),
        getCoreNamespace(),
        ToHexString(pCommandModelInfo->getId()).data(),
        ToWideString(pCommandModelInfo->getType()).c_str(),
        getFoundationNamespace(),
        nullptr);

    _osModelInc << typeInfoHeader;

    // generate Source Content
    _osTypeInfoIIDsSrc << "const IID " << this->getNativeNamespace() << "::" << commandModelInfoNameInc << "::IIDType = ";
    formatIIDType(_osTypeInfoIIDsSrc, pCommandModelInfo->getType());
    _osTypeInfoIIDsSrc << std::endl;

    _osModelSrc << FormatW(_commandModelInfoSourceTemplate,
           commandModelInfoNameInc.data(),
           pCommandModelInfo->getFullName().data(),
           this->getNativeNamespace().data(),
           getCoreNamespace(),
           pCommandModelInfo->getIsAsync() ? L"true" : L"false",
           nullptr);

    // Emit ParameterTypeInfo
    emitPropertyTypeInfo(
        _osModelSrc,
        L"ParameterTypeInfo",
        pCommandModelInfo,
        pCommandModelInfo->getParameterTypeInfo());

    // Emit ResultTypeInfo
    emitPropertyTypeInfo(
        _osModelSrc,
        L"ResultTypeInfo",
        pCommandModelInfo,
        pCommandModelInfo->getResultTypeInfo());

    _osModelSrc<< std::endl;

}

void CGenerateTypeInfo::codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo)
{
    std::wstring ayncOperationModelInfoName = getModelTypeInfoIncName(pAsyncOperationModelInfo);

    std::wstring typeInfoHeader = FormatW(_asynOperationModelInfoHeaderTemplate,
        ayncOperationModelInfoName.data(),
        _modelApi.data(),
        getFoundationNamespace(),
        ToHexString(pAsyncOperationModelInfo->getId()).data(),
        ToWideString(pAsyncOperationModelInfo->getType()).c_str(),
        nullptr);

    _osModelInc << typeInfoHeader;

    // generate Source Content
    _osTypeInfoIIDsSrc << "const IID " << this->getNativeNamespace() << "::" << ayncOperationModelInfoName << "::IIDType = ";
    formatIIDType(_osTypeInfoIIDsSrc,pAsyncOperationModelInfo->getType());
    _osTypeInfoIIDsSrc << std::endl;

    _osModelSrc << FormatW(_asyncOperationModelInfoSourceTemplate,
           ayncOperationModelInfoName.data(),
           pAsyncOperationModelInfo->getFullName().data(),
           this->getNativeNamespace().data(),
           getFoundationNamespace(),
           nullptr
           );

    // Emit ResultTypeInfo
    emitPropertyTypeInfo(
        _osModelSrc,
        L"ResultTypeInfo",
        pAsyncOperationModelInfo,
        pAsyncOperationModelInfo->getResultTypeInfo());
    _osModelSrc<< std::endl;

}

void CGenerateTypeInfo::codeGenerate(const schema::CTypeInfo *pModelTypeInfo)
{
    CCodeGenToolBase::codeGenerate(pModelTypeInfo);

    // skip intrinsic Enumeration from model type entries
    if((pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableCollection ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableList ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Enumerable) &&
        ((const schema::CEnumerableTypeInfo *)pModelTypeInfo)->getItemTypeInfo()->getModelType() == nullptr)
    {
        return;
    }

    if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo* pCommandModelInfo = (const schema::CCommandTypeInfo*)pModelTypeInfo;
        assert(pCommandModelInfo != nullptr);

        // skip intrinsic commands from model type entries
        if (isSimpleCommand(pCommandModelInfo) || isObjectObjectCommand(pCommandModelInfo))
        {
            return;
        }
    }

    _osModelTypeInfoEntries << "\t&" << this->getNativeNamespace() << "::";
    if(pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableObject ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ViewModel)
    {
        _osModelTypeInfoEntries << toTypeInfoName(pModelTypeInfo->getName());
    }
    else
    {
        _osModelTypeInfoEntries << getModelTypeInfoIncName(pModelTypeInfo);
    }
    _osModelTypeInfoEntries << "::";
	if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObjectDispatch)
	{
		_osModelTypeInfoEntries << "ObjectDispatchInfo";
	}
	else if(pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableObject ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ViewModel)
    {
        _osModelTypeInfoEntries << "ObservableObjectInfo";
    }
    else if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Command)
    {
        _osModelTypeInfoEntries << "CommandTypeInfo";
    }
    else if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Enumerable)
    {
        _osModelTypeInfoEntries << "EnumerableTypeInfo";
    }
    else if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableCollection ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableList)
    {
        _osModelTypeInfoEntries << "CollectionTypeInfo";
    }
    else if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_AsyncOperation)
    {
        _osModelTypeInfoEntries << "AsyncOperationInfo";
    }
    _osModelTypeInfoEntries << ',' << std::endl;
}

void CGenerateTypeInfo::emitModelTypeInfoRef(std::wostream& os,const schema::CTypeInfo *pModelInfoType)
{
    assert(pModelInfoType);

    // support for 'Core' schema
    if(pModelInfoType->getParent().isFactorySchema())
    {
        if (pModelInfoType->getModelType() == schema::ModelTypeInfoType_Object)
        {
            os << "&" << getFoundationNamespace() << "::_ObjectTypeInfo";
        }
        else if (pModelInfoType->getModelType() == schema::ModelTypeInfoType_Dictionary)
        {
            os << "&" << getFoundationNamespace() << "::_DictionaryTypeInfo";
        }
        else
        {
            os << "&" << getCoreNamespace() << "::";
            switch (pModelInfoType->getModelType())
            {
            case schema::ModelTypeInfoType_ObservableObject:
                os << "_ObservableObjectType";
                break;
            case schema::ModelTypeInfoType_Command:
                os << "_CommandType";
                break;
            case schema::ModelTypeInfoType_ObservableCollection:
            case schema::ModelTypeInfoType_ObservableList:
                os << "_ObservableCollectionType";
                break;
            case schema::ModelTypeInfoType_AsyncOperation:
                os << "_AsyncOperationType";
                break;
            default:
                foundation_assert(false); // not supported
                break;
            }
        }
        return;
    }

    if(pModelInfoType->getModelType() == schema::ModelTypeInfoType_ObservableCollection ||
        pModelInfoType->getModelType() == schema::ModelTypeInfoType_ObservableList)
    {
        const schema::CEnumerableTypeInfo *pEnumerationTypeInfo = (const schema::CEnumerableTypeInfo *)pModelInfoType;
        if(pEnumerationTypeInfo->getItemTypeInfo()->getModelType() == nullptr)
        {
            os << "&" << getCoreNamespace() << "::_CollectionTypeInfo_";
            os << toTypeDesc(pEnumerationTypeInfo->getItemTypeInfo()->getPropertyType());
            return;
        }
    }

    if(pModelInfoType->getModelType() == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo* pCommandModelInfo = (const schema::CCommandTypeInfo*)pModelInfoType;
        assert(pCommandModelInfo != nullptr);

        // skip intrinsic commands from model type entries
        const bool fIsAsync = pCommandModelInfo->getIsAsync();
        if (isSimpleCommand(pCommandModelInfo))
        {
            os << "&" << getCoreNamespace() << "::Simple" << (fIsAsync ? "Async" : "") << "CommandTypeInfo::_CommandTypeInfo";
            return;
        }

        if (isObjectObjectCommand(pCommandModelInfo))
        {
            os << "&" << getCoreNamespace() << (fIsAsync ? "::ObjectObjectCommandModelInfo" : "::AsyncCommandModelInfo") << "::_CommandTypeInfo";
            return;
        }
    }

    // native namespace using lookup
    os << "&" << this->getNativeNamespaceLookup(pModelInfoType->getParent().getNamespace());

    switch (pModelInfoType->getModelType())
    {
    case schema::ModelTypeInfoType_ObjectDispatch:
        os << "::" << toTypeInfoName(pModelInfoType->getName()) << "::";
        os << "ObjectDispatchInfo";
        break;
    case schema::ModelTypeInfoType_ObservableObject:
    case schema::ModelTypeInfoType_ViewModel:
        os << "::" << toTypeInfoName(pModelInfoType->getName()) << "::";
        os << "ObservableObjectInfo";
        break;
    case schema::ModelTypeInfoType_Command:
        os << "::" << getModelTypeInfoIncName(pModelInfoType) << "::";
        os << "CommandTypeInfo";
        break;
    case schema::ModelTypeInfoType_AsyncOperation:
        os << "::" << getModelTypeInfoIncName(pModelInfoType) << "::";
        os << "AsyncOperationInfo";
        break;
    case schema::ModelTypeInfoType_ObservableCollection:
    case schema::ModelTypeInfoType_ObservableList:
        os << "::" << getModelTypeInfoIncName(pModelInfoType) << "::";
        os << "CollectionTypeInfo";
        break;
    case schema::ModelTypeInfoType_Enum:
		os << "::" << toTypeInfoName((const schema::CEnumTypeInfo *)pModelInfoType) << "::";
        os << "EnumTypeInfo";
        break;
    default:
        assert(false); // not supported
        break;
    }
}

void CGenerateTypeInfo::emitPropertyTypeInfo(
        std::wostream& os,
        const wchar_t *propertyTypeInfoName,
        const schema::CTypeInfo *pModelTypeInfo,
        const schema::CPropertyTypeInfo *pPropertyTypeInfo)
{
    std::wstring typeInfoName = 
        this->getNativeNamespace() + L"::" + getModelTypeInfoIncName(pModelTypeInfo) + L"::" + propertyTypeInfoName;
    emitPropertyTypeInfo(os,L"const",typeInfoName.data(),pPropertyTypeInfo);
}

void CGenerateTypeInfo::emitPropertyTypeInfo(
        std::wostream& os,
        const wchar_t *prefix,
        const wchar_t *typeInfoName,
        const schema::CPropertyTypeInfo *pPropertyTypeInfo)
{
    os << prefix << " " << getFoundationNamespace() << "::PropertyTypeInfo ";
    os << typeInfoName << "(";
    os << toPropertyTypeDesc(pPropertyTypeInfo->getPropertyType());
    os << ',';
    if(pPropertyTypeInfo->getModelType() == nullptr)
    {
        os << "nullptr";
    }
    else
    {
        emitModelTypeInfoRef(os,pPropertyTypeInfo->getModelType());
    }
    os << ");" << std::endl;
}
