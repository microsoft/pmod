/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateModelAdapter.cpp
****/

#include "GenerateModelAdapter.h"

// Format template for Model Inc Header File
// {0} = API
// {1} = Namespace
// {2} = Dependency Model/Adapter Inc Files
// {3} = Core Namespace
// {4} = Forward interfaces
// {5} = Declare interfaces
const wchar_t *_modelAdapterInc_Template = 
L"\n\
#pragma once \n\
\n\
// depependency Files\n\
{2}\n\
\n\
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
{1} \n\
\n\
{0} HRESULT STDAPICALLTYPE RegisterModelTypeAdapters();\n\
\n\
// forward interfaces \n\
{4}\n\
\n\
// command model typedef interfaces \n\
{5}\n\
\n\
// declare interfaces \n\
{6}\n\
{7}\n\
";

// Format template for Model Adapter Src File
// {0} = Model Adapter Inc Header
// {1} = Core Namespace
// {2} = Namespace
// {3} = Adapter template/classes
// {4} = Adapter Factory entries
// {5} = Foundation Namespace
const wchar_t *_modelAdapterSrc_Template =
L"\n\
// std library support needed\n\
#include <vector>\n\
\n\
// Public Property Model Interfaces\n\
#include <pmod/library/type_adapter_base.h>\n\
#include <pmod/library/library_util.h>\n\
#include <foundation/pv_util.h>\n\
#include <foundation/library/library_util.h>\n\
\n\
#include \"{0}\"\n\
\n\
{3}\n\
static const {5}::library::_TypeAdapterEntry _modelFactoryEntries [] = {\n\
{4}\n\
    { foundation_GUID_NULL, foundation_GUID_NULL, 0L }\n\
};\n\
\n\
static HRESULT CreateModelTypeAdapter(\n\
    _COM_Outptr_ foundation::library::IInterfaceAdapterFactory **ppModelAdapterFactory)\n\
{\n\
    return {5}::library::CreateTypeAdapterFactory(_modelFactoryEntries,ppModelAdapterFactory);\n\
}\n\
\n\
HRESULT  STDAPICALLTYPE {2}::RegisterModelTypeAdapters()\n\
{\n\
    foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spModelAdapterFactory;\n\
    IFR_ASSERT(CreateModelTypeAdapter(spModelAdapterFactory.GetAddressOf()));\n\
    return foundation::library::RegisterInterfaceAdapterFactory(spModelAdapterFactory);\n\
}\n\
\n\
";

// Format template for IEnumerable_impl declare
// {0} = Interface Name
// {1} = Item Type
// {2} = Collection Model Info
// {3} = Public Namespace
const wchar_t *_enumerableImplTypedef_Template =
L"\
typedef {3}::IEnumerable_impl<{1},{2}::IIDType> {0};\n\
";

// Format template for IObservableCollection_impl declare
// {0} = Interface Name
// {1} = Item Type
// {2} = Collection Model Info
// {3} = Public Namespace
const wchar_t *_collectionModelTypedef_Template = 
L"\
typedef {3}::IObservableCollection_impl<{1},{2}::IIDType> {0};\n\
";

// Format template for collection_model_AdapterBase declare
// {0} = Interface Name
// {1} = Item Type
// {2} = Collection Model Info
// {3} = Namespace
// {4} = Public Namespace of Adapter
// {5} = Foundation Namespace
// {6} = Adapter class name
const wchar_t *_collectionModelTypeAdapter_Template =
L"\
typedef {4}::library::{6}<{1}*,{3}::{0},&{3}::{2}::IIDType,{5}::_InspectableConverter > C{2}_Adapter;\n\
";
const wchar_t *_collectionEnumTypeAdapter_Template =
L"\
 typedef {4}::library::{6}<{1},{3}::{0},&{3}::{2}::IIDType,{5}::_PropertyValueConverter > C{2}_Adapter;\n\
 ";

// {0} = Interface Name
// {1} = Command Model Info
// {2} = Core Namespace
const wchar_t *_simpleCommandModelTypedef_Template =
L"\
typedef {2}::ISimpleCommand_impl<{1}::IIDType> {0};\n\
";

// {0} = Interface Name
// {1} = Parameter Type
// {2} = Command Model Info
// {3} = Core Namespace
const wchar_t *_commandModelParameterTypedef_Template = 
L"\
typedef {3}::ICommandParameter_impl<{1},{2}::IIDType> {0};\n\
";

