/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CodeGenToolBase.cpp
****/

#include "GenerateTypeInfo.h"
#include "ModelSchema.h"
#include "SchemaSerializer.h"
#include "CodeGenUtil.h"
#include "ToolUtil.h"
#include "guidhelper.h"

#include <fstream>
#include <sstream>
#include <iomanip>

//
// DEVNOTE: the GUIDs below are the replicas of the actual interface IIDs from propertyModelInterfaces.cpp
//          looks like we should explore a more robust way of introducing these interfaces to the codegen
//
// pmod::IID_ISimpleCommand:
const wchar_t * CCodeGenToolBase::_simpleCommandTypeId = L"81C05E25-DB59-4A54-A7DD-405E9AC97C7B";
// pmod::IID_ISimpleAsyncCommand:
const wchar_t * CCodeGenToolBase::_simpleAsyncCommandTypeId = L"11B0E777-37AA-4DCA-B0D5-68C9A86E9839";
// pmod::IID_ICommandModel:
const wchar_t * CCodeGenToolBase::_objectObjectCommandTypeId = L"AFAD3B17-330C-4DF2-A194-321DDA117988";
// pmod::IID_IAsyncCommand:
const wchar_t * CCodeGenToolBase::_objectObjectAsyncCommandTypeId = L"81378F87-8A76-492D-A6D1-905DCB237B26";

using namespace foundation;
using namespace foundation::library;

CCodeGenToolBase::~CCodeGenToolBase()
{
    delete _pModelSchema;
}

void CCodeGenToolBase::start(foundation::library::DictionaryClassPtr& parameters)
{
    initialize(parameters);
    this->codeGenerateStart(parameters);
    this->codeGenerateEnd(parameters);
}

void CCodeGenToolBase::help()
{
    std::wostringstream os;
    os << "Options supported:" << std::endl;
    helpInternal(os);
    wprintf(os.str().data());
}

void CCodeGenToolBase::helpInternal(std::wostream& os)
{
    os << "model:path to Unified model schema binary file" << std::endl;
    os << "namespacelookup: 'namespaceKey:namespaceValue;...'" << std::endl;
}

void CCodeGenToolBase::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters)
{
    UNREFERENCED_PARAMETER(parameters);
}


bool CCodeGenToolBase::getBoolParameter(
    foundation::library::DictionaryClassPtr& parameters,
    const wchar_t *key,
    const bool default_value)
{
    foundation::string_t key_t = foundation::to_platform_string(key);
    foundation::string_t value_t;
    if (parameters.HasKey(key_t.c_str()))
    {
        return parameters.Get<bool>(key_t.c_str());
    }  

    return default_value;
}


std::wstring CCodeGenToolBase::getStringParameter(
    foundation::library::DictionaryClassPtr& parameters,
    const wchar_t *key,
    const wchar_t *default_value)
{
    foundation::string_t key_t = foundation::to_platform_string(key);
    foundation::string_t value_t;
    if (parameters.HasKey(key_t.c_str()))
    {
        return foundation::to_wstring(parameters.Get<foundation::HStringPtr>(key_t.c_str()));
    }

    if (default_value == nullptr)
    {
        // no default was specified so throw
        _throwError(L"Parameter:'%ls' missing", key);
    }

    return default_value;
}

