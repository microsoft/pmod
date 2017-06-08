/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SchemaSerializer.cpp
****/

#include "SchemaSerializer.h"
#include "ModelSchema.h"
#include "SerializeUtil.h"

#include "StringUtils.h"
#include "GuidHelper.h"

// for Json support
#include "jansson.h"

const char *_NAMESPACE_KEY = "namespace";
const char *_ID_KEY = "id";
const char *_NAME_KEY = "name";
const char *_CUSTOM_INTERFACE_NAME_KEY = "customInterfaceName";
const char *_MODEL_IS_CUSTOM_PRIVATE_INTERFACE_KEY = "isCustomPrivateInterface";
const char *_MODEL_IS_CUSTOM_CONTAINER_KEY = "isCustomContainer";
const char *_MODEL_RUNTIME_CLASS_OPTIONS_KEY = "runtimeClassOptions";
const char *_MODEL_OPTION_FLAGS_KEY = "optionFlags";

const char *_VALUE_KEY = "value";
const char *_IID_KEY = "iid";
const char *_LITERAL_TYPE_KEY = "literalType";

const char *_ENUM_ISFLAGS_KEY = "isFlags";
const char *_ENUM_VALUES_KEY = "enumvalues";

const char *_ENUM_TYPES_KEY = "enumTypes";
const char *_SCHEMA_REFERENCES_KEY = "references";
const char *_SCHEMA_PROPERTY_MODEL_TYPES_KEY = "propertyModelTypes";
const char *_SCHEMA_MODEL_TYPES_KEY = "modelTypes";

const char *_PROPERTY_MODEL_TYPE_INFO_TYPE = "modelType";
const char *_PROPERTY_MODEL_IS_INHERITED_KEY = "isInherited";
const char *_PROPERTY_MODEL_PROPERTIES_KEY = "properties";
const char *_PROPERTY_MODEL_METHODS_KEY = "methods";
const char *_PROPERTY_MODEL_EVENTS_KEY = "events";
const char *_PROPERTY_FLAGS_KEY = "flags";
const char *_PROPERTY_ATTRIBUTES_KEY = "attributes";

const char *_IS_AUTO_GENERATE_KEY = "isAutoGenerate";
const char *_IS_UNKNOWNSTATE_KEY = "isUnknownState";

const char *_PARAMETERS_KEY = "parameters";

const char *_BASE_TYPE_KEY = "baseType";
const char *_SOURCE_TYPE_INFO_KEY = "sourceTypeInfo";
const char *_EVENT_ARGS_TYPE_INFO_KEY = "eventArgsTypeInfo";
const char *_PARAMETER_TYPE_INO_KEY = "parameterTypeInfo";
const char *_RESULT_TYPE_INFO_KEY = "resultTypeInfo";
const char *_ITEM_TYPE_INFO_KEY = "itemTypeInfo";
const char *_IS_ASYNC_KEY = "isAsync";

const char *_MODEL_TYPE_INFO_REF_KEY = "modelTypeInfoRef";
const char *_MODEL_TYPE_REF_KEY = "modelTypeRef";
const char *_MODEL_TYPE_KEY = "modelType";
const char *_PROPERTY_TYPE_KEY = "propertyType";

using namespace schema;

static json_t *json_wstring(const wchar_t *value)
{
    return json_string(ToUtf8String(value).c_str());
}

static json_t *json_wstring(const std::wstring& s)
{
    return json_wstring(s.c_str());
}

static json_t *json_iid(GUID iidType)
{
    return json_string(::ToString(iidType).c_str());
}

static std::wstring  json_wstring_value(const json_t *string)
{
    return foundation::to_pal_wstring(json_string_value(string)).get();
}

static IID  json_iid_value(const json_t *string)
{
    IID iid;
    ::FromString(json_string_value(string), iid);
    return iid;
}

static json_t *json_createModelTypeInfoRef(const CTypeInfo *pModelTypeInfo)
{
    if(pModelTypeInfo)
    {
        std::wstring typeNameAndNamespace = pModelTypeInfo->getParent().getNamespace() +
            L"." + pModelTypeInfo->getName();
        return json_wstring(typeNameAndNamespace);
    }
    else
    {
        return json_null();
    }
}

static void json_setModelTypeInfoRef(json_t* json_object,const char*key,const CTypeInfo *pModelTypeInfo)
{
    json_object_set(json_object,key,json_createModelTypeInfoRef(pModelTypeInfo));
}

static json_t *json_createPropertyTypeInfoRef(const CPropertyTypeInfo* propertyTypeInfo)
{
    if(propertyTypeInfo)
    {
        json_t *jsonPropertyTypeInfoRef = json_object();
        json_object_set_new(jsonPropertyTypeInfoRef, _PROPERTY_TYPE_KEY, json_integer(propertyTypeInfo->getPropertyType()));
        json_object_set_new(jsonPropertyTypeInfoRef,_MODEL_TYPE_INFO_REF_KEY,json_createModelTypeInfoRef(propertyTypeInfo->getModelType()));
        return jsonPropertyTypeInfoRef;
    }
    else
    {
        return json_null();
    }
}