// {0} = Interface Name
// {1} = Result Type
// {2} = Command Model Info
// {3} = Core Namespace
const wchar_t *_commandModelResultTypedef_Template = 
L"\
typedef {3}::ICommandResult_impl<{1},{2}::IIDType> {0};\n\
";

// {0} = Interface Name
// {1} = Parameter Type
// {2} = Result Type
// {3} = Command Model Info
// {4} = Core Namespace
const wchar_t *_commandModelTypedef_Template = 
L"\
typedef {4}::ICommand_impl<{1},{2},{3}::IIDType> {0};\n\
";

// {0} = Interface Name
// {1} = Parameter Type
// {2} = Command Model Info
// {3} = Namespace
// {4} = Core Namespace
// {5} = Type Converter
const wchar_t *_commandModelParameterTypeAdapter_Template = 
L"\
typedef {4}::library::_CommandParameterAdapterBase<{1},{3}::{0},&{3}::{2}::IIDType,{5} > C{2}_Adapter;\n\
";

// {0} = Interface Name
// {1} = Result Type
// {2} = Command Model Info
// {3} = Namespace
// {4} = Core Namespace
// {5} = Type Converter
const wchar_t *_commandModelResultTypeAdapter_Template = 
L"\
typedef {4}::library::_CommandResultAdapterBase<{1},{3}::{0},&{3}::{2}::IIDType,{5} > C{2}_Adapter;\n\
";

// {0} = Interface Name
// {1} = Parameter Type
// {2} = Result Type
// {3} = Command Model Info
// {4} = Namespace
// {5} = Core Namespace
// {6} = Parameter Type Converter
// {7} = Result Type Converter
const wchar_t *_commandModelTypeAdapter_Template = 
L"\
typedef {5}::library::_CommandAdapterBase<{1},{2},{4}::{0},&{4}::{3}::IIDType,{6},{7} > C{3}_Adapter;\n\
";

// Format template for IAsyncOperation_impl declare
// {0} = Interface Name
// {1} = Result Type
// {2} = Async Model Info
// {3} = Foundation Namespace
const wchar_t *_asyncOperationModelTypedef_Template = 
L"\
typedef {3}::IAsyncOperation_impl<{1},{2}::IIDType> {0};\n\
";

// Format template for _AsyncOperationAdapterBase declare
// {0} = Interface Name
// {1} = Result Type
// {2} = Async Model Info
// {3} = Namespace
// {4} = Foundation namespace
// {4} = Value Converter
const wchar_t *_asyncOperationModelTypeAdapter_Template =
L"\
typedef {4}::_AsyncOperationAdapterBase<{1},{3}::{0},&{3}::{2}::IIDType {5}> C{2}_Adapter;\n\
";


// {0} = Model Info
// {1} = Namespace
// {2} = Core Namespace
const wchar_t *_modelTypeAdapterEntry_Template =
L"\
    \t{ {1}::{0}::IIDType,{1}::{0}::IIDType,{2}::CreateAdapter<C{0}_Adapter>},\n\
";

const wchar_t *_propertyDataAdapter_Template =
L"\
template <class T, const IID* piid>\n\
class _{0}PropertyAdapter :\n\
    public {1}\n\
{\n\
";


const wchar_t *_propertyDataAdapterTypedef_Template =
L"\
typedef _{0}PropertyAdapter<{1}::{2}, &{1}::{0}::IIDType> C{0}_Adapter;\n\
";

void CGenerateModelAdapter::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
    _modelAdapterApi = getStringParameter(parameters, L"modeladapterapi", nullptr);
}

void CGenerateModelAdapter::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "modeladapterapi:API keyword if export/import is supported" << std::endl;
    os << "modelAdapterInc: Adapter header file" << std::endl;
    os << "modelAdapterSrc: Adapter source file" << std::endl;
    os << "modelIncReferences: list of Type Info Ref header files" << std::endl;
    os << "modelAdapterIncReferences: list of Adapter Ref header files" << std::endl;
}

