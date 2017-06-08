/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateWindowsRuntimeAdapter.cpp
****/

#include "GenerateWindowsRuntimeAdapter.h"
#include "guidhelper.h"
#include "md5Util.h"

// Format template for idl Enum
// {0} = Enum Name
// {1} = Enum Values
const wchar_t * _winRTEnum_Template =
L"\
    [version(NTDDI_WIN8)]\n\
    typedef [v1_enum] enum {0}\n\
    {\n\
{1}\n\
    }{0};\n\
\n\
";

const wchar_t * _winRTRuntimeClass_Template =
L"\
    [version(NTDDI_WIN8)]\n\
	[threading(both)]\n\
	[marshaling_behavior(agile)]\n\
	runtimeclass {0}\n\
    {\n\
        [default] interface {1};\n\
    }\n\
";

// Format template for idl Interface
// {0} = Interface Name
// {1} = IID
// {2} = Base Interface
// {3} = Properties
// {4} = Methods
const wchar_t * _winRTInterface_Template =
L"\
    /////////////////////////////// {0} ////////////////////////////////\n\
    [version(NTDDI_WIN8)]\n\
    [uuid({1})]\n\
    interface {0}: IInspectable requires {2}\n\
    {\n\
        // Properties\n\
{3}\n\
        // Methods\n\
{4}\n\
    };\n\
";

const wchar_t * _getPropertyArrayIdl_Template =
L"\
        [propget] HRESULT {0}([out]UINT32 *length, [out, size_is(,*length),retval] {1}* pValue);\n\
";

const wchar_t * _setPropertyArrayIdl_Template =
L"\
        [propput] HRESULT {0}([in]UINT32 length, [in, size_is(length)] {1} value);\n\
";

const wchar_t * _getPropertyIdl_Template =
L"\
        [propget] HRESULT {0}([out, retval] {1}* pValue);\n\
";

const wchar_t * _setPropertyIdl_Template =
L"\
        [propput] HRESULT {0}([in] {1} value);\n\
";

const wchar_t * _getSourceModelIdl_Template =
L"\
        [propget] HRESULT Source([out, retval] {0}** ppValue);\n\
";

const wchar_t * _methodIdl_Template =
L"\
        HRESULT {0}({1});\n\
";

const wchar_t * _commandModelMethodsIdl_Template =
L"\
        HRESULT Execute({0}{2}{1});\n\
        HRESULT CanExecute({0}{3}[out, retval]boolean* value);\n\
";

const wchar_t * _commandParameterIdl_Template = L"[in]{0} parameter";
const wchar_t * _commandArrayParameterIdl_Template = L"[in]UINT32 size, [in, size_is(size)]{0} values";
const wchar_t * _commandResultIdl_Template = L"[out, retval] {0}* pValue";
const wchar_t * _commandArrayResultIdl_Template = L"[out]UINT32* pSize, [out, retval]{0}* ppValues";

/// <summary>
/// {0}: class Name
/// {1}: interface name
/// {2}: Parameter Type
/// {3}: Result Type 
/// {4}: Execute parameter separator 
/// {5}: CanExecute parameter separator 
/// {6}: 
/// {7}: 
/// </summary>
const wchar_t * _classCommandModelAdapter_Template =
L"\n\
class CMo{0}ModelAdapter: \n\
    public TMoCommandAdapterBase<{1},&__uuidof({1})>\n\
{\n\
protected:\n\
    HRESULT STDMETHODCALLTYPE Execute({2}{4}{3}) override\n\
    {\n\
        foundation::InspectablePtr spParameter;\n\
        {6}\n\
        foundation::InspectablePtr spResult;\n\
        IFR(m_pInner->Execute(spParameter, spResult.GetAddressOf()));\n\
        {7}\n\
        return S_OK;\n\
    }\n\
\n\
    HRESULT STDMETHODCALLTYPE CanExecute({2}{5} boolean* value) override\n\
    {\n\
        foundation::InspectablePtr spParameter;\n\
        {6}     \n\
        bool _bCanExecute;\n\
        IFR(m_pInner->CanExecute(spParameter, &_bCanExecute));\n\
        *value = _bCanExecute ? true : false;\n\
        return S_OK;\n\
    }\n\
};\n\
";

const wchar_t * _classPropertyModelAdapter_Template =
L"\n\
class CMo{0}ModelAdapter: \n\
    public TMoObservableObjectAdapterBase<{1},&__uuidof({1})>\n\
{\n\
protected:\n\
{2}\n\
};\n\
";

const wchar_t * _getProperty_Template =
L"\n\
    HRESULT STDMETHODCALLTYPE get_{0}( \n\
        {1} *pValue) override\n\
    {\n\
        return GetProperty({2}::Property_{0},pValue);\n\
    }\n\
 ";

const wchar_t * _getPropertyArray_Template =
L"\n\
    HRESULT STDMETHODCALLTYPE get_{0}( \n\
        UINT32 *length,\n\
        {1} *pValue) override\n\
    {\n\
        return GetProperty({2}::Property_{0},length,pValue);\n\
    }\n\
 ";

const wchar_t * _getSourceModel_Template =
L"\n\
    HRESULT STDMETHODCALLTYPE get_Source( \n\
        {0} **ppValue) override\n\
    {\n\
        return GetProperty({1}::Property_Source,ppValue);\n\
    }\n\
 ";

const wchar_t * _setPropertyTemplate =
L"\n\
    HRESULT STDMETHODCALLTYPE put_{0}( \n\
        {1} value) override \n\
    {\n\
        return SetProperty({2}::Property_{0},{3}value);\n\
    }\n\
 ";

const wchar_t * _setPropertyArrayTemplate =
L"\n\
    HRESULT STDMETHODCALLTYPE put_{0}( \n\
        UINT32 length,\n\
        {1} value) override \n\
    {\n\
        return SetProperty({2}::Property_{0},length,{3}value);\n\
    }\n\
 ";

const wchar_t * _createParameterAdapter =
L"\n\
        IFHR_ASSERT({3}()->Create{2}({0},parameters[{1}].GetAddressOf()));";


const wchar_t * _returnValueAdapter =
L"\n\
        IFCEXPECT_ASSERT(spResult);\n\
\n\
        IFHR(foundation::pv_util::GetValue(spResult,{1}result));\n\
";

const wchar_t * _returnEnumValueAdapter =
L"\n\
        UINT32 enumValue;\n\
        IFR(foundation::GetValue(spResult,enumValue));\n\
        *result = ({0})enumValue;\n\
";

const wchar_t * _returnModelAdapter =
L"\n\
        if(spResult)\n\
        {\n\
            return  spResult->QueryInterface(__uuidof({0}),(void **)result);\n\
        }\n\
        else\n\
        {\n\
            *result = nullptr;\n\
        }\n\
";

