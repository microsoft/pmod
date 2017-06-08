/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateObjectiveC.cpp
****/

#include "GenerateObjectiveC.h"


const wchar_t *_nsAdapterInc_Template =
L"\
 // {0}\n\
#import <Foundation/Foundation.h>\n\
\n\
// pmod clss forwards\n\
@class PMODDictionary;\n\
// pmod protocols forwards\n\
@protocol PMODObservableObject;\n\
@protocol PMODCollection;\n\
@protocol PMODList;\n\
@protocol PMODCommand;\n\
@protocol PMODAsyncOperation;\n\
// Core class forwards\n\
@class PMODGuid;\n\
@class PMODAdapterFactory;\n\
\n\
// protocols forwards\n\
{1}\n\
\n\
// Import references\n\
{2}\n\
\n\
// Enums defined\n\
{3}\n\
\n\
// Protocols defined\n\
{4}\n\
\n\
// Properties defined\n\
{5}\n\
\n\
// Events defined\n\
{6}\n\
\n\
// Factory registration Class\n\
__attribute__((visibility(\"default\")))\n\
@interface PMOD{7}Factory : NSObject {\n\
}\n\
\n\
// Types declared\n\
{8}\n\
\n\
+(void) registerAdapterFactories:(PMODAdapterFactory *)adapterFactory;\n\
@end\n\
";

const wchar_t *_nsAdapterSrc_Template =
L"\
// {0}\n\
#import \"{1}\"\n\
#import \"PMODAdapterFactory.h\"\n\
#import \"PMODDictionary.h\"\n\
#import \"PMODCommand.h\"\n\
#import \"PMODCollection.h\"\n\
#import \"PMODObservableObject.h\"\n\
#import \"PMODInterop.h\"\n\
#import <foundation/com_ptr.h>\n\
#import <pmod/interfaces.h>\n\
#import <foundation/boxing_util.h>\n\
#import <vector>\n\
\n\
// Declare Private Property model selectors\n\
@interface PMODObservableObject (Private)\n\
-(void) initSource: (pmod::IObservableObject *)pObservableObject;\n\
@end\n\
\n\
// Dependency PMODAdapter Inc Files\n\
{2}\n\
// Protocols implementation\n\
{3}\n\
\n\
// Factory registration Class implementation\n\
@implementation PMOD{4}Factory\n\
\n\
{5}\n\
+(void) registerAdapterFactories:(PMODAdapterFactory *)adapterFactory\n\
{\n\
{6}\n\
}\n\
\n\
@end\n\
";

const wchar_t *_nsFactoryTypeSrc_Template =
L"\
+(NSString *) {0}Type\n\
{\n\
    return [PMODInterop toIIDString:{1}::{2}::IIDType];\n\
}\n\
";

// Format template for Method Invoke
// {0} = Type Info Name
// {1} = Method Name
// {2} = Number of arguments
// {3} = push parameters
// {4} = Return statement
const wchar_t *_ns_MethodInvoke_Template =
L"\
{\n\
\tHRESULT hr;\n\
\tfoundation::InspectablePtr result;\n\
\tstd::vector< foundation::InspectablePtr> params;\n\
\tparams.resize({2});\n\
\t// Add Parameters to Array\n\
{3}\n\
\thr = [self propertyModel]->InvokeMethod({0}::Method_{1},(UINT32)params.size(),params.size() ? params.front().GetAddressOfPtr():nullptr,result.GetAddressOf());\n\
\t[PMODInterop throwIfFailed:hr];\n\
{4}\n\
}\n\
\n\
";


const wchar_t *_ns_SourceInc_Template =
L"\
-(id<{0}>) source;\n\
";

const wchar_t *_ns_SourceSrc_Template =
L"\
-(id<{0}>) source\n\
{\n\
	return (id<{0}>)[self valueForProperty:pmod::Property_Source];\n\
}\n\
";

const wchar_t *_ns_InterfaceAdapterSelectors_Template =
L"\
-(void) initObservableObject: (pmod::IObservableObject *)pObservableObject\n\
{\n\
    {0} *pInterfaceAdapter = nullptr;\n\
    if(pObservableObject != nullptr)\n\
    {\n\
        foundation::QueryWeakReference(pObservableObject,&pInterfaceAdapter);\n\
        NSAssert(pInterfaceAdapter, @\"pInterfaceAdapter != nil\");\n\
    }\n\
    [self initSource : pInterfaceAdapter];\n\
}\n\
\n\
-({0} *) {1}\n\
{\n\
    return ({0} *)[self object];\n\
}\n\
";