void CCodeGenToolBase::initializeNamespaceLookup(
    foundation::library::DictionaryClassPtr& parameters,
    LPCSTR_t key,
    foundation::library::DictionaryClassPtr& namespaceLookup,
    LPCSTR_t namespaceReplace)
{
    if (parameters.HasKey(key))
    {
        foundation::InspectablePtr value = parameters.GetComPtr<foundation::IInspectable>(key);

        // support parsing a namespace lookup
        if (foundation::GetPropertyType(value) == foundation::PropertyType_String)
        {
            std::wstring namespacelookup = to_wstring(getvalue<foundation::HStringPtr>(value));

            std::vector<std::wstring> values = SplitW(namespacelookup, L',');

            for (std::vector<std::wstring>::iterator iter = values.begin();
            iter != values.end();
                ++iter)
            {
                std::vector<std::wstring> kvp = SplitW((*iter), L':');
                assert(kvp.size() == 2);
                if (kvp.size() != 2)
                {
                    _throwError(L"Wrong namespace lookup in:%ls", (*iter).data());
                }
                namespaceLookup.Set(
                    to_platform_string(kvp[0].c_str()).c_str(),
                    to_platform_string(kvp[1].c_str()).c_str());

            }
        }
        else
        {
            namespaceLookup = DictionaryClassPtr(value);
        }

        if (namespaceReplace != nullptr)
        {
            DictionaryClassPtr::view kvp_view = namespaceLookup.getview();

            for (DictionaryClassPtr::view::const_iterator iter = kvp_view.begin();
            iter != kvp_view.end();
                ++iter)
            {
                string_t namespace_lookup = get_string_t((*iter).first);
                string_t namespace_value = getvalue<string_t>((*iter).second);

                string_t this_namespace = namespaceReplace;

                FindAndReplace(namespace_lookup, string_t(U("{namespace}")), this_namespace);
                FindAndReplace(namespace_value, string_t(U("{namespace}")), this_namespace);

                namespaceLookup.Set(namespace_lookup.c_str(), namespace_value.c_str());
            }
        }
    }
}

void CCodeGenToolBase::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    std::wstring modelPath = getStringParameter(parameters,L"model");
    
    std::ios_base::openmode mode = std::ios_base::in;

    std::fstream fis(ToUtf8String(modelPath.data()),mode);
	if (!fis.good())
	{
		_throwError(L"File:%s not found", modelPath.c_str());
	}
    _pModelSchema = CSchemaSerializer::deserializeJson(fis);
    //wprintf(L"Schema successfully deserialized\n");
    
    initializeNamespaceLookup(
        parameters,
        U("namespacelookup"),
        this->_namespaceLookup,
        to_platform_string(_pModelSchema->getNamespace().c_str()).c_str());
}

std::wstring CCodeGenToolBase::replaceNamespace(const std::wstring& value)
{
    std::wstring result = value;

	FindAndReplace(result, std::wstring(L"{namespace}"), _pModelSchema->getNamespace());
    return result;
}

std::wstring CCodeGenToolBase::getNamespaceLookup(
    DictionaryClassPtr& namespaceLookup,
    const std::wstring& namespaceStr)
{
    string_t namespace_str_t = to_platform_string(namespaceStr.c_str());
    if (namespaceLookup.HasKey(namespace_str_t.c_str()))
    {
        return to_wstring(namespaceLookup.Get<string_t>(namespace_str_t.c_str()).c_str());
    }
    return std::wstring(namespaceStr);
}

std::wstring CCodeGenToolBase::getNamespaceLookup(const std::wstring& namespaceStr)
{
    return getNamespaceLookup(_namespaceLookup, namespaceStr);
}

std::wstring CCodeGenToolBase::getNamespaceLookup(const schema::CTypeInfo *pModelTypeInfo)
{
    return getNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
}

std::wstring CCodeGenToolBase::getNativeNamespaceLookup(const std::wstring& namespaceStr)
{
    return toNativeNamespace(getNamespaceLookup(namespaceStr));
}

std::wstring CCodeGenToolBase::getNamespace()
{
    assert(_pModelSchema);

    return getNamespaceLookup(_pModelSchema->getNamespace());
}

std::wstring CCodeGenToolBase::getNativeNamespace()
{
    return toNativeNamespace(getNamespace());
}

std::wstring CCodeGenToolBase::toNativeNamespace(const std::wstring& modelNamespace)
{
    std::wstring namespace_str = modelNamespace;
    replace(namespace_str, L".", L"::");
    return namespace_str;
}