const wchar_t * _methodTemplateAdapter =
L"\n\
    HRESULT STDMETHODCALLTYPE {0}({1}) override\n\
    {\n\
        HRESULT hr = S_OK;\n\
        {7} spResult;\n\
\n\
        {4}\n\
        IFHR(InvokeMethod({3}::Method_{0},{2},{5},spResult.GetAddressOf()));\n\
{6}\n\
        return S_OK;\n\
    }\n\
 ";

const wchar_t * _modelAdapterEntryFactory_Template =
L"\n\
       { __uuidof({0}),{1}::IIDType, {2}::CreateAdapter<CMo{1}ModelAdapter> },";

const wchar_t * _modelAdapterEntryFactory2_Template =
L"\n\
        { __uuidof({0}),{3}::IIDType, {2}::CreateAdapter<CMo{1}ModelAdapter> },";

const wchar_t * _forwardVectorIdlDeclare_Template =
L"      interface Windows.Foundation.Collections.IVector<{0}>;\n\
";

const wchar_t * _forwardIterableIdlDeclare_Template =
L"      interface Windows.Foundation.Collections.IIterable<{0}>;\n\
";

const wchar_t * _forwardEnumIdlDeclare_Template =
L"      interface Windows.Foundation.IReference<{0}>;\n\
 ";

const wchar_t * _forwardAsyncOperationIdlDeclare_Template =
L"      interface {0};\n\
";


const wchar_t * _classCollectionModelAdapter_Template =
L"\n\
typedef TInspectableMoObservableCollectionAdapterBase<{0} *> CMo{1}ModelAdapter;\n\
";
const wchar_t * _classCollectionEnumAdapter_Template =
L"\n\
 typedef TPropertyValueMoObservableCollectionAdapterBase<{0}> CMo{1}ModelAdapter;\n\
 ";

const wchar_t * _vectorIdlDeclare_Template = 
          L"Windows.Foundation.Collections.IVector<{0}>";

const wchar_t * _iterableIdlDeclare_Template =
          L"Windows.Foundation.Collections.IIterable<{0}>";

const wchar_t * _vectorCollectionType =
          L"ABI::Windows::Foundation::Collections::IVector<{0}>";
const wchar_t * _iterableCollectionType =
          L"ABI::Windows::Foundation::Collections::IIterable<{0}>";
const wchar_t * _vectorViewCollectionType =
          L"ABI::Windows::Foundation::Collections::IVectorView<{0}>";

const wchar_t * _vectorAbiDeclareType =
          L"ABI::Windows::Foundation::Collections::__FIVector_1_{0}_t";

const wchar_t * _iterableAbiDeclareType =
          L"ABI::Windows::Foundation::Collections::__FIIterable_1_{0}_t";

const wchar_t * _idlDeclareGeneric_Template =
L"\
    // Declare Generic {1}\n\
    declare\n\
    {\n\
{0}\n\
    }\n\
";

const wchar_t * _enumValueTypedefAdapter_Template =
L"typedef TMoEnumValueAdapterBase<ABI::Windows::Foundation::IReference<enum ABI::{1}::{0}>, ABI::{1}::{0}> CMo{0}_EnumValue;\n";

const wchar_t * _enumValueAdapterEntry_Template =
L"{ __uuidof(ABI::Windows::Foundation::IReference<enum ABI::{1}::{0}>), {0}Enum::IIDType, foundation::CreateAdapter<CMo{0}_EnumValue> },\n";

const wchar_t * _asyncOperationTypedefAdapter_Template =
L"typedef TMoAsyncOperationAdapterBase<\n\
    {0},\n\
    {1}\n\
> CMo{2};\n";

const wchar_t * _asyncOperationAdapterEntry_Template =
L"\n{ __uuidof({0}), \n\
    {1}::IIDType, \n\
    foundation::CreateAdapter<CMo{1}> },\n";

// Format template for Model WinRT IDL File
// {0} = Idl Namespace
// {1} = Forwards Declare
// {2} = Enum Types
// {3} = generic collections
// {4} = interface types
const wchar_t * _idlSource_Template =
L"\
#include \"sdkddkver.h\"\n\
\n\
namespace {0}\n\
{\n\
\n\
    // Forward declarations\n\
{1}\n\
\n\
    // Enum Types declarations\n\
{2}\n\
\n\
{3}\n\
\n\
{4}\n\
\n\
{5}\n\
    // Interface Types declarations\n\
{6}\n\
    // Runtime Types declarations\n\
{7}\n\
\n\
}\n\
";

const wchar_t * _modelAdapterSource_Template =
L"\
\n\
// Idl Output Include\n\
{4}\n\
#include <pmod/library/library_util.h>\n\
\n\
#include <winrt/MoObservableObjectAdapterBase.h>\n\
#include <winrt/MoObservableCollectionAdapterBase.h>\n\
#include <winrt/MoCommandAdapterBase.h>\n\
#include <winrt/MoEnumValueAdapterBase.h>\n\
#include <winrt/MoAsyncOperationAdapterBase.h>\n\
\n\
// Type Infos generated Includes\n\
{5}\n\
\n\
using namespace {6};\n\
\n\
    // Model Adapter classes\n\
{0}\n\
\n\
    // Model  Factory Entries\n\
    static const {2}::library::_TypeAdapterEntry {3} [] = {\n\
{1}\n\
    { GUID_NULL, GUID_NULL, 0L }\n\
};\n\
\n\
{7}\n\
HRESULT RegisterModernTypeAdapters()\n\
{\n\
	foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spInterfaceAdapterFactory; \n\
	IFR(foundation::library::CreateTypeAdapterFactory({3}, spInterfaceAdapterFactory.GetAddressOf())); \n\
	IFR(foundation::library::RegisterInterfaceAdapterFactory(spInterfaceAdapterFactory)); \n\
	return S_OK; \n\
}\n\
{8}\n\
";

const wchar_t * _abiCoreNamespace = L"ABI::Microsoft::PropertyModel";
const wchar_t * _idlCoreNamespace = L"Microsoft.PropertyModel";

void CGenerateWindowsRuntimeAdapter::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);

    initializeNamespaceLookup(
        parameters,
        U("winrtNamespacelookup"),
        this->_winrtNamespaceLookup,
        nullptr);

    
    _isGenerateGenericVector = getBoolParamater(parameters, L"useIdlGenericVector", true);
    _isGenerateCommand = getBoolParamater(parameters, L"useIdlCommand", true);
    _isGenerateGenericAsyncOperation = getBoolParamater(parameters, L"useIdlGenericAsyncOperation", true);
    if (!_isGenerateCommand)
    {
        assert(false);
        throw std::runtime_error("Windows runtime generation w/o generated commands is not implemented.");
    }
}

void CGenerateWindowsRuntimeAdapter::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "modelAdapterIdl: Adapter Idl file" << std::endl;
    os << "modelAdapterSrc: Adapter source file" << std::endl;
    os << "useIdlGeneric: (true/false) Idl generic support" << std::endl;
    os << "useIdlCommand: (true/false) if generate custom command" << std::endl;
}

