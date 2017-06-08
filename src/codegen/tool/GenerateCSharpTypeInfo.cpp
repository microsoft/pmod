/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCSharpTypeInfo.cpp
****/

#include "GenerateCSharpTypeInfo.h"
#include "guidhelper.h"


const wchar_t *_createPropertyInfo_Template = 
    L"\t\t\t\tPropertyModelHelper.CreatePropertyInfo((uint){0}Properties.{1},\"{1}\",0x{2},Windows.Foundation.PropertyType.{3},{4}),\n\
";

const wchar_t *_typeInfoProperties_Template = 
L"\
 public enum {0}Properties\n\
 {\n\
 {1}\
 }\n\
 ";

const wchar_t *_typeInfoMethods_Template = 
L"\
 public enum {0}Methods\n\
 {\n\
 {1}\
 }\n\
 ";

const wchar_t *_createPropertyTypeInfo_Template =
L"PropertyModelHelper.CreatePropertyTypeInfo(Windows.Foundation.PropertyType.{0},{1})";

// Format template for Enum Type Info
// {0} = Type Info Name
// {1} = Type Id
// {2} = Guid
// {3} = isFlags
// {4} = names
// {5} = values
// {6} = Name
// {7} = type info postfix
const wchar_t *_enumTypeInfo_Template = 
L"\
partial class {0}{7}\n\
{\n\
// private static members \n\
    private static Guid _iidType;\n\
    private static EnumTypeInfo _typeInfo;\n\
\n\
    public static Guid IIdType\n\
    {\n\
        get\n\
        {\n\
            if(_iidType == Guid.Empty)\n\
            {\n\
                _iidType = new Guid(\"{2}\");\n\
            }\n\
            return _iidType;\n\
        }\n\
    }\n\
    public static EnumTypeInfo TypeInfo\n\
    {\n\
        get\n\
        {\n\
            if(_typeInfo == null)\n\
            {\n\
                CreateTypeInfo();\n\
            }\n\
            return _typeInfo;\n\
        }\n\
    }\n\
\n\
// Type Id const \n\
    public const int TypeId = 0x{1};\n\
\n\
    private static void CreateTypeInfo()\n\
    {\n\
        _typeInfo =\n\
            new EnumTypeInfo(\n\
            TypeId,\n\
            IIdType,\n\
            \"{6}\",\n\
            {3},\n\
            new string[]{ {4} },\n\
            new uint[]{ {5} }\n\
            );\n\
    }\n\
}\n\
\n\
";
// Format template for Collection Model Info
// {0} = Type Info Name
// {1} = Type Id
// {2} = Guid
// {3} = Item Type Property Type
// {4} = Name
// {5} = type info postfix
const wchar_t *_collectionModelType_Template = 
L"\
partial class {0}{5}\n\
{\n\
// private static members \n\
    private static Guid _iidType;\n\
    private static CollectionTypeInfo _typeInfo;\n\
\n\
    public static Guid IIdType\n\
    {\n\
        get\n\
        {\n\
            if(_iidType == Guid.Empty)\n\
            {\n\
                _iidType = new Guid(\"{2}\");\n\
            }\n\
            return _iidType;\n\
        }\n\
    }\n\
    public static CollectionTypeInfo TypeInfo\n\
    {\n\
        get\n\
        {\n\
            if(_typeInfo == null)\n\
            {\n\
                CreateModelInfo();\n\
            }\n\
            return _typeInfo;\n\
        }\n\
    }\n\
\n\
// Type Id const \n\
    public const int TypeId = 0x{1};\n\
\n\
    private static void CreateModelInfo()\n\
    {\n\
        _typeInfo =\n\
            new CollectionTypeInfo(\n\
            TypeId,\n\
            IIdType,\n\
            \"{4}\",\n\
            {3}\n\
            );\n\
    }\n\
}\n\
\n\
";