static void json_setPropertyTypeInfoRef(json_t* json_object,const char*key,const CPropertyTypeInfo* propertyTypeInfo)
{
    json_object_set(json_object,key,json_createPropertyTypeInfoRef(propertyTypeInfo));
}

static const CTypeInfo *json_getModelTypeInfoRef(json_t *jsonModelTypeRef,const CModelSchema& modelSchema)
{
    if(!json_is_null(jsonModelTypeRef))
    {
        std::wstring typeNameAndNamespace = json_wstring_value(jsonModelTypeRef);

        return modelSchema.resolveModelType(typeNameAndNamespace.data());
    }
    else
    {
        return nullptr;
    }
}

static const CTypeInfo *json_getModelTypeInfoRef(json_t* json_object,const char*key,const CModelSchema& modelSchema)
{
    return json_getModelTypeInfoRef(json_object_get(json_object,key),modelSchema);
}

static CPropertyTypeInfo *json_getPropertyTypeInfoRef(json_t *jsonPropertyTypeInfoRef,const CModelSchema& modelSchema)
{
    if(!json_is_null(jsonPropertyTypeInfoRef))
    {
        const CTypeInfo *pModelTypeInfo = json_getModelTypeInfoRef(json_object_get(jsonPropertyTypeInfoRef,_MODEL_TYPE_INFO_REF_KEY),modelSchema);
        foundation::PropertyType propertyType = 
            (foundation::PropertyType)json_integer_value(json_object_get(jsonPropertyTypeInfoRef, _PROPERTY_TYPE_KEY));
        return new CPropertyTypeInfo(propertyType,pModelTypeInfo);
    }
    return nullptr;
}

static CPropertyTypeInfo *json_getPropertyTypeInfoRef(json_t* json_object,const char*key,const CModelSchema& modelSchema)
{
    return json_getPropertyTypeInfoRef(json_object_get(json_object,key),modelSchema);
}

static void serializeModelTypeInfoRef(binary_serializer& os,const CTypeInfo *pModelTypeInfo)
{
    os.serializeBoolean(pModelTypeInfo != nullptr);
    if(pModelTypeInfo)
    {
        std::wstring typeNameAndNamespace = pModelTypeInfo->getParent().getNamespace() +
            L"." + pModelTypeInfo->getName();
        os.serializeWideString(typeNameAndNamespace);
    }
}

static const CTypeInfo * deserializeModelTypeInfoRef(binary_deserializer& is,const CModelSchema& modelSchema)
{
    if(is.deserializeBoolean())
    {
        std::wstring typeNameAndNamespace = is.deserializeWideString();
        return modelSchema.resolveModelType(typeNameAndNamespace.data());
    }
    return nullptr;
}

static void serializePropertyTypeInfoRef(binary_serializer& os,const CPropertyTypeInfo* propertyTypeInfo)
{
    os.serializeBoolean(propertyTypeInfo != nullptr);
    if(propertyTypeInfo)
    {
        os.serializeUInt(propertyTypeInfo->getPropertyType());
        serializeModelTypeInfoRef(os,propertyTypeInfo->getModelType());
    }
}

static CPropertyTypeInfo *deserializePropertyTypeInfoRef(binary_deserializer& is,const CModelSchema& modelSchema)
{
    if(is.deserializeBoolean())
    {
        foundation::PropertyType propertyType = 
            (foundation::PropertyType)is.deserializeUInt();
        const CTypeInfo *pModelTypeInfo = deserializeModelTypeInfoRef(is,modelSchema);
        return new CPropertyTypeInfo(propertyType,pModelTypeInfo);
    }
    return nullptr;
}

static json_t *dictionaryToJson(const foundation::library::DictionaryClassPtr& dictionary)
{
    foundation::library::DictionaryClassPtr::view dictionary_view = dictionary.getview();

    json_t *json_Dictionary = json_object();
    for (foundation::library::DictionaryClassPtr::view::const_iterator iter = dictionary_view.begin();
        iter != dictionary_view.end();
        ++iter)
    {
        std::string name = ToUtf8String((*iter).first);

        foundation::PropertyType propertyType;
        foundation::pv_util::GetType((*iter).second, &propertyType);
        // only suppor a short set of conversion
        if (propertyType == foundation::PropertyType_Boolean)
        {
            boolean bval(false);
            foundation::pv_util::GetBooleanValue((*iter).second, &bval);
            json_object_set_new(json_Dictionary, name.c_str(), json_boolean(bval ? true : false));
        }
        else if (propertyType == foundation::PropertyType_String)
        {
            foundation::HStringPtr value;
            foundation::pv_util::GetStringValue((*iter).second, value.GetAddressOf());
            json_object_set_new(json_Dictionary, name.c_str(), json_string(ToUtf8String((HSTRING)value).c_str()));
        }
        else
        {
            INT64 value(0);
            foundation::pv_util::GetInt64Value((*iter).second, &value);
            json_object_set_new(json_Dictionary, name.c_str(), json_integer(value));
        }
    }

    return json_Dictionary;
}