std::wstring CCodeGenToolBase::toNativeNamespaceDeclare(const std::wstring& nativeNamespace)
{
    std::wstring namespaces_declare;

    std::vector<std::wstring> namespaces = SplitW(nativeNamespace, L"::");
    for (std::vector<std::wstring>::const_iterator
        iter = namespaces.begin();
        iter != namespaces.end();
    ++iter)
    {
        namespaces_declare += L"namespace ";
        namespaces_declare += (*iter);
        namespaces_declare += L" { ";
    }

    return namespaces_declare;
}

std::wstring CCodeGenToolBase::toNativeNamespaceEnd(const std::wstring& nativeNamespace)
{
    std::wstring namespaces_end;

    std::vector<std::wstring> namespaces = SplitW(nativeNamespace, L"::");
    for (std::vector<std::wstring>::const_iterator
        iter = namespaces.begin();
        iter != namespaces.end();
    ++iter)
    {
        namespaces_end += L"} ";
    }

    return namespaces_end;
}

std::wstring CCodeGenToolBase::toNamespaceFullName(const std::wstring& nativeNamespace, const std::wstring& name)
{
    return nativeNamespace + L"." + name;
}

void CCodeGenToolBase::codeGenerateStart(foundation::library::DictionaryClassPtr& parameters)
{
    for(schema::_EnumTypeInfoIteratorType::const_iterator iter = this->getModelSchema()->getEnumTypes(). begin();
        iter != this->getModelSchema()->getEnumTypes().end();
        ++iter)
    {
        codeGenerate((*iter));
    }

    for(schema::_TypeInfoIteratorType::const_iterator iter = this->getModelSchema()->getModelTypes(). begin();
        iter != this->getModelSchema()->getModelTypes().end();
        ++iter)
    {
        this->codeGenerate((*iter));
    }
}

void CCodeGenToolBase::codeGenerate(const schema::CTypeInfo *pModelTypeInfo)
{
	if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObjectDispatch)
	{
		codeGenerate((const schema::CObjectDispatchInfo *)pModelTypeInfo);
	}
	else if( pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableObject)
    {
        codeGenerate( (const schema::CObservableObjectInfo *)pModelTypeInfo);
    }
    else if( 
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Enumerable ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableCollection ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableList)
    {
        codeGenerate( (const schema::CEnumerableTypeInfo *)pModelTypeInfo);
    }
    else if( pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Command)
    {
        codeGenerate( (const schema::CCommandTypeInfo *)pModelTypeInfo);
    }
    else if( pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_AsyncOperation)
    {
        codeGenerate( (const schema::CAsyncOperationTypeInfo *)pModelTypeInfo);
    }
    else if( pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ViewModel)
    {
        codeGenerate( (const schema::CPropertyViewModelInfo *)pModelTypeInfo);
    }
}

const wchar_t *CCodeGenToolBase::getCoreNamespace()
{
    return L"pmod";
}

const wchar_t *CCodeGenToolBase::getFoundationNamespace()
{
    return L"foundation";
}

std::wstring CCodeGenToolBase::toTypeInfoName(const std::wstring& typeInfoDeclare)
{
    if (typeInfoDeclare.length() && typeInfoDeclare[0] == L'I')
    {
        return typeInfoDeclare.substr(1);
    }

    return typeInfoDeclare;
}

void CCodeGenToolBase::formatHexBytes(std::wostream& os,UINT8 *&pIIdType,unsigned int nbytes)
{
    while(nbytes--)
    {
        os << std::setw(2) << std::setfill(L'0') <<  *pIIdType;
        ++pIIdType;
    }
}

void CCodeGenToolBase::formatIIDType(std::wostream& os,GUID iidType)
{
    UINT8 *pIIdType = (UINT8 *)&iidType;

    os << std::hex;

    os << "{";
    // first 4 bytes
    os << "0x";
    SwapUInt32(pIIdType);
    formatHexBytes(os,pIIdType,4);
    os << ",";
    // next 2 bytes
    os << "0x";
    SwapUInt16(pIIdType);
    formatHexBytes(os,pIIdType,2);
    os << ",";
    // next 2 bytes
    os << "0x";
    SwapUInt16(pIIdType);
    formatHexBytes(os,pIIdType,2);
    os << ",";
    // rest 8 bytes
    os << "{";
    for(int i = 0; i < 8; ++i)
    {
        if(i > 0)
        {
            os << ",";
        }
        os << "0x";
        formatHexBytes(os,pIIdType,1);
    }
    os << "}};" << std::endl;
    os << std::dec;
}