// Format template for Command Model Info
// {0} = Type Info Name
// {1} = Type Id
// {2} = Guid
// {3} = Parameter Type Property Type
// {4} = Result Type Property Type
// {5} = isAsync
// {6} = Name
// {7} = type info postfix
const wchar_t *_commandModelType_Template = 
L"\
partial class {0}{7}\n\
{\n\
// private static members \n\
    private static Guid _iidType;\n\
    private static CommandTypeInfo _typeInfo;\n\
\n\
    public static Guid IIdType\n\
    {\n\
        get\n\
        {\n\
            if(_iidType == Guid.Empty)\n\
            {\n\
                _iidType = new Guid(\"{2}\");\n\
            }\n\
            return _iidType;\n\
        }\n\
    }\n\
    public static CommandTypeInfo TypeInfo\n\
    {\n\
        get\n\
        {\n\
            if(_typeInfo == null)\n\
            {\n\
                CreateModelInfo();\n\
            }\n\
            return _typeInfo;\n\
        }\n\
    }\n\
\n\
// Type Id const \n\
    public const int TypeId = 0x{1};\n\
\n\
    private static void CreateModelInfo()\n\
    {\n\
        _typeInfo =\n\
            new CommandTypeInfo(\n\
            TypeId,\n\
            IIdType,\n\
            \"{6}\",\n\
            {3},\n\
            {4},\n\
            {5}\n\
            );\n\
    }\n\
}\n\
\n\
";

// Format template for Property Model Info
// {0} = Name
// {1} = Type Id
// {2} = Guid
// {3} = Base Type
// {4} = properties infos
// {5} = methods infos
// {6} = events infos
// {7} = Name
// {8} = type info postfix
const wchar_t *_propertyModelType_Template = 
L"\
partial class {0}{8}\n\
{\n\
// private static members \n\
    private static Guid _iidType;\n\
    private static ObservableObjectInfo _typeInfo;\n\
\n\
    public static Guid IIdType\n\
    {\n\
        get\n\
        {\n\
            if(_iidType == Guid.Empty)\n\
            {\n\
                _iidType = new Guid(\"{2}\");\n\
            }\n\
            return _iidType;\n\
        }\n\
    }\n\
    public static ObservableObjectInfo TypeInfo\n\
    {\n\
        get\n\
        {\n\
            if(_typeInfo == null)\n\
            {\n\
                CreateModelInfo();\n\
            }\n\
            return _typeInfo;\n\
        }\n\
    }\n\
\n\
// Type Id const \n\
    public const int TypeId = 0x{1};\n\
\n\
    private static void CreateModelInfo()\n\
    {\n\
        _typeInfo =\n\
            new ObservableObjectInfo(\n\
            TypeId,\n\
            IIdType,\n\
            \"{7}\",\n\
            {3},\n\
            {4},\n\
            {5},\n\
            {6}\n\
            );\n\
    }\n\
}\n\
\n\
";

// Format template for Property ViewModel Info
// {0} = Type Info Name
// {1} = Type Id
// {2} = Guid
// {3} = Base Type
// {4} = properties infos
// {5} = methods infos
// {6} = events infos
// {7} = name
// {8} = type info postfix
// {9} = source type ref
// {10} = isInherited
const wchar_t *_propertyViewModelType_Template = 
L"\
partial class {0}{8}\n\
{\n\
// private static members \n\
    private static Guid _iidType;\n\
    private static PropertyViewModelInfo _typeInfo;\n\
\n\
    public static Guid IIdType\n\
    {\n\
        get\n\
        {\n\
            if(_iidType == Guid.Empty)\n\
            {\n\
                _iidType = new Guid(\"{2}\");\n\
            }\n\
            return _iidType;\n\
        }\n\
    }\n\
    public static PropertyViewModelInfo TypeInfo\n\
    {\n\
        get\n\
        {\n\
            if(_typeInfo == null)\n\
            {\n\
                CreateModelInfo();\n\
            }\n\
            return _typeInfo;\n\
        }\n\
    }\n\
\n\
// Type Id const \n\
    public const int TypeId = 0x{1};\n\
\n\
    private static void CreateModelInfo()\n\
    {\n\
        _typeInfo =\n\
            new PropertyViewModelInfo(\n\
            TypeId,\n\
            IIdType,\n\
            \"{7}\",\n\
            {3},\n\
            {4},\n\
            {5},\n\
            {6},\n\
            {9},\n\
            {10}\n\
            );\n\
    }\n\
}\n\
\n\
";