static void jsonToDictionary(json_t *jsonObj, foundation::library::DictionaryClassPtr& dictionary)
{
    const char *key;
    json_t *value;
    json_object_foreach(jsonObj, key, value)
    {
        foundation::InspectablePtr spValue;
        foundation::string_t key_t = foundation::to_platform_string(key);
        //IFHR_ASSERT(JsonToInspectable(value, spValue.GetAddressOf()));
        if (json_is_integer(value))
        {
            foundation::pv_util::CreateInt64Value(json_integer_value(value), spValue.GetAddressOf());
        }
        else if (json_is_boolean(value))
        {
            foundation::pv_util::CreateBooleanValue(json_is_true(value), spValue.GetAddressOf());
        }
        else if (json_is_string(value))
        {
            foundation::pv_util::CreateStringValue(foundation::to_platform_string(json_string_value(value)).c_str(), spValue.GetAddressOf());
        }
        dictionary.Set(key_t.c_str(), spValue.Get());
    }

}

static json_t *serializeModelTypeInfo(const CTypeInfo *pModelTypeInfo)
{
    json_t *jsonModelTypeInfo = json_object();
    // serialize the basic stub
    json_object_set_new(jsonModelTypeInfo, _ID_KEY, json_integer(pModelTypeInfo->getId()));
    json_object_set_new(jsonModelTypeInfo, _NAME_KEY, json_wstring(pModelTypeInfo->getName()));

    json_object_set_new(jsonModelTypeInfo, _IID_KEY, json_iid(pModelTypeInfo->getType()));

    return jsonModelTypeInfo;
}

static json_t *serializeParameters(const _MethodParamIteratorType& parameters)
{
    json_t *jsonParameters = json_array();
    for (_MethodParamIteratorType::const_iterator iterParameter = parameters.begin();
        iterParameter != parameters.end();
        ++iterParameter)
    {
        json_t *json_Parameter = json_object();
        json_object_set_new(json_Parameter, _NAME_KEY, json_wstring((*iterParameter)->getParameterName()));
        json_object_set_new(json_Parameter, _PROPERTY_TYPE_KEY, json_integer((*iterParameter)->getPropertyType()));
        json_setModelTypeInfoRef(json_Parameter, _MODEL_TYPE_REF_KEY, (*iterParameter)->getModelType());

        json_array_append_new(jsonParameters, json_Parameter);
    }
    return jsonParameters;
}

template <class T>
static json_t *serializeMethod(const T *pMethod)
{
    json_t *json_Method = json_object();
    json_object_set_new(json_Method, _ID_KEY, json_integer(pMethod->getId()));
    json_object_set_new(json_Method, _NAME_KEY, json_wstring(pMethod->getName()));

    json_setPropertyTypeInfoRef(json_Method, _RESULT_TYPE_INFO_KEY, pMethod->getResultType());
    json_object_set_new(json_Method, _IS_ASYNC_KEY, json_boolean(pMethod->getIsAsync()));

    json_object_set_new(json_Method, _PARAMETERS_KEY, serializeParameters(pMethod->getParameters()));
    return json_Method;
}

template <class T>
static json_t *serializeProperty(const T *pProperty)
{
    json_t *json_Property = json_object();
    json_object_set_new(json_Property, _ID_KEY, json_integer(pProperty->getId()));
    json_object_set_new(json_Property, _NAME_KEY, json_wstring(pProperty->getName()));
    json_object_set_new(json_Property, _PROPERTY_TYPE_KEY, json_integer(pProperty->getPropertyType()));

    json_setModelTypeInfoRef(json_Property, _MODEL_TYPE_REF_KEY, pProperty->getModelType());

    json_object_set_new(json_Property, _PROPERTY_FLAGS_KEY, json_integer(pProperty->getFlags()));
    return json_Property;
}

