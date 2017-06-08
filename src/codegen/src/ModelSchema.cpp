/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelSchema.cpp
****/

#include "ModelSchema.h"
#include "CodeGenUtil.h"
#include <sstream>

static const GUID IID_IObservableObject =
{ 0x8165a381, 0xc2bf, 0x444e, { 0xb8, 0x4b, 0xdf, 0x43, 0xca, 0x3, 0x58, 0x6e } };

namespace schema {

    // dictionary access kyes
    const LPCSTR_t _PROPERTY_KEY_NAME = U("name");

    const LPCSTR_t _PROPERTY_KEY_PROPERTY_MODEL_ATTRIBUTES = U("propertyModelAttributes");
    const LPCSTR_t _PROPERTY_KEY_PROPERTIES = U("properties");
    const LPCSTR_t _PROPERTY_KEY_ATTRIBUTES = U("attributes");
    // property type info models
    const LPCSTR_t _PROPERTY_ATTRIBUTE_CUSTOM_CONTAINER_NAME_KEY = U("customContainerName");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_CUSTOM_PRIVATE_INTERFACE_KEY = U("isCustomPrivateInterface");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_RUNTIME_CLASS_OPTIONS_KEY = U("runtimeClassOptions");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_OPTION_FLAGS_KEY = U("optionFlags");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_CUSTOM_CONTAINER_KEY = U("isCustomContainer");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_PARENT_KEY = U("isParent");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_WEAK_REFERENCE_KEY = U("isWeakReference");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_BINDING_SOURCE_PATH_KEY = U("bindingSourcePath");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_AUTO_GENERATE_KEY = U("isAutoGenerate");
    const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_RUNTIME_ACTIVATED_KEY = U("isRuntimeActivated");
 
    const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_SOURCE_INHERITED_KEY = U("isSourceInherited");
}

using namespace schema;

UINT32 CPropertyInfo::getRuntimeClassOptions() const
{
    return getValueWithParentCheck<UINT32>(_PROPERTY_ATTRIBUTE_RUNTIME_CLASS_OPTIONS_KEY, 0);
}

void CPropertyInfo::setRuntimeClassOptions(UINT32 classOptions) 
{
    _attributes.Set(_PROPERTY_ATTRIBUTE_RUNTIME_CLASS_OPTIONS_KEY, classOptions);
}

UINT32 CPropertyInfo::getOptionFlags() const
{
    return _attributes.Get<UINT32>(_PROPERTY_ATTRIBUTE_OPTION_FLAGS_KEY, 0);
}

void CPropertyInfo::setOptionFlags(UINT32 optionFlags)
{
    _attributes.Set(_PROPERTY_ATTRIBUTE_OPTION_FLAGS_KEY, optionFlags);
}

const std::wstring CPropertyInfo::getBindingSourcePath() const
{
    return foundation::to_wstring(_attributes.Get<foundation::string_t>(_PROPERTY_ATTRIBUTE_BINDING_SOURCE_PATH_KEY, U("")).c_str());
}
void CPropertyInfo::setBindingSourcePath(const wchar_t *bindingSourcePath)
{
    _attributes.Set(_PROPERTY_ATTRIBUTE_BINDING_SOURCE_PATH_KEY, foundation::to_platform_string(bindingSourcePath).c_str());
}

bool CPropertyInfo::isAutoGenerate() const
{
    return _attributes.Get(_PROPERTY_ATTRIBUTE_IS_AUTO_GENERATE_KEY, true);
}
void CPropertyInfo::setIsAutoGenerate(bool isAutoGenerate)
{
    _attributes.Set(_PROPERTY_ATTRIBUTE_IS_AUTO_GENERATE_KEY, isAutoGenerate);
}

bool CPropertyInfo::isParent() const
{
    return _attributes.Get(_PROPERTY_ATTRIBUTE_IS_PARENT_KEY, false);
}
void CPropertyInfo::setIsParent(bool isParent)
{
    _attributes.Set(_PROPERTY_ATTRIBUTE_IS_PARENT_KEY, isParent);
}
bool CPropertyInfo::isCustomContainer() const
{
    return getValueWithParentCheck(_PROPERTY_ATTRIBUTE_IS_CUSTOM_CONTAINER_KEY, false);
}

void CPropertyInfo::setIsCustomContainer(bool isCustomContainer)
{
    _attributes.Set(_PROPERTY_ATTRIBUTE_IS_CUSTOM_CONTAINER_KEY, isCustomContainer);
}