const wchar_t *_newInfo_Template = 
L"\
new {0}[]\n\
            {\n\
{1}\n\
            }";

const wchar_t *_csharpTypeInfoFile_Template = 
L"\
using System;\n\
using Microsoft.PropertyModel;\n\
using Microsoft.PropertyModel.Library;\n\
\n\
namespace {0}\n\
{\n\
{1}\n\
public static class ModelFactoryTypes\n\
{\n\
    public static void RegisterTypes()\n\
    {\n\
{2}\
    }\n\
}\n\
}\n\
";

void CGenerateCSharpTypeInfo::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
    _typeInfoPostfix = getStringParameter(parameters, L"typeInfoPostfix", L"");
    _exportModelMetadata = getBoolParameter(parameters, L"exportModelMetadata", true);
}

void CGenerateCSharpTypeInfo::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "csharpTypeInfoSrc:C# type info file generation" << std::endl;
}

void CGenerateCSharpTypeInfo::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring csharpTypeInfoSrcPath = getStringParameter(parameters, L"csharpTypeInfoSrc", nullptr);


    // TODO: we would need a better way to manage dependencies
    // this tecnique will fail
    for(schema::_TypeInfoIteratorType::const_iterator iter = this->getModelSchema()->getModelTypes(). begin();
        iter != this->getModelSchema()->getModelTypes().end();
        ++iter)
    {
        if( 
            (*iter)->getModelType() == schema::ModelTypeInfoType_ObjectDispatch ||
            (*iter)->getModelType() == schema::ModelTypeInfoType_ObservableObject ||
            (*iter)->getModelType() == schema::ModelTypeInfoType_ViewModel)
        {
            _osRegisterTypes << FormatW(
                L"\t\tPropertyModelInfo.RegisterType({0}{1}.TypeInfo);\n",
                toTypeInfoName((*iter)->getName()).c_str(),
                _typeInfoPostfix.c_str(),
                nullptr);
        }
    }

    // generate C# Classes file
    std::wofstream fileCSharpTypeInfo(ToUtf8String(csharpTypeInfoSrcPath.data()),std::ios_base::out);
    fileCSharpTypeInfo << FormatW(_csharpTypeInfoFile_Template,
        this->getNamespace().data(),        
        _osModelTypes.str().data(),
        _osRegisterTypes.str().data(),
        nullptr);
    fileCSharpTypeInfo.close();
}

void CGenerateCSharpTypeInfo::codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo)
{   
    std::wstring enumTypeInfoNameInc = getModelTypeInfoIncName(pEnumTypeInfo);
    std::wstring typeIdStr = ToHexString(pEnumTypeInfo->getId());
    std::wstring guidStr = ToWideString(pEnumTypeInfo->getType());

    std::wostringstream osNames,osValues;

    // Iterate Enums
    for (schema::_EnumInfoVectorType::const_iterator iter = pEnumTypeInfo->getEnums().begin();
        iter != pEnumTypeInfo->getEnums().end();
        ++iter)
    {
        // generate enum
        if (iter != pEnumTypeInfo->getEnums().begin())
        {
            osNames << ",";
            osValues << ",";
        }
        osNames << "\"" << std::get<0>(*iter) << "\"";
        osValues << std::get<1>(*iter);
    }

    _osModelTypes << FormatW(_enumTypeInfo_Template,
        enumTypeInfoNameInc.c_str(),
        typeIdStr.c_str(),
        guidStr.c_str(),
        pEnumTypeInfo->isFlags() ? L"true" : L"false",
        osNames.str().c_str(),
        osValues.str().c_str(),
        // TODO: until we find the root of the C# winRT interop problems
        // we will pass "" to force our runtime to fallback to IReference<UINT32>
        // runtime class and so allow the language to interop correctly
        // pEnumTypeInfo->getFullName().c_str(),
        L"",
        _typeInfoPostfix.c_str(),
        nullptr);

    _osRegisterTypes << FormatW(
        L"\t\tEnumTypeInfo.RegisterType({0}{1}.TypeInfo);\n",
        enumTypeInfoNameInc.c_str(),
        _typeInfoPostfix.c_str(),
        nullptr);
}