static json_t *serializeJsonHelper(const CModelSchema& modelSchema)
{
    json_t *jsonSchema = json_object();
    json_object_set_new(jsonSchema, _NAMESPACE_KEY, json_wstring(modelSchema.getNamespace()));

    
    // serialize schema references
    json_t *jsonSchemaReferences = json_array();

    for(_ModelSchemaIteratorType::const_iterator iter = modelSchema.getReferenceSchemas().begin();
        iter != modelSchema.getReferenceSchemas().end();
        ++iter)
    {
        json_array_append_new(jsonSchemaReferences,serializeJsonHelper(*(*iter)));
    }
    json_object_set_new(jsonSchema, _SCHEMA_REFERENCES_KEY, jsonSchemaReferences);

    // serialize schema attributes
    json_object_set_new(jsonSchema, _PROPERTY_ATTRIBUTES_KEY, dictionaryToJson(modelSchema.getAttributes()));

    json_t *jsonEnumTypes = json_array();

    for(_EnumTypeInfoIteratorType::const_iterator iter = modelSchema.getEnumTypes().begin();
        iter != modelSchema.getEnumTypes().end();
        ++iter)
    {
        json_t *jsonEnumType = json_object();

        // serialize Id,Enum Name & Type
        json_object_set_new(jsonEnumType, _ID_KEY, json_integer((*iter)->getId()));
        json_object_set_new(jsonEnumType, _NAME_KEY, json_wstring((*iter)->getName()));
        json_object_set_new(jsonEnumType, _IID_KEY, json_iid((*iter)->getType()));
        json_object_set_new(jsonEnumType, _ENUM_ISFLAGS_KEY, json_boolean((*iter)->isFlags()));
        // serialize base type
        json_setModelTypeInfoRef(jsonEnumType, _BASE_TYPE_KEY, (*iter)->getBaseType());

        json_t *jsonEnumTypeValues = json_array();

        for(_EnumInfoVectorType::const_iterator iterEnumItem = (*iter)->getEnums().begin();
            iterEnumItem != (*iter)->getEnums().end();
            ++iterEnumItem)
        {
            json_t *jsonEnumTypeValue = json_object();
            json_object_set_new(jsonEnumTypeValue, _NAME_KEY, json_wstring(std::get<0>(*iterEnumItem)));
            json_object_set_new(jsonEnumTypeValue, _VALUE_KEY, json_integer(std::get<1>(*iterEnumItem)));
            json_object_set_new(jsonEnumTypeValue, _LITERAL_TYPE_KEY, json_integer(std::get<2>(*iterEnumItem)));

            json_array_append_new(jsonEnumTypeValues,jsonEnumTypeValue);
        }
        json_object_set_new(jsonEnumType, _ENUM_VALUES_KEY, jsonEnumTypeValues);

        json_array_append_new(jsonEnumTypes,jsonEnumType);
    }
    
    json_object_set_new(jsonSchema, _ENUM_TYPES_KEY,jsonEnumTypes);

    // serialize 'stub' model types
    json_t *jsonPropertyModelTypes = json_array();
    for(_TypeInfoIteratorType::const_iterator iter = modelSchema.getModelTypes().begin();
        iter != modelSchema.getModelTypes().end();
        ++iter)
    {
        
        if( (*iter)->getModelType() == ModelTypeInfoType_ObservableObject ||
            (*iter)->getModelType() == ModelTypeInfoType_ViewModel)
        {
            CObservableObjectInfo *pPropertyModelInfo = (CObservableObjectInfo *)(*iter);
            json_t *jsonPropertyModelType = serializeModelTypeInfo(pPropertyModelInfo);

            json_object_set_new(jsonPropertyModelType, _PROPERTY_MODEL_TYPE_INFO_TYPE, json_integer((*iter)->getModelType()));

            // codegen tool support
            json_object_set_new(jsonPropertyModelType, _PROPERTY_ATTRIBUTES_KEY, dictionaryToJson(pPropertyModelInfo->getAttributes()));
            
            // support for View Model
            if((*iter)->getModelType() == ModelTypeInfoType_ViewModel)
            {
                json_object_set_new(jsonPropertyModelType, _PROPERTY_MODEL_IS_INHERITED_KEY, json_boolean(((CPropertyViewModelInfo *)(*iter))->getIsInherited()));
            }

            // serialize base type
            json_setModelTypeInfoRef(jsonPropertyModelType,_BASE_TYPE_KEY,(*iter)->getBaseType());

            // serialize properties
            json_t *jsonProperties = json_array();

            for(_PropertyIteratorType::const_iterator iterProperty = pPropertyModelInfo->getProperties().begin();
                iterProperty != pPropertyModelInfo->getProperties().end();
                ++iterProperty)
            {
                json_t *json_Property = serializeProperty(*iterProperty);

                // codegen tool support
                json_object_set_new(json_Property, _PROPERTY_ATTRIBUTES_KEY, dictionaryToJson((*iterProperty)->getAttributes()));

                json_array_append_new(jsonProperties,json_Property);

            }
            json_object_set_new(jsonPropertyModelType,_PROPERTY_MODEL_PROPERTIES_KEY,jsonProperties);

            // serialize methods
            json_t *jsonMethods = json_array();

            for(_MethodIteratorType::const_iterator iterMethod = pPropertyModelInfo->getMethods().begin();
                iterMethod != pPropertyModelInfo->getMethods().end();
                ++iterMethod)
            {
                json_t *json_Method = serializeMethod(*iterMethod);
                json_array_append_new(jsonMethods,json_Method);
            }

            json_object_set_new(jsonPropertyModelType, _PROPERTY_MODEL_METHODS_KEY, jsonMethods);

            // serialize events
            json_t *jsonEvents = json_array();

            for(_EventIteratorType::const_iterator iterEvent = pPropertyModelInfo->getEvents().begin();
                iterEvent != pPropertyModelInfo->getEvents().end();
                ++iterEvent)
            {
                json_t *json_Event = json_object();
                json_object_set_new(json_Event, _ID_KEY, json_integer((*iterEvent)->getId()));
                json_object_set_new(json_Event, _NAME_KEY, json_wstring((*iterEvent)->getName()));

                json_setPropertyTypeInfoRef(json_Event,_EVENT_ARGS_TYPE_INFO_KEY,(*iterEvent)->getEventArgsType());

                json_array_append_new(jsonEvents,json_Event);
            }
            json_object_set_new(jsonPropertyModelType, _PROPERTY_MODEL_EVENTS_KEY, jsonEvents);

            // support for View Model
            if((*iter)->getModelType() == ModelTypeInfoType_ViewModel)
            {
                CPropertyViewModelInfo *pPropertyViewModelInfo = (CPropertyViewModelInfo *)(*iter);
                
                json_setModelTypeInfoRef(jsonPropertyModelType,_SOURCE_TYPE_INFO_KEY,pPropertyViewModelInfo->getSourceType());
            }

            json_array_append_new(jsonPropertyModelTypes,jsonPropertyModelType);
        }
        else if ((*iter)->getModelType() == ModelTypeInfoType_ObjectDispatch)
        {
            CObjectDispatchInfo *pPropertyDataInfo = (CObjectDispatchInfo *)(*iter);
            json_t *jsonPropertyDataInfo = serializeModelTypeInfo(pPropertyDataInfo);
            json_object_set_new(jsonPropertyDataInfo, _PROPERTY_MODEL_TYPE_INFO_TYPE, json_integer(ModelTypeInfoType_ObjectDispatch));

            // serialize base type
            json_setModelTypeInfoRef(jsonPropertyDataInfo, _BASE_TYPE_KEY, (*iter)->getBaseType());

            // serialize properties
            json_t *jsonProperties = json_array();

            for (_PropertyIteratorType::const_iterator iterProperty = pPropertyDataInfo->getProperties().begin();
                iterProperty != pPropertyDataInfo->getProperties().end();
                ++iterProperty)
            {
                json_t *json_Property = serializeProperty(*iterProperty);
                json_array_append_new(jsonProperties, json_Property);
            }
            json_object_set_new(jsonPropertyDataInfo, _PROPERTY_MODEL_PROPERTIES_KEY, jsonProperties);

            // serialize methods
            json_t *jsonMethods = json_array();

            for (_MethodIteratorType::const_iterator iterMethod = pPropertyDataInfo->getMethods().begin();
                iterMethod != pPropertyDataInfo->getMethods().end();
                ++iterMethod)
            {
                json_t *json_Method = serializeMethod(*iterMethod);
                json_array_append_new(jsonMethods, json_Method);
            }

            json_object_set_new(jsonPropertyDataInfo, _PROPERTY_MODEL_METHODS_KEY, jsonMethods);

            json_array_append_new(jsonPropertyModelTypes, jsonPropertyDataInfo);

        }
    }
    json_object_set_new(jsonSchema, _SCHEMA_PROPERTY_MODEL_TYPES_KEY,jsonPropertyModelTypes);
    
    // continue with the non property model types
    json_t *jsonModelTypes = json_array();
    for(_TypeInfoIteratorType::const_iterator iter = modelSchema.getModelTypes().begin();
        iter != modelSchema.getModelTypes().end();
        ++iter)
    {
        if( (*iter)->getModelType() == ModelTypeInfoType_ObservableObject ||
            (*iter)->getModelType() == ModelTypeInfoType_ViewModel ||
            (*iter)->getModelType() == ModelTypeInfoType_ObjectDispatch)
        {
            continue;
        }
        json_t *jsonModelType = serializeModelTypeInfo(*iter);
        // base type serialization
        json_object_set_new(jsonModelType, _MODEL_TYPE_KEY, json_integer((*iter)->getModelType()));

        json_setModelTypeInfoRef(jsonModelType,_BASE_TYPE_KEY,(*iter)->getBaseType());

        switch ((*iter)->getModelType())
        {
        case ModelTypeInfoType_Command:
            json_setPropertyTypeInfoRef(jsonModelType,_PARAMETER_TYPE_INO_KEY,((CCommandTypeInfo *)(*iter))->getParameterTypeInfo());
            json_setPropertyTypeInfoRef(jsonModelType,_RESULT_TYPE_INFO_KEY,((CCommandTypeInfo *)(*iter))->getResultTypeInfo());
            json_object_set_new(jsonModelType, _IS_ASYNC_KEY, json_boolean(((CCommandTypeInfo *)(*iter))->getIsAsync()));
            json_object_set_new(jsonModelType, _IS_UNKNOWNSTATE_KEY, json_boolean(((CCommandTypeInfo *)(*iter))->getIsUnknownState()));
            break;
        case ModelTypeInfoType_ObservableCollection:
        case ModelTypeInfoType_ObservableList:
        case ModelTypeInfoType_Enumerable:
            json_setPropertyTypeInfoRef(jsonModelType,_ITEM_TYPE_INFO_KEY,((CEnumerableTypeInfo *)(*iter))->getItemTypeInfo());
            break;
        case ModelTypeInfoType_AsyncOperation:
            json_setPropertyTypeInfoRef(jsonModelType,_RESULT_TYPE_INFO_KEY,((CAsyncOperationTypeInfo *)(*iter))->getResultTypeInfo());
            break;
        default:
            assert(false);
            break;
        }
        json_array_append_new(jsonModelTypes,jsonModelType);
    }
    json_object_set_new(jsonSchema, _SCHEMA_MODEL_TYPES_KEY,jsonModelTypes);

    return jsonSchema;
}