std::wstring CCodeGenToolBase::getFilePath(const std::wstring& path)
{
    size_t pos = path.find_last_of(L"\\/");
    if(pos != std::wstring::npos)
    {
        return path.substr(pos + 1);
    }
    return path;
}

void CCodeGenToolBase::emitIncludeReferences(
        std::wostream& os,
        foundation::library::DictionaryClassPtr& parameters,
        const wchar_t *parameterName,
        const wchar_t *includeKeyword)
{
    std::wstring modelIncReferences = getStringParameter(parameters, parameterName,L"");
        
    if(modelIncReferences.length())
    {
        std::vector<std::wstring> includeReferencesVector = SplitW(modelIncReferences,L',');
        for(std::vector<std::wstring>::iterator iter = includeReferencesVector.begin();
            iter != includeReferencesVector.end();
            ++iter)
        {
            os << "#" << includeKeyword;
            if ((*iter)[0] == L'<')
            {
                os << ' ' << *iter;
            }
            else
            {
                os << "\"" << getFilePath(*iter) << "\"";
            }
            os << std::endl;
        }
    }
}

std::wstring CCodeGenToolBase::toPropertyTypeName(foundation::PropertyType propertyType)
{
    bool isArray = (propertyType & 0x400) != 0;

    std::wstring typeName = L"";
    switch(propertyType & 0xff)
    {
    case foundation::PropertyType::PropertyType_UInt8:
        typeName = L"UInt8";
        break;
    case foundation::PropertyType::PropertyType_Int16:
        typeName = L"Int16";
        break;
    case foundation::PropertyType::PropertyType_UInt16:
        typeName = L"UInt16";
    case foundation::PropertyType::PropertyType_Int32:
        typeName = L"Int32";
        break;
    case foundation::PropertyType::PropertyType_UInt32:
        typeName = L"UInt32";
        break;
    case foundation::PropertyType::PropertyType_Int64:
        typeName = L"Int64";
        break;
    case foundation::PropertyType::PropertyType_UInt64:
        typeName = L"UInt64";
        break;
    case foundation::PropertyType::PropertyType_Single:
        typeName = L"Single";
        break;
    case foundation::PropertyType::PropertyType_Double:
        typeName = L"Double";
        break;
    case foundation::PropertyType::PropertyType_Char16:
        typeName = L"Char16";
        break;
    case foundation::PropertyType::PropertyType_Boolean:
        typeName = L"Boolean";
        break;
    case foundation::PropertyType::PropertyType_String:
        typeName = L"String";
        break;
    case foundation::PropertyType::PropertyType_Inspectable:
        typeName = L"Inspectable";
        break;
    case foundation::PropertyType::PropertyType_DateTime:
        typeName = L"DateTime";
        break;
    case foundation::PropertyType::PropertyType_TimeSpan:
        typeName = L"TimeSpan";
        break;
    case foundation::PropertyType::PropertyType_Guid:
        typeName = L"Guid";
        break;
    case foundation::PropertyType::PropertyType_Point:
    case foundation::PropertyType::PropertyType_Size:
    case foundation::PropertyType::PropertyType_Rect:
    default:
        assert(false);
    }

    if (isArray)
    {
        typeName += L"Array";
    }
    return typeName;
}

std::wstring CCodeGenToolBase::toCoreType(foundation::PropertyType propertyType)
{
    std::wstring type = L"";

    switch(propertyType & 0xff)
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
        type = L"foundation::IInspectable *";
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
    return toPropertyType(propertyType,type.c_str(),L"*");
}

