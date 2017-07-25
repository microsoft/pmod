/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SchemaParser.cpp
****/
#include "SchemaParser.h"
#include "ModelSchema.h"
#include "ParserEngineUtil.h"
#include "CodeGenUtil.h"
#include "StringUtils.h"
#include "md5Util.h"
#include "ModelSymbolParser.h"
#include "ModelSchema.h"
#include "SchemaSerializer.h"
#include "assert.h"

#include <memory>
#include <map>
#include <algorithm>
#include <fstream>
#include <foundation/library/string_util.h>
#include <foundation/library/json_statics_util.h>

using namespace schema;

// from Public
#include <pmod/interfaces.h>

using namespace foundation;
using namespace foundation::library;

typedef std::map<std::wstring,void *, WStringLessThan> _AttributeExpressionMap;
typedef std::vector<std::pair<std::wstring,std::string> > vector_file_reference_type;

static void *lookupAttribute(const _AttributeExpressionMap& attributes,const wchar_t *pAttributeName)
{
    _AttributeExpressionMap::const_iterator iter = attributes.find(pAttributeName);
    if(iter != attributes.end())
    {
        return (*iter).second;
    }
    return nullptr;
}

static std::wstring replaceNamespaceDelimiter(const wchar_t *str,const wchar_t *replace)
{
    std::wstring s(str);
	string_util::find_and_replace(s,std::wstring(L"."),std::wstring(replace));
    return s;
}

static std::string dirnameOf(const std::string& fname)
{
     size_t pos = fname.find_last_of("\\/");
     return (std::string::npos == pos)
         ? ""
         : fname.substr(0, pos + 1);
}