void CGenerateObjectiveC::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
    _adapterPrefix = getStringParameter(parameters, L"adapterPrefix", L"PMOD");
    _useInterfaceAdapter = getBoolParamater(parameters, L"useInterfaceAdapter", true);
}

void CGenerateObjectiveC::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "modelPMODAdapterInc: generated Objective C header" << std::endl;
    os << "modelPMODAdapterSrc: generated Objective C source" << std::endl;
    os << "modelIncReferences: list of Type Info Ref header files" << std::endl;
    os << "modelPMODAdapterIncReferences: list of PMOD Adapter Inc Ref header files" << std::endl;
    os << "adapterPrefix: prefix to Adapter classes" << std::endl;
}

void CGenerateObjectiveC::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring modelPMODAdapterIncPath = getStringParameter(parameters, L"modelPMODAdapterInc", nullptr);
    std::wstring modelPMODAdapterSrcPath = getStringParameter(parameters, L"modelPMODAdapterSrc", nullptr);

    // generate PMOD Inc file
    std::wostringstream osNSModelAdapterIncReferences;
    emitIncludeReferences(osNSModelAdapterIncReferences,parameters,L"modelPMODAdapterIncReferences",L"import");

    std::wofstream filePMODAdapterInc(ToUtf8String(modelPMODAdapterIncPath.data()),std::ios_base::out);
    filePMODAdapterInc << FormatW(_nsAdapterInc_Template,
        getFilePath(modelPMODAdapterIncPath).data(),      // {0}
        _osForwardsInc.str().data(),                    // {1}
        osNSModelAdapterIncReferences.str().data(),     // {2}
        _osEnumsInc.str().data(),                       // {3}
        _osProtocolsInc.str().data(),                   // {4}
        _osEnumsPropertiesInc.str().data(),             // {5}
        _osEnumsEventsInc.str().data(),                 // {6}
        this->getNamespace().data(),                    // {7}
        _osFactoryTypesInc.str().data(),                // {8}
        nullptr
        );
    filePMODAdapterInc.close();


    // generate Model Source File
    std::wostringstream osModelIncReferences;
    emitIncludeReferences(osModelIncReferences,parameters,L"modelIncReferences",L"import");
    // generate PMOD Src file
    std::wofstream filePMODAdapterSrc(ToUtf8String(modelPMODAdapterSrcPath.data()),std::ios_base::out);
    filePMODAdapterSrc << FormatW(_nsAdapterSrc_Template,
        getFilePath(modelPMODAdapterSrcPath).data(),
        getFilePath(modelPMODAdapterIncPath).data(),
        osModelIncReferences.str().data(),
        _osProtocolsSrc.str().data(),
        this->getNamespace().data(),
        _osFactoryTypesSrc.str().data(),
        _osFactoryEntries.str().data(),
        nullptr
        );
    filePMODAdapterSrc.close();

}

void CGenerateObjectiveC::codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo)
{
    _osEnumsInc << "typedef enum " << std::endl;
    _osEnumsInc << "{" << std::endl;
    
    for(schema::_EnumInfoVectorType::const_iterator iter = pEnumTypeInfo->getEnums().begin();
        iter != pEnumTypeInfo->getEnums().end();
        ++iter)
    {
        _osEnumsInc << '\t' << pEnumTypeInfo->getName().data() << "_" << std::get<0>(*iter) << " = " << std::get<1>(*iter) << ',' << std::endl;
    }
    _osEnumsInc << "} " << pEnumTypeInfo->getName().data() << ";" << std::endl;
    _osEnumsInc << std::endl;
}

void CGenerateObjectiveC::codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo)
{
    if(pCollectionModelInfo->getItemTypeInfo()->getModelType() == nullptr)
    {
        // the Item Type on the schema is a supported type on the 'Core' library so we don't need to declare it
        return;
    }

    // factory types
    _osFactoryTypesInc << "+(NSString *) " << pCollectionModelInfo->getName() << "Type;" << std::endl;
    _osFactoryTypesSrc << FormatW(
        _nsFactoryTypeSrc_Template,
        pCollectionModelInfo->getName().c_str(),
        this->getNamespace().c_str(),
        getModelTypeInfoIncName(pCollectionModelInfo).c_str(),
        nullptr);
}