void CCodeGenToolBase::emitCoreModelTypeInfo(
        std::wostream& os,
        schema::ModelTypeInfoType modelTypeInfoType)
{
    if (modelTypeInfoType == schema::ModelTypeInfoType_Dictionary)
    {
        os << getFoundationNamespace() << "::IDictionary";
        return;
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Object)
    {
        os << getFoundationNamespace() << "::IObject";
        return;
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_ObjectDispatch)
    {
        os << getFoundationNamespace() << "::IObjectDispatch";
        return;
    }

    os << getCoreNamespace() << "::";
    if(modelTypeInfoType == schema::ModelTypeInfoType_ObservableObject || modelTypeInfoType == schema::ModelTypeInfoType_ViewModel)
    {
        os << "IObservableObject";
    }
    else if(modelTypeInfoType ==  schema::ModelTypeInfoType_ObservableCollection)
    {
        os << "IObservableCollection";
    }
    else if(modelTypeInfoType ==  schema::ModelTypeInfoType_ObservableList)
    {
        os << "IObservableList";
    }
    else if(modelTypeInfoType ==  schema::ModelTypeInfoType_Command)
    {
        os << "ICommand";
    }
    else if(modelTypeInfoType ==  schema::ModelTypeInfoType_AsyncOperation)
    {
        os << "IAsyncOperationModel";
    }
    else
    {
        assert(0);
    }
}

void CCodeGenToolBase::emitModelTypeInfo(
        std::wostream& os,
        const schema::CTypeInfo*pModelTypeInfo)
{
    assert(pModelTypeInfo);

    schema::ModelTypeInfoType modelTypeInfoType = pModelTypeInfo->getModelType();
    if(
        modelTypeInfoType == schema::ModelTypeInfoType_ObjectDispatch ||
        modelTypeInfoType == schema::ModelTypeInfoType_ObservableObject || 
        modelTypeInfoType == schema::ModelTypeInfoType_ViewModel
        )
    {
        emitFullName(os,pModelTypeInfo);
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection)
    {
        const schema::CEnumerableTypeInfo *pCollectionModelInfo = (const schema::CEnumerableTypeInfo *)pModelTypeInfo;
        if(pCollectionModelInfo->getItemTypeInfo()->getModelType() == nullptr)
        {
            os << this->getCoreNamespace() << "::" << "IObservableCollection_" << toPropertyTypeName(pCollectionModelInfo->getItemTypeInfo()->getPropertyType());
        }
        else
        {
            emitFullName(os,pModelTypeInfo);
        }
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo *pCommmandModelInfo = (const schema::CCommandTypeInfo *)pModelTypeInfo;
        if (isCoreCommand(pCommmandModelInfo))
        {
            os << this->getCoreNamespace() << "::I";
            os << (isSimpleCommand(pCommmandModelInfo) ? "Simple" : "");
            os << (pCommmandModelInfo->getIsAsync() ? "Async" : "");
            os << "Command";
        }
        else
        {
            emitFullName(os,pModelTypeInfo);
        }
    }
    else
    {
        emitCoreModelTypeInfo(os,modelTypeInfoType);
    }
}

std::wstring CCodeGenToolBase::toModelTypeInfo(const schema::CTypeInfo*pModelTypeInfo)
{
    std::wostringstream os;
    emitModelTypeInfo(os, pModelTypeInfo);
    return os.str();
}

void CCodeGenToolBase::emitFullName(std::wostream& os, const schema::CTypeInfo*pModelTypeInfo)
{
    assert(pModelTypeInfo);
    if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Object ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Dictionary)
    {
        os << getFoundationNamespace();
    }
    else
    {
        os << this->getNativeNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
    }
    os << "::" << pModelTypeInfo->getName();
}

std::wstring CCodeGenToolBase::toFullName(const schema::CTypeInfo*pModelTypeInfo)
{
    std::wostringstream os;
    emitFullName(os,pModelTypeInfo);
    return os.str();
}