void CGenerateWindowsRuntimeAdapter::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring modelAdapterIdlPath = this->replaceNamespace(getStringParameter(parameters, L"modelAdapterIdl", nullptr));
    std::wstring modelAdapterSrcPath = getStringParameter(parameters, L"modelAdapterSrc", nullptr);
    std::wstring modelFactoryEntriesName = getStringParameter(parameters, L"modelFactoryEntries", L"_modelFactoryEntries");

	bool generateRuntimeClasses = getBoolParameter(parameters, L"runtimeClass", true);

	std::wstring nameSpace = this->getNamespace();

    // generate Model Adapter Idl File

    std::wstring genericVectorForwardIdlStr = _osGenericVectorForwardIdl.str();
    std::wstring genericEnumForwardIdlStr = _osGenericEnumForwardIdl.str();
    std::wstring genericAsyncOperationForwardIdlStr = _osGenericAsyncOperationForwardIdl.str();

    std::wofstream fileModelAdapterIdl(ToUtf8String(modelAdapterIdlPath.data()),std::ios_base::out);
    fileModelAdapterIdl << FormatW(_idlSource_Template,
        getNamespaceLookup(this->_winrtNamespaceLookup, nameSpace).data(),
        _osInterfaceForwardIdl.str().data(),
        _osEnumsIdl.str().data(),
        genericVectorForwardIdlStr.size() ?  
		FormatW(_idlDeclareGeneric_Template,genericVectorForwardIdlStr.data(),L"IVector",nullptr).data():L"",
        genericEnumForwardIdlStr.size() ?
		FormatW(_idlDeclareGeneric_Template, genericEnumForwardIdlStr.data(), L"IReference", nullptr).data() : L"",
        genericAsyncOperationForwardIdlStr.size() ?
		FormatW(_idlDeclareGeneric_Template, genericAsyncOperationForwardIdlStr.data(), L"IAsyncOperation", nullptr).data() : L"",
        _osInterfacesIdl.str().data(),
		generateRuntimeClasses ? _osRuntimeClassesIdl.str().data() : L"",
        nullptr);
    fileModelAdapterIdl.close();

    // generate Model Source File
	std::wostringstream osTypeInfoIncReferences;
	emitIncludeReferences(osTypeInfoIncReferences, parameters, L"modelTypeInfoIncReferences");
	if (!osTypeInfoIncReferences.tellp())
	{
		osTypeInfoIncReferences << "#include \"" << nameSpace << "TypeInfo.g.h\"" << std::endl;
	}

	std::wstring defaultIdlIncFileName = FormatW(L"#include \"{0}.h\"", this->getWindowsRuntimeNamespace().c_str(), nullptr);
	std::wstring idlIncFileName = getStringParameter(parameters, L"idlIncFileName", defaultIdlIncFileName.c_str());

    auto namespaceDeclare = toNativeNamespaceDeclare(this->getNativeNamespace());
    auto namespaceEnd = toNativeNamespaceEnd(this->getNativeNamespace());

    std::wofstream fileModelAdapterSrc(ToUtf8String(modelAdapterSrcPath.data()),std::ios_base::out);
    fileModelAdapterSrc << FormatW(_modelAdapterSource_Template,
        _osModelAdapterSrc.str().data(),
        _osModelAdapterFactoryEntries.str().data(),
        getFoundationNamespace(),
        modelFactoryEntriesName.c_str(),
		idlIncFileName.c_str(),
		osTypeInfoIncReferences.str().c_str(),
        toNativeNamespace(nameSpace).c_str(),
        namespaceDeclare.c_str(),
        namespaceEnd.c_str(),
         nullptr);
    fileModelAdapterSrc.close();
}

std::wstring CGenerateWindowsRuntimeAdapter::getWindowsRuntimeNamespace()
{
    return getNamespaceLookup(this->_winrtNamespaceLookup, getNamespace());
}

void CGenerateWindowsRuntimeAdapter::codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo)
{
    if(pEnumTypeInfo->getEnums().size() == 0)
    {
        // skip zero base enum (mostly used in defining categories
        return;
    }
    std::wostringstream osEnumValues;
    osEnumValues << std::hex;

    for(schema::_EnumInfoVectorType::const_iterator iter = pEnumTypeInfo->getEnums().begin();
        iter != pEnumTypeInfo->getEnums().end();
        ++iter)
    {
        osEnumValues << "\t\t" << std::get<0>(*iter) << " = 0x" << std::hex << std::get<1>(*iter) << std::dec << ',' << std::endl;
    }
    _osEnumsIdl << FormatW(_winRTEnum_Template,
            pEnumTypeInfo->getName().data(),
            osEnumValues.str().data(),
            nullptr);

    // generate forward enum declare
    _osGenericEnumForwardIdl << FormatW(_forwardEnumIdlDeclare_Template,
        pEnumTypeInfo->getName().data(),
        nullptr);
    // generate typedef and entries
    std::wstring cpp_name_space = this->getWindowsRuntimeNamespace();
    replace(cpp_name_space, L".", L"::");

    _osModelAdapterSrc << FormatW(
        _enumValueTypedefAdapter_Template,
        pEnumTypeInfo->getName().data(),
        cpp_name_space.c_str(),
        nullptr);

    _osModelAdapterFactoryEntries << FormatW(
        _enumValueAdapterEntry_Template,
        pEnumTypeInfo->getName().data(),
        cpp_name_space.c_str(),
        nullptr);

}

void CGenerateWindowsRuntimeAdapter::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    bool isViewModel = pPropertyModelInfo->getModelType() == ::schema::ModelTypeInfoType_ViewModel;

    _osInterfaceForwardIdl << "\t"  << "interface "<< pPropertyModelInfo->getName() <<";" << std::endl;

    std::wstring typeInfoName = toTypeInfoName(pPropertyModelInfo->getName());

#if 0
	_osRuntimeClassesIdl << FormatW(_winRTRuntimeClass_Template,
		typeInfoName.c_str(),
		pPropertyModelInfo->getName().c_str(),
		nullptr);	