void CGenerateModelAdapter::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring modelAdapterIncPath = getStringParameter(parameters, L"modelAdapterInc", nullptr);
    std::wstring modelAdapterSrcPath = getStringParameter(parameters, L"modelAdapterSrc", nullptr);

    std::wostringstream osModelIncReferences;
    emitIncludeReferences(osModelIncReferences,parameters,L"modelIncReferences");
    emitIncludeReferences(osModelIncReferences,parameters,L"modelAdapterIncReferences");

    auto namespaceDeclare = toNativeNamespaceDeclare(this->getNativeNamespace());
    auto namespaceEnd = toNativeNamespaceEnd(this->getNativeNamespace());

    std::wstring modelAdapterIncContent = FormatW(_modelAdapterInc_Template,
        _modelAdapterApi.data(),    // {0}
        namespaceDeclare.data(),// {1}
        osModelIncReferences.str().data(),      // {2}
        getCoreNamespace(),                     // {3}
        _osModelAdapterForwardInc.str().data(), // {4}
        _osModelAdapterCommandForwardInc.str().data(), // {5}
        _osModelAdapterInc.str().data(),        // {6},
        namespaceEnd.data(),                     // {7}
        nullptr);

    // generate Model Adapter Header File
    std::wofstream fileModelAdapterInc(ToUtf8String(modelAdapterIncPath.data()),std::ios_base::out);
    fileModelAdapterInc << modelAdapterIncContent;
    fileModelAdapterInc.close();

    // generate Model Source File
    std::wstring modelAdapterSrcContent = FormatW(_modelAdapterSrc_Template,
        getFilePath(modelAdapterIncPath).data(),    // {0}
        getCoreNamespace(),                     // {1}
        this->getNativeNamespace().data(),            // {2}
        _osModelAdapterSrc.str().data(),            // {3}
        _osModelAdapterFactoryEntries.str().data(),        // {4}
        getFoundationNamespace(),
        nullptr);

    std::wofstream fileModelAdapterSrc(ToUtf8String(modelAdapterSrcPath.data()),std::ios_base::out);
    fileModelAdapterSrc << modelAdapterSrcContent;
    fileModelAdapterSrc.close();
}

std::wstring CGenerateModelAdapter::toTypeInfoConverter(const schema::CPropertyTypeInfo *pPropertyTypeInfo)
{
    std::wstring typeConverter = getFoundationNamespace();
    typeConverter += L"::";

    const schema::CTypeInfo *pModelTypeInfo = pPropertyTypeInfo->getModelType();
    if ((pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Inspectable) ||
        (pModelTypeInfo != nullptr && pModelTypeInfo->getModelType() != schema::ModelTypeInfoType_Enum) ||
        ((pPropertyTypeInfo->getPropertyType() & 0x400) != 0))
    {
        typeConverter += L"_InspectableConverter";
    }
    else
    {
        typeConverter += L"_PropertyValueConverter";
    }
    return typeConverter;
}

std::wstring CGenerateModelAdapter::toTypeInfoConverter(const schema::CPropertyTypeInfo *pPropertyTypeInfo,bool isAsync)
{
    if(isAsync)
    {
        std::wstring typeConverter = getFoundationNamespace();
        typeConverter += L"::";
        typeConverter += L"_InspectableConverter";
        return typeConverter;
    }
    else
    {
        return toTypeInfoConverter(pPropertyTypeInfo);
    }
}

void CGenerateModelAdapter::codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo)
{
    if(pAsyncOperationModelInfo->getResultTypeInfo()->getModelType() == nullptr)
    {
        // the Item Type on the schema is a supported type on the 'Core' library so we don't need to declare it
        return;
    }

    std::wstring resultType = toFullName(pAsyncOperationModelInfo->getResultTypeInfo()->getModelType());
    bool isEnum = isPropertyTypeInfoEnum(pAsyncOperationModelInfo->getResultTypeInfo());

    std::wstring resultTypeTemplateType = isEnum ? resultType : resultType + L"*";
    std::wstring modelInfoTypeName = getModelTypeInfoIncName(pAsyncOperationModelInfo);

    _osModelAdapterForwardInc << FormatW(_asyncOperationModelTypedef_Template,
        pAsyncOperationModelInfo->getName().c_str(),
        resultTypeTemplateType.c_str(),
        modelInfoTypeName.c_str(),
        getFoundationNamespace(), 
        nullptr);

    _osModelAdapterSrc << FormatW(_asyncOperationModelTypeAdapter_Template,
        pAsyncOperationModelInfo->getName().c_str(),
        resultTypeTemplateType.c_str(),
        modelInfoTypeName.c_str(),
        this->getNamespace().c_str(),
        getFoundationNamespace(),
        isPropertyTypeInfoEnum(pAsyncOperationModelInfo->getResultTypeInfo()) ?
            L"":
            FormatW(L",{0}::_InspectableConverter",getFoundationNamespace(),nullptr).c_str(),
        nullptr);

    _osModelAdapterFactoryEntries << FormatW(_modelTypeAdapterEntry_Template,
        modelInfoTypeName.c_str(),
        this->getNativeNamespace().c_str(),
        getFoundationNamespace(),
        nullptr);
}