void CCodeGenToolBase::emitPropertyTypeInfo(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    if(pPropertyTypeInfo->getModelType())
    {
        if(pPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum)
        {
            // Enum Type Info generation
            emitFullName(os,pPropertyTypeInfo->getModelType());
        }
        else
        {
            emitModelTypeInfo(os,pPropertyTypeInfo->getModelType());
            os << " *";
        }

        // if array add indirection
        if(pPropertyTypeInfo->getPropertyType() & 0x400)
        {
            os << "*";
        }
    }
    else
    {
        os << toCoreType(pPropertyTypeInfo->getPropertyType());
    }
}

void CCodeGenToolBase::emitPropertyTypeName(
	std::wostream& os,
	const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
	if (pPropertyTypeInfo->getModelType())
	{
		if (pPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum)
		{
			// Enum Type Info generation
			emitFullName(os, pPropertyTypeInfo->getModelType());
		}
		else
		{
			emitModelTypeInfo(os, pPropertyTypeInfo->getModelType());
		}
	}
	else
	{
		os << toCoreType(pPropertyTypeInfo->getPropertyType());
	}
}

void CCodeGenToolBase::emitSalOutputTypeInfo(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    bool isAsync)
{
    if (isAsync)
    {
        os << " _COM_Outptr_ ";
    }
    else
    {
        if (pPropertyTypeInfo->getPropertyType() & 0x400)
        {
            os << " _Outptr_result_maybenull_ ";
        }
        else if (isPropertyTypeInfoObject(pPropertyTypeInfo))
        {
            os << " _COM_Outptr_result_maybenull_ ";
        }
        else
        {
            os << " _Out_ ";
        }
    }
}

void CCodeGenToolBase::emitDeclarePropertyTypeInfo(
        std::wostream& os,
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,
        bool isOutput,
        int parameterIndex)
{
    if(pPropertyTypeInfo->getPropertyType() & 0x400)
    {
        if (isOutput)
        {
            os << "_Out_ ";
        }
        os << "UINT32 ";
        if(isOutput)
        {
            os << "*";
        }
        os << "size";
        if(parameterIndex)
        {
            os << "_" << parameterIndex;
        }
        os << ",";
    }
    if (isOutput)
    {
        emitSalOutputTypeInfo(os, pPropertyTypeInfo, false);
    }
    emitPropertyTypeInfo(os,pPropertyTypeInfo);
}

std::wstring CCodeGenToolBase::toDeclarePropertyTypeInfo(
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    bool isOutput,
    int parameterIndex)
{
    std::wostringstream os;
    emitDeclarePropertyTypeInfo(os, pPropertyTypeInfo, isOutput, parameterIndex);
    return os.str();
}

std::wstring CCodeGenToolBase::toPropertyTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    std::wostringstream os;
    emitPropertyTypeInfo(os,pPropertyTypeInfo);
    return os.str();
}

std::wstring CCodeGenToolBase::toPropertyTypeName(const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
	std::wostringstream os;
	emitPropertyTypeName(os, pPropertyTypeInfo);
	return os.str();
}

std::wstring CCodeGenToolBase::toTypeInfoName(const schema::CEnumTypeInfo *pEnumTypeInfo)
{
	return pEnumTypeInfo->getName() + L"Enum";
}

bool CCodeGenToolBase::isInspectableType(const schema::CPropertyTypeInfo *pTypeInfo)
{
    return pTypeInfo->getModelType()==nullptr && pTypeInfo->getPropertyType() == foundation::PropertyType_Inspectable;
}

bool CCodeGenToolBase::isModelInSchema(const schema::CTypeInfo *pModelTypeInfo)
{
    return pModelTypeInfo && &pModelTypeInfo->getParent() == this->getModelSchema();
}