void CGenerateObjectiveC::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    bool isViewModel = pPropertyModelInfo->getModelType() == schema::ModelTypeInfoType_ViewModel;

    std::wstring typeInfoName = toTypeInfoName(pPropertyModelInfo->getName());

    schema::CObservableObjectInfo *pBaseType = (schema::CObservableObjectInfo *)pPropertyModelInfo->getBaseType();
    
    std::wstring protocolBaseName = pBaseType ? pBaseType->getName(): L"PMODObservableObject";
    
    _osForwardsInc << "@protocol " << pPropertyModelInfo->getName() << ";" << std::endl;
    _osProtocolsInc << "@protocol " << pPropertyModelInfo->getName() << " <" << protocolBaseName << ">" <<  std::endl;
    
    // protocol implementation
    std::wstring baseClassName = pBaseType ? _adapterPrefix + toTypeInfoName(pBaseType->getName()) : L"PMODObservableObject";
    _osProtocolsSrc << "@interface " << _adapterPrefix << typeInfoName << ": " << baseClassName << "<" << pPropertyModelInfo->getName() << ">" << std::endl;

    _osProtocolsSrc << "@end" << std::endl << std::endl;
    _osProtocolsSrc << "@implementation " << _adapterPrefix << typeInfoName << std::endl;

    std::wstring interfaceAdapterSelector;
    if (_useInterfaceAdapter)
    {
        interfaceAdapterSelector = typeInfoName;
        interfaceAdapterSelector[0] = tolower(interfaceAdapterSelector[0]);
        
        std::wstring typeInfoNameWithNamespace = this->getNativeNamespaceLookup(pPropertyModelInfo->getParent().getNamespace());
        typeInfoNameWithNamespace += L"::";
        typeInfoNameWithNamespace += pPropertyModelInfo->getName();
        
        _osProtocolsSrc << FormatW(
                                  _ns_InterfaceAdapterSelectors_Template,
                                  typeInfoNameWithNamespace.c_str(),
                                  interfaceAdapterSelector.c_str(),
                                  nullptr
                                  );
    }

    if(isViewModel)
    {
        const schema::CPropertyViewModelInfo *pPropertyViewModelInfo = (const schema::CPropertyViewModelInfo *)pPropertyModelInfo;
        
        std::wstring sourcetInterfaceName = pPropertyViewModelInfo->getSourceType()->getName();
        // forward source type
        _osForwardsInc << "@protocol " << sourcetInterfaceName << ";" << std::endl;
        
        _osProtocolsInc << FormatW(_ns_SourceInc_Template,
                                  sourcetInterfaceName.data(),
                                  nullptr);

        _osProtocolsSrc << FormatW(_ns_SourceSrc_Template,
                                  sourcetInterfaceName.data(),
                                  nullptr);

    }

    // Properties enum Start
    if(pPropertyModelInfo->getProperties().size())
    {
        _osEnumsPropertiesInc << "typedef enum " << std::endl;
        _osEnumsPropertiesInc << "{" << std::endl;
    }
    
    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        // generate property id
        _osEnumsPropertiesInc << '\t' << typeInfoName << "_Property_" << (*iter)->getName() << " = " << (*iter)->getId() << ',' << std::endl;

        UINT32 flags = (*iter)->getFlags();
        std::wstring objectiveCTypeName = toObjectiveCType(*iter);
        bool isNumberValue = (*iter)->getPropertyType() <= foundation::PropertyType::PropertyType_Boolean;
        
        if((flags & PropertyFlagType_CanRead))
        {
            std::wstring nsPropertyName = (*iter)->getName();
            nsPropertyName[0] = tolower(nsPropertyName[0]);
            
            // Emit Inc 
            _osProtocolsInc << "-(" << objectiveCTypeName << ") " << nsPropertyName << ";" << std::endl;
            
            // Emit Src
            _osProtocolsSrc << "-(" << objectiveCTypeName << ") " << nsPropertyName  << std::endl;
            _osProtocolsSrc << "{" << std::endl;
            
            if (_useInterfaceAdapter)
            {
                _osProtocolsSrc << "\t";
                emitModelTypeVarDeclare(_osProtocolsSrc, (*iter), L"value");

                _osProtocolsSrc << "\t" << "[PMODInterop throwIfFailed : (";
                _osProtocolsSrc << "self." << interfaceAdapterSelector << "->Get" << (*iter)->getName() << "(";

                emitOutputPtrVar(_osProtocolsSrc,(*iter),L"value");
                _osProtocolsSrc  << "))];" << std::endl;

                _osProtocolsSrc << "\t";
                emitNSReturn(_osProtocolsSrc,*iter,L"value");
            }
            else
            {
                _osProtocolsSrc << "\t" << "return ";
                if(isNumberValue)
                {
                    if(flags & PropertyFlagType_IsEnum)
                    {
                        // cast to enum return
                        _osProtocolsSrc << "(" << (*iter)->getModelType()->getName() << ")";
                    }
                    _osProtocolsSrc << "[[";
                }
                else
                {
                    // generate cast to the returned value
                    _osProtocolsSrc << "(" << objectiveCTypeName << ")[";
                }
                _osProtocolsSrc << "self valueForProperty:" << getNamespace()
                    << "::" << typeInfoName << "::Property_" << (*iter)->getName() << "]";
                if(isNumberValue)
                {
                    _osProtocolsSrc << " " << toNSNumberValue((*iter)->getPropertyType()) << "]";
                }
                _osProtocolsSrc << ";" << std::endl;
            }
            _osProtocolsSrc << "}" << std::endl;
        }
        if((flags & PropertyFlagType_CanWrite))
        {
            // Emit Inc
            _osProtocolsInc << "-(void) set" << (*iter)->getName() << ":(" << objectiveCTypeName << ") value;" << std::endl;
            
            // Emit Src
            _osProtocolsSrc << "-(void) set" << (*iter)->getName() << ":(" << objectiveCTypeName << ") value" << std::endl;
            _osProtocolsSrc << "{" << std::endl;
 
            if (_useInterfaceAdapter)
            {
                emitFoundationVar(
                    _osProtocolsSrc,
                    (*iter),
                    L"value",
                    L"value_foundation");
                _osProtocolsSrc << "\t" << "[PMODInterop throwIfFailed : (";
                _osProtocolsSrc << "self." << interfaceAdapterSelector << "->Set" << (*iter)->getName() << "(";
                emitInputPtrVar(_osProtocolsSrc,*iter,L"value_foundation");
                _osProtocolsSrc << "))];" << std::endl;
            }
            else
            {
                _osProtocolsSrc << "\t" << "[self setValueForProperty:" << getNamespace()
                    << "::" << typeInfoName << "::Property_" << (*iter)->getName() << " value:";
                if(isNumberValue)
                {
                    _osProtocolsSrc << "[NSNumber " << toNSNumberWithValue((*iter)->getPropertyType()) << ":value]";
                }
                else
                {
                    _osProtocolsSrc << "value";
                }
                _osProtocolsSrc << "];" << std::endl;
            }

            _osProtocolsSrc << "}" << std::endl;
        }
        _osProtocolsInc << std::endl;
        _osProtocolsSrc << std::endl;
    }
    
    // Iterate Methods
    for(schema::_MethodIteratorType::const_iterator iter = pPropertyModelInfo->getMethods().begin();
        iter != pPropertyModelInfo->getMethods().end();
        ++iter)
    {
        std::wstring nsMethodName = (*iter)->getName();
        nsMethodName[0] = tolower(nsMethodName[0]);

        bool isAsync = (*iter)->getIsAsync();
        
        std::wstring objcResultTypeName = isAsync ?
            toObjectiveCAsyncOperation():
            toObjectiveCType((*iter)->getResultType());
        
        bool hasResult = (*iter)->getResultType() && (*iter)->getResultType()->getPropertyType() != foundation::PropertyType_Empty;
        // Emit Inc
        _osProtocolsInc << "-(" << objcResultTypeName << ") " << nsMethodName;
        // Emit Src
        _osProtocolsSrc << "-(" << objcResultTypeName << ") " << nsMethodName;
       
        std::wostringstream osAdaptersVars;
        std::wostringstream osAdapterInvoke;

        std::wostringstream osAttachParameters;
        // generate Param Array
        for(schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            std::wstring objcParamTypeName = toObjectiveCType(*iterParam);

            if (iterParam != (*iter)->getParameters().begin())
            {
                _osProtocolsInc << (*iterParam)->getParameterName();
                _osProtocolsSrc << (*iterParam)->getParameterName();
                if (_useInterfaceAdapter)
                {
                    osAdapterInvoke << ",";
                }
            }
            // Emit Inc
            _osProtocolsInc << ":";
            _osProtocolsInc << "(" << objcParamTypeName << ") " << (*iterParam)->getParameterName() << ' ';
            // Emit Src
            _osProtocolsSrc << ":";
            _osProtocolsSrc << "(" << objcParamTypeName << ") " << (*iterParam)->getParameterName() << ' ';

            if (_useInterfaceAdapter)
            {
                std::wstring varAdapterName = (*iterParam)->getParameterName() + L"_foundation";
                emitFoundationVar(
                    osAdaptersVars,
                    (*iterParam),
                    (*iterParam)->getParameterName().c_str(),
                    varAdapterName.c_str());

                emitInputPtrVar(osAdapterInvoke, (*iterParam), varAdapterName.c_str());
            }
            else
            {
                // attach parameter
                osAttachParameters << '\t' << "params[" << (size_t)(iterParam-(*iter)->getParameters().begin()) << "]";
            
                foundation::PropertyType paramPropertyType =  (*iterParam)->getPropertyType();
            
                if(paramPropertyType >= foundation::PropertyType_UInt8 &&
                   paramPropertyType <= foundation::PropertyType_Boolean)
                {
                    osAttachParameters << " = " << getFoundationNamespace()
                    << "::CreateValue(" << (*iterParam)->getParameterName();
                    if(paramPropertyType == foundation::PropertyType_Boolean)
                    {
                        osAttachParameters << " ? true:false";
                    }
                    osAttachParameters << ");" << std::endl;
                }
                else
                {
                    osAttachParameters << ".Attach(";
                    osAttachParameters << "[PMODInterop fromNSValue:" << (*iterParam)->getParameterName() << "]);" << std::endl;
                }
            }
        }
        _osProtocolsInc << ";" << std::endl;
        
        std::wostringstream osReturnStatement;
        if(hasResult || isAsync)
        {
            if (_useInterfaceAdapter)
            {
                osAdaptersVars << "\t";
                if (isAsync)
                {
                    osAdaptersVars
                        << "foundation::ComPtr<"
                        << toPropertyTypeInfoAsync((*iter)->getResultType(),true)
                        << "> result;" << std::endl;
                }
                else
                {
                    emitModelTypeVarDeclare(osAdaptersVars, (*iter)->getResultType(), L"result");
                }
            }
            else
            {
                foundation::PropertyType resultPropertyType = (*iter)->getResultType()->getPropertyType();
                if(! isAsync && (
                   resultPropertyType >= foundation::PropertyType_UInt8 &&
                   resultPropertyType <= foundation::PropertyType_Boolean)
                   )
                {
                    osReturnStatement << '\t' << toCoreType(resultPropertyType) << " resultValue;" << std::endl;
                    osReturnStatement << '\t' << getFoundationNamespace() << "::pv_util::GetValue(result,&resultValue);" << std::endl;
                    osReturnStatement << '\t' << "return resultValue;";
                }
                else
                {
                    osReturnStatement << '\t' << "return (" << objcResultTypeName << ") [self toNSObject:result];" << std::endl;
                }
            }
        }

        if (_useInterfaceAdapter)
        {
            _osProtocolsSrc << std::endl << "{" << std::endl;
            _osProtocolsSrc << osAdaptersVars.str().c_str() << std::endl;
            _osProtocolsSrc << "\t" << "[PMODInterop throwIfFailed : ";
            _osProtocolsSrc << "self." << interfaceAdapterSelector << "->" << (*iter)->getName() << "(";
            _osProtocolsSrc << osAdapterInvoke.str();
            if (hasResult || isAsync)
            {
                if ((*iter)->getParameters().size())
                {
                    _osProtocolsSrc << ",";
                }
                if (isAsync)
                {
                    _osProtocolsSrc << "result.GetAddressOf()";
                }
                else
                {
                    emitOutputPtrVar(_osProtocolsSrc, (*iter)->getResultType(),L"result");
                }
            }
            _osProtocolsSrc << ")];" << std::endl;
            _osProtocolsSrc << "\t";
            if (hasResult || isAsync)
            {
                if (isAsync)
                {
                    _osProtocolsSrc << "return (id<PMODAsyncOperation>)[self toNSObject : result];" << std::endl;
                }
                else
                {
                    emitNSReturn(_osProtocolsSrc, (*iter)->getResultType(), L"result");
                }
            }

            _osProtocolsSrc << "}" << std::endl;
        }
        else
        {
            std::wostringstream osCount;
            osCount << (*iter)->getParameters().size();
        
            _osProtocolsSrc << FormatW(_ns_MethodInvoke_Template,
                         (getNamespace() + L"::" + typeInfoName).data(),
                         (*iter)->getName().data(),
                         osCount.str().data(),
                         osAttachParameters.str().data(),
                         osReturnStatement.str().data(),
                         nullptr);
        }
     }
    
    _osProtocolsInc << "@end" << std::endl << std::endl;
    _osProtocolsSrc << "@end" << std::endl << std::endl;
 
    if(pPropertyModelInfo->getProperties().size())
    {
        _osEnumsPropertiesInc << "} " << typeInfoName << "_Properties" << ";" << std::endl;
        _osEnumsPropertiesInc << std::endl;
    }

    // generate EventInfo enums
    if(pPropertyModelInfo->getEvents().size())
    {
        _osEnumsEventsInc << "typedef enum " << std::endl;
        _osEnumsEventsInc << "{" << std::endl;
        for(schema::_EventIteratorType::const_iterator iter = pPropertyModelInfo->getEvents().begin();
            iter != pPropertyModelInfo->getEvents().end();
            ++iter)
        {
            // generate event id
            _osEnumsEventsInc << '\t' << typeInfoName << "_Event_" << (*iter)->getName() << " = " << (*iter)->getId() << ',' << std::endl;
        }
        _osEnumsEventsInc << "} " << typeInfoName << "_Events" << ";" << std::endl;
        _osEnumsEventsInc << std::endl;
        
    }

    // register factory entries
    _osFactoryEntries <<  "\t" << "[adapterFactory registerAdapterFactoryClass:[PMODInterop toIIDString:" <<
        this->getNamespace() << "::" << typeInfoName << "::IIDType] adapterClass:[" << _adapterPrefix << typeInfoName << " class]];" << std::endl;
    
    // factory types 
    _osFactoryTypesInc << "+(NSString *) " << typeInfoName << "Type;" << std::endl;
    _osFactoryTypesSrc << FormatW(
        _nsFactoryTypeSrc_Template,
        typeInfoName.data(),
        this->getNamespace().data(),
        typeInfoName.data(),
        nullptr);
    

}