void CGenerateModelAdapter::codeGenerate(const schema::CCommandTypeInfo *pCommmandModelInfo)
{
    if (isCoreCommand(pCommmandModelInfo))
    {
        return;
    }

    std::wstring interfaceName = pCommmandModelInfo->getName();
    std::wstring modelTypeInfoName = getModelTypeInfoIncName(pCommmandModelInfo);

    const bool isAsync = pCommmandModelInfo->getIsAsync();

    const bool hasParameter = pCommmandModelInfo->getParameterTypeInfo()->getPropertyType() != foundation::PropertyType_Empty;
    const bool isArrayParam = (hasParameter && (pCommmandModelInfo->getParameterTypeInfo()->getPropertyType() & 0x400) != 0);

    std::wstring paramType = (
        !hasParameter || isArrayParam ?
        L"foundation::IInspectable*" :
        toPropertyTypeInfo(pCommmandModelInfo->getParameterTypeInfo()).c_str());

    const bool hasResult = isAsync || pCommmandModelInfo->getResultTypeInfo()->getPropertyType() != foundation::PropertyType_Empty;
    std::wstring resultType;
    if (hasResult)
    {
        resultType = toPropertyTypeInfoAsync(pCommmandModelInfo->getResultTypeInfo(), isAsync);
        if (isAsync)
        {
            resultType += L"*";
        }
    }

    if (hasParameter && !hasResult)
    {
        _osModelAdapterCommandForwardInc << FormatW(_commandModelParameterTypedef_Template,
            interfaceName.c_str(),
            paramType.c_str(),
            modelTypeInfoName.c_str(),
            getCoreNamespace(), 
            nullptr);
        _osModelAdapterSrc << FormatW(_commandModelParameterTypeAdapter_Template,
            interfaceName.c_str(),
            paramType.c_str(),
            modelTypeInfoName.c_str(),
            this->getNamespace().c_str(),
            getCoreNamespace(),
            toTypeInfoConverter(pCommmandModelInfo->getParameterTypeInfo()).c_str(),
            nullptr);
    }
    else if (!hasParameter && hasResult)
    {
        _osModelAdapterCommandForwardInc << FormatW(_commandModelResultTypedef_Template,
            interfaceName.c_str(),
            resultType.c_str(),
            modelTypeInfoName.c_str(),
            getCoreNamespace(), 
            nullptr);
        _osModelAdapterSrc << FormatW(_commandModelResultTypeAdapter_Template,
            interfaceName.c_str(),
            resultType.c_str(),
            modelTypeInfoName.c_str(),
            this->getNamespace().c_str(),
            getCoreNamespace(),
            toTypeInfoConverter(pCommmandModelInfo->getResultTypeInfo(), isAsync).c_str(),
            nullptr);
    }
    else if (hasParameter && hasResult)
    {
        _osModelAdapterCommandForwardInc << FormatW(_commandModelTypedef_Template,
            interfaceName.c_str(),
            paramType.c_str(),
            resultType.c_str(),
            modelTypeInfoName.c_str(),
            getCoreNamespace(), 
            nullptr);
        _osModelAdapterSrc << FormatW(_commandModelTypeAdapter_Template,
            interfaceName.c_str(),
            paramType.c_str(),
            resultType.c_str(),
            modelTypeInfoName.c_str(),
            this->getNamespace().c_str(),
            getCoreNamespace(),
            toTypeInfoConverter(pCommmandModelInfo->getParameterTypeInfo()).c_str(),
            toTypeInfoConverter(pCommmandModelInfo->getResultTypeInfo(), isAsync).c_str(),
            nullptr);
    }
    else
    {
     assert(pCommmandModelInfo->getIsUnknownState());
     _osModelAdapterCommandForwardInc << FormatW(_simpleCommandModelTypedef_Template,
         interfaceName.c_str(),
         modelTypeInfoName.c_str(),
         getCoreNamespace(),
         nullptr);
    }

    if (hasParameter || hasResult)
    {
        _osModelAdapterFactoryEntries << FormatW(_modelTypeAdapterEntry_Template,
            modelTypeInfoName.c_str(),
            this->getNamespace().c_str(),
            getFoundationNamespace(),
            nullptr);
    }
}