void CGenerateCSharpTypeInfo::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    bool isViewModel = pPropertyModelInfo->getModelType() == schema::ModelTypeInfoType_ViewModel;

    std::wstring typeInfoName = toTypeInfoName(pPropertyModelInfo->getName());

    std::wostringstream osPropertiesEnumValues;
    std::wostringstream osCreatePropertyInfos;
    std::wostringstream osCreateMethodInfos;
    std::wostringstream osCreateEventInfos;
    std::wostringstream osMethodsEnumValues;

    // Iterate Events
    for(schema::_EventIteratorType::const_iterator iter = pPropertyModelInfo->getEvents().begin();
        iter != pPropertyModelInfo->getEvents().end();
        ++iter)
    {
    }

    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        // generate public property id
        osPropertiesEnumValues << "\t" << (*iter)->getName() << " = ";
        if((*iter)->isParent())
        {
            osPropertiesEnumValues << "0x2000";
        }
        else
        {
            osPropertiesEnumValues << (*iter)->getId();
        }
        if(isViewModel && !(*iter)->isParent())
        {
            osPropertiesEnumValues << " + 0x1000";
        }
        osPropertiesEnumValues << "," << std::endl;

        // generate create property info
        const schema::CTypeInfo *pModelInfoType = (*iter)->getModelType();
        std::wstring modelTypeInfoRef;
        // TODO: support Enum generation
        if (pModelInfoType != nullptr)
        {
            modelTypeInfoRef = toIIdType(pModelInfoType);
        }
        else
        {
            modelTypeInfoRef = L"Guid.Empty";
        }

        // Get OptionFlags from Schema.        
        std::wostringstream flagsValues;
        flagsValues << ToHexString((*iter)->getFlags()).c_str();

        UINT32 optionFlags = (*iter)->getOptionFlags();
        if (optionFlags != 0)
        {
            optionFlags = optionFlags << 16;
            flagsValues << std::hex << " + 0x" << optionFlags;
            flagsValues << std::dec;
        }

        osCreatePropertyInfos << FormatW(_createPropertyInfo_Template,
                                typeInfoName.c_str(),
                                (*iter)->getName().c_str(),
                                flagsValues.str().data(),
                                toTypeDesc((*iter)->getPropertyType()).c_str(),
                                modelTypeInfoRef.c_str(),
                                nullptr);
    }
    // Iterate Methods
    for(schema::_MethodIteratorType::const_iterator iter = pPropertyModelInfo->getMethods().begin();
        iter != pPropertyModelInfo->getMethods().end();
        ++iter)
    {
        osMethodsEnumValues << "\t" << (*iter)->getName() << " = ";
        osMethodsEnumValues << (*iter)->getId();
        osMethodsEnumValues << "," << std::endl;
    }

    std::wstring propertyInfos = osCreatePropertyInfos.tellp() ? 
        FormatW(_newInfo_Template,L"PropertyInfo",osCreatePropertyInfos.str().c_str(),nullptr):
        L"null";

    std::wstring methodInfos = osCreateMethodInfos.tellp() ? 
        FormatW(_newInfo_Template,L"m_parameterName",osCreateMethodInfos.str().c_str(),nullptr):
        L"null";

    std::wstring eventInfos = osCreateEventInfos.tellp() ? 
        FormatW(_newInfo_Template,L"EventInfo",osCreateEventInfos.str().c_str(),nullptr):
        L"null";

    std::wstring typeIdStr = ToHexString(pPropertyModelInfo->getId());
    std::wstring guidStr = ToWideString(pPropertyModelInfo->getType());
    std::wstring baseTypeStr = toPropertyModelInfoRef(pPropertyModelInfo->getBaseType());

    if (pPropertyModelInfo->getProperties().size() && _exportModelMetadata)
    {
        _osModelTypes << FormatW(_typeInfoProperties_Template,
            typeInfoName.c_str(),
            osPropertiesEnumValues.str().c_str(),
            nullptr);
    }

    if (pPropertyModelInfo->getMethods().size() && _exportModelMetadata)
    {
        _osModelTypes << FormatW(_typeInfoMethods_Template,
            typeInfoName.c_str(),
            osMethodsEnumValues.str().c_str(),
            nullptr);
    }

    if(isViewModel)
    {
        const schema::CPropertyViewModelInfo *pPropertyViewModelInfo = (const schema::CPropertyViewModelInfo *)pPropertyModelInfo;
        _osModelTypes << FormatW(_propertyViewModelType_Template,
            typeInfoName.c_str(),
            typeIdStr.c_str(),
            guidStr.c_str(),
            baseTypeStr.c_str(),
            propertyInfos.c_str(),
            methodInfos.c_str(),
            eventInfos.c_str(),
            pPropertyModelInfo->getFullName().c_str(),
            _typeInfoPostfix.c_str(),
            toPropertyModelInfoRef(pPropertyViewModelInfo->getSourceType()).c_str(),
            pPropertyViewModelInfo->getIsInherited() ? L"true":L"false",
            nullptr);
    }
    else
    {
        _osModelTypes << FormatW(_propertyModelType_Template,
            typeInfoName.c_str(),
            typeIdStr.c_str(),
            guidStr.c_str(),
            baseTypeStr.c_str(),
            propertyInfos.c_str(),
            methodInfos.c_str(),
            eventInfos.c_str(),
            pPropertyModelInfo->getFullName().c_str(),
            _typeInfoPostfix.c_str(),
            nullptr);
    }
}

