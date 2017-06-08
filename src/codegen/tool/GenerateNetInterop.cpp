/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateNetInterop.cpp
****/

#include "GenerateNetInterop.h"

const wchar_t *_property_Declare_Template =
L"\
\n\
        /// <summary>\n\
        /// Property:{0}\n\
        /// </summary>\n\
        public {1} {0}\n\
        {\n\
{2}\n\
{3}\n\
        }\n\
";

const wchar_t *_property_get_Template =
L"\
            get\n\
            {\n\
                return GetProperty<{1}>((uint)Properties.{0});\n\
            }\n\
";

const wchar_t *_property_set_Template =
L"\
            set\n\
            {\n\
                SetProperty((uint)Properties.{0},value);\n\
            }\n\
";

const wchar_t *_property_Source_Template =
L"\
        public  {0} Source\n\
        {\n\
            get { return GetProperty<{0}>(0x2002); }\n\
        }\n\
";

        /// <summary>
        /// {0}: Class Name
        /// {1}: Base Class
        /// {2}: IID Type
        /// {3}: 'new' prefix
        /// {4}: Properties
        /// {5}: Methods
        /// </summary>
const wchar_t *_interopPropertyModelClass_Template =
L"\
    public class {0} : {1}\n\
    {\n\
        public static {3} Guid IIdType { get; private set; }\n\
 \n\
        static {0}()\n\
        {\n\
            IIdType = new Guid(\"{2}\");\n\
        }\n\
 \n\
        public {0}(Microsoft.PropertyModel.ObservableObjectAdapter adapter,ClassFactory classFactory) :\n\
            base(adapter,classFactory)\n\
        {\n\
        }\n\
\n\
        #region Internal Properties\n\
{4}\n\
        #endregion\n\
\n\
        #region Internal Methods\n\
{5}\n\
        #endregion\n\
    }\n\
";

const wchar_t *_propertiesEnum_Template =
L"\
        public {0} enum Properties\n\
        {\n\
{1}\n\
        }\n\
";

const wchar_t *_methodsEnum_Template =
L"\
        public {0} enum Methods\n\
        {\n\
{1}\n\
        }\n\
";

const wchar_t *_factoryTypeEntry_Template =
L"\
            new KeyValuePair<Guid, Type>({0},{1}),\n\
";

const wchar_t *_classFactoryTypes_Template =
L"\
    /// <summary>\n\
    /// Class:ClassFactoryTypes\n\
    /// </summary>\n\
\n\
    public static class ClassFactoryTypes\n\
    {\n\
        private static KeyValuePair<Guid, Type>[] _iidTypes = new KeyValuePair<Guid, Type>[] {\n\
{0}\n\
        };\n\
\n\
        public static KeyValuePair<Guid, Type>[] Types\n\
        {\n\
            get\n\
            {\n\
                return _iidTypes;\n\
            }\n\
        }\n\
    }\n\
";

const wchar_t *_csharpInteropFile_Template =
L"\
using System;\n\
using System.Windows.Input;\n\
using System.Collections;\n\
using System.Collections.Generic;\n\
using System.ComponentModel;\n\
using Microsoft.PropertyModel;\n\
\n\
namespace {0}\n\
{\n\
#if !WINDOWS_PHONE\n\
{1}\n\
\n\
#endif\n\
{2}\n\
    \n\
{3}\n\
}\n\
";

void CGenerateNetInterop::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
}

void CGenerateNetInterop::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "netInteropSrc:C# classes file generation" << std::endl;
}

void CGenerateNetInterop::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring csharpNetInteropSrcPath = getStringParameter(parameters, L"netInteropSrc", nullptr);

    // generate C# Classes file
    std::wofstream fileCSharpClasses(ToUtf8String(csharpNetInteropSrcPath.data()),std::ios_base::out);
    fileCSharpClasses << FormatW(_csharpInteropFile_Template,
        this->getNamespace().data(),
        _osCSharpInteropEnum.str().data(),
        _osCSharpInteropSrc.str().data(),
        FormatW(_classFactoryTypes_Template,_osFactoryTypeEntrySrc.str().data(),nullptr).data(),
        nullptr);
    fileCSharpClasses.close();
}

void CGenerateNetInterop::codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo)
{
    emitEnum(_osCSharpInteropEnum, pEnumTypeInfo);

    // add factory type
    _osFactoryTypeEntrySrc << FormatW(
        _factoryTypeEntry_Template,
        FormatW(L"new Guid(\"{0}\")", toCSharpIIDType(pEnumTypeInfo->getType()).data(), nullptr).data(),
        FormatW(L"typeof({0})", pEnumTypeInfo->getName().data(), nullptr).data(),
        nullptr);
}