void CGenerateModelAdapter::codeGenerate(const schema::CEnumerableTypeInfo *pEnumerationTypeInfo)
{
    const schema::CPropertyTypeInfo *pItemTypeInfo = pEnumerationTypeInfo->getItemTypeInfo();
    if (pItemTypeInfo->getModelType() == nullptr)
    {
        // the Item Type on the schema is a supported type on the 'Core' library so we don't need to declare it
        return;
    }
    schema::ModelTypeInfoType typeInfoType = pEnumerationTypeInfo->getModelType();

    std::wstring itemType = toFullName(pItemTypeInfo->getModelType());
    std::wstring modelInfoTypeName = getModelTypeInfoIncName(pEnumerationTypeInfo);

    _osModelAdapterForwardInc << FormatW(
        typeInfoType == schema::ModelTypeInfoType_Enumerable ? _enumerableImplTypedef_Template : _collectionModelTypedef_Template,
        pEnumerationTypeInfo->getName().c_str(),
        isPropertyTypeInfoEnum(pItemTypeInfo) ? itemType.c_str() : (itemType + L" *").c_str(),
        modelInfoTypeName.c_str(),
        typeInfoType == schema::ModelTypeInfoType_Enumerable ? getFoundationNamespace() : getCoreNamespace(),
        nullptr);


    _osModelAdapterSrc << FormatW(
        isPropertyTypeInfoEnum(pItemTypeInfo) ?
        _collectionEnumTypeAdapter_Template:
        _collectionModelTypeAdapter_Template,
        pEnumerationTypeInfo->getName().c_str(),
        itemType.c_str(),
        modelInfoTypeName.c_str(),
        this->getNamespace().c_str(),
        typeInfoType == schema::ModelTypeInfoType_Enumerable ? getFoundationNamespace() : getCoreNamespace(),
        getFoundationNamespace(),
        typeInfoType == schema::ModelTypeInfoType_Enumerable ? L"_EnumerableAdapterBase" : L"_ObservableCollectionAdapterBase",
        nullptr);

    _osModelAdapterFactoryEntries << FormatW(_modelTypeAdapterEntry_Template,
        modelInfoTypeName.c_str(),
        this->getNamespace().c_str(),
        getFoundationNamespace(),
        nullptr);
}

void CGenerateModelAdapter::emitPropertyInfo(
    const std::wstring& typeInfoName,
    const schema::CPropertyTypeInfo *pTypeInfo,
    const std::wstring& name,
    UINT32 flags)
{
    bool isArray = (pTypeInfo->getPropertyType() & 0x400) != 0;

    if ((flags & PropertyFlagType_CanRead))
    {
        // Emit Header Adapter
        _osModelAdapterInc << "\tvirtual HRESULT Get" << name << "(";
        emitDeclarePropertyTypeInfo(_osModelAdapterInc, pTypeInfo, true);
        _osModelAdapterInc << "* value) = 0;" << std::endl;
        // Emit Source Adapter
        _osModelAdapterSrc << "\tHRESULT Get" << name << "(";
        emitDeclarePropertyTypeInfo(_osModelAdapterSrc, pTypeInfo, true);
        _osModelAdapterSrc << "* value)" << std::endl;
        _osModelAdapterSrc << "\t{" << std::endl;

        _osModelAdapterSrc << FormatW(
            L"\t\treturn this->_GetPropertyValue({0}::{1}::Property_{2}{3}, value);",
            this->getNativeNamespace().data(),
            typeInfoName.data(),
            name.data(),
            isArray ? L", size":L"",
            nullptr) << std::endl;

        _osModelAdapterSrc << "\t}" << std::endl;

    }
    if ((flags & PropertyFlagType_CanWrite))
    {
        // Emit Header Adapter
        _osModelAdapterInc << "\tvirtual HRESULT Set" << name << "(";
        emitDeclarePropertyTypeInfo(_osModelAdapterInc, pTypeInfo, false);
        _osModelAdapterInc << " value) = 0;" << std::endl;
        // Emit Source Adapter
        _osModelAdapterSrc << "\tHRESULT Set" << name << "(";
        emitDeclarePropertyTypeInfo(_osModelAdapterSrc, pTypeInfo, false);
        _osModelAdapterSrc << " value)" << std::endl;
        _osModelAdapterSrc << "\t{" << std::endl;

        _osModelAdapterSrc << FormatW(
            L"\t\treturn this->_SetPropertyValue({0}::{1}::Property_{2}{3}, value);",
            this->getNativeNamespace().data(),
            typeInfoName.data(),
            name.data(),
            isArray ? L", size" : L"",
            nullptr) << std::endl;

        _osModelAdapterSrc << "\t}" << std::endl;
    }
}