#endif

    std::wostringstream osIdlProperties;
    std::wostringstream osIdlMethods;

    std::wostringstream osIdlPropertiesEnums;
    std::wostringstream osIdlMethodEnums;
    std::wostringstream osIdlEventsEnums;

    std::wostringstream ocClassAdapterContent;

    std::vector<const schema::CObservableObjectInfo *> descendents;
    pPropertyModelInfo->getDescendents(descendents);
    // TODO: we are only generating 'Source' property for 'final'
    // interfaces to avoid the duplication of property names
    if (isViewModel && descendents.size() == 0 )
    {
        const schema::CObservableObjectInfo *pSourceType = ((const schema::CPropertyViewModelInfo *)pPropertyModelInfo)->getSourceType();

        if (!pSourceType->getParent().isFactorySchema())
        {
            // Generate IDL source code
            osIdlProperties << FormatW(_getSourceModelIdl_Template,
                toIdlModelTypeInfo(pSourceType).data(),
                nullptr);

            // Generate Model Adapter source code
            ocClassAdapterContent << FormatW(_getSourceModel_Template,
                toNativeModelTypeName(pSourceType).data(),
                getCoreNamespace(),
                nullptr);
        }
    }

    // Iterate Events
    for(schema::_EventIteratorType::const_iterator iter = pPropertyModelInfo->getEvents().begin();
        iter != pPropertyModelInfo->getEvents().end();
        ++iter)
    {
        // Emit Enum Events
        osIdlEventsEnums << "\t\t" << (*iter)->getName() << " = " << (*iter)->getId() <<  "," << std::endl;
    }

    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        bool isPropertyArray = ((*iter)->getPropertyType() & 0x400) != 0;

        // Emit Enum Properties
        osIdlPropertiesEnums << "\t\t" << (*iter)->getName() << " = ";
        if((*iter)->isParent())
        {
            osIdlPropertiesEnums << "0x2001";
        }
        else
        {
            osIdlPropertiesEnums << (*iter)->getId();
        }
        if(isViewModel)
        {
            osIdlPropertiesEnums << " + 0x1000";
        }
        osIdlPropertiesEnums << "," << std::endl;

        UINT32 flags = (*iter)->getFlags();
        if((flags & PropertyFlagType_CanRead))
        {
            // Idl generation
            osIdlProperties << FormatW(isPropertyArray ? _getPropertyArrayIdl_Template:_getPropertyIdl_Template,
                (*iter)->getName().data(),
                toIdlPropertyTypeInfo(*iter).data(),
                nullptr);
            // class adapter generation
            ocClassAdapterContent << FormatW(isPropertyArray ? _getPropertyArray_Template: _getProperty_Template,
                (*iter)->getName().data(),
                toNativePropertyTypeInfo(*iter).data(),
                typeInfoName.data(),
                nullptr);

        }
        if((flags & PropertyFlagType_CanWrite))
        {
            // Idl generation
            osIdlProperties << FormatW(isPropertyArray ? _setPropertyArrayIdl_Template:_setPropertyIdl_Template,
                (*iter)->getName().data(),
                toIdlPropertyTypeInfo(*iter).data(),
                nullptr);

            std::wstring castValue = L"";
            if((flags & PropertyFlagType_IsEnum) && !isPropertyTypeInfoArray(*iter))
            {
                castValue = L"(UINT32)";
            }
            // class adapter generation
            ocClassAdapterContent << FormatW(isPropertyArray ? _setPropertyArrayTemplate: _setPropertyTemplate,
                (*iter)->getName().data(),
                toNativePropertyTypeInfo(*iter).data(),
                typeInfoName.data(),
                castValue.data(),
                nullptr);
        }
    }

    // Iterate Methods
    for(schema::_MethodIteratorType::const_iterator iter = pPropertyModelInfo->getMethods().begin();
        iter != pPropertyModelInfo->getMethods().end();
        ++iter)
    {
       // Emit Enum Methods
        osIdlMethodEnums << "\t\t" << (*iter)->getName() << " = " << (*iter)->getId();
        if(isViewModel)
        {
            osIdlMethodEnums << " + 0x1000";
        }
        osIdlMethodEnums << "," << std::endl;

        // Emit Idl declare method & Adapter Class Content
        osIdlMethods << "\t\t" << "HRESULT " << (*iter)->getName() << "(";
        ocClassAdapterContent << "\t" << "HRESULT STDMETHODCALLTYPE " << (*iter)->getName() << "(";

        std::wostringstream osAdapterParams;
        if((*iter)->getParameters().size())
        {
            osAdapterParams << "\t\t" << "foundation::InspectablePtr parameters[" << (*iter)->getParameters().size() << "];" << std::endl;

        }

        int parameterIndex = 1;
        for(schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            bool isParamerterArray = ((*iterParam)->getPropertyType() & 0x400) != 0;

            if(iterParam != (*iter)->getParameters().begin())
            {
                osIdlMethods << ",";
                ocClassAdapterContent << ",";
            }
            if(isParamerterArray)
            {
                osIdlMethods << "[in]UINT32 length_" << parameterIndex << ",";
                ocClassAdapterContent << "UINT32 length_" << parameterIndex << ",";
            }

            osIdlMethods << "[in";
            if(isParamerterArray)
            {
                osIdlMethods << ",size_is(length_" << parameterIndex << ")";
            }

            osIdlMethods << "]" << toIdlPropertyTypeInfo(*iterParam) << " " << (*iterParam)->getParameterName();
            ocClassAdapterContent << toNativePropertyTypeInfo(*iterParam) << " " << (*iterParam)->getParameterName();

            if ((*iterParam)->getPropertyType() == foundation::PropertyType_Inspectable)
            {
                osAdapterParams
                    << "\t\tparameters["
                    << (int)(iterParam - (*iter)->getParameters().begin())
                    << "] = " << (*iterParam)->getParameterName() << ";" << std::endl;
            }
            else
            {
                osAdapterParams << "\t\t" << "IFHR_ASSERT(foundation::pv_util::GetPropertyValueStatics()->Create"
                    << toPropertyTypeName((*iterParam)->getPropertyType()) << "(";

                if (isParamerterArray)
                {
                    osAdapterParams << "length_" << parameterIndex << ",";
                }

                // TODO: workaround to force an Enum type to be casted as UINT32
                // the Parameter Info does not give me a way to know a type if an Enum
                if ((*iterParam)->getPropertyType() == foundation::PropertyType_UInt32)
                {
                    osAdapterParams << "(UINT32)";
                }

                if (isParamerterArray && hasPropertyTypeInfoModel(*iterParam))
                {
                    osAdapterParams << "reinterpret_cast<IInspectable **>(" << (*iterParam)->getParameterName() << ")";
                }
                else
                {
                    osAdapterParams << (*iterParam)->getParameterName();
                }
                osAdapterParams
                    << ",parameters["
                    << (int)(iterParam - (*iter)->getParameters().begin())
                    << "].GetAddressOfPtr()));" << std::endl;
            }

            ++parameterIndex;
        }

        bool hasResult = (*iter)->getResultType()->getPropertyType() != foundation::PropertyType::PropertyType_Empty;
        bool isAsync = (*iter)->getIsAsync();
        
        bool isResultArray = ((*iter)->getResultType()->getPropertyType() & 0x400) != 0;
        if (hasResult || isAsync)
        {
            if((*iter)->getParameters().size())
            {
                osIdlMethods << ",";
                ocClassAdapterContent << ",";
            }
            if(isResultArray)
            {
                osIdlMethods << "[out]UINT32 *length,";
                ocClassAdapterContent << "UINT32 *length,";
            }
            osIdlMethods << "[out,";
            if(!(*iter)->getIsAsync() && isResultArray)
            {
                osIdlMethods << "size_is(,*length),";
            }
            osIdlMethods << "retval]" << toIdlResultTypeInfo((*iter)->getResultType(),(*iter)->getIsAsync()) << "*result";
            ocClassAdapterContent << toNativeResultTypeInfo((*iter)->getResultType(),(*iter)->getIsAsync()) << "*result";
        }
        osIdlMethods << ");" << std::endl;
        ocClassAdapterContent << ")" << std::endl;
        ocClassAdapterContent << "\t" << "{" <<std::endl;
        ocClassAdapterContent << "\t\t" << "HRESULT hr = S_OK;" <<std::endl;
        ocClassAdapterContent << "\t\t" << "foundation::InspectablePtr spResult;" <<std::endl;
        ocClassAdapterContent << osAdapterParams.str();
        ocClassAdapterContent << "\t\t" << "IFHR(__super::Invoke(" << typeInfoName << "::Method_" 
            << (*iter)->getName() << "," << (*iter)->getParameters().size() << ",";
        if((*iter)->getParameters().size())
        {
            ocClassAdapterContent << "parameters[0].GetAddressOfPtr()";
        }
        else
        {
            ocClassAdapterContent << "nullptr";
        }
        ocClassAdapterContent << ",spResult.GetAddressOf()));"  <<std::endl;
        if (hasResult || isAsync)
        {
            if((*iter)->getIsAsync())
            {
                ocClassAdapterContent << FormatW(
                    _returnModelAdapter,
                    toNativeAsyncOperation((*iter)->getResultType()).data(),
                    nullptr);
            }
            else if((*iter)->getResultType()->getPropertyType() == foundation::PropertyType::PropertyType_Inspectable)
            {
                ocClassAdapterContent << FormatW(_returnModelAdapter,
                    toNativePropertyTypeInfo((*iter)->getResultType()).data(),
                            nullptr);
            }
            else
            {
                if (isPropertyTypeInfoEnum((*iter)->getResultType()))
                {
                    ocClassAdapterContent << FormatW(_returnEnumValueAdapter,
                        toNativePropertyTypeInfo((*iter)->getResultType()).c_str(),
                        nullptr);
                }
                else
                {
                    ocClassAdapterContent << FormatW(_returnValueAdapter,
                        toPropertyTypeName((*iter)->getResultType()->getPropertyType()).c_str(),
                        isResultArray ? L"length," : L"",
                        nullptr);
                }
            }
        }
        ocClassAdapterContent << "\t\t" << "return S_OK;" <<std::endl;
        ocClassAdapterContent << "\t" << "}" <<std::endl;
    }

    std::wstring idlBaseType;
    if(pPropertyModelInfo->getBaseType())
    {
        idlBaseType = toIdlModelTypeName(pPropertyModelInfo->getBaseType());
    }
    else
    {
        idlBaseType = _idlCoreNamespace;
        idlBaseType += L".IObservableObject";
    }

    _osInterfacesIdl << FormatW(_winRTInterface_Template,
        pPropertyModelInfo->getName().data(),
        toIdlIIDType(pPropertyModelInfo).data(),
        idlBaseType.data(),
        osIdlProperties.str().data(),
        osIdlMethods.str().data(),
        nullptr);

    if(pPropertyModelInfo->getProperties().size())
    {
        _osInterfacesIdl << FormatW(_winRTEnum_Template,
            (typeInfoName + L"Properties").data(),
            osIdlPropertiesEnums.str().data(),
            nullptr);
    }

    if(pPropertyModelInfo->getMethods().size())
    {
        _osInterfacesIdl << FormatW(_winRTEnum_Template,
            (typeInfoName + L"Methods").data(),
            osIdlMethodEnums.str().data(),
            nullptr);
    }

    if(pPropertyModelInfo->getEvents().size())
    {
        _osInterfacesIdl << FormatW(_winRTEnum_Template,
            (typeInfoName + L"Events").data(),
            osIdlEventsEnums.str().data(),
            nullptr);
    }
    
    // Emit Source Adapter Class
    _osModelAdapterSrc << FormatW(
        _classPropertyModelAdapter_Template,
        typeInfoName.data(),
        toNativeModelTypeName(pPropertyModelInfo).data(),
        ocClassAdapterContent.str().data(),
        nullptr);

    // Emit type adapter factory entry
    _osModelAdapterFactoryEntries << FormatW(
        _modelAdapterEntryFactory_Template,
        toNativeModelTypeName(pPropertyModelInfo).data(),
        typeInfoName.data(),
        getFoundationNamespace(),
        nullptr);
}