std::wstring CGenerateObjectiveC::toObjectiveCAsyncOperation()
{
    return L"id<PMODAsyncOperation>";
}

std::wstring CGenerateObjectiveC::toObjectiveCType(const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    if(pPropertyTypeInfo->getPropertyType() & 0x400)
    {
        foundation::PropertyType itemArrayType = (foundation::PropertyType)(pPropertyTypeInfo->getPropertyType() & 0xff);
        return itemArrayType==foundation::PropertyType_UInt8 ? L"NSData *":L"NSArray *";
    }

    if(pPropertyTypeInfo->getModelType())
    {
        schema::ModelTypeInfoType modelTypeInfoType = pPropertyTypeInfo->getModelType()->getModelType();
        
        if(modelTypeInfoType == schema::ModelTypeInfoType_Dictionary)
        {
            return L"PMODDictionary *";
        }
        else if(modelTypeInfoType == schema::ModelTypeInfoType_Object)
        {
            return L"id<PMODObservableObject>";
        }
        else if(modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection)
        {
            return L"id<PMODCollection>";
        }
        else if(modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
        {
            return L"id<PMODList>";
        }
        else if(modelTypeInfoType == schema::ModelTypeInfoType_Command)
        {
            return L"id<PMODCommand>";
        }
        else if(modelTypeInfoType == schema::ModelTypeInfoType_AsyncOperation)
        {
            return toObjectiveCAsyncOperation();
        }
        else if(modelTypeInfoType == schema::ModelTypeInfoType_Enum)
        {
            return pPropertyTypeInfo->getModelType()->getName();
        }
        else
        {
            return FormatW(L"id<{0}>",pPropertyTypeInfo->getModelType()->getName().data(),nullptr);
        }
    }
    else
    {
        return toObjectiveCType(pPropertyTypeInfo->getPropertyType());
    }
}

const wchar_t *CGenerateObjectiveC::toObjectiveCType(foundation::PropertyType propertyType)
{
    switch(propertyType)
    {
    case foundation::PropertyType::PropertyType_Empty:
        return L"void";
    case foundation::PropertyType::PropertyType_UInt8:
        return L"unsigned char";
    case foundation::PropertyType::PropertyType_Int16:
        return L"short";
    case foundation::PropertyType::PropertyType_UInt16:
        return L"unsigned short";
    case foundation::PropertyType::PropertyType_Int32:
        return L"int";
    case foundation::PropertyType::PropertyType_UInt32:
        return L"unsigned int";
    case foundation::PropertyType::PropertyType_Int64:
        return L"long long";
    case foundation::PropertyType::PropertyType_UInt64:
        return L"unsigned long long";
    case foundation::PropertyType::PropertyType_Single:
        return L"float";
    case foundation::PropertyType::PropertyType_Double:
        return L"double";
    case foundation::PropertyType::PropertyType_Char16:
        return L"char";
    case foundation::PropertyType::PropertyType_Boolean:
        return L"BOOL";
    case foundation::PropertyType::PropertyType_String:
        return L"NSString *";
    case foundation::PropertyType::PropertyType_Inspectable:
        return L"id";
    case foundation::PropertyType::PropertyType_DateTime:
        return L"NSDate *";
    case foundation::PropertyType::PropertyType_Guid:
        return L"PMODGuid *";
    case foundation::PropertyType::PropertyType_Point:
    case foundation::PropertyType::PropertyType_Size:
    case foundation::PropertyType::PropertyType_Rect:
    default:
        assert(false);
    }
    return L"";
}

const wchar_t *CGenerateObjectiveC::toNSNumberWithValue(foundation::PropertyType propertyType)
{
    switch(propertyType)
    {
        case foundation::PropertyType::PropertyType_UInt8:
            return L"numberWithUnsignedChar";
        case foundation::PropertyType::PropertyType_Int16:
            return L"numberWithShort";
        case foundation::PropertyType::PropertyType_UInt16:
            return L"numberWithUnsignedShort";
        case foundation::PropertyType::PropertyType_Int32:
            return L"numberWithInt";
        case foundation::PropertyType::PropertyType_UInt32:
            return L"numberWithUnsignedInt";
        case foundation::PropertyType::PropertyType_Int64:
            return L"numberWithLongLong";
        case foundation::PropertyType::PropertyType_UInt64:
            return L"numberWithUnsignedLongLong";
        case foundation::PropertyType::PropertyType_Single:
            return L"numberWithFloat";
        case foundation::PropertyType::PropertyType_Double:
            return L"numberWithDouble";
        case foundation::PropertyType::PropertyType_Char16:
            return L"numberWithChar";
        case foundation::PropertyType::PropertyType_Boolean:
            return L"numberWithBool";
        default:
            assert(false);
    }
    return L"";
}

const wchar_t *CGenerateObjectiveC::toNSNumberValue(foundation::PropertyType propertyType)
{
    switch(propertyType)
    {
        case foundation::PropertyType::PropertyType_UInt8:
            return L"unsignedCharValue";
        case foundation::PropertyType::PropertyType_Int16:
            return L"shortValue";
        case foundation::PropertyType::PropertyType_UInt16:
            return L"unsignedShortValue";
        case foundation::PropertyType::PropertyType_Int32:
            return L"intValue";
        case foundation::PropertyType::PropertyType_UInt32:
            return L"unsignedIntValue";
        case foundation::PropertyType::PropertyType_Int64:
            return L"longLongValue";
        case foundation::PropertyType::PropertyType_UInt64:
            return L"unsignedLongLongValue";
        case foundation::PropertyType::PropertyType_Single:
            return L"floatValue";
        case foundation::PropertyType::PropertyType_Double:
            return L"doubleValue";
        case foundation::PropertyType::PropertyType_Char16:
            return L"charValue";
        case foundation::PropertyType::PropertyType_Boolean:
            return L"boolValue";
        default:
            assert(false);
    }
    return L"";
}

void CGenerateObjectiveC::emitNSReturn(
                std::wostream& os,
                const schema::CPropertyTypeInfo* pPropertyTypeInfo,
                const wchar_t *pVarName)
{
    
    if (isPropertyTypeInfoArray(pPropertyTypeInfo) ||
        (hasPropertyTypeInfoModel(pPropertyTypeInfo) && !isPropertyTypeInfoEnum(pPropertyTypeInfo)) ||
        pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Guid ||
        pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_DateTime ||
        isPropertyTypeInfoObject(pPropertyTypeInfo))
    {
        os << "return (" << toObjectiveCType(pPropertyTypeInfo) << ")[self toNSObject : ";
        os << "foundation::CreateValue(";
        if (isPropertyTypeInfoArray(pPropertyTypeInfo))
        {
            os << pVarName << ".GetSize(),(";
            emitDeclareArrayWrapperType(os, pPropertyTypeInfo);
            os << "::_Item_Type *)" << pVarName << ".GetBuffer())";
        }
        else
        {
            os << pVarName << ")";
        }
        os << "];";
    }
    else
    {
        if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType::PropertyType_Boolean)
        {
            os << "return " << pVarName << " ? TRUE:FALSE;";
        }
        else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType::PropertyType_String)
        {
            os << "return [PMODInterop toNSString:" << pVarName << "];";
        }
        else
        {
            os << "return (" << toObjectiveCType(pPropertyTypeInfo) << ")" << pVarName << ";";
        }
    }
    os << std::endl;    
}