void CGenerateModelAdapter::emitMethods(
    const std::wstring& typeInfoName,
    const schema::CObjectDispatchInfo *pObjectDispatchInfo)
{
    // Iterate Methods
    for (schema::_MethodIteratorType::const_iterator iter = pObjectDispatchInfo->getMethods().begin();
        iter != pObjectDispatchInfo->getMethods().end();
        ++iter)
    {
        bool isAsync = (*iter)->getIsAsync();
        bool hasResult = (*iter)->getResultType() && (*iter)->getResultType()->getPropertyType() != foundation::PropertyType_Empty;

        // Emit Header Adapter
        _osModelAdapterInc << "\tvirtual ";
        emitMethodDeclare(_osModelAdapterInc, *iter);
        _osModelAdapterInc << " = 0;" << std::endl;
        // Emit Source Adapter
        _osModelAdapterSrc << "\t";
        emitMethodDeclare(_osModelAdapterSrc, *iter);
        _osModelAdapterSrc << std::endl;
        _osModelAdapterSrc << "\t{" << std::endl;
        _osModelAdapterSrc << "\t\tHRESULT hr = S_OK;" << std::endl;
        _osModelAdapterSrc << "\t\tfoundation::InspectablePtr result;" << std::endl;
        _osModelAdapterSrc << "\t\tstd::vector< foundation::InspectablePtr > params;" << std::endl;

        int parameterIndex = 1;

        for (schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            _osModelAdapterSrc
                << "\t\tparams.push_back(" << getFoundationNamespace() << "::CreateValue(";
            if ((*iterParam)->getPropertyType() & 0x400)
            {
                _osModelAdapterSrc << "size_" << parameterIndex << ",";
            }
            _osModelAdapterSrc << (*iterParam)->getParameterName() << "));" << std::endl;
            ++parameterIndex;
        }

        bool isComOutPtr = isAsync || (hasResult && isPropertyTypeInfoObject((*iter)->getResultType()));

        _osModelAdapterSrc << "\t\t";
        if (isComOutPtr)
        {
            _osModelAdapterSrc << "_IFHR_COM_OUT_PTR_";
        }
        else
        {
            _osModelAdapterSrc << "_IFHR_";
        }
        _osModelAdapterSrc << "(this->InvokeMethod(" << this->getNativeNamespace() << "::" << typeInfoName << "::Method_" << (*iter)->getName()
            << ",(UINT32)params.size(),params.size() ? params.front().GetAddressOfPtr():nullptr,result.GetAddressOf())";
        if (isComOutPtr)
        {
            _osModelAdapterSrc << ",pResult";
        }
        _osModelAdapterSrc << ");" << std::endl;

        if (hasResult || isAsync)
        {
            if (isAsync   // if Async
                ||
                isPropertyTypeInfoObject((*iter)->getResultType()) // or IInspectable type
                )
            {
                _osModelAdapterSrc << "\t\t_IFHR_(" << getFoundationNamespace() << "::QueryInterface";
                if (!isAsync)
                {
                    _osModelAdapterSrc << "If";
                }
                _osModelAdapterSrc << "(result,";
                // emit IID
                if (isAsync)
                {
                    _osModelAdapterSrc << toPropertyTypeInfoAsync((*iter)->getResultType(), true) << "::GetIID()";
                }
                else if ((*iter)->getResultType()->getModelType())
                {
                    this->emitModelTypeInfo(_osModelAdapterSrc, (*iter)->getResultType()->getModelType());
                    _osModelAdapterSrc << "::GetIID()";
                }
                else
                {
                    _osModelAdapterSrc << "foundation_IID_IInspectable";
                }
                _osModelAdapterSrc << ",pResult));" << std::endl;
            }
            else
            {
                bool isResultArray = ((*iter)->getResultType()->getPropertyType() & 0x400) != 0;
                _osModelAdapterSrc << "\t\t_IFHR_(" << getFoundationNamespace() << "::pv_util::GetValue(result,";
                if (isResultArray)
                {
                    _osModelAdapterSrc << "size,";
                }
                _osModelAdapterSrc << "pResult));" << std::endl;
            }
        }

        _osModelAdapterSrc << "\t\treturn S_OK;" << std::endl;
        _osModelAdapterSrc << "\t}" << std::endl;
    }
}