void CGenerateWindowsRuntimeAdapter::codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo)
{
    if(_isGenerateGenericVector && pCollectionModelInfo->getItemTypeInfo())
    {
        const schema::CTypeInfo *pItemTypeInfo = pCollectionModelInfo->getItemTypeInfo()->getModelType();
        // only Generic vectors for this schema model types
        if (isModelInSchema(pItemTypeInfo))
        {
            schema::ModelTypeInfoType typeInfoType = pCollectionModelInfo->getModelType();

            _osGenericVectorForwardIdl << FormatW(
                typeInfoType == schema::ModelTypeInfoType_Enumerable ? 
                    _forwardIterableIdlDeclare_Template : _forwardVectorIdlDeclare_Template,
                pItemTypeInfo->getName().data(),
                nullptr);

            std::wstring itemTypeNativeName = toNativeModelTypeName(pItemTypeInfo);
            std::wstring typeInfoName = getModelTypeInfoIncName(pCollectionModelInfo);

            _osModelAdapterSrc << FormatW(
                pItemTypeInfo->getModelType() == schema::ModelTypeInfoType_Enum ?
                _classCollectionEnumAdapter_Template:
                _classCollectionModelAdapter_Template,
                itemTypeNativeName.data(),
                typeInfoName.data(),
                nullptr);

            std::wstring itemTypeName = itemTypeNativeName;
            if (pItemTypeInfo->getModelType() != schema::ModelTypeInfoType_Enum)
            {
                itemTypeName += L" *";
            }
            // Add IIterable<> entry
            _osModelAdapterFactoryEntries << FormatW(
                _modelAdapterEntryFactory_Template,
                FormatW(_iterableCollectionType, itemTypeName.data(), nullptr).data(),
                typeInfoName.data(),
                getFoundationNamespace(),
                nullptr);
            if (typeInfoType != schema::ModelTypeInfoType_Enumerable)
            {
                // Add IVector<> entry
                _osModelAdapterFactoryEntries << FormatW(
                    _modelAdapterEntryFactory_Template,
                    FormatW(_vectorCollectionType, itemTypeName.data(), nullptr).data(),
                    typeInfoName.data(),
                    getFoundationNamespace(),
                    nullptr);
                // Add IVectorView<> entry
                _osModelAdapterFactoryEntries << FormatW(
                    _modelAdapterEntryFactory_Template,
                    FormatW(_vectorViewCollectionType, itemTypeName.data(), nullptr).data(),
                    typeInfoName.data(),
                    getFoundationNamespace(),
                    nullptr);
            }
        }
    }
}