std::wstring CCodeGenToolBase::getModelTypeInfoIncName(const schema::CTypeInfo *pModelTypeInfo)
{
    std::wstring name = pModelTypeInfo->getName();
    if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Enumerable)
    {
        replace(name, L"IEnumerable_", L"Enumerable_");
    }
    else if(pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableCollection)
    {
        replace(name,L"ICollection_",L"ObservableCollection_");
    }
    else if(pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableList)
    {
        replace(name,L"IList_",L"ListModel_");
    }
    else if(pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Command)
    {
        replace(name,L"ICommand_",L"Command_");
    }
    else if(pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_AsyncOperation)
    {
        replace(name,L"IAsyncOperation_",L"AsyncOperation_");
    }
    else if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Enum)
    {
        name = L"EnumTypeInfo_" + name;
    }
    else
    {
        // default is PropertyModel and ViewModel types
        name = toTypeInfoName(name);
    }
    return name;
}

std::wstring CCodeGenToolBase::getModelTypeInfoIIdType(const schema::CTypeInfo *pModelTypeInfo)
{
    assert(pModelTypeInfo);

    if(pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableCollection ||
        pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableList)
    {
        const schema::CEnumerableTypeInfo *pCollectionModelInfo = (const schema::CEnumerableTypeInfo *)pModelTypeInfo;
        if(pCollectionModelInfo->getItemTypeInfo()->getModelType() == nullptr)
        {
            return FormatW(L"{0}::_CollectionTypeInfo_{1}.m_iid_type",
                        getCoreNamespace(),
                        toTypeDesc(pCollectionModelInfo->getItemTypeInfo()->getPropertyType()).c_str(),
                        nullptr);
        }
    }
    return this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace()) +
        L"::" + 
        getModelTypeInfoIncName(pModelTypeInfo) +
        L"::IIDType";
}

std::wstring CCodeGenToolBase::toTypeDesc(foundation::PropertyType propertyType)
{
    std::wstring typeDesc;

    switch(propertyType & 0xff)
    {
    case foundation::PropertyType::PropertyType_Empty:
        typeDesc = L"Empty";
        break;
    case foundation::PropertyType::PropertyType_UInt8:
        typeDesc = L"UInt8";
        break;
    case foundation::PropertyType::PropertyType_Int16:
        typeDesc = L"Int16";
        break;
    case foundation::PropertyType::PropertyType_UInt16:
        typeDesc = L"UInt16";
        break;
    case foundation::PropertyType::PropertyType_Int32:
        typeDesc = L"Int32";
        break;
    case foundation::PropertyType::PropertyType_UInt32:
        typeDesc = L"UInt32";
        break;
    case foundation::PropertyType::PropertyType_Int64:
        typeDesc = L"Int64";
        break;
    case foundation::PropertyType::PropertyType_UInt64:
        typeDesc = L"UInt64";
        break;
    case foundation::PropertyType::PropertyType_Single:
        typeDesc = L"Single";
        break;
    case foundation::PropertyType::PropertyType_Double:
        typeDesc = L"Double";
        break;
    case foundation::PropertyType::PropertyType_Char16:
        typeDesc = L"Char16";
        break;
    case foundation::PropertyType::PropertyType_Boolean:
        typeDesc = L"Boolean";
        break;
    case foundation::PropertyType::PropertyType_String:
        typeDesc = L"String";
        break;
    case foundation::PropertyType::PropertyType_Inspectable:
        typeDesc = L"Inspectable";
        break;
    case foundation::PropertyType::PropertyType_DateTime:
        typeDesc = L"DateTime";
        break;
    case foundation::PropertyType::PropertyType_TimeSpan:
        typeDesc = L"TimeSpan";
        break;
    case foundation::PropertyType::PropertyType_Guid:
        typeDesc = L"Guid";
        break;
    case foundation::PropertyType::PropertyType_Point:
        typeDesc = L"Point";
        break;
    case foundation::PropertyType::PropertyType_Size:
        typeDesc = L"Size";
        break;
    case foundation::PropertyType::PropertyType_Rect:
        typeDesc = L"Rect";
        break;
    default:
        assert(false);
    }
    return toPropertyType(propertyType,typeDesc.c_str(),L"Array");
}