void CGenerateObjectiveC::emitFoundationVar(
    std::wostream& os,
    const schema::CPropertyTypeInfo* pPropertyTypeInfo,
    const wchar_t *pObjectiveCVar,
    const wchar_t *pFoundationVar)
{
    os << "\t";
    emitModelTypeVarDeclare(os, pPropertyTypeInfo, pFoundationVar);
    os << "\t";
    if (isPropertyTypeInfoArray(pPropertyTypeInfo))
    {
        os << "foundation::pv_util::GetValue(";
        os << "foundation::InspectablePtr::AttachPtr([PMODInterop ";
        if(getPropertyTypeInfoArrayItemType(pPropertyTypeInfo) == foundation::PropertyType_UInt8)
        {
            os << "fromNSData";
        }
        else
        {
            os << "fromNSArray";
        }
        os << ":" << pObjectiveCVar << "])";
        os << "," << pFoundationVar << ".GetSizeAddressOf()," << pFoundationVar << ".GetBufferAddressOf());";
    }
    else if (hasPropertyTypeInfoModel(pPropertyTypeInfo) && !isPropertyTypeInfoEnum(pPropertyTypeInfo))
    {
        os << "foundation::QueryInterfaceIf(";
        os << "[" << pObjectiveCVar << " object],";
        os << pFoundationVar << ".GetAddressOf());";
    }
    else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_DateTime)
    {
        os << "foundation::pv_util::GetValue(";
        os << "foundation::InspectablePtr::AttachPtr([PMODInterop fromNSDate:" << pObjectiveCVar;
        os << "]),&" << pFoundationVar << ");";
    }
    else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Inspectable)
    {
        os << pFoundationVar << ".Attach([PMODInterop fromNSObject:" << pObjectiveCVar << "]);";
    }
    else
    {
        os << pFoundationVar << " = ";

        if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Boolean)
        {
            os << pObjectiveCVar << "? true:false;";
        }
        else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_String)
        {
            os << "[" << pObjectiveCVar << " UTF8String];";
        }
        else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Guid)
        {
            os << "*((GUID *)[" << pObjectiveCVar << " iid]);";
        }
        else
        {
            os << "(";
            if (isPropertyTypeInfoEnum(pPropertyTypeInfo))
            {
                emitFullName(os, pPropertyTypeInfo->getModelType());
            }
            else
            {
                os << toCoreType(pPropertyTypeInfo->getPropertyType());
            }
            os << ")" << pObjectiveCVar << ";";
        }
    }
    os << std::endl;
}