void CGenerateWindowsRuntimeAdapter::codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo)
{
    if (_isGenerateGenericAsyncOperation && pAsyncOperationModelInfo->getResultTypeInfo())
    {
        const schema::CTypeInfo *pResultTypeInfo = pAsyncOperationModelInfo->getResultTypeInfo()->getModelType();
        // only Generic Async Operations for this schema model types
        if (isModelInSchema(pResultTypeInfo))
        {
            _osGenericAsyncOperationForwardIdl << FormatW(_forwardAsyncOperationIdlDeclare_Template,
                toIdlAsyncOperation(pAsyncOperationModelInfo->getResultTypeInfo()).data(),
                nullptr);
            std::wstring ayncOperationModelInfoName = getModelTypeInfoIncName(pAsyncOperationModelInfo);

            std::wstring asyncOperationType = toNativeFoundationAsyncOperation(pAsyncOperationModelInfo->getResultTypeInfo());
            _osModelAdapterSrc << FormatW(_asyncOperationTypedefAdapter_Template,
                asyncOperationType.data(),
                toNativeFoundationAsyncOperation("ABI::Windows::Foundation::IAsyncOperationCompletedHandler", pAsyncOperationModelInfo->getResultTypeInfo()).data(),
                ayncOperationModelInfoName.data(),
                nullptr);

            _osModelAdapterFactoryEntries << FormatW(
                _asyncOperationAdapterEntry_Template,
                asyncOperationType.data(),
                ayncOperationModelInfoName.data(),
                nullptr);
        }
    }
}