std::wstring CGenerateCSharpTypeInfo::getPropertyTypeInfoCreator(const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    return FormatW(_createPropertyTypeInfo_Template,
        toTypeDesc(pPropertyTypeInfo->getPropertyType()).c_str(),
        pPropertyTypeInfo->getModelType() == nullptr || 
        // TODO: we don't support Enums yet
        pPropertyTypeInfo->getModelType()->getModelType()==schema::ModelTypeInfoType_Enum ? 
            L"Guid.Empty" :
            toIIdType(pPropertyTypeInfo->getModelType()).c_str(),
        nullptr);
}

void CGenerateCSharpTypeInfo::codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo)
{
    if(pCollectionModelInfo->getItemTypeInfo()->getModelType() == nullptr)
    {
        // the Item Type on the schema is a supported type on the 'Core' library so we don't need to declare it
        return;
    }
    std::wstring collectionModelInfoNameInc = getModelTypeInfoIncName(pCollectionModelInfo);
    std::wstring typeIdStr = ToHexString(pCollectionModelInfo->getId());
    std::wstring guidStr = ToWideString(pCollectionModelInfo->getType());

    _osModelTypes << FormatW(_collectionModelType_Template,
        collectionModelInfoNameInc.c_str(),
        typeIdStr.c_str(),
        guidStr.c_str(),
        getPropertyTypeInfoCreator(pCollectionModelInfo->getItemTypeInfo()).c_str(),
        pCollectionModelInfo->getFullName().c_str(),
        _typeInfoPostfix.c_str(),
        nullptr);

    _osRegisterTypes << FormatW(
        L"\t\tCollectionModelInfo.RegisterType({0}{1}.TypeInfo);\n",
        collectionModelInfoNameInc.c_str(),
        _typeInfoPostfix.c_str(),
        nullptr);
}

void CGenerateCSharpTypeInfo::codeGenerate(const schema::CCommandTypeInfo *pCommmandModelInfo)
{

    if (isSimpleCommand(pCommmandModelInfo) || isObjectObjectCommand(pCommmandModelInfo))
    {
        // the Item Type on the schema is a supported type on the 'Core' library so we don't need to declare it
        return;
    }

    std::wstring commandModelInfoNameInc = getModelTypeInfoIncName(pCommmandModelInfo);
    std::wstring typeIdStr = ToHexString(pCommmandModelInfo->getId());
    std::wstring guidStr = ToWideString(pCommmandModelInfo->getType());

    _osModelTypes << FormatW(_commandModelType_Template,
        commandModelInfoNameInc.c_str(),
        typeIdStr.c_str(),
        guidStr.c_str(),
        getPropertyTypeInfoCreator(pCommmandModelInfo->getParameterTypeInfo()).c_str(),
        getPropertyTypeInfoCreator(pCommmandModelInfo->getResultTypeInfo()).c_str(),
        pCommmandModelInfo->getIsAsync() ? L"true" : L"false",
        pCommmandModelInfo->getFullName().c_str(),
        _typeInfoPostfix.c_str(),
        nullptr);

    _osRegisterTypes << FormatW(
        L"\t\tCommandModelInfo.RegisterType({0}{1}.TypeInfo);\n",
        commandModelInfoNameInc.c_str(),
        _typeInfoPostfix.c_str(),
        nullptr);
}