bool CPropertyInfo::isRuntimeActivated() const
{
    return _attributes.Get(_PROPERTY_ATTRIBUTE_IS_RUNTIME_ACTIVATED_KEY, false);
}

void CPropertyInfo::setIsRuntimeActivated(bool isRuntimeActivated)
{
    _attributes.Set(_PROPERTY_ATTRIBUTE_IS_RUNTIME_ACTIVATED_KEY, isRuntimeActivated);
}

const CModelSchema& CModelSchema::getFactorySchema()
{
    static CModelSchema factorySchema(L"pmod");
    static bool initialized;
    if(!initialized)
    {
        factorySchema._isFactorySchema = true;

        // create an Object type
        factorySchema.addModelType(new CTypeInfo(
            0,
            L"IObject",
            foundation::IID_IObject,
            ModelTypeInfoType_Object,
            nullptr,
            factorySchema));

        // create a dictionary type
        factorySchema.addModelType(new CTypeInfo(
            0,
            L"IDictionary",
            foundation::IID_IDictionary,
            ModelTypeInfoType_Dictionary,
            nullptr,
            factorySchema));

        // create a observable object info
        factorySchema.addModelType(
            new CObservableObjectInfo(0,
            L"IObservableObject",
            IID_IObservableObject,
            nullptr,
            factorySchema
            ));
        initialized = true;
    }
    return factorySchema;
}

const schema::CEnumTypeInfo *CModelSchema::resolveEnumType(const wchar_t *typeEnumName) const
{
    std::wstring _namespace;
    std::wstring typeName;

    const schema::CEnumTypeInfo *pEnumTypeInfo = nullptr;

    GetNamespaceAndType(typeEnumName,_namespace,typeName);

    if(_namespace.size()==0 || _namespace == getNamespace())
    {
        pEnumTypeInfo = resolveEnumTypeLocal(typeName.data());
    }

    if(!pEnumTypeInfo)
    {
        for(_ModelSchemaIteratorType::const_iterator iter = _referenceSchemas.begin();
            iter != _referenceSchemas.end();
            ++iter)
        {
            if( (*iter)->getNamespace() == _namespace)
            {
                pEnumTypeInfo = (*iter)->resolveEnumTypeLocal(typeName.data());
                if(pEnumTypeInfo != nullptr)
                {
                    break;
                }
            }
        }
    }
    return pEnumTypeInfo;
}

const schema::CEnumTypeInfo *CModelSchema::resolveEnumTypeLocal(const wchar_t *typeEnumName) const
{
    for(_EnumTypeInfoIteratorType::const_iterator iter = _enumTypes.begin();
        iter != _enumTypes.end();
        ++iter)
    {
        if( (*iter)->getName() == typeEnumName)
        {
            return *iter;
        }
    }
    return nullptr;
}

const CTypeInfo *CModelSchema::resolveModelType(const wchar_t *typeModelName) const
{
    std::wstring namespaceName;
    std::wstring typeName;

    const CTypeInfo *pModelTypeInfo = nullptr;

    GetNamespaceAndType(typeModelName, namespaceName,typeName);

    if(namespaceName.size()==0 || namespaceName == getNamespace())
    {
        pModelTypeInfo = this->resolveModelTypeLocal(typeName.data());
    }

    if(!pModelTypeInfo)
    {
        for(_ModelSchemaIteratorType::const_iterator iter = _referenceSchemas.begin();
            iter != _referenceSchemas.end();
            ++iter)
        {
            if( (*iter)->getNamespace() == namespaceName)
            {
                pModelTypeInfo = (*iter)->resolveModelTypeLocal(typeName.data());
                if(pModelTypeInfo != nullptr)
                {
                    break;
                }
            }
        }
        // if still not found we will look into the 'Factory' schema
        if(namespaceName.size()==0 || namespaceName == L"pmod")
        {
            pModelTypeInfo = getFactorySchema().resolveModelTypeLocal(typeName.data());
        }
    }
    return pModelTypeInfo;
}

const CTypeInfo *CModelSchema::resolveModelTypeLocal(const wchar_t *typeModelName) const
{
    for(_TypeInfoIteratorType::const_iterator iter = _modelTypes.begin();
        iter != _modelTypes.end();
        ++iter)
    {
        if( (*iter)->getName() == typeModelName)
        {
            return *iter;
        }
    }
	// we could not resolve in Model Types so attempt the Enum Types
    return resolveEnumTypeLocal(typeModelName);
}