void CGenerateModelAdapter::codeGenerate(const schema::CObjectDispatchInfo *pObjectDispatchInfo)
{
    std::wstring typeInfoName = toTypeInfoName(pObjectDispatchInfo->getName());
    _osModelAdapterForwardInc << "struct " << pObjectDispatchInfo->getName() << ";" << std::endl;

    _osModelAdapterInc <<
        FormatW(
            L"BEGIN_DECLARE_CLASS_IID({0}, {1}, {2}::IIDType)",
            pObjectDispatchInfo->getName().data(),
            L"foundation::IObjectDispatch",
            typeInfoName.data(),
            nullptr) << std::endl;

    _osModelAdapterSrc <<
        FormatW(
            _propertyDataAdapter_Template,
            typeInfoName.data(),
            L"foundation::library::_ObjectDispatchAdapterBase<T,piid>",
            nullptr) << std::endl;

    // Iterate Properties
    for (schema::_PropertyIteratorType::const_iterator iter = pObjectDispatchInfo->getProperties().begin();
    iter != pObjectDispatchInfo->getProperties().end();
        ++iter)
    {
        emitPropertyInfo(
            typeInfoName,
            (*iter),
            (*iter)->getName(),
            (*iter)->getFlags());
    }

    // Iterate Methods
    emitMethods(typeInfoName, pObjectDispatchInfo);

    _osModelAdapterInc << "END_DECLARE_INTERFACE()" << std::endl;
    _osModelAdapterSrc << "};" << std::endl;
    _osModelAdapterSrc <<
        FormatW(
            _propertyDataAdapterTypedef_Template,
            typeInfoName.data(),
            this->getNativeNamespace().data(),
            pObjectDispatchInfo->getName().data(),
            nullptr);

    _osModelAdapterFactoryEntries << FormatW(_modelTypeAdapterEntry_Template,
        typeInfoName.c_str(),
        this->getNativeNamespace().c_str(),
        getFoundationNamespace(),
        nullptr);
}