void CGenerateCSharpTypeInfo::codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo)
{
}

void CGenerateCSharpTypeInfo::codeGenerate(const schema::CTypeInfo *pModelTypeInfo)
{
    CCodeGenToolBase::codeGenerate(pModelTypeInfo);

}

std::wstring CGenerateCSharpTypeInfo::toIIdType(const schema::CTypeInfo *pModelTypeInfo)
{
    assert(pModelTypeInfo);

    std::wstring iidTypeRef;

    // support for 'Core' schema
    if (pModelTypeInfo->getParent().isFactorySchema())
    {
        iidTypeRef = L"ObjectTypeInfo.";

        switch (pModelTypeInfo->getModelType())
        {
        case schema::ModelTypeInfoType_ObjectDispatch:
            iidTypeRef += L"PropertyDataType";
            break;
        case schema::ModelTypeInfoType_ObservableObject:
            iidTypeRef += L"ObservableObjectType";
            break;
        case schema::ModelTypeInfoType_Command:
            iidTypeRef += L"CommandType";
            break;
        case schema::ModelTypeInfoType_ObservableCollection:
        case schema::ModelTypeInfoType_ObservableList:
            iidTypeRef += L"ObservableCollectionType";
            break;
        case schema::ModelTypeInfoType_AsyncOperation:
            iidTypeRef += L"AsyncOperationType";
            break;
        case schema::ModelTypeInfoType_Object:
            iidTypeRef += L"ObjectType";
            break;
        case schema::ModelTypeInfoType_Dictionary:
            iidTypeRef += L"DictionaryType";
            break;
        default:
            assert(false); // not supported
            break;
        }

        iidTypeRef += L".IIdType";
        return iidTypeRef;
    }

    if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableCollection &&
            ((const schema::CEnumerableTypeInfo *)pModelTypeInfo)->getItemTypeInfo()->getModelType() == nullptr)
    {
        foundation::PropertyType itemPropertyType = ((const schema::CEnumerableTypeInfo *)pModelTypeInfo)->getItemTypeInfo()->getPropertyType();
        iidTypeRef = L"CollectionModel.";
        iidTypeRef += itemPropertyType == foundation::PropertyType_Inspectable ? L"Object" : toTypeDesc(itemPropertyType);
        iidTypeRef += L"Type";

        return iidTypeRef;
    }

    if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo* pCommandModelInfo = (const schema::CCommandTypeInfo *)pModelTypeInfo;
        const bool fIsAsync = pCommandModelInfo->getIsAsync();

        if (isSimpleCommand(pCommandModelInfo))
        {
            return FormatW(
                L"new Guid(\"{0}\")",
                fIsAsync ? _simpleAsyncCommandTypeId : _simpleCommandTypeId,
                nullptr);
        }

        if (isObjectObjectCommand(pCommandModelInfo))
        {
            return FormatW(
                L"new Guid(\"{0}\")",
                fIsAsync ? _objectObjectAsyncCommandTypeId : _objectObjectCommandTypeId,
                nullptr);
        }
    }

    iidTypeRef = this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
    iidTypeRef += L".";
    iidTypeRef += getModelTypeInfoIncName(pModelTypeInfo);
    iidTypeRef += _typeInfoPostfix;
    iidTypeRef += L".IIdType";

    return iidTypeRef;
}

std::wstring CGenerateCSharpTypeInfo::toPropertyModelInfoRef(const schema::CTypeInfo *pModelTypeInfo)
{
    if(pModelTypeInfo != nullptr)
    {
        return FormatW(L"ObservableObjectInfo.GetPropertyModelInfo({0})",
            toIIdType(pModelTypeInfo).c_str(),
            nullptr);
    }
    return L"null";
}