void CSchemaSerializer::serializeJson(const CModelSchema& modelSchema,std::ostream& os)
{
    json_t *jsonSchema = serializeJsonHelper(modelSchema);

    char *json_str = json_dumps(jsonSchema, JSON_INDENT(1) | JSON_PRESERVE_ORDER | JSON_ENCODE_ANY | JSON_ESCAPE_SLASH);
    json_decref(jsonSchema);
    os << json_str << std::endl;
    free((void *)json_str);
}

static CMethodInfo *deserializeMethod(
    json_t *json_Method,
    CObjectDispatchInfo *pMethodParent,
    CModelSchema *pModelSchema)
{
    UINT32 methodId = (UINT32)json_integer_value(json_object_get(json_Method, _ID_KEY));
    std::wstring methodName = json_wstring_value(json_object_get(json_Method, _NAME_KEY));

    CPropertyTypeInfo *pResultType = json_getPropertyTypeInfoRef(json_Method, _RESULT_TYPE_INFO_KEY, *pModelSchema);
    bool isAsync = json_is_true(json_object_get(json_Method, _IS_ASYNC_KEY));

    CMethodInfo *pMethodInfo = new CMethodInfo(
        methodId,
        methodName.data(),
        pResultType,
        isAsync,
        *pMethodParent);
    pMethodParent->addMethod(pMethodInfo);
    // deserialize parameters
    json_t *jsonParameters = json_object_get(json_Method, _PARAMETERS_KEY);
    for (size_t indexParameter = 0; indexParameter < json_array_size(jsonParameters); ++indexParameter)
    {
        json_t *json_Parameter = json_array_get(jsonParameters, indexParameter);
        std::wstring parameterName = json_wstring_value(json_object_get(json_Parameter, _NAME_KEY));
        foundation::PropertyType propertyType =
            (foundation::PropertyType)json_integer_value(json_object_get(json_Parameter, _PROPERTY_TYPE_KEY));
        const CTypeInfo *pModelType = json_getModelTypeInfoRef(json_Parameter, _MODEL_TYPE_REF_KEY, *pModelSchema);
        pMethodInfo->addParameter(
            new CMethodParamInfo(
                parameterName.data(),
                propertyType,
                pModelType));
    }
    return pMethodInfo;
}