std::wstring CGenerateWindowsRuntimeAdapter::toIdlAsyncOperation(
    const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    std::wostringstream os;
    os << "Windows.Foundation.IAsyncOperation<";

    if (pPropertyTypeInfo->getModelType() == nullptr)
    {
        if (isPropertyTypeInfoArray(pPropertyTypeInfo) || pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Empty)
        {
            // for Empty types or Arrays
            os << "IInspectable *";
        }
        else
        {
            os << toIdlType(pPropertyTypeInfo->getPropertyType());
        }
    }
    else
    {
        const schema::CTypeInfo *pModelTypeInfo = pPropertyTypeInfo->getModelType();
        if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableCollection)
        {
            const schema::CPropertyTypeInfo*pItemTypeInfo =
                ((schema::CEnumerableTypeInfo *)pModelTypeInfo)->getItemTypeInfo();
            if (_isGenerateGenericVector)
            {
                os << "Windows.Foundation.Collections.IVector<";
                pModelTypeInfo = pItemTypeInfo->getModelType();
                if (pModelTypeInfo != nullptr)
                {
                    os << pModelTypeInfo->getName();
                }
                else
                {
                    os << toIdlType(pItemTypeInfo->getPropertyType());
                }
                os << ">";
            }
            else
            {
                os << _idlCoreNamespace << ".IObservableCollection>";
            }
            os << "*";
        }
        else
        {
            
            os << toIdlModelTypeInfo(pModelTypeInfo);
            if (pModelTypeInfo->getModelType() != schema::ModelTypeInfoType_Enum)
            {
                os << "*";
            }
        }
    }
    os << ">";
    return os.str();
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativeFoundationAsyncOperation(
    const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    return toNativeFoundationAsyncOperation("ABI::Windows::Foundation::IAsyncOperation", pPropertyTypeInfo);
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativeAsyncOperation(
    const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    if (_isGenerateGenericAsyncOperation)
    {
        return toNativeFoundationAsyncOperation(pPropertyTypeInfo);
    }
    else
    {
        return std::wstring(_abiCoreNamespace) + L"::IAsyncOperation";
    }
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativeFoundationAsyncOperation(
    const char *prefix,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    std::wostringstream os;
    os << prefix;
    os << "<";

    if (pPropertyTypeInfo->getModelType() == nullptr)
    {
        if (isPropertyTypeInfoArray(pPropertyTypeInfo) || pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Empty)
        {
            os << "IInspectable *";
        }
        else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Boolean)
        {
            // we want IAsyncOperation<bool>
            os << "bool";
        }
        else
        {
            os << toWindowsRuntimeCoreType(pPropertyTypeInfo->getPropertyType());
        }
    }
    else
    {
        const schema::CTypeInfo *pModelTypeInfo = pPropertyTypeInfo->getModelType();
        if (pModelTypeInfo->getModelType() == schema::ModelTypeInfoType_ObservableCollection)
        {
            const schema::CPropertyTypeInfo*pItemTypeInfo =
                ((schema::CEnumerableTypeInfo *)pModelTypeInfo)->getItemTypeInfo();
            if (_isGenerateGenericVector)
            {
                os << "ABI::Windows::Foundation::Collections::IVector<";
                pModelTypeInfo = pItemTypeInfo->getModelType();
                if (pModelTypeInfo != nullptr)
                {
                    os << toNativeModelTypeName(pModelTypeInfo);
                }
                else
                {
                    os << toIdlType(pItemTypeInfo->getPropertyType());
                }
                os << "*>";
            }
            else
            {
                os << _abiCoreNamespace << "::IObservableCollection>";
            }
            os << "*";
        }
        else
        {
            os << toNativeModelTypeName(pModelTypeInfo);
            if (pModelTypeInfo->getModelType() != schema::ModelTypeInfoType_Enum)
            {
                os << "*";
            }
        }
    }
    os << ">";
    return os.str();
}
void CGenerateWindowsRuntimeAdapter::codeGenerate(const schema::CCommandTypeInfo *pCommmandModelInfo)
{
    const bool isAsync = pCommmandModelInfo->getIsAsync();
    const bool isIfcGenerationRequired = !this->isCoreCommand(pCommmandModelInfo);

    if (isIfcGenerationRequired)
    {
        _osInterfaceForwardIdl << "\t" << "interface " << pCommmandModelInfo->getName() << ";" << std::endl;
    }

    const bool isVoidParameter = (pCommmandModelInfo->getParameterTypeInfo()->getPropertyType() ==
        foundation::PropertyType::PropertyType_Empty);
    const bool isArrayParameter = ((pCommmandModelInfo->getParameterTypeInfo()->getPropertyType() & 0x400) != 0);

    const bool isVoidResult = (!isAsync && pCommmandModelInfo->getResultTypeInfo()->getPropertyType() ==
        foundation::PropertyType::PropertyType_Empty);

    std::wstring parameterIdlDeclare;
    std::wstring resultIdlDeclare;

    if (!isVoidParameter)
    {
        parameterIdlDeclare = FormatW(
            isArrayParameter ? _commandArrayParameterIdl_Template : _commandParameterIdl_Template,
            toIdlPropertyTypeInfo(pCommmandModelInfo->getParameterTypeInfo()).data(),
            nullptr);
    }

    if (!isVoidResult)
    {
        resultIdlDeclare = FormatW(_commandResultIdl_Template,
            toIdlResultTypeInfo(pCommmandModelInfo->getResultTypeInfo(),pCommmandModelInfo->getIsAsync()).data(),
            nullptr);
    }

    if (isIfcGenerationRequired)
    {
        std::wstring commandMethodsIdlDeclare = FormatW(
            _commandModelMethodsIdl_Template,
            parameterIdlDeclare.data(),    // {0}
            resultIdlDeclare.data(),       // {1}
            isVoidParameter || isVoidResult ? L"" : L", ", // {2}
            isVoidParameter ? L"" : L", ", // {3}
            nullptr
            );

        _osInterfacesIdl << FormatW(_winRTInterface_Template,
            pCommmandModelInfo->getName().data(),
            toIdlIIDType(pCommmandModelInfo).data(),
            (std::wstring(_idlCoreNamespace) + L".ICommandBase").data(),
            L"",
            commandMethodsIdlDeclare.data(),
            nullptr);
    }

    std::wstring createParamValue;
    if (!isVoidParameter)
    {
        createParamValue = FormatW(
            isArrayParameter ?
                L"IFR(CreateValue(size, {0}values, spParameter.GetAddressOf()));" :
                L"IFR(CreateValue({0}parameter, spParameter.GetAddressOf()));",
            isPropertyTypeInfoEnum(pCommmandModelInfo->getParameterTypeInfo()) ? L"(UINT32)" : L"",
            nullptr
        );
    }

    if (isIfcGenerationRequired)
    {
        std::wstring commandModelInfoName = getModelTypeInfoIncName(pCommmandModelInfo);

        _osModelAdapterSrc << FormatW(_classCommandModelAdapter_Template,
            commandModelInfoName.data(),
            toNativeModelTypeName(pCommmandModelInfo).data(),
            isVoidParameter ? L"" :
            FormatW(
                L"{0}{1}",
                isArrayParameter ? L"UINT32 size, " : L"",
                (toNativePropertyTypeInfo(pCommmandModelInfo->getParameterTypeInfo()) + (isArrayParameter ? L" values" : L" parameter")).data(),
                nullptr).c_str(),
            isVoidResult ? L"" : (toNativeResultTypeInfo(pCommmandModelInfo->getResultTypeInfo(),pCommmandModelInfo->getIsAsync()) + L" *result").data(),
            isVoidParameter || isVoidResult ? L"" : L",",
            isVoidParameter ? L"" : L",",
            createParamValue.data(),
            isVoidResult ? L"" : L"IFR(ReturnValue(spResult, result));",
            nullptr);

        // Emit type adapter factory entry
        _osModelAdapterFactoryEntries << FormatW(
            _modelAdapterEntryFactory2_Template,
            toNativeModelTypeName(pCommmandModelInfo).data(),
            commandModelInfoName.data(),
            getFoundationNamespace(),
            commandModelInfoName.data(),
            nullptr);
    }
}

std::wstring CGenerateWindowsRuntimeAdapter::toIdlType(foundation::PropertyType propertyType)
{
    switch(propertyType)
    {
        case foundation::PropertyType::PropertyType_DateTime:
            return  L"Windows.Foundation.DateTime";
        case foundation::PropertyType::PropertyType_UInt16:
            return  L"unsigned short";
        case foundation::PropertyType::PropertyType_UInt32:
            return  L"unsigned int";
        default:
            break;
    }
    return toWindowsRuntimeCoreType(propertyType);
}

std::wstring CGenerateWindowsRuntimeAdapter::toIdlModelTypeName(const schema::CTypeInfo*pModelTypeInfo)
{
    assert(pModelTypeInfo);
    std::wstring idlTypeName;

    if(&pModelTypeInfo->getParent() != this->getModelSchema())
    {
        idlTypeName = this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
        idlTypeName += L".";
    }
    idlTypeName += pModelTypeInfo->getName();
    return idlTypeName;
}

std::wstring  CGenerateWindowsRuntimeAdapter::toIdlModelTypeInfo(
        const schema::CTypeInfo* pModelTypeInfo)
{
    assert(pModelTypeInfo);

    schema::ModelTypeInfoType modelTypeInfoType = pModelTypeInfo->getModelType();

    if(modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection || modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
    {
        if(_isGenerateGenericVector)
        {
            const schema::CEnumerableTypeInfo *pCollectionModelInfo = (const schema::CEnumerableTypeInfo *)pModelTypeInfo;
            if(pCollectionModelInfo->getItemTypeInfo()->getPropertyType() 
                != foundation::PropertyType::PropertyType_Inspectable ||
                pCollectionModelInfo->getItemTypeInfo()->getModelType() != nullptr)
            {
                const schema::CPropertyTypeInfo *pItemTypeInfo = pCollectionModelInfo->getItemTypeInfo();
                std::wstring vectorType;
                if(pItemTypeInfo->getModelType())
                {
                    vectorType = toIdlModelTypeName(pItemTypeInfo->getModelType());
                }
                else
                {
                    vectorType = toIdlType(pItemTypeInfo->getPropertyType());
                }

                return FormatW(_vectorIdlDeclare_Template,
                    vectorType.data(),
                    nullptr);
            }
        }

        // fallback
        return  std::wstring(_idlCoreNamespace) + L".IObservableCollection";
    }

    if (modelTypeInfoType == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo* pCommandModelInfo = (const schema::CCommandTypeInfo*)pModelTypeInfo;
        assert(pCommandModelInfo != nullptr);

        const bool fIsAsync = pCommandModelInfo->getIsAsync();
        if (isSimpleCommand(pCommandModelInfo))
        {
            return std::wstring(_idlCoreNamespace) + FormatW(L".ISimple{0}Command", fIsAsync ? L"Async" : L"", nullptr);
        }

        if (isObjectObjectCommand(pCommandModelInfo))
        {
            return std::wstring(_idlCoreNamespace) + FormatW(L".I{0}Command", fIsAsync ? L"Async" : L"", nullptr);
        }
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Object)
    {
        return std::wstring(_idlCoreNamespace) + L".IObject";
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Dictionary)
    {
        return std::wstring(_idlCoreNamespace) + L".IDictionary";
    }

    return toIdlModelTypeName(pModelTypeInfo);
}

std::wstring CGenerateWindowsRuntimeAdapter::toIdlPropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    if (pPropertyTypeInfo->getModelType() != nullptr)
    {
        std::wstring idlModelTypeName = toIdlModelTypeInfo(pPropertyTypeInfo->getModelType());
        if((pPropertyTypeInfo->getPropertyType() & 0xff) == foundation::PropertyType_Inspectable)
        {
            idlModelTypeName += L" *";
        }

        return toPropertyType(pPropertyTypeInfo->getPropertyType(),idlModelTypeName.c_str(),L"*");
    }

    return toIdlType(pPropertyTypeInfo->getPropertyType());
}

std::wstring CGenerateWindowsRuntimeAdapter::toIdlResultTypeInfo(
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    bool isAsync)
{
    if(isAsync)
    {
        if (_isGenerateGenericAsyncOperation)
        {
            return toIdlAsyncOperation(pPropertyTypeInfo) + L" *";
        }
        else
        {
            return std::wstring(_idlCoreNamespace) + L".IAsyncOperation *";
        }
    }

    return toIdlPropertyTypeInfo(pPropertyTypeInfo);
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativeModelTypeName(const wchar_t *_namespace,const wchar_t * name)
{
    std::wstring nativeTypeName = getNamespaceLookup(this->_winrtNamespaceLookup, _namespace);
	FindAndReplace(nativeTypeName, std::wstring(L"."), std::wstring(L"::"));

    nativeTypeName = L"ABI::" + nativeTypeName;
    nativeTypeName += L"::";
    nativeTypeName += name;
    return nativeTypeName;
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativeModelTypeName(const schema::CTypeInfo* pModelTypeInfo)
{
    schema::ModelTypeInfoType modelTypeInfoType = pModelTypeInfo->getModelType();
    
    if (modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection || modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
    {
        const schema::CEnumerableTypeInfo *pCollectionModelInfo = (const schema::CEnumerableTypeInfo *)pModelTypeInfo;
        return toNativeCollectionModelTypeName(pCollectionModelInfo);
    }

    if (modelTypeInfoType == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo *pCommandModelInfo = (const schema::CCommandTypeInfo *)pModelTypeInfo;
        return toNativeCommandModelTypeName(pCommandModelInfo);
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Object)
    {
        return FormatW(L"{0}::IObject", _abiCoreNamespace, nullptr);
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Dictionary)
    {
        return FormatW(L"{0}::IDictionary", _abiCoreNamespace, nullptr);
    }

    return toNativeModelTypeName(
        this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace()).data(),
        pModelTypeInfo->getName().data());
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativeCollectionModelTypeName(
    const schema::CEnumerableTypeInfo* pCollectionModelInfo
    )
{
    assert(pCollectionModelInfo != nullptr);

    const schema::CPropertyTypeInfo* pItemTypeInfo = pCollectionModelInfo->getItemTypeInfo();
    if (_isGenerateGenericVector && !isInspectableType(pItemTypeInfo))
    {

        std::wstring nativeTypeName;
        if (pItemTypeInfo->getModelType())
        {
            nativeTypeName = this->getNamespaceLookup(pItemTypeInfo->getModelType()->getParent().getNamespace());
			FindAndReplace(nativeTypeName, std::wstring(L"."), std::wstring(L"__C"));
            nativeTypeName += L"__C";
            nativeTypeName += pItemTypeInfo->getModelType()->getName();
        }
        else
        {
            if (pItemTypeInfo->getPropertyType() == foundation::PropertyType_Int32)
            {
                nativeTypeName = L"int";
            }
            else
            {
                nativeTypeName = toCoreType(pItemTypeInfo->getPropertyType());
            }
        }

        // use generated type
        return FormatW(_vectorAbiDeclareType,
            nativeTypeName.data(),
            nullptr);
    }

    // fallback for object collections
    return  toNativeModelTypeName(_idlCoreNamespace, L"IObservableCollection");
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativeCommandModelTypeName(const schema::CCommandTypeInfo* pCommandModelInfo)
{
    assert(pCommandModelInfo != nullptr);

    const bool fIsAsync = pCommandModelInfo->getIsAsync();

    if (isObjectObjectCommand(pCommandModelInfo))
    {
        return toNativeModelTypeName(_idlCoreNamespace, (fIsAsync ? L"IAsyncCommand" : L"ICommand"));
    }

    if (isSimpleCommand(pCommandModelInfo))
    {
        return toNativeModelTypeName(_idlCoreNamespace, (fIsAsync ? L"ISimpleAsyncCommand" : L"ISimpleCommand"));
    }

    return toNativeModelTypeName(
        this->getNamespaceLookup(pCommandModelInfo->getParent().getNamespace()).data(),
        pCommandModelInfo->getName().data());
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativeResultTypeInfo(
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    bool isAsync)
{
    if (isAsync)
    {
        return toNativeAsyncOperation(pPropertyTypeInfo) + L"*";
    }

    return toNativePropertyTypeInfo(pPropertyTypeInfo);
}

std::wstring CGenerateWindowsRuntimeAdapter::toNativePropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    if (pPropertyTypeInfo->getModelType() != nullptr)
    {
        std::wstring nativeTypeInfoName = toNativeModelTypeName(pPropertyTypeInfo->getModelType());
        if ((pPropertyTypeInfo->getPropertyType() & 0xff) == foundation::PropertyType_Inspectable)
        {
            nativeTypeInfoName += L" *";
        }

        return toPropertyType(pPropertyTypeInfo->getPropertyType(),nativeTypeInfoName.c_str(),L"*");
    }

    return toWindowsRuntimeCoreType(pPropertyTypeInfo->getPropertyType());
}

std::wstring CGenerateWindowsRuntimeAdapter::toIdlIIDType(GUID iidType)
{
    return ToWideString(::ToString(iidType).c_str());
}

std::wstring CGenerateWindowsRuntimeAdapter::toIdlIIDType(const schema::CTypeInfo*pModelTypeInfo)
{
    return toIdlIIDType(createUniqueIId(pModelTypeInfo));
}

std::wstring CGenerateWindowsRuntimeAdapter::toWindowsRuntimeCoreType(foundation::PropertyType propertyType)
{
    if ((propertyType & 0xff) == foundation::PropertyType::PropertyType_Boolean)
    {
        return toPropertyType(propertyType, L"boolean", L"*");
    }
    
    if ((propertyType & 0xff) == foundation::PropertyType::PropertyType_Inspectable)
    {
        return toPropertyType(propertyType, L"IInspectable *", L"*");
    }

    return toCoreType(propertyType);
}

IID CGenerateWindowsRuntimeAdapter::createUniqueIId(const schema::CTypeInfo*pModelTypeInfo)
{
    std::wstring fullName = 
            L"windowsruntime" +
            pModelTypeInfo->getParent().getNamespace() + 
            L"." + 
            pModelTypeInfo->getName();
    return CreateIIDTypeFromString(fullName.c_str());
}