static bool hasPropertyAttribute(CPropertyReferenceSymbol *pPropertyRefSymbol,const wchar_t* pAttributeName)
{ 
    // resolve attributes at the property level
    for (std::vector<CAttributeReferenceSymbol *>::const_iterator
        iterAttribute = pPropertyRefSymbol->getAttributes().begin();
        iterAttribute != pPropertyRefSymbol->getAttributes().end();
        ++iterAttribute)
    {
        if (wcscmp((*iterAttribute)->getReferenceValue(), L"PropertyAttribute") == 0)
        {
            for (std::vector<CAttributeExpressionSymbol *>::const_iterator
                iterAttributeExpression = (*iterAttribute)->getAttributeExpressions().begin();
                iterAttributeExpression != (*iterAttribute)->getAttributeExpressions().end();
            ++iterAttributeExpression)
            {
                const wchar_t *propertyAttributeName = (*iterAttributeExpression)->getReferenceValue();
                if (wcscmp(propertyAttributeName, pAttributeName) == 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

static int countProperties(const std::vector<CPropertyReferenceSymbol *>&properties)
{
    int totalProperties = 0;
    for (std::vector<CPropertyReferenceSymbol *>::const_iterator iterProperty =
        properties.begin(); iterProperty != properties.end(); ++iterProperty
        )
    {
        if (!hasPropertyAttribute(*iterProperty, L"IsParent"))
        {
            ++totalProperties;
        }
    }
    return totalProperties;
}

static const CTypeInfo *resolveTypeFromSymbol(
    const IReferenceSymbol& refSymbol,
    CModelSchemaSymbols& modelSchemaSymbols,
    CModelSchema *pModelSchema)
{
    const CTypeInfo *pModelTypeInfo = pModelSchema->resolveModelType(
        refSymbol.getReferenceValue());
    if(pModelTypeInfo == nullptr)
    {
        _throwError(L"Identifier :%ls not found on File:%ls, line:%d col:%d", 
            refSymbol.getReferenceValue(),
            modelSchemaSymbols.getModelFilePath().data(),
            refSymbol.getSymbolReference()->Line,
            refSymbol.getSymbolReference()->Column
            );
    }
    return pModelTypeInfo;
}

static unsigned long parseUInteger(
    const IReferenceSymbol& refSymbol,
    CModelSchemaSymbols& modelSchemaSymbols)
{
    // TODO: trigger error & support hex
    return wcstoul(refSymbol.getReferenceValue(), nullptr, 10);
}

static bool parseBoolean(
    const IReferenceSymbol& refSymbol,
    CModelSchemaSymbols& modelSchemaSymbols)
{
    if(wcscmp(refSymbol.getReferenceValue(),L"true") == 0)
    {
        return true;
    }
    else if(wcscmp(refSymbol.getReferenceValue(),L"false") == 0)
    {
        return false;
    }
    _throwError(L"bool:%ls expected on File:%ls, line:%d col:%d", 
        refSymbol.getReferenceValue(),
        modelSchemaSymbols.getModelFilePath().data(),
        refSymbol.getSymbolReference()->Line,
        refSymbol.getSymbolReference()->Column
        );
    return false;
}

static bool resolvePropertyType(
    const wchar_t *typeName,
    PropertyType& propertyType)
{
    if(wcscmp(L"void",typeName) == 0)
    {
        propertyType = PropertyType_Empty;
    }
    else if(wcscmp(L"byte",typeName) == 0)
    {
        propertyType = PropertyType_UInt8;
    }
    else if(wcscmp(L"short",typeName) == 0)
    {
        propertyType = PropertyType_Int16;
    }
    else if(wcscmp(L"ushort",typeName) == 0)
    {
        propertyType = PropertyType_UInt16;
    }
    else if(wcscmp(L"int",typeName) == 0)
    {
        propertyType = PropertyType_Int32;
    }
    else if(wcscmp(L"uint",typeName) == 0)
    {
        propertyType = PropertyType_UInt32;
    }
    else if(wcscmp(L"long",typeName) == 0)
    {
        propertyType = PropertyType_Int64;
    }
    else if(wcscmp(L"ulong",typeName) == 0)
    {
        propertyType = PropertyType_UInt64;
    }
    else if(wcscmp(L"string",typeName) == 0)
    {
        propertyType = PropertyType_String;
    }
    else if(wcscmp(L"double",typeName) == 0)
    {
        propertyType = PropertyType_Double;
    }
    else if(wcscmp(L"float",typeName) == 0)
    {
        propertyType = PropertyType_Single;
    }
    else if(wcscmp(L"char",typeName) == 0)
    {
        propertyType = PropertyType_Char16;
    }
    else if(wcscmp(L"bool",typeName) == 0)
    {
        propertyType = PropertyType_Boolean;
    }
    else if (wcscmp(L"Guid", typeName) == 0)
    {
        propertyType = PropertyType_Guid;
    }
    else if(wcscmp(L"DateTime",typeName) == 0 || wcscmp(L"System.DateTime",typeName) == 0)
    {
        propertyType = PropertyType_DateTime;
    }
    else if(wcscmp(L"object",typeName) == 0 || wcscmp(L"Object",typeName) == 0 || wcscmp(L"System.Object",typeName) == 0)
    {
        propertyType = PropertyType_Inspectable;
    }
    else
    {
        propertyType = PropertyType_Inspectable;
        return false;
    }
    return true;
}

static void resolveType(
    const IReferenceSymbol& refSymbol,
    CModelSchemaSymbols& modelSchemaSymbols,
    CModelSchema *pModelSchema,
    PropertyType& propertyType,
    const CTypeInfo *&pModelInfoType)
{
    pModelInfoType = nullptr;

    if(!resolvePropertyType(refSymbol.getReferenceValue(),propertyType))
    {
        // not known type so we will attempt to resolve a symbol
        if((pModelInfoType = pModelSchema->resolveEnumType(refSymbol.getReferenceValue())) != nullptr)
        {
            // we found a Enum Type Info for this but we still want to define a simple UInt32
            // as the main type
            propertyType = PropertyType_UInt32;
        }
        else
        {
            // fully resolve a Symbol
            pModelInfoType = resolveTypeFromSymbol(
            refSymbol,
            modelSchemaSymbols,
            pModelSchema);
        }
    }
}

static void resolveTypeType(
    const IReferenceSymbol& refSymbol,
    CModelSchemaSymbols& modelSchemaSymbols,
    CModelSchema *pModelSchema,
    PropertyType& propertyType,
    const CTypeInfo *&pModelInfoType)
{
    resolveType(refSymbol,modelSchemaSymbols,pModelSchema,propertyType,pModelInfoType);
    if(CModelSchemaSymbols::isTypeArray(refSymbol))
    {
        propertyType = (PropertyType)((int)propertyType + 1024);
    }
}

static CPropertyTypeInfo *createPropertyTypeInfo(
    const IReferenceSymbol* pRefSymbol,
    CModelSchemaSymbols& modelSchemaSymbols,
    CModelSchema *pModelSchema)
{
    PropertyType propertyType = pRefSymbol != nullptr ? PropertyType_Empty : PropertyType_Inspectable;
    const CTypeInfo *pModelInfoType = nullptr;

    if(pRefSymbol)
    {
        resolveTypeType(
            *pRefSymbol,
            modelSchemaSymbols,
            pModelSchema,
            propertyType,
            pModelInfoType);
    }

    return new CPropertyTypeInfo(propertyType,pModelInfoType);
}

static CEnumerableTypeInfo *lookupOrCreateEnumerationType(
    UINT32& id,
    const std::wstring& collectionTypeName,
    const IReferenceSymbol *pEnumerationItemType,
    CModelSchemaSymbols& modelSchemaSymbols,
    CModelSchema &modelSchema,
    schema::ModelTypeInfoType typeInfoType)
{
    // attempt to resolve on our schema
    CEnumerableTypeInfo *pEnumerationTypeInfo = (CEnumerableTypeInfo *)modelSchema.resolveModelType(collectionTypeName.data());
    if(!pEnumerationTypeInfo)
    {
        std::wstring typeNameAndNamespace = modelSchema.getNamespace() + L"." + collectionTypeName;
        IID iidType = CreateIIDTypeFromString(typeNameAndNamespace.data());

        pEnumerationTypeInfo = new CEnumerableTypeInfo(
            id++,
            collectionTypeName.data(),
            iidType,
            modelSchema,
            createPropertyTypeInfo(pEnumerationItemType,modelSchemaSymbols,&modelSchema),
            typeInfoType
            );
        modelSchema.addModelType(pEnumerationTypeInfo);
    }
    return pEnumerationTypeInfo;
}

static CEnumerableTypeInfo *lookupOrCreateEnumerationType(
    UINT32& id,
    const IReferenceSymbol *pEnumerationItemType,
    CModelSchemaSymbols& modelSchemaSymbols,
    CModelSchema &modelSchema,
    schema::ModelTypeInfoType typeInfoType)
{
    // build a unique type name for this Collection type
    std::wstring _namespace,typeName;
    GetNamespaceAndType(pEnumerationItemType ? pEnumerationItemType->getReferenceValue():L"Object",_namespace,typeName);

    std::wstring enumerationTypeName = _namespace;
    if(_namespace.length())
    {
        enumerationTypeName += L".";
    }
    if(typeInfoType == schema::ModelTypeInfoType_ObservableList)
    {
        enumerationTypeName += L"IList_";
    }
    else if (typeInfoType == schema::ModelTypeInfoType_ObservableCollection)
    {
        enumerationTypeName += L"ICollection_";
    }
    else if (typeInfoType == schema::ModelTypeInfoType_Enumerable)
    {
        enumerationTypeName += L"IEnumerable_";
    }

    enumerationTypeName += typeName;
    return lookupOrCreateEnumerationType(id, enumerationTypeName,pEnumerationItemType,modelSchemaSymbols,modelSchema, typeInfoType);
}

static bool isPropertyTypeReferenceSymbol(const IReferenceSymbol& refSymbol)
{
    foundation::PropertyType propertyType;
    return resolvePropertyType(refSymbol.getReferenceValue(),propertyType);
}

static CAsyncOperationTypeInfo *lookupOrCreateAsyncOperationType(
    UINT32& id,
    const IReferenceSymbol *pResultType,
    CModelSchemaSymbols& modelSchemaSymbols,
    CModelSchema &modelSchema)
{
    std::wstring asyncOperationTypeName = L"IAsyncOperation_";
    asyncOperationTypeName += pResultType ? replaceNamespaceDelimiter(pResultType->getReferenceValue(),L"_"):L"Object";
    // attempt to resolve on our schema
    CAsyncOperationTypeInfo *pAsyncOperationModelInfo = (CAsyncOperationTypeInfo *)modelSchema.resolveModelType(asyncOperationTypeName.data());
    if(!pAsyncOperationModelInfo)
    {
        std::wstring typeNameAndNamespace = modelSchema.getNamespace() + L"." + asyncOperationTypeName;
        IID iidType = CreateIIDTypeFromString(typeNameAndNamespace.data());

        pAsyncOperationModelInfo = new CAsyncOperationTypeInfo(
            id++,
            asyncOperationTypeName.data(),
            iidType,
            modelSchema,
            createPropertyTypeInfo(pResultType,modelSchemaSymbols,&modelSchema)
            );
        modelSchema.addModelType(pAsyncOperationModelInfo);
    }
    return pAsyncOperationModelInfo;
}

// parse the Model attributes
static void parseModelAttributes(
    CAttributeReferenceSymbol *pAttributeReferenceSymbol,
    const CModelSchemaSymbols& modelSchemaSymbols,
    _AttributeExpressionMap& attributes)
{
    assert(pAttributeReferenceSymbol);

    for(std::vector<CAttributeExpressionSymbol *>::const_iterator  
        iterAttributeExpression = pAttributeReferenceSymbol->getAttributeExpressions().begin();
        iterAttributeExpression != pAttributeReferenceSymbol->getAttributeExpressions().end();
        ++iterAttributeExpression)
    {
        const wchar_t *modelAttributeExpression = (*iterAttributeExpression)->getReferenceValue();

        if(wcscmp(modelAttributeExpression,L"SourceType") == 0)
        {
            attributes[L"SourceType"] = (void *)&(*iterAttributeExpression)->getReferenceSymbol();
        }
        else if (wcscmp(modelAttributeExpression, L"CustomInterfaceName") == 0)
        {
            attributes[L"CustomInterfaceName"] = (void *)&(*iterAttributeExpression)->getReferenceSymbol();
        }
        else if (wcscmp(modelAttributeExpression, L"IsCustomPrivateInterface") == 0)
        {
            attributes[L"IsCustomPrivateInterface"] = (void *)&(*iterAttributeExpression)->getReferenceSymbol();
        }        
        else
        {
            // generic load for the symbols
            // we know support any new attributes added in the schema and this will be passed in the 
            // attribute dictionary 
            attributes[modelAttributeExpression] = (void *)&(*iterAttributeExpression)->getReferenceSymbol();         
        }
    }
}

// parse the collection attributes
static void parseEnumerationAttributes(
    CAttributeReferenceSymbol *pAttributeReferenceSymbol,
    const CModelSchemaSymbols& modelSchemaSymbols,
    _AttributeExpressionMap& attributes)
{
    for(std::vector<CAttributeExpressionSymbol *>::const_iterator  
        iterAttributeExpression = pAttributeReferenceSymbol->getAttributeExpressions().begin();
        iterAttributeExpression != pAttributeReferenceSymbol->getAttributeExpressions().end();
        ++iterAttributeExpression)
    {
        const wchar_t *collectionAttributeExpression = (*iterAttributeExpression)->getReferenceValue();
        if(wcscmp(collectionAttributeExpression,L"ItemType") == 0)
        {
            attributes[L"ItemType"] = (void *)&(*iterAttributeExpression)->getReferenceSymbol();
        }
        else
        {
            _throwError(L"Unrecognized expression:%ls expected on File:%ls, line:%d col:%d",
                collectionAttributeExpression,
                modelSchemaSymbols.getModelFilePath().data(),
                (*iterAttributeExpression)->getSymbolReference()->Line,
                (*iterAttributeExpression)->getSymbolReference()->Column
            );
        }
    }    
}

// parse the event attributes
static void parseEventAttributes(
    CAttributeReferenceSymbol *pAttributeReferenceSymbol,
    const CModelSchemaSymbols& modelSchemaSymbols,
    _AttributeExpressionMap& attributes)
{
    for(std::vector<CAttributeExpressionSymbol *>::const_iterator  
        iterAttributeExpression = pAttributeReferenceSymbol->getAttributeExpressions().begin();
        iterAttributeExpression != pAttributeReferenceSymbol->getAttributeExpressions().end();
        ++iterAttributeExpression)
    {
        const wchar_t *eventAttributeExpression = (*iterAttributeExpression)->getReferenceValue();
        if(wcscmp(eventAttributeExpression,L"EventArgs") == 0)
        {
            attributes[L"EventArgs"] = (void *)&(*iterAttributeExpression)->getReferenceSymbol();
        }
        else
        {
            _throwError(L"Unrecognized expression:%ls expected on File:%ls, line:%d col:%d",
                eventAttributeExpression,
                modelSchemaSymbols.getModelFilePath().data(),
                (*iterAttributeExpression)->getSymbolReference()->Line,
                (*iterAttributeExpression)->getSymbolReference()->Column
            );
        }
    }    
}

static void getRelativeFilePath(
    const char *modelSchemaPath,
    const std::wstring& pathAttribute,
    std::string& filePath)
{
	filePath = ToUtf8String(pathAttribute.c_str());
    filePath = dirnameOf(modelSchemaPath) + filePath;
}

static void GetFileReferences(
    const char *pModelSchemaPath,
    DictionaryPtr& schemaPtr,
    LPCSTR_t key,
    vector_file_reference_type& fileReferences,
    const _ToolParameterType& parameters)
{
    // process 'reference' tag
    HStringArrayWrapper references;
    schemaPtr.GetValue(key, references);


    for (UINT32 index = 0; index < references.GetSize();++index)
    {
        
        std::wstring pathAttributeValue = to_wstring(references[index]);
        ReplaceProperties(parameters,pathAttributeValue);

        std::string filePath;
        if(isAbsolutePath(pathAttributeValue.c_str()))
        {
            filePath = ToUtf8String(pathAttributeValue.c_str());
        }
        else
        {
            getRelativeFilePath(pModelSchemaPath,pathAttributeValue,filePath);
        }

        fileReferences.push_back(std::make_pair(pathAttributeValue,filePath));
    }
}

static std::wstring buildUniqueCommandTypeName(
    const IReferenceSymbol* pCommandParameterType,
    const IReferenceSymbol* pCommandResultType,
    bool commandIsAsync,
    bool commandIsUnknownState
    )
{
    std::wstring commandTypeName = L"ICommand_";

    if (pCommandParameterType == nullptr)
    {
        commandTypeName += L"object";
    }
    else
    {
        commandTypeName += replaceNamespaceDelimiter(pCommandParameterType->getReferenceValue(), L"_");
        if (CModelSchemaSymbols::isTypeArray(*pCommandParameterType))
        {
            commandTypeName += L"Array";
        }
    }

    commandTypeName += L"_";

    if (pCommandResultType == nullptr)
    {
        commandTypeName += L"object";
    }
    else
    {
        commandTypeName += replaceNamespaceDelimiter(pCommandResultType->getReferenceValue(), L"_");
        if (CModelSchemaSymbols::isTypeArray(*pCommandResultType))
        {
            commandTypeName += L"Array";
        }
    }

    if (commandIsAsync)
    {
        commandTypeName += L"_async";
    }

    if (commandIsUnknownState)
    {
        commandTypeName += L"_canExecute";
    }

    return commandTypeName;
}


bool CSchemaParser::checkFileDependencies(
    int _fh,
    const char *pModelSchemaPath,
    const _ToolParameterType& parameters)
{
    // load model schema
    DictionaryPtr schemaPtr;
    json_util::ParseFromFile(pModelSchemaPath, schemaPtr);

    if (schemaPtr == nullptr)
    {
        _throwError(L"Failed to parse json file:%s", pModelSchemaPath);
    }

    // process 'reference' tag
    vector_file_reference_type referenceModelSchemas;
    GetFileReferences(pModelSchemaPath, schemaPtr,U("references"),referenceModelSchemas,parameters);

    for(vector_file_reference_type::const_iterator iter = referenceModelSchemas.begin();
        iter != referenceModelSchemas.end();
        ++iter)
    {
        std::string modelSchemaRelativePath = (*iter).second;
        if(!checkFileDependencies(_fh,modelSchemaRelativePath.c_str(),parameters))
        {
            return false;
        }
    }

    // process 'model' tags
    vector_file_reference_type referenceModels;
    GetFileReferences(pModelSchemaPath, schemaPtr,U("models"),referenceModels,parameters);

    std::vector<std::string> files;
    std::transform(
        referenceModels.begin(),
        referenceModels.end(),
        std::back_inserter(files),
        [](const vector_file_reference_type::value_type &pairItem){return pairItem.second;});


    return CheckFileDependencies(_fh,files);
};

template <class T>
static typename T::const_iterator findMatchingIterator(
            const T& arrayWrapper,
            LPCSTR_t targetProperty,
            const wchar_t *targetValue)
{
    foundation::string_t target_value_t = foundation::to_platform_string(targetValue);
    return std::find_if(arrayWrapper.begin(), arrayWrapper.end(), [=](foundation::IInspectable* item)
    {
        DictionaryClassPtr dictionary(item);
        string_t dictionaryValue = dictionary.Get<string_t>(targetProperty);
        return (_pal_strcmp(target_value_t.c_str(), dictionaryValue.c_str()) == 0);
    });
};

CModelSchema *CSchemaParser::parse(
    const char *pModelSchemaPath,
    const _ToolParameterType& parameters,
    DictionaryClassPtr& attribute_dictionary)
{
    assert(pModelSchemaPath);
    std::string grammarsDir = ToUtf8String(GetParameterValue(parameters,L"grammars",L"").c_str());
    // initialize C# grammar path
    std::string cSharpGrammarPath = grammarsDir + "CSharp_2.0_r7.cgt";
    
    // load model schema
    DictionaryPtr schemaPtr;
    json_util::ParseFromFile(pModelSchemaPath, schemaPtr);

    if (schemaPtr == nullptr)
    {
        _throwError(L"Failed to parse json file:%s", pModelSchemaPath);
    }

    HStringPtr _namespaceVal;
    schemaPtr.GetValue(U("namespace"), _namespaceVal);

    std::wstring _namespace = to_wstring(_namespaceVal);
    // Create the Model Schema
    std::unique_ptr<CModelSchema> modelSchema(new CModelSchema(_namespace.data()));

    AutoDeleteVector<CModelSchemaSymbols *> modelSchemaSymbolsVector;

    // process 'reference' tag
    vector_file_reference_type referenceModelSchemas;
    GetFileReferences(pModelSchemaPath, schemaPtr,U("references"),referenceModelSchemas,parameters);

    for(vector_file_reference_type::const_iterator iter = referenceModelSchemas.begin();
        iter != referenceModelSchemas.end();
        ++iter)
    {
        // pass empty attributes dictionary since
        // we parse this reference schema without the need to run any code gen
        const std::string& filePath((*iter).second);
        const std::string _jsonExtension = ".json";

        schema::CModelSchema *pReferenceModelSchema;
        if (0 == filePath.compare(filePath.length() - _jsonExtension.length(), _jsonExtension.length(), _jsonExtension))
        {
            // file path ends with .json extension so we can parse it directly

            std::fstream fis(filePath.c_str(), std::ios_base::in);
            pReferenceModelSchema = CSchemaSerializer::deserializeJson(fis);
        }
        else
        {
            DictionaryClassPtr reference_attribute_dictionary;
            pReferenceModelSchema = CSchemaParser::parse(
                (*iter).second.data(),
                parameters,
                reference_attribute_dictionary);
        }
        modelSchema->addReferenceSchema(pReferenceModelSchema);
    }

    // process default attributes for Model Schema root
    if (attribute_dictionary.HasKey(_PROPERTY_KEY_ATTRIBUTES))
    {
        DictionaryClassPtr schema_attributes = attribute_dictionary.Get<DictionaryClassPtr>(_PROPERTY_KEY_ATTRIBUTES);
        modelSchema->setAttributes(schema_attributes);
    }

    bool defaultIsSourceInherited = modelSchema->getAttribute(_PROPERTY_ATTRIBUTE_IS_SOURCE_INHERITED_KEY, false);

    // process 'model' tags
    vector_file_reference_type referenceModels;
    GetFileReferences(pModelSchemaPath, schemaPtr, U("models"),referenceModels,parameters);
    for(vector_file_reference_type::const_iterator iter = referenceModels.begin();
        iter != referenceModels.end();
        ++iter)
    {

        auto modelRdc = CParserEngineUtil::parseFile(
            (*iter).second.data(),
            cSharpGrammarPath.data()
            );
        modelSchemaSymbolsVector.push_back(CModelSchemaSymbols::parse((*iter).first.data(),modelRdc));
    }

    UINT32 typeUniqueId = 1;
    schemaPtr.GetValue(U("typeId"), typeUniqueId);

    bool isReserved = false;
    schemaPtr.GetValue(U("isReserved"), isReserved);
    if (isReserved)
    {
        typeUniqueId += 0xffff0000;
    }

    // Map to storage all the Model Types added by this schema
    class CPropertyModelInfoMap
    {
    public:
        CPropertyModelInfoMap(CInterfaceReferenceSymbol *pInterfaceReferenceSymbol,
            CModelSchemaSymbols *pModelSchemaSymbols,
            const IReferenceSymbol *pSourceType):
            _pModelSchemaSymbols(pModelSchemaSymbols),
            _pSourceType(pSourceType)
        {
            assert(pInterfaceReferenceSymbol);
            addInterfaceReferenceSymbol(pInterfaceReferenceSymbol);
        }

        CInterfaceReferenceSymbol *getPrimaryInterfaceSymbol()
        {
            return _interfaceReferenceSymbol.front();
        }
        CModelSchemaSymbols *getModelSchemaSymbols()
        {
            return _pModelSchemaSymbols;
        }
        const IReferenceSymbol *getSourceType()
        {
            return _pSourceType;
        }

        const std::vector<CEventReferenceSymbol *>& getAllEvents() { return _allEvents; }
        const std::vector<CPropertyReferenceSymbol *>& getAllProperties() { return _allProperties; }
        const std::vector<CMethodReferenceSymbol *>& getAllMethods() { return _allMethods; }

        void addInterfaceReferenceSymbol(CInterfaceReferenceSymbol *pInterfaceReferenceSymbol)
        {
            _interfaceReferenceSymbol.push_back(pInterfaceReferenceSymbol);
            _allEvents.insert(_allEvents.end(),pInterfaceReferenceSymbol->getEvents().begin(),pInterfaceReferenceSymbol->getEvents().end());
            _allProperties.insert(_allProperties.end(),pInterfaceReferenceSymbol->getProperties().begin(),pInterfaceReferenceSymbol->getProperties().end());
            _allMethods.insert(_allMethods.end(),pInterfaceReferenceSymbol->getMethods().begin(),pInterfaceReferenceSymbol->getMethods().end());
        }
    private:
        std::vector<CInterfaceReferenceSymbol*> _interfaceReferenceSymbol;
        CModelSchemaSymbols *_pModelSchemaSymbols;
        const IReferenceSymbol *_pSourceType;
        std::wstring _customInterfaceName;

        std::vector<CEventReferenceSymbol *>     _allEvents;
        std::vector<CPropertyReferenceSymbol *>  _allProperties;
        std::vector<CMethodReferenceSymbol *>    _allMethods;

    };

	typedef std::map<CObjectDispatchInfo *, std::pair<CStructReferenceSymbol *, CModelSchemaSymbols * >> _StructTypesSymbolMap;
	_StructTypesSymbolMap structTypesMap;

    typedef std::map<CObjectDispatchInfo *,std::shared_ptr<CPropertyModelInfoMap> > _ModelTypesSymbolMap;
    _ModelTypesSymbolMap modelTypesMap;

    typedef std::map<CInterfaceReferenceSymbol *,std::pair<CAttributeReferenceSymbol *,CModelSchemaSymbols *>> _EnumerationTypesSymbolMap;
    _EnumerationTypesSymbolMap enumerationTypesMap;

    // filter all shema attributes down to this model type
    foundation::InspectableArrayWrapper modelAttributes;
    if (attribute_dictionary.HasKey(_PROPERTY_KEY_PROPERTY_MODEL_ATTRIBUTES))
    {
        modelAttributes = attribute_dictionary.Get<foundation::InspectableArrayWrapper>(_PROPERTY_KEY_PROPERTY_MODEL_ATTRIBUTES);
    }

    // now create all the types collected
    for(std::vector<CModelSchemaSymbols *>::const_iterator iter = modelSchemaSymbolsVector.begin();
        iter != modelSchemaSymbolsVector.end();
        ++iter)
    {
        // Iterate Enums
        for(std::vector<CEnumTypeReferenceSymbol *>::const_iterator iterEnum = (*iter)->getEnumTypes().begin();
            iterEnum != (*iter)->getEnumTypes().end();
            ++iterEnum)
        {
            bool isFlags = false;
            const CEnumTypeInfo *pBaseEnumTypeInfo = nullptr;

            // resolve attributes at the interface level
            for (std::vector<CAttributeReferenceSymbol *>::const_iterator
                iterAttribute = (*iterEnum)->getAttributes().begin();
                iterAttribute != (*iterEnum)->getAttributes().end();
            ++iterAttribute)
            {
                if (wcscmp((*iterAttribute)->getReferenceValue(), L"Flags") == 0)
                {
                    isFlags = true;
                }
                else if (wcscmp((*iterAttribute)->getReferenceValue(), L"EnumBaseType") == 0)
                {
                    _AttributeExpressionMap attributes;
                    parseModelAttributes(*iterAttribute, *(*iter), attributes);
                    const IReferenceSymbol *pEnumBaseType = (const IReferenceSymbol *)lookupAttribute(attributes, L"BaseType");
                    if (pEnumBaseType != nullptr)
                    {
                        // Note: we don't support forward declaration of base types they have to be
                        // parsed before this type. Also we don't attempt to resolve this type on another
                        // schema namespace
                        pBaseEnumTypeInfo = modelSchema->resolveEnumType(pEnumBaseType->getReferenceValue());
                    }
                }
            }

            std::wstring typeEnumName =  (*iterEnum)->getReferenceValue();
            std::wstring typeEnumNameAndNamespace = _namespace + L"." + typeEnumName;
            IID iidType = CreateIIDTypeFromString(typeEnumNameAndNamespace.data());
            CEnumTypeInfo *pEnumTypeInfo = new CEnumTypeInfo(
                typeUniqueId++,
                typeEnumName.data(),
                iidType,
                isFlags,
                pBaseEnumTypeInfo,
                *(modelSchema.get()));

            UINT32 enumItemValue = 0;
            if (pBaseEnumTypeInfo != nullptr && pBaseEnumTypeInfo->getEnums().size())
            {
                // initial Enum Value will be inherited from base type
                enumItemValue = std::get<1>(pBaseEnumTypeInfo->getEnums()[pBaseEnumTypeInfo->getEnums().size() - 1]) + 1;
            }

            for(std::vector<CEnumItemReferenceSymbol *>::const_iterator iterEnumItem = (*iterEnum)->getEnumItems().begin();
                iterEnumItem != (*iterEnum)->getEnumItems().end();
                ++iterEnumItem)
            {
                LiteralType literalType = schema::decimal;
                const wchar_t *enumItemName = (*iterEnumItem)->getReferenceValue();
                if((*iterEnumItem)->getValue().hasReference())
                {
                    int radix = 10;
                    const wchar_t *pEnumValue = (*iterEnumItem)->getValue().getReferenceValue();
                    size_t len = wcslen(pEnumValue);
                    if(len && pEnumValue[0] == L'x')
                    {
                        ++pEnumValue;
                        radix = 16;
                        literalType = schema::hexadecimal;
                    }
                    else if(len>2 && pEnumValue[0] == L'0' && pEnumValue[1] == L'x')
                    {
                        pEnumValue += 2;
                        radix = 16;
                        literalType = schema::hexadecimal;
                    }
                    enumItemValue = (UINT32)wcstoul(pEnumValue,nullptr,radix);
                }
                pEnumTypeInfo->addEnumItem(enumItemName, enumItemValue, literalType);
                ++enumItemValue;
            }
            modelSchema->addEnumType(pEnumTypeInfo);
        }

		// Iterate Structs
		for (std::vector<CStructReferenceSymbol *>::const_iterator iterStruct = (*iter)->getStructTypes().begin();
		iterStruct != (*iter)->getStructTypes().end();
			++iterStruct)
		{
			std::wstring typeName = (*iterStruct)->getReferenceValue();

			std::wstring typeNameAndNamespace = _namespace + L"." + typeName;
			IID iidType = CreateIIDTypeFromString(typeNameAndNamespace.data());

			CObjectDispatchInfo *pPropertyDataInfo = new CObjectDispatchInfo(
				typeUniqueId,
				typeName.data(),
				iidType,
                ModelTypeInfoType_ObjectDispatch,
				nullptr,
				*(modelSchema.get()));

			modelSchema->addModelType(pPropertyDataInfo);
			structTypesMap[pPropertyDataInfo] = std::make_pair(*iterStruct,*iter);
			// next unique id
			++typeUniqueId;
		}

        // Iterate Interfaces
        for(std::vector<CInterfaceReferenceSymbol *>::const_iterator iterInterface = (*iter)->getInterfaceTypes().begin();
            iterInterface != (*iter)->getInterfaceTypes().end();
            ++iterInterface)
        {
            std::wstring typeName = (*iterInterface)->getReferenceValue();
                  
            _ModelTypesSymbolMap::iterator iterModelTypes = std::find_if(
                modelTypesMap.begin(),
                modelTypesMap.end(),
                [&typeName](const std::pair<CObjectDispatchInfo *,std::shared_ptr<CPropertyModelInfoMap>>& item)
                { 
                    return typeName == item.first->getName(); 
                });

            if(iterModelTypes != modelTypesMap.end())
            {
                (*iterModelTypes).second->addInterfaceReferenceSymbol(*iterInterface);
                continue;
            }

            std::wstring typeNameAndNamespace = _namespace + L"." + typeName;
            IID iidType = CreateIIDTypeFromString(typeNameAndNamespace.data());

            // find the model to get properties
            foundation::InspectableArrayWrapper::const_iterator itmodel =
                findMatchingIterator(modelAttributes, _PROPERTY_KEY_NAME, typeName.c_str());
            
            DictionaryClassPtr model_attributes;
            // we found the overrides for the schema. 
            if (itmodel != modelAttributes.end())
            {
                foundation::ComPtr<foundation::IInspectable> dictionaryVal;
                DictionaryClassPtr currentModel(static_cast<foundation::IInspectable*>(*itmodel));
                dictionaryVal = currentModel.Get<foundation::IInspectable*>(_PROPERTY_KEY_ATTRIBUTES,nullptr);
                if (dictionaryVal)
                {
                    model_attributes = DictionaryClassPtr(dictionaryVal);
                }
            }

            // Source Type Info support
            const IReferenceSymbol *pSourceType = nullptr;
            bool isInherited = defaultIsSourceInherited;
            bool isEnumerationInterface = false;
            std::wstring customInterfaceName = L"";
            bool isCustomPrivateInterface = true;
            bool isObservableObject = true;

            // resolve attributes at the interface level
            for(std::vector<CAttributeReferenceSymbol *>::const_iterator  
                iterAttribute = (*iterInterface)->getAttributes().begin();
                iterAttribute != (*iterInterface)->getAttributes().end();
                ++iterAttribute)
            {
                if( wcscmp((*iterAttribute)->getReferenceValue(),L"ModelAttribute") == 0)
                {
                    _AttributeExpressionMap attributes;
                    parseModelAttributes(*iterAttribute,*(*iter),attributes);
                    // attempt to find the direct 'SourceType'
                    pSourceType = (const IReferenceSymbol *)lookupAttribute(attributes,L"SourceType");
                    if (lookupAttribute(attributes, L"CustomInterfaceName"))
                    {
                        customInterfaceName = ((const IReferenceSymbol *)lookupAttribute(attributes, L"CustomInterfaceName"))->getReferenceValue();
                        // remove quotes
                        string_util::find_and_replace(customInterfaceName, std::wstring(L"\""), std::wstring(L""));
                        model_attributes.Set(_PROPERTY_ATTRIBUTE_CUSTOM_CONTAINER_NAME_KEY, customInterfaceName.c_str());

                    }
                    if (lookupAttribute(attributes, L"IsCustomPrivateInterface"))
                    {
                        model_attributes.Set(_PROPERTY_ATTRIBUTE_IS_CUSTOM_PRIVATE_INTERFACE_KEY, parseBoolean(*(const IReferenceSymbol *)lookupAttribute(attributes, L"IsCustomPrivateInterface"), *(*iter)));
                    }
                    if (lookupAttribute(attributes, L"IsCustomContainer"))
                    {
                        model_attributes.Set(_PROPERTY_ATTRIBUTE_IS_CUSTOM_CONTAINER_KEY, parseBoolean(*(const IReferenceSymbol *)lookupAttribute(attributes, L"IsCustomContainer"), *(*iter)));
                    }
                    if (lookupAttribute(attributes, L"RuntimeClassOptions"))
                    {
                        model_attributes.Set(_PROPERTY_ATTRIBUTE_RUNTIME_CLASS_OPTIONS_KEY, (UINT32)parseUInteger(*(const IReferenceSymbol *)lookupAttribute(attributes, L"RuntimeClassOptions"), *(*iter)));
                    }
                    if (lookupAttribute(attributes, L"IsObservableObject"))
                    {
                        isObservableObject = parseBoolean(*(const IReferenceSymbol *)lookupAttribute(attributes, L"IsObservableObject"), *(*iter));
                    }

                }
                else if (wcscmp((*iterAttribute)->getReferenceValue(), L"CollectionAttribute") == 0 ||
                    wcscmp((*iterAttribute)->getReferenceValue(), L"EnumerationAttribute") == 0)
                {
                    enumerationTypesMap[*iterInterface] = std::make_pair(*iterAttribute, *iter);
                    isEnumerationInterface = true;
                    break;
                }
            }
            if (isEnumerationInterface)
            {
                // skip Model creation
                continue;
            }

            CObjectDispatchInfo *pPropertyDataInfo = nullptr;

            if (!pSourceType && (*iterInterface)->getBaseType().hasReference())
            {
                const CQualifiedReferenceSymbol *pBaseType = &((*iterInterface)->getBaseType());
                // Source Type is null but we will attempt to navigate the tree looking for 
                // a BaseType that already define a 'SourceType' attribute
                CInterfaceReferenceSymbol *pBaseInterfaceSymbol = nullptr;

                while (pBaseType->hasReference() &&
                    (pBaseInterfaceSymbol =
                        CModelSchemaSymbols::lookupInterface(modelSchemaSymbolsVector,
                            pBaseType->getReferenceValue()))
                    && pSourceType == nullptr)
                {
                    for (std::vector<CAttributeReferenceSymbol *>::const_iterator
                        iterAttribute = pBaseInterfaceSymbol->getAttributes().begin();
                        iterAttribute != pBaseInterfaceSymbol->getAttributes().end();
                        ++iterAttribute)
                    {
                        if (wcscmp((*iterAttribute)->getReferenceValue(), L"ModelAttribute") == 0)
                        {
                            _AttributeExpressionMap attributes;
                            parseModelAttributes(*iterAttribute, *(*iter), attributes);
                            pSourceType = (const IReferenceSymbol *)lookupAttribute(attributes, L"SourceType");
                            break;
                        }
                    }
                    pBaseType = &pBaseInterfaceSymbol->getBaseType();
                }
            }

            if (pSourceType)
            {
                pPropertyDataInfo = new CPropertyViewModelInfo(
                    typeUniqueId,
                    typeName.data(),
                    iidType,
                    nullptr,
                    nullptr,            // null Source Type for now
                    isInherited,
                    *(modelSchema.get()));
            }
            else
            {
                if (!isObservableObject)
                {
                    pPropertyDataInfo = new CObjectDispatchInfo(
                        typeUniqueId,
                        typeName.data(),
                        iidType,
                        ModelTypeInfoType_ObjectDispatch,
                        nullptr,
                        *(modelSchema.get()));

                }
                else
                {
                    pPropertyDataInfo = new CObservableObjectInfo(
                        typeUniqueId,
                        typeName.data(),
                        iidType,
                        nullptr,
                        *(modelSchema.get()));
                }
            }
            modelSchema->addModelType(pPropertyDataInfo);

            // populate all the code generation attributes
            pPropertyDataInfo->setAttributes(model_attributes);

            // next unique id
            ++typeUniqueId;
            // add to our local map
            modelTypesMap[pPropertyDataInfo] = _ModelTypesSymbolMap::mapped_type(new CPropertyModelInfoMap(*iterInterface, *iter, pSourceType));
        }
    }

    // iterate to discovered 'Enumeration' custom types
    for (_EnumerationTypesSymbolMap::const_iterator iter = enumerationTypesMap.begin();
    iter != enumerationTypesMap.end();
        ++iter)
    {
        CInterfaceReferenceSymbol *pEnumerationInterfaceRefSymbol = (*iter).first;
        _AttributeExpressionMap attributes;
        parseEnumerationAttributes((*iter).second.first, *((*iter).second.second), attributes);
        const IReferenceSymbol *pEnumerationItemType = (const IReferenceSymbol *)lookupAttribute(attributes, L"ItemType");
        if (pEnumerationItemType)
        {
            schema::ModelTypeInfoType enumerationType;
            if (wcscmp(((*iter).second.first)->getReferenceValue(), L"EnumerationAttribute") == 0)
            {
                enumerationType = schema::ModelTypeInfoType_Enumerable;
            }
            else
            {
                // TODO: extract the model type based on the type name
                enumerationType = schema::ModelTypeInfoType_ObservableCollection;
            }
            // define a type safe enumeration type
            lookupOrCreateEnumerationType(
                typeUniqueId,
                pEnumerationInterfaceRefSymbol->getReferenceValue(),
                pEnumerationItemType,
                *((*iter).second.second),
                *modelSchema.get(),
                enumerationType);
            // TODO: verify this interface is empty
        }
    }

    // all the 'stub' types are on the schema now resolve properties/methods/base type/etc.
	for (_StructTypesSymbolMap::const_iterator iter = structTypesMap.begin();
		iter != structTypesMap.end();
		++iter)
	{
		UINT32 propertyId = 1;

		CStructReferenceSymbol *pStructFieldReferenceSymbol = (*iter).second.first;

		// add properties
		for (std::vector<CStructFieldReferenceSymbol *>::const_iterator
			iterField = pStructFieldReferenceSymbol->getFields().begin();
			iterField != pStructFieldReferenceSymbol->getFields().end();
			++iterField)
		{
			const wchar_t *propertyName = (*iterField)->getReferenceValue();
			const wchar_t *propertyTypeIdentifier = (*iterField)->getType().getReferenceValue();
			PropertyType propertyType = PropertyType_Empty;

			const CTypeInfo *pModelInfoType = nullptr;
			resolveTypeType(
				(*iterField)->getType(),
				*((*iter).second.second),
				modelSchema.get(),
				propertyType,
				pModelInfoType);
			UINT32 propertyFlags = foundation::PropertyFlagType::CanRead |
				foundation::PropertyFlagType::CanWrite;
			// add Enum Flags if needed
			if (pModelInfoType && pModelInfoType->getModelType() == ModelTypeInfoType_Enum)
			{
				propertyFlags |= foundation::PropertyFlagType::IsEnum;
			}

			CPropertyInfo *pStructPropertyInfo = new CPropertyInfo(
				propertyId,
				propertyName,
				propertyType,
				pModelInfoType,
				propertyFlags,
				*((*iter).first));

			(*iter).first->addProperty(pStructPropertyInfo);
			++propertyId;
		}
	}


    // first pass will resolve the BaseType & SourceType
    for(_ModelTypesSymbolMap::const_iterator iter = modelTypesMap.begin();
        iter != modelTypesMap.end();
        ++iter)
    {
        CTypeInfo *pTypeInfo = (*iter).first;
        CInterfaceReferenceSymbol *pInterfaceReferenceSymbol = (*iter).second->getPrimaryInterfaceSymbol();
        CModelSchemaSymbols *pModelSchemaSymbols =  (*iter).second->getModelSchemaSymbols();
        // resolve Base Type
        if(pInterfaceReferenceSymbol->getBaseType().hasReference())
        {
            pTypeInfo->setBaseType(
                resolveTypeFromSymbol(
                pInterfaceReferenceSymbol->getBaseType(),
                *pModelSchemaSymbols,
                modelSchema.get()));
        }
        // resolve Source Type for View Models
        if(pTypeInfo->getModelType() == ModelTypeInfoType_ViewModel)
        {
            assert((*iter).second->getSourceType());
            ((CPropertyViewModelInfo *)pTypeInfo)->setSourceType(
                (CObservableObjectInfo *) resolveTypeFromSymbol(
                *((*iter).second->getSourceType()),
                *pModelSchemaSymbols,
                modelSchema.get())
                );
        }
    }

    // second pass will add properties/methods
    for(_ModelTypesSymbolMap::const_iterator iter = modelTypesMap.begin();
        iter != modelTypesMap.end();
        ++iter)
    {
        CObjectDispatchInfo *pPropertyDataInfo = (*iter).first;
        CPropertyModelInfoMap *pPropertyModelInfoMap = (*iter).second.get();
        CModelSchemaSymbols *pModelSchemaSymbols =  (*iter).second->getModelSchemaSymbols();

        UINT32 propertyId = 1;
        UINT32 methodId = 1;
        UINT32 eventId = 1;

        // next block will calculate the offset for properties/methods/events id
        CObjectDispatchInfo *pBaseType = (CObjectDispatchInfo *)pPropertyDataInfo->getBaseType();
        while(pBaseType != nullptr)
        {
            if(&pBaseType->getParent() == modelSchema.get())
            {
                // the base type is in our namespace but we are not sure if the properties
                // were populated so we will need the symbols info
                _ModelTypesSymbolMap::const_iterator iterFind = modelTypesMap.find(pBaseType);
                assert(iterFind != modelTypesMap.end());
                // add properties offset
                propertyId += (UINT32)countProperties((*iterFind).second->getAllProperties());
                // add properties offset
                methodId += (UINT32)(*iterFind).second->getAllMethods().size();
                // add events offset
                eventId += (UINT32)(*iterFind).second->getAllEvents().size();
            }
            else
            {
                // this type is on a referenced Model schema and their properties were already populated
                // add properties offset
                propertyId += (UINT32)pBaseType->getProperties().size();
                methodId += (UINT32)pBaseType->getMethods().size();

                if (pPropertyDataInfo->getModelType() != ModelTypeInfoType_ObjectDispatch)
                {
                    eventId += (UINT32)((CObservableObjectInfo *)pBaseType)->getEvents().size();
                }
            }
            // next base type
            pBaseType = (CObservableObjectInfo *)pBaseType->getBaseType();
        }

        // add properties
        for(std::vector<CPropertyReferenceSymbol *>::const_iterator  
            iterProperty = pPropertyModelInfoMap->getAllProperties().begin();
            iterProperty != pPropertyModelInfoMap->getAllProperties().end();
            ++iterProperty)
        {
            const wchar_t *propertyName = (*iterProperty)->getReferenceValue();
            const wchar_t *propertyTypeIdentifier = (*iterProperty)->getType().getReferenceValue();
            PropertyType propertyType = PropertyType_Empty;
            const CTypeInfo *pModelInfoType = nullptr;

            // find the model to get properties
            foundation::InspectableArrayWrapper::const_iterator itmodel =
                findMatchingIterator(modelAttributes, _PROPERTY_KEY_NAME, pPropertyDataInfo->getName().c_str());

            foundation::InspectableArrayWrapper property_attributes;
            // we found the overrides for the schema. 
            if (itmodel != modelAttributes.end())
            {
                DictionaryClassPtr currentModel(static_cast<foundation::IInspectable*>(*itmodel));
                property_attributes = currentModel.Get<foundation::InspectableArrayWrapper>(_PROPERTY_KEY_PROPERTIES);
            }

            schema::ModelTypeInfoType enumerationType = ModelTypeInfoType_None;
            if (wcscmp(propertyTypeIdentifier, L"IList") == 0)
            {
                enumerationType = schema::ModelTypeInfoType_ObservableList;
            }
            else if (wcscmp(propertyTypeIdentifier, L"ICollection") == 0)
            {
                enumerationType = schema::ModelTypeInfoType_ObservableCollection;
            }
            else if (wcscmp(propertyTypeIdentifier, L"IEnumerable") == 0)
            {
                enumerationType = schema::ModelTypeInfoType_Enumerable;
            }

            bool isCommand = wcscmp(propertyTypeIdentifier,L"ICommand") == 0;

            // Command Type Info support
            const IReferenceSymbol *pCommandParameterType = nullptr;
            const IReferenceSymbol *pCommandResultType = nullptr;
            bool commandIsAsync = false;
            bool commandIsUnknownState = false;
            // Collection Type Info support
            const IReferenceSymbol *pEnumerationItemType = nullptr;

            DictionaryClassPtr attributes;

            foundation::InspectableArrayWrapper::const_iterator it =
                findMatchingIterator(property_attributes, _PROPERTY_KEY_NAME, propertyName);
         
            // we found the overrides for the schema. 
            if (it != property_attributes.end())
            {
                DictionaryClassPtr propertydic = DictionaryClassPtr((*it));
                foundation::ComPtr<foundation::IInspectable> attbs = propertydic.Get<foundation::IInspectable *>(_PROPERTY_KEY_ATTRIBUTES);
                attributes = DictionaryClassPtr(attbs);
            }
            
            // get all the keys in the dictionary
            DictionaryClassPtr default_attributes_vals = DictionaryClassPtr();
            std::vector<DictionaryClassPtr::pair> attribute_view = attributes.getview();

            // iterate all the keys and add the default attributes that were define in the json fall.
            std::for_each(attribute_view.begin(), attribute_view.end(), [&default_attributes_vals](DictionaryClassPtr::pair p) {
                default_attributes_vals.Set(foundation::get_string_t(p.first).c_str(), p.second.Get());
            });
                
            UINT32 propertyFlags = 0;
            // resolve attributes at the property level
            for(std::vector<CAttributeReferenceSymbol *>::const_iterator  
                iterAttribute = (*iterProperty)->getAttributes().begin();
                iterAttribute != (*iterProperty)->getAttributes().end();
                ++iterAttribute)
            {
                if( wcscmp((*iterAttribute)->getReferenceValue(),L"PropertyAttribute") == 0)
                {
                    for(std::vector<CAttributeExpressionSymbol *>::const_iterator  
                        iterAttributeExpression = (*iterAttribute)->getAttributeExpressions().begin();
                        iterAttributeExpression != (*iterAttribute)->getAttributeExpressions().end();
                        ++iterAttributeExpression)
                    {
                        if(wcscmp((*iterAttributeExpression)->getReferenceValue(),L"BindingSourcePath") == 0)
                        {
                            std::wstring bindingPath = (*iterAttributeExpression)->getReferenceSymbol().getReferenceValue();
                            // remove quotes
                            string_util::find_and_replace(bindingPath, std::wstring(L"\""), std::wstring(L""));
                            default_attributes_vals.Set(U("bindingSourcePath"), bindingPath.c_str());
                        }
                        else
                        {
                            const wchar_t *propertyAttributeName = (*iterAttributeExpression)->getReferenceValue();
                            if (wcscmp(propertyAttributeName, L"RuntimeClassOptions") == 0)
                            {
                                default_attributes_vals.Set(U("runtimeClassOptions"), (UINT32)parseUInteger((*iterAttributeExpression)->getReferenceSymbol(), *pModelSchemaSymbols));
                            }
                            else if(wcscmp(propertyAttributeName, L"OptionFlags") == 0)
                            {
                                default_attributes_vals.Set(U("optionFlags"), (UINT32)parseUInteger((*iterAttributeExpression)->getReferenceSymbol(), *pModelSchemaSymbols));
                            }
                            else
                            {
                                bool boolValue = parseBoolean((*iterAttributeExpression)->getReferenceSymbol(),*pModelSchemaSymbols);

                                if (wcscmp(propertyAttributeName, L"IsCacheEnabled") == 0)
                                {
                                    if (!boolValue)
                                    {
                                        propertyFlags |= foundation::PropertyFlagType::NoCache;
                                    }
                                }
                                else if (wcscmp(propertyAttributeName, L"IsReserved") == 0)
                                {
                                    if (boolValue)
                                    {
                                        propertyFlags |= foundation::PropertyFlagType::IsReserved;
                                    }
                                }
                                else if (wcscmp(propertyAttributeName, L"IsAncestor") == 0)
                                {
                                    if (boolValue)
                                    {
                                        propertyFlags |= foundation::PropertyFlagType::IsAncestor;
                                    }
                                }
                                else if (wcscmp(propertyAttributeName, L"IsWeakReference") == 0)
                                {
                                    if (boolValue)
                                    {
                                        propertyFlags |= foundation::PropertyFlagType::IsWeakReference;
                                    }
                                }
                                else if (wcscmp(propertyAttributeName, L"IsParent") == 0)
                                {
                                    default_attributes_vals.Set(U("isParent"), boolValue);
                                }
                                else if (wcscmp(propertyAttributeName, L"IsCustomContainer") == 0)
                                {
                                    default_attributes_vals.Set(U("isCustomContainer"), boolValue);
                                }
                                else if (wcscmp(propertyAttributeName, L"IsRuntimeActivated") == 0)
                                {
                                    default_attributes_vals.Set(U("isRuntimeActivated"), boolValue);
                                }
                                else
                                {
                                    _throwError(L"Unsupported Attribute on File:%ls, line:%d col:%d",
                                        pModelSchemaSymbols->getModelFilePath().data(),
                                        (*iterAttribute)->getSymbolReference()->Line,
                                        (*iterAttribute)->getSymbolReference()->Column
                                        );
                                }
                            }
                        }
                    }
                }
                else if(wcscmp((*iterAttribute)->getReferenceValue(),L"CommandAttribute") == 0)
                {
                    if(!isCommand)
                    {
                        _throwError(L"Command type expected on File:%ls, line:%d col:%d",
                            pModelSchemaSymbols->getModelFilePath().data(),
                            (*iterAttribute)->getSymbolReference()->Line,
                            (*iterAttribute)->getSymbolReference()->Column
                            );
                    }

                    for(std::vector<CAttributeExpressionSymbol *>::const_iterator  
                        iterAttributeExpression = (*iterAttribute)->getAttributeExpressions().begin();
                        iterAttributeExpression != (*iterAttribute)->getAttributeExpressions().end();
                        ++iterAttributeExpression)
                    {
                        const wchar_t *commandAttributeExpression = (*iterAttributeExpression)->getReferenceValue();

                        if(wcscmp(commandAttributeExpression,L"Parameter") == 0)
                        {
                            pCommandParameterType = &(*iterAttributeExpression)->getReferenceSymbol();
                        }
                        else if(wcscmp(commandAttributeExpression,L"Result") == 0)
                        {
                            pCommandResultType = &(*iterAttributeExpression)->getReferenceSymbol();
                        }
                        else if(wcscmp(commandAttributeExpression,L"IsAutoGenerate") == 0)
                        {
                            default_attributes_vals.Set(U("isAutoGenerate"), parseBoolean((*iterAttributeExpression)->getReferenceSymbol(), *pModelSchemaSymbols));
                        }
                        else if(wcscmp(commandAttributeExpression,L"IsAsync") == 0)
                        {
                            commandIsAsync = parseBoolean((*iterAttributeExpression)->getReferenceSymbol(),*pModelSchemaSymbols);
                        }
                        else if(wcscmp(commandAttributeExpression,L"IsUnknownState") == 0)
                        {
                            commandIsUnknownState = parseBoolean((*iterAttributeExpression)->getReferenceSymbol(),*pModelSchemaSymbols);
                        }                       
                        else
                        {
                            _throwError(L"Unrecognized expression:%ls expected on File:%ls, line:%d col:%d",
                                commandAttributeExpression,
                                pModelSchemaSymbols->getModelFilePath().data(),
                                (*iterAttributeExpression)->getSymbolReference()->Line,
                                (*iterAttributeExpression)->getSymbolReference()->Column
                            );
                        }
                    }
                }
                else if(wcscmp((*iterAttribute)->getReferenceValue(),L"CollectionAttribute") == 0)
                {
                    if(enumerationType == schema::ModelTypeInfoType_None)
                    {
                        _throwError(L"Collection type expected on File:%ls, line:%d col:%d",
                            pModelSchemaSymbols->getModelFilePath().data(),
                            (*iterAttribute)->getSymbolReference()->Line,
                            (*iterAttribute)->getSymbolReference()->Column
                            );
                    }
                    _AttributeExpressionMap attributes;
                    parseEnumerationAttributes(*iterAttribute,*pModelSchemaSymbols,attributes);
                    pEnumerationItemType = (const IReferenceSymbol *)lookupAttribute(attributes,L"ItemType");
                }
                else if(wcscmp((*iterAttribute)->getReferenceValue(), L"PropertyAttributeEx") == 0)
                {
                    // for any custom property attributes, just spit them all out to .json's attributes section as they are
                    for(std::vector<CAttributeExpressionSymbol *>::const_iterator
                            iterAttributeExpression = (*iterAttribute)->getAttributeExpressions().begin();
                        iterAttributeExpression != (*iterAttribute)->getAttributeExpressions().end();
                        ++iterAttributeExpression)
                    {
                        std::wstring value = (*iterAttributeExpression)->getReferenceSymbol().getReferenceValue();
                        string_util::find_and_replace(value, std::wstring(L"\""), std::wstring(L""));
                        default_attributes_vals.Set(
                            foundation::to_platform_string((*iterAttributeExpression)->getReferenceValue()).c_str(),
                            foundation::to_platform_string(value.c_str()).c_str());
                    }
                }
            }

            if((*iterProperty)->hasGet())
            {
                propertyFlags |= foundation::PropertyFlagType::CanRead;
            }

            if((*iterProperty)->hasSet())
            {
                propertyFlags |= foundation::PropertyFlagType::CanWrite;
            }

            if(isCommand)
            {
                propertyType = PropertyType_Inspectable;

                // build a unique type name for this command type
                std::wstring commandTypeName = buildUniqueCommandTypeName(
                    pCommandParameterType,
                    pCommandResultType,
                    commandIsAsync,
                    commandIsUnknownState);

                // attempt to resolve on our schema
                pModelInfoType = modelSchema->resolveModelType(commandTypeName.data());
                if(!pModelInfoType)
                {
                    std::wstring typeNameAndNamespace = _namespace + L"." + commandTypeName;
                    IID iidType = CreateIIDTypeFromString(typeNameAndNamespace.data());

                    pModelInfoType = new CCommandTypeInfo(
                        typeUniqueId++,
                        commandTypeName.data(),
                        iidType,
                        *modelSchema.get(),
                        createPropertyTypeInfo(pCommandParameterType,*pModelSchemaSymbols,modelSchema.get()),
                        createPropertyTypeInfo(pCommandResultType,*pModelSchemaSymbols,modelSchema.get()),
                        commandIsAsync,
                        commandIsUnknownState
                        );
                    modelSchema->addModelType(pModelInfoType);
                    if(commandIsAsync && pCommandResultType && !isPropertyTypeReferenceSymbol(*pCommandResultType))
                    {
                        lookupOrCreateAsyncOperationType(typeUniqueId,pCommandResultType,*pModelSchemaSymbols,*modelSchema.get());
                    }
                }
            }
            else if(enumerationType != schema::ModelTypeInfoType_None)
            {
                propertyType = PropertyType_Inspectable;
                pModelInfoType = lookupOrCreateEnumerationType(typeUniqueId,pEnumerationItemType,*pModelSchemaSymbols,*modelSchema.get(), enumerationType);
            }
            else
            {
                resolveTypeType(
                    (*iterProperty)->getType(),
                    *pModelSchemaSymbols,
                    modelSchema.get(),
                    propertyType,
                    pModelInfoType);
                // add Enum Flags if needed
                if(pModelInfoType && pModelInfoType->getModelType() == ModelTypeInfoType_Enum)
                {
                    propertyFlags |= foundation::PropertyFlagType::IsEnum;
                }
            }
            assert(propertyType != PropertyType_Empty);
            
            bool isParent = default_attributes_vals.HasKey(U("isParent")) && default_attributes_vals.Get<bool>(U("isParent"));

            CPropertyInfo *pPropertyInfo = new CPropertyInfo(
                isParent ? 0x2000 + 1 : propertyId,
                propertyName,
                propertyType,
                pModelInfoType,
                propertyFlags,
                *pPropertyDataInfo);

            // update the property info attributes
            pPropertyInfo->setAttributes(default_attributes_vals);

            bool isParentTest = pPropertyInfo->isParent();

            // add to Model Type Info
            pPropertyDataInfo->addProperty(pPropertyInfo);
            if(!isParent)
            {
                // next Property Id
                ++propertyId;
            }
        }

        // add methods
        for(std::vector<CMethodReferenceSymbol *>::const_iterator  
            iterMethod = pPropertyModelInfoMap->getAllMethods().begin();
            iterMethod != pPropertyModelInfoMap->getAllMethods().end();
            ++iterMethod)
        {
            bool methodIsAsync = false;
            // resolve attributes at the method level
            for(std::vector<CAttributeReferenceSymbol *>::const_iterator  
                iterAttribute = (*iterMethod)->getAttributes().begin();
                iterAttribute != (*iterMethod)->getAttributes().end();
                ++iterAttribute)
            {
                if( wcscmp((*iterAttribute)->getReferenceValue(),L"MethodAttribute") == 0)
                {
                    for(std::vector<CAttributeExpressionSymbol *>::const_iterator  
                        iterAttributeExpression = (*iterAttribute)->getAttributeExpressions().begin();
                        iterAttributeExpression != (*iterAttribute)->getAttributeExpressions().end();
                    ++iterAttributeExpression)
                    {
                        const wchar_t *methodAttributeExpression = (*iterAttributeExpression)->getReferenceValue();
                        if(wcscmp(methodAttributeExpression,L"IsAsync") == 0)
                        {
                            methodIsAsync = parseBoolean((*iterAttributeExpression)->getReferenceSymbol(),*pModelSchemaSymbols);
                        }
                        else
                        {
                            _throwError(L"Unrecognized expression:%ls expected on File:%ls, line:%d col:%d",
                                methodAttributeExpression,
                                pModelSchemaSymbols->getModelFilePath().data(),
                                (*iterAttributeExpression)->getSymbolReference()->Line,
                                (*iterAttributeExpression)->getSymbolReference()->Column
                                );
                        }
                    }
                }
            }

            const wchar_t *methodName = (*iterMethod)->getReferenceValue();
            PropertyType resultType;
            const CTypeInfo *pResultModelInfoType = nullptr;
            resolveTypeType(
                (*iterMethod)->getResultType(),
                *pModelSchemaSymbols,
                modelSchema.get(),
                resultType,
                pResultModelInfoType);

            CMethodInfo *pMethodInfo = new CMethodInfo(
                methodId,
                methodName,
                new CPropertyTypeInfo(resultType,pResultModelInfoType),
                methodIsAsync,
                *pPropertyDataInfo);

            for(std::vector<CFormalReferenceSymbol *>::const_iterator  
                iterFormalParameter = (*iterMethod)->getParameters().begin();
                iterFormalParameter != (*iterMethod)->getParameters().end();
                ++iterFormalParameter)
            {
                const wchar_t *parameterName = (*iterFormalParameter)->getReferenceValue();
                PropertyType parameterType;
                const CTypeInfo *pParameterModelInfoType = nullptr;
                // resolve
                resolveTypeType(
                    (*iterFormalParameter)->getParameterType(),
                    *pModelSchemaSymbols,
                    modelSchema.get(),
                    parameterType,
                    pParameterModelInfoType);

                CMethodParamInfo *pMethodParamInfo = new CMethodParamInfo(
                    parameterName,
                    parameterType,
                    pParameterModelInfoType);
                pMethodInfo->addParameter(pMethodParamInfo);
            }
            
            auto resultTypeSymbol = (*iterMethod)->getResultType();
            if(methodIsAsync
                && (pResultModelInfoType != nullptr && !pResultModelInfoType->getParent().isFactorySchema())
                && !isPropertyTypeReferenceSymbol(resultTypeSymbol))
            {
                lookupOrCreateAsyncOperationType(typeUniqueId,&resultTypeSymbol,*pModelSchemaSymbols,*modelSchema.get());
            }

            pPropertyDataInfo->addMethod(pMethodInfo);
            // next Method Id
            ++methodId;
        }
        if (pPropertyDataInfo->getModelType() != ModelTypeInfoType_ObjectDispatch)
        {

            CObservableObjectInfo *pObservableObjectInfo = (CObservableObjectInfo *)pPropertyDataInfo;
        // add events
            for (std::vector<CEventReferenceSymbol *>::const_iterator
                iterEvent = pPropertyModelInfoMap->getAllEvents().begin();
                iterEvent != pPropertyModelInfoMap->getAllEvents().end();
                ++iterEvent)
            {
                const wchar_t *eventName = (*iterEvent)->getReferenceValue();
                // EventArgs Type Info support
                const IReferenceSymbol *pEventArgsType = nullptr;

                // resolve attributes at the event level
                for (std::vector<CAttributeReferenceSymbol *>::const_iterator
                    iterAttribute = (*iterEvent)->getAttributes().begin();
                    iterAttribute != (*iterEvent)->getAttributes().end();
                    ++iterAttribute)
                {
                    if (wcscmp((*iterAttribute)->getReferenceValue(), L"EventAttribute") == 0)
                    {
                        _AttributeExpressionMap attributes;
                        parseEventAttributes(*iterAttribute, *pModelSchemaSymbols, attributes);
                        pEventArgsType = (const IReferenceSymbol *)lookupAttribute(attributes, L"EventArgs");
                    }
                }

                // Add Event Info
                CEventInfo *pEventInfo = new CEventInfo(
                    eventId,
                    eventName,
                    createPropertyTypeInfo(pEventArgsType, *pModelSchemaSymbols, modelSchema.get()),
                    *pObservableObjectInfo);

                pObservableObjectInfo->addEvent(pEventInfo);
                // next Event Id
                ++eventId;
            }
        }
    }

    return modelSchema.release();
}