void CGenerateNetInterop::codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo)
{
    // add factory type
    _osFactoryTypeEntrySrc << FormatW(
            _factoryTypeEntry_Template,
            FormatW(L"new Guid(\"{0}\")",toCSharpIIDType(pAsyncOperationModelInfo->getType()).data(),nullptr).data(),
            toCSharpModelTypeName(pAsyncOperationModelInfo,true).data(),
            nullptr);
}

void CGenerateNetInterop::codeGenerate(const schema::CEnumerableTypeInfo *pEnumerationTypeInfo)
{
    // add factory type
    _osFactoryTypeEntrySrc << FormatW(
            _factoryTypeEntry_Template,
            FormatW(L"new Guid(\"{0}\")",toCSharpIIDType(pEnumerationTypeInfo->getType()).data(),nullptr).data(),
            toCSharpModelTypeName(pEnumerationTypeInfo,true).data(),
            nullptr);
}

void CGenerateNetInterop::codeGenerate(const schema::CCommandTypeInfo *pCommmandModelInfo)
{
    // add factory type
    _osFactoryTypeEntrySrc << FormatW(
            _factoryTypeEntry_Template,
            FormatW(L"new Guid(\"{0}\")",
                toCSharpIIDType(pCommmandModelInfo->getType()).data(),nullptr).data(),
            toCSharpModelTypeName(pCommmandModelInfo,true).data(),
            nullptr);
}