std::wstring CCodeGenToolBase::toPropertyTypeDesc(foundation::PropertyType propertyType)
{
    return L"foundation::PropertyType_" + toTypeDesc(propertyType);
}

std::wstring CCodeGenToolBase::toPropertyType(
        foundation::PropertyType propertyType,
        const wchar_t *typeName,
        const wchar_t *arrayPostfix)
{
    bool isArray = (propertyType & 0x400) != 0;
    return isArray ? std::wstring(typeName) + arrayPostfix : typeName;
}

bool CCodeGenToolBase::isSimpleCommand(const schema::CCommandTypeInfo *pCommandModelInfo)
{
    assert(pCommandModelInfo);

    return (pCommandModelInfo->getParameterTypeInfo() == nullptr || pCommandModelInfo->getParameterTypeInfo()->getPropertyType() == foundation::PropertyType_Empty) &&
           (pCommandModelInfo->getResultTypeInfo() == nullptr || pCommandModelInfo->getResultTypeInfo()->getPropertyType() == foundation::PropertyType_Empty);
}

bool CCodeGenToolBase::isCommandResult(const schema::CCommandTypeInfo *pCommandModelInfo)
{
    assert(pCommandModelInfo);

    return (pCommandModelInfo->getParameterTypeInfo() == nullptr || pCommandModelInfo->getParameterTypeInfo()->getPropertyType() == foundation::PropertyType_Empty) &&
        (pCommandModelInfo->getResultTypeInfo() != nullptr && pCommandModelInfo->getResultTypeInfo()->getPropertyType() != foundation::PropertyType_Empty);
}

bool CCodeGenToolBase::isCommandParameter(const schema::CCommandTypeInfo *pCommandModelInfo)
{
    assert(pCommandModelInfo);

    return (pCommandModelInfo->getParameterTypeInfo() != nullptr && pCommandModelInfo->getParameterTypeInfo()->getPropertyType() != foundation::PropertyType_Empty) &&
        (pCommandModelInfo->getResultTypeInfo() == nullptr || pCommandModelInfo->getResultTypeInfo()->getPropertyType() == foundation::PropertyType_Empty);
}

bool CCodeGenToolBase::isObjectObjectCommand(const schema::CCommandTypeInfo *pCommandModelInfo)
{
    assert(pCommandModelInfo);

    const schema::CPropertyTypeInfo* paramTypeInfo = pCommandModelInfo->getParameterTypeInfo();
    const schema::CPropertyTypeInfo* resultTypeInfo = pCommandModelInfo->getResultTypeInfo();
    if (paramTypeInfo == nullptr || resultTypeInfo == nullptr)
    {
        return false;
    }

    const foundation::PropertyType paramType = paramTypeInfo->getPropertyType();
    const foundation::PropertyType resultType = resultTypeInfo->getPropertyType();
    if (paramType != foundation::PropertyType_Inspectable || resultType != foundation::PropertyType_Inspectable)
    {
        return false;
    }

    const schema::CTypeInfo* paramModelTypeInfo = paramTypeInfo->getModelType();
    const schema::CTypeInfo* resultModelTypeInfo = resultTypeInfo->getModelType();

    return (
        (paramModelTypeInfo == nullptr || paramModelTypeInfo->getModelType() == schema::ModelTypeInfoType_None) &&
        (resultModelTypeInfo == nullptr || resultModelTypeInfo->getModelType() == schema::ModelTypeInfoType_None)
        );
}

bool CCodeGenToolBase::isCoreCommand(const schema::CCommandTypeInfo *pCommandModelInfo)
{
    assert(pCommandModelInfo);

    return isSimpleCommand(pCommandModelInfo) || isObjectObjectCommand(pCommandModelInfo);
}

bool CCodeGenToolBase::getBoolParamater(
        foundation::library::DictionaryClassPtr& parameters,
        const wchar_t *parameterName,
        bool defaultValue)
{
    return parameters.Get(foundation::to_platform_string(parameterName).c_str(), defaultValue);
}