static CPropertyInfo *deserializeProperty(
    json_t *jsonProperty,
    CObjectDispatchInfo *pPropertyParent,
    CModelSchema *pModelSchema)
{
    UINT32 propertyId = (UINT32)json_integer_value(json_object_get(jsonProperty, _ID_KEY));
    std::wstring propertyName = json_wstring_value(json_object_get(jsonProperty, _NAME_KEY));

    foundation::PropertyType propertyType =
        (foundation::PropertyType)json_integer_value(json_object_get(jsonProperty, _PROPERTY_TYPE_KEY));
    const CTypeInfo *pModelType = json_getModelTypeInfoRef(jsonProperty, _MODEL_TYPE_REF_KEY, *pModelSchema);
    UINT32 flags = (UINT32)json_integer_value(json_object_get(jsonProperty, _PROPERTY_FLAGS_KEY));
    CPropertyInfo *pProperty = new CPropertyInfo(
        propertyId,
        propertyName.data(),
        propertyType,
        pModelType,
        flags,
        *pPropertyParent);
    pPropertyParent->addProperty(pProperty);
    return pProperty;
}


static CModelSchema *deserializeJsonHelper(json_t *jsonSchema)
{
    CModelSchema *pModelSchema = new CModelSchema(json_wstring_value(json_object_get(jsonSchema, _NAMESPACE_KEY)).c_str());

    // deserialize schema references
    json_t *jsonSchemaReferences = json_object_get(jsonSchema, _SCHEMA_REFERENCES_KEY);
    
    for(size_t index = 0;index < json_array_size(jsonSchemaReferences);++index)
    {
        pModelSchema->addReferenceSchema(deserializeJsonHelper(json_array_get(jsonSchemaReferences, index)));
    }

    //deserialize the schema attributes
    jsonToDictionary(json_object_get(jsonSchema, _PROPERTY_ATTRIBUTES_KEY), pModelSchema->getAttributes());

    json_t *jsonEnumTypes = json_object_get(jsonSchema, _ENUM_TYPES_KEY);

    for(size_t index = 0;index < json_array_size(jsonEnumTypes);++index)
    {
        json_t *jsonEnumType = json_array_get(jsonEnumTypes, index);

        UINT32 id = (UINT32)json_integer_value(json_object_get(jsonEnumType,_ID_KEY));
        std::wstring enumTypeName = json_wstring_value(json_object_get(jsonEnumType,_NAME_KEY));
        IID iidType = json_iid_value(json_object_get(jsonEnumType,_IID_KEY));
        bool isFlags = json_is_true(json_object_get(jsonEnumType,_ENUM_ISFLAGS_KEY));

        const schema::CEnumTypeInfo *pEnumTypeInfoBase = (const schema::CEnumTypeInfo *)
            json_getModelTypeInfoRef(jsonEnumType, _BASE_TYPE_KEY, *pModelSchema);

        schema::CEnumTypeInfo *pEnumTypeInfo = new schema::CEnumTypeInfo(
            id, 
            enumTypeName.data(), 
            iidType, 
            isFlags,
            pEnumTypeInfoBase,
            *pModelSchema);
        pModelSchema->addEnumType(pEnumTypeInfo);

        json_t *jsonEnumTypeValues = json_object_get(jsonEnumType,_ENUM_VALUES_KEY);
        for(size_t indexEnumValue = 0;indexEnumValue < json_array_size(jsonEnumTypeValues);++indexEnumValue)
        {
            json_t *jsonEnumTypeValue = json_array_get(jsonEnumTypeValues,indexEnumValue);
            std::wstring enumItemName = json_wstring_value(json_object_get(jsonEnumTypeValue,_NAME_KEY));
            UINT32 enumItemValue = (UINT32)json_integer_value(json_object_get(jsonEnumTypeValue,_VALUE_KEY));
            LiteralType literalType = (LiteralType)json_integer_value(json_object_get(jsonEnumTypeValue, _LITERAL_TYPE_KEY));
            pEnumTypeInfo->addEnumItem(enumItemName.data(), enumItemValue, literalType);
        }
    }
    // deserialize 'stub'  model types
    json_t *jsonPropertyModelTypes = json_object_get(jsonSchema, _SCHEMA_PROPERTY_MODEL_TYPES_KEY);
    for(size_t index = 0;index < json_array_size(jsonPropertyModelTypes);++index)
    {
        json_t *jsonPropertyModelType = json_array_get(jsonPropertyModelTypes, index);

        UINT32 id = (UINT32)json_integer_value(json_object_get(jsonPropertyModelType,_ID_KEY));
        std::wstring typeModelName = json_wstring_value(json_object_get(jsonPropertyModelType,_NAME_KEY));
        IID iidType = json_iid_value(json_object_get(jsonPropertyModelType,_IID_KEY));
        ModelTypeInfoType modelTypeInfoType = (ModelTypeInfoType)json_integer_value(json_object_get(jsonPropertyModelType, _PROPERTY_MODEL_TYPE_INFO_TYPE));

        CTypeInfo *pTypeInfo = nullptr;
        if (modelTypeInfoType == ModelTypeInfoType_ObjectDispatch)
        {
            pTypeInfo = new CObjectDispatchInfo(
                id,
                typeModelName.data(),
                iidType,
                ModelTypeInfoType_ObjectDispatch,
                nullptr,    // base type is null here
                *pModelSchema
            );
        }
        else if(modelTypeInfoType == ModelTypeInfoType_ViewModel)
        {
            bool isInherited = json_is_true(json_object_get(jsonPropertyModelType,_PROPERTY_MODEL_IS_INHERITED_KEY));
            pTypeInfo = new CPropertyViewModelInfo(
                id,
                typeModelName.data(),
                iidType,
                nullptr,    // base type is null here
                nullptr,    // source type is null here
                isInherited,
                *pModelSchema
                );
        }
        else
        {
            pTypeInfo = new CObservableObjectInfo(
                id,
                typeModelName.data(),
                iidType,
                nullptr,    // base type is null here
                *pModelSchema
                );
        }        

        pModelSchema->addModelType(pTypeInfo);
        
        //deserialize attributes
        if (modelTypeInfoType != ModelTypeInfoType_ObjectDispatch)
        {
            jsonToDictionary(json_object_get(jsonPropertyModelType,_PROPERTY_ATTRIBUTES_KEY), ((CObservableObjectInfo *)pTypeInfo)->getAttributes());
        }
    }

    // deserialize non property model types
    json_t *jsonModelTypes = json_object_get(jsonSchema, _SCHEMA_MODEL_TYPES_KEY);

    for(size_t index = 0;index < json_array_size(jsonModelTypes);++index)
    {
        json_t *jsonModelType = json_array_get(jsonModelTypes, index);

        UINT32 id = (UINT32)json_integer_value(json_object_get(jsonModelType,_ID_KEY));
        std::wstring typeModelName = json_wstring_value(json_object_get(jsonModelType,_NAME_KEY));
        IID iidType = json_iid_value(json_object_get(jsonModelType,_IID_KEY));

        ModelTypeInfoType modelTypeInfo = (ModelTypeInfoType)json_integer_value(json_object_get(jsonModelType,_MODEL_TYPE_KEY));

        switch (modelTypeInfo)
        {
        case ModelTypeInfoType_Command:
            {
                const CPropertyTypeInfo* pParameterTypeInfo = json_getPropertyTypeInfoRef(jsonModelType,_PARAMETER_TYPE_INO_KEY,*pModelSchema);
                const CPropertyTypeInfo* pResultTypeInfo = json_getPropertyTypeInfoRef(jsonModelType,_RESULT_TYPE_INFO_KEY,*pModelSchema);
                bool isAsync = json_is_true(json_object_get(jsonModelType,_IS_ASYNC_KEY));
                bool isUnknownState = json_is_true(json_object_get(jsonModelType,_IS_UNKNOWNSTATE_KEY));

                pModelSchema->addModelType(new CCommandTypeInfo(
                                            id,
                                            typeModelName.data(),
                                            iidType,
                                            *pModelSchema,
                                            pParameterTypeInfo,
                                            pResultTypeInfo,
                                            isAsync,
                                            isUnknownState));
            }
            break;
        case ModelTypeInfoType_Enumerable:
        case ModelTypeInfoType_ObservableList:
        case ModelTypeInfoType_ObservableCollection:
            {
                const CPropertyTypeInfo* pItemTypeInfo = json_getPropertyTypeInfoRef(jsonModelType,_ITEM_TYPE_INFO_KEY,*pModelSchema);

                pModelSchema->addModelType(new CEnumerableTypeInfo(
                                            id,
                                            typeModelName.data(),
                                            iidType,
                                            *pModelSchema,
                                            pItemTypeInfo,
                                            modelTypeInfo));
            }
            break;
        case ModelTypeInfoType_AsyncOperation:
            {
                const CPropertyTypeInfo* pResultTypeInfo = json_getPropertyTypeInfoRef(jsonModelType,_RESULT_TYPE_INFO_KEY,*pModelSchema);

                pModelSchema->addModelType(new CAsyncOperationTypeInfo(
                                            id,
                                            typeModelName.data(),
                                            iidType,
                                            *pModelSchema,
                                            pResultTypeInfo));
            }
            break;
        default:
            assert(false);
            break;
        }
    }

    // deserialize rest of'stub' model types
    for(size_t index = 0;index < json_array_size(jsonPropertyModelTypes);++index)
    {
        json_t *jsonPropertyModelType = json_array_get(jsonPropertyModelTypes, index);
        std::wstring typeModelName = json_wstring_value(json_object_get(jsonPropertyModelType,_NAME_KEY));

        CObjectDispatchInfo *pPropertyDataInfo = (CObjectDispatchInfo *)pModelSchema->resolveModelTypeLocal(typeModelName.data());
        assert(pPropertyDataInfo);

        const CTypeInfo *pBaseTypeInfo = json_getModelTypeInfoRef(jsonPropertyModelType,_BASE_TYPE_KEY,*pModelSchema);
        pPropertyDataInfo->setBaseType(pBaseTypeInfo);

        ModelTypeInfoType modelTypeInfoType = (ModelTypeInfoType)json_integer_value(json_object_get(jsonPropertyModelType, _PROPERTY_MODEL_TYPE_INFO_TYPE));

        // deserialize properties
        json_t *jsonProperties = json_object_get(jsonPropertyModelType, _PROPERTY_MODEL_PROPERTIES_KEY);
        for (size_t indexProperty = 0; indexProperty < json_array_size(jsonProperties); ++indexProperty)
        {
            json_t *jsonProperty = json_array_get(jsonProperties, indexProperty);

            CPropertyInfo *pPropertyInfo = deserializeProperty(jsonProperty, pPropertyDataInfo, pModelSchema);
            // deserialize attributes
            jsonToDictionary(json_object_get(jsonProperty, _PROPERTY_ATTRIBUTES_KEY), pPropertyInfo->getAttributes());
        }
        // deserialize methods
        json_t *jsonMethods = json_object_get(jsonPropertyModelType, _PROPERTY_MODEL_METHODS_KEY);
        for (size_t indexMethod = 0; indexMethod < json_array_size(jsonMethods); ++indexMethod)
        {
            json_t *json_Method = json_array_get(jsonMethods, indexMethod);
            deserializeMethod(json_Method, pPropertyDataInfo, pModelSchema);
        }

        if (modelTypeInfoType != ModelTypeInfoType_ObjectDispatch)
        {
            CObservableObjectInfo *pObservableObjectInfo = (CObservableObjectInfo *)pPropertyDataInfo;
            // deserialize events
            json_t *jsonEvents = json_object_get(jsonPropertyModelType, _PROPERTY_MODEL_EVENTS_KEY);
            for (size_t indexEvent = 0; indexEvent < json_array_size(jsonEvents); ++indexEvent)
            {
                json_t *json_Event = json_array_get(jsonEvents, indexEvent);
                UINT32 eventId = (UINT32)json_integer_value(json_object_get(json_Event, _ID_KEY));
                std::wstring eventName = json_wstring_value(json_object_get(json_Event, _NAME_KEY));

                CPropertyTypeInfo *pEventArgsType = json_getPropertyTypeInfoRef(json_Event, _EVENT_ARGS_TYPE_INFO_KEY, *pModelSchema);

                CEventInfo *pEventInfo = new CEventInfo(
                    eventId,
                    eventName.data(),
                    pEventArgsType,
                    *pObservableObjectInfo);
                pObservableObjectInfo->addEvent(pEventInfo);
            }
        }

        // support for View Model
        if (pPropertyDataInfo->getModelType() == ModelTypeInfoType_ViewModel)
        {
            const CObservableObjectInfo *pSourceType = (const CObservableObjectInfo *)
                json_getModelTypeInfoRef(jsonPropertyModelType, _SOURCE_TYPE_INFO_KEY, *pModelSchema);
            ((CPropertyViewModelInfo *)pPropertyDataInfo)->setSourceType(pSourceType);
        }
    }
    return pModelSchema;
}

CModelSchema *CSchemaSerializer::deserializeJson(std::istream& is)
{
    std::string str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    json_error_t error;
    json_t *jsonSchema = json_loads(str.c_str(), JSON_DECODE_ANY, &error);
    if(jsonSchema == nullptr)
    {
        return nullptr;
    }
    CModelSchema *pModelSchema = deserializeJsonHelper(jsonSchema);
    json_decref(jsonSchema);

    return pModelSchema;
}