void CGenerateNetInterop::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    const schema::CObservableObjectInfo *pSourceType = nullptr;
    bool isViewModelInherited = false;
    if(pPropertyModelInfo->getModelType() == schema::ModelTypeInfoType_ViewModel)
    {
       pSourceType = ((const schema::CPropertyViewModelInfo *)pPropertyModelInfo)->getSourceType();
       isViewModelInherited = ((const schema::CPropertyViewModelInfo *)pPropertyModelInfo)->getIsInherited();
    }

    std::wstring typeInfoName = toTypeInfoName(pPropertyModelInfo->getName());
    std::wstring baseClass;

    std::wostringstream osEnumProperties;
    std::wostringstream osEnumMethods;

    std::wostringstream osSrcProperties;
    std::wostringstream osSrcMethods;

    // add factory type
    _osFactoryTypeEntrySrc << FormatW(
            _factoryTypeEntry_Template,
            FormatW(L"{0}.IIdType",typeInfoName.data(),nullptr).data(),
            toCSharpModelTypeName(pPropertyModelInfo,true).data(),
            nullptr);

    const schema::CObservableObjectInfo *pBaseModelTypeInfo = nullptr;
    if(pPropertyModelInfo->getBaseType())
    {
        pBaseModelTypeInfo = (const schema::CObservableObjectInfo *)pPropertyModelInfo->getBaseType();

        //if(isViewModelInherited && 
        //    (pBaseModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ViewModel) &&
        //    (pSourceType != ((const schema::CPropertyViewModelInfo *)pPropertyModelInfo)->getSourceType()))
        //{
        //    pBaseModelTypeInfo = pSourceType;
        //}
    }
    else if(pSourceType && !pSourceType->getParent().isFactorySchema() && isViewModelInherited)
    {
        pBaseModelTypeInfo = pSourceType;
    }

    if(pBaseModelTypeInfo != nullptr)
    {
        baseClass = this->getNamespaceLookup(pBaseModelTypeInfo->getParent().getNamespace());
        baseClass += L".";
        baseClass += toTypeInfoName(pBaseModelTypeInfo->getName());
    }
    else
    {
        baseClass = L"ObservableObject";
    }

    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        std::wstring propertyName = (*iter)->getName();
        osEnumProperties << "\t\t\t" << propertyName << " = 0x" << std::hex << (*iter)->getId() << std::dec;
        if(pSourceType != nullptr)
        {
            osEnumProperties << " + 0x1000";
        }
        osEnumProperties << "," << std::endl;

        std::wstring propertyTypeInfoName = toCSharpPropertyTypeInfo(*iter,false);
        std::wstring getProperty = L"";
        if((*iter)->getFlags() & PropertyFlagType_CanRead)
        {
            getProperty = FormatW(_property_get_Template,propertyName.data(),propertyTypeInfoName.data(),nullptr);
        }
        std::wstring setProperty = L"";
        if((*iter)->getFlags() & PropertyFlagType_CanWrite)
        {
            setProperty = FormatW(_property_set_Template,propertyName.data(),nullptr);
        }
        osSrcProperties << FormatW(
                    _property_Declare_Template,
                    propertyName.data(),
                    propertyTypeInfoName.data(),
                    getProperty.data(),
                    setProperty.data(),
                    nullptr);
    }

    if(pSourceType && !pSourceType->getParent().isFactorySchema() &&
        (pBaseModelTypeInfo==nullptr || pBaseModelTypeInfo->getModelType() != schema::ModelTypeInfoType_ViewModel))
    {
        osSrcProperties << FormatW(_property_Source_Template,
            toCSharpModelTypeName(pSourceType,false).data(),
            nullptr);
    }

    // Iterate Methods
    for(schema::_MethodIteratorType::const_iterator iter = pPropertyModelInfo->getMethods().begin();
        iter != pPropertyModelInfo->getMethods().end();
        ++iter)
    {
        std::wstring methodName = (*iter)->getName();
        osEnumMethods << "\t\t\t" << methodName << " = 0x" << std::hex << (*iter)->getId() << std::dec;
        if(pSourceType != nullptr)
        {
            osEnumMethods << " + 0x1000";
        }
        osEnumMethods << "," << std::endl;

        bool hasResultType;
        bool isAsync = (*iter)->getIsAsync();

        std::wstring returnTypeInfo;
        if( isAsync ||
            ((*iter)->getResultType() && (*iter)->getResultType()->getPropertyType() != foundation::PropertyType_Empty))
        {
            hasResultType = true;
            if(isAsync)
            {
                returnTypeInfo = toAsyncOperationTypeInfo((*iter)->getResultType(),false);
            }
            else
            {
                returnTypeInfo = toCSharpPropertyTypeInfo((*iter)->getResultType(),false);
            }
        }
        else
        {
            hasResultType = false;
            returnTypeInfo = L"void";
        }

        osSrcMethods << "\t\tpublic " << ((*iter)->getIsAsync() ? toTaskTypeInfo((*iter)->getResultType(),false):returnTypeInfo) << " " << (*iter)->getName() << "(";

        for(schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            if(iterParam != (*iter)->getParameters().begin())
            {
                osSrcMethods << ",";
            }
            osSrcMethods << toCSharpPropertyTypeInfo(*iterParam,false) << " " << (*iterParam)->getParameterName();
        }
        osSrcMethods << ")" << std::endl;
        osSrcMethods << "\t\t{" << std::endl;
        osSrcMethods << "\t\t\t";
        if(hasResultType)
        {
            osSrcMethods << "return ";
        }
        osSrcMethods << "Invoke";
        if(hasResultType)
        {
            osSrcMethods << "<" << returnTypeInfo << ">";
        }
        osSrcMethods << "((uint)Methods." << (*iter)->getName();
        for(schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            osSrcMethods << ",";
            osSrcMethods << (*iterParam)->getParameterName();
        }
        osSrcMethods << ")" << (isAsync ? ".GetTask();" : ";") << std::endl;

        osSrcMethods << "\t\t}" << std::endl;
    }

    std::wstring propertiesRegion = L"";
    if(pPropertyModelInfo->getProperties().size() > 0)
    {
        propertiesRegion = FormatW(
            _propertiesEnum_Template,
            pBaseModelTypeInfo != nullptr && 
            pBaseModelTypeInfo->getProperties().size()>0 ? L"new" : L"",
            osEnumProperties.str().data(),
            nullptr);
        propertiesRegion += osSrcProperties.str();
    }

    std::wstring methodsRegion = L"";
    if(pPropertyModelInfo->getMethods().size() > 0)
    {
        methodsRegion = FormatW(
            _methodsEnum_Template,
            pBaseModelTypeInfo != nullptr && 
            pBaseModelTypeInfo->getMethods().size()>0 ? L"new" : L"",
            osEnumMethods.str().data(),
            nullptr);
        methodsRegion += osSrcMethods.str();
    }

    // Emit C# Class implementation
    _osCSharpInteropSrc << FormatW(
        _interopPropertyModelClass_Template,
        typeInfoName.data(),
        baseClass.data(),
        toCSharpIIDType(pPropertyModelInfo->getType()).data(),
        pBaseModelTypeInfo != nullptr ? L"new" : L"",
        propertiesRegion.data(),
        methodsRegion.data(),
        nullptr
        );
}

std::wstring CGenerateNetInterop::toAsyncOperationTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo,bool isTypeOf)
{
    std::wstring resultTypeInfo = toCSharpPropertyTypeInfo(pPropertyTypeInfo,isTypeOf);
    return isTypeOf ?
        FormatW(L"typeof(AsyncOperation<>).MakeGenericType({0})",
            resultTypeInfo.data(),nullptr):
        FormatW(L"AsyncOperation<{0}>",
            resultTypeInfo.data(),nullptr);
}