void CGenerateModelAdapter::codeGenerate(const schema::CObservableObjectInfo *pObservableObjectInfo)
{
    bool isViewModel = pObservableObjectInfo->getModelType() == schema::ModelTypeInfoType_ViewModel;

    _osModelAdapterForwardInc << "struct " << pObservableObjectInfo->getName() << ";" << std::endl;

    std::wstring typeInfoName = toTypeInfoName(pObservableObjectInfo->getName());

    // Emit Header
    _osModelAdapterInc << "BEGIN_DECLARE_CLASS_IID(" << pObservableObjectInfo->getName() << ",";
    // Emit Source
    std::wstring modelAdapterTemplate = L"T" + typeInfoName + L"ModelAdapter";
    std::wstring modelAdapterClass = L"C" + typeInfoName + L"ModelAdapter";

    _osModelAdapterSrc << "template <class T,const IID* piid>" << std::endl;
    _osModelAdapterSrc << "class " << modelAdapterTemplate << ":" << std::endl;
    _osModelAdapterSrc << "\tpublic ";
    if(pObservableObjectInfo->getBaseType())
    {
        _osModelAdapterSrc << "T" << toTypeInfoName(pObservableObjectInfo->getBaseType()->getName()) << L"ModelAdapter";
    }
    else
    {
        _osModelAdapterSrc << getCoreNamespace() << "::library::_ObservableObjectAdapterBase";
    }
    _osModelAdapterSrc << "<T,piid>" << std::endl;
    _osModelAdapterSrc << "{" << std::endl << "protected:" << std::endl;

    if(pObservableObjectInfo->getBaseType())
    {
        _osModelAdapterInc  << this->getNativeNamespaceLookup(pObservableObjectInfo->getBaseType()->getParent().getNamespace());
        _osModelAdapterInc << "::" << pObservableObjectInfo->getBaseType()->getName();
    }
    else
    {
        _osModelAdapterInc << getCoreNamespace() << "::IObservableObject";
    }
    _osModelAdapterInc << "," << typeInfoName << "::IIDType)" << std::endl;

    if(isViewModel)
    {
        const schema::CObservableObjectInfo *pSourceType = ((const schema::CPropertyViewModelInfo *)pObservableObjectInfo)->getSourceType();
        // Emit Header
        _osModelAdapterInc << "\tvirtual HRESULT GetSource(";
        this->emitModelTypeInfo(_osModelAdapterInc,pSourceType);
        _osModelAdapterInc << "** ppValue) = 0;" << std::endl;
        // Emit Source
        _osModelAdapterSrc << "\tHRESULT GetSource(";
        this->emitModelTypeInfo(_osModelAdapterSrc,pSourceType);
        _osModelAdapterSrc << "** ppValue)" << std::endl;
        _osModelAdapterSrc << "\t{" << std::endl;
        _osModelAdapterSrc << "\t\treturn this->_GetPropertyValue(" << getCoreNamespace() << "::Property_Source,ppValue);"  << std::endl;
        _osModelAdapterSrc << "\t}" << std::endl;
    }

    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pObservableObjectInfo->getProperties().begin();
        iter != pObservableObjectInfo->getProperties().end();
        ++iter)
    {
        emitPropertyInfo(
            typeInfoName,
            (*iter),
            (*iter)->getName(),
            (*iter)->getFlags());
    }

    // Iterate Methods
    emitMethods(typeInfoName, pObservableObjectInfo);

    _osModelAdapterInc << "END_DECLARE_INTERFACE()"  << std::endl << std::endl;
    _osModelAdapterSrc << "};"  << std::endl << std::endl;
    // define concrete class for this template
    auto _namespace = this->getNativeNamespace();

    _osModelAdapterSrc 
            << "typedef " << modelAdapterTemplate << "<" 
            << _namespace << "::" << pObservableObjectInfo->getName()
            << ",&" << _namespace << "::" << typeInfoName << "::IIDType> "
            << modelAdapterClass << ";" << std::endl << std::endl;
    // add to Adapter Factory entries
    _osModelAdapterFactoryEntries << "\t{ ";
    _osModelAdapterFactoryEntries << _namespace << "::" << typeInfoName << "::IIDType,";
    _osModelAdapterFactoryEntries << _namespace << "::" << typeInfoName << "::IIDType,";
    _osModelAdapterFactoryEntries << getFoundationNamespace() << "::CreateAdapter<" << modelAdapterClass << ">";
    _osModelAdapterFactoryEntries << "}," << std::endl;
}

void CGenerateModelAdapter::emitMethodDeclare(std::wostream& os,const schema::CMethodInfo*pMethodInfo)
{
    os << "HRESULT " << pMethodInfo->getName() << "(";
    int parameterIndex = 1;
    for(schema::_MethodParamIteratorType::const_iterator iter = pMethodInfo->getParameters().begin();
        iter != pMethodInfo->getParameters().end();
        ++iter)
    {
        if(iter != pMethodInfo->getParameters().begin())
        {
            os << ", ";
        }

        emitDeclarePropertyTypeInfo(os, (*iter), false, parameterIndex++);
        os << " " << (*iter)->getParameterName();
    }

    if (pMethodInfo->getIsAsync()
        || (pMethodInfo->getResultType() && pMethodInfo->getResultType()->getPropertyType() != foundation::PropertyType_Empty))
    {
        if(pMethodInfo->getParameters().size())
        {
            os << ", ";
        }

        emitResultTypeInfo(os, pMethodInfo->getResultType(), pMethodInfo->getIsAsync());
        os << " *pResult";
    }

    os << ")";
}

void CGenerateModelAdapter::emitResultTypeInfo(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    bool isAsync)
{
    if(isAsync)
    {
        emitSalOutputTypeInfo(os, pPropertyTypeInfo, true);
        os << toPropertyTypeInfoAsync(pPropertyTypeInfo, true) << "*";
    }
    else
    {
        emitDeclarePropertyTypeInfo(os,pPropertyTypeInfo,true);
    }
}