std::wstring CGenerateNetInterop::toTaskTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo, bool isTypeOf)
{
    std::wstring resultTypeInfo = toCSharpPropertyTypeInfo(pPropertyTypeInfo, isTypeOf);
    return isTypeOf ?
        FormatW(L"typeof(System.Threading.Tasks.Task<>).MakeGenericType({0})",
            resultTypeInfo.data(), nullptr) :
        FormatW(L"System.Threading.Tasks.Task<{0}>",
            resultTypeInfo.data(), nullptr);
}

std::wstring CGenerateNetInterop::toCSharpModelTypeName(const schema::CTypeInfo*pModelTypeInfo,bool isTypeOf)
{
    schema::ModelTypeInfoType modelTypeInfoType = pModelTypeInfo->getModelType();
    if(modelTypeInfoType == schema::ModelTypeInfoType_Enumerable ||
        modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection ||
        modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
    {
        const schema::CEnumerableTypeInfo *pEnumerationTypeInfo = (const schema::CEnumerableTypeInfo *)pModelTypeInfo;
        std::wstring itemTypeInfo = toCSharpPropertyTypeInfo(pEnumerationTypeInfo->getItemTypeInfo(),isTypeOf);
        std::wstring className = modelTypeInfoType == schema::ModelTypeInfoType_Enumerable ?
            L"Enumerable" : L"CollectionModel";
        return isTypeOf ?
                FormatW(L"typeof({1}<>).MakeGenericType({0})",
                    itemTypeInfo.data(), className.c_str(),nullptr):
                FormatW(L"{1}<{0}>",
                    itemTypeInfo.data(), className.c_str(), nullptr);
    }
    else if(modelTypeInfoType == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo *pCommmandModelInfo = (const schema::CCommandTypeInfo *)pModelTypeInfo;
        std::wstring parameterTypeInfo = toCSharpPropertyTypeInfo(pCommmandModelInfo->getParameterTypeInfo(),isTypeOf);
        std::wstring resultTypeInfo;

        if(pCommmandModelInfo->getIsAsync())
        {
            resultTypeInfo = toTaskTypeInfo(pCommmandModelInfo->getResultTypeInfo(),isTypeOf);
        }
        else
        {
            resultTypeInfo = toCSharpPropertyTypeInfo(pCommmandModelInfo->getResultTypeInfo(),isTypeOf);
        }

        return isTypeOf ?
                FormatW(L"typeof(Command<,,>).MakeGenericType(typeof(CommandState),{0},{1})",
                parameterTypeInfo.data(),
                resultTypeInfo.data(),
                nullptr):
                FormatW(L"Command<CommandState,{0},{1}>",
                parameterTypeInfo.data(),
                resultTypeInfo.data(),
                nullptr);
    }
    else if(modelTypeInfoType == schema::ModelTypeInfoType_AsyncOperation)
    {
        const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo = (const schema::CAsyncOperationTypeInfo *)pModelTypeInfo;
        return toAsyncOperationTypeInfo(pAsyncOperationModelInfo->getResultTypeInfo(),isTypeOf);
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Object)
    {
        return L"IObject";
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Dictionary)
    {
        return L"Dictionary";
    }
    else
    {
        std::wstring typeName;
        if(&pModelTypeInfo->getParent() != this->getModelSchema())
        {
            typeName = this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
            typeName += L".";
        }
        if(modelTypeInfoType == schema::ModelTypeInfoType_Enum)
        {
            typeName += pModelTypeInfo->getName();
        }
        else
        {
            typeName += toTypeInfoName(pModelTypeInfo->getName());
        }
        return isTypeOf ? FormatW(L"typeof({0})",typeName.data(),nullptr):typeName;
    }
}

std::wstring CGenerateNetInterop::toCSharpPropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo,bool isTypeOf)
{
    const schema::CTypeInfo*pModelTypeInfo = pPropertyTypeInfo->getModelType();
    if(pModelTypeInfo)
    {
        return toCSharpModelTypeName(pModelTypeInfo,isTypeOf);
    }
    else
    {
        std::wstring cSharpType = toCSharpType(pPropertyTypeInfo->getPropertyType());
        return isTypeOf ? FormatW(L"typeof({0})",cSharpType.c_str(),nullptr) : cSharpType;
    }
}

std::wstring CGenerateNetInterop::toCSharpType(foundation::PropertyType propertyType)
{
    // exceptions for Windows runtime
    if((propertyType & 0xff) == foundation::PropertyType::PropertyType_DateTime)
    {
        return toPropertyType(propertyType,L"DateTime",L"[]");
    }
    else if(propertyType == foundation::PropertyType::PropertyType_Empty)
    {
        return L"VoidModelType";
    }
    else
    {
        return CGenerateCSharpBase::toCSharpType(propertyType);
    }

}


