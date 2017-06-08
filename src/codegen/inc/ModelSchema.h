/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ModelSchema.h
****/
#ifndef _MODEL_SCHEMA_DEFINED_
#define _MODEL_SCHEMA_DEFINED_

#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <assert.h>

#include "foundation/pal.h"
#include "foundation/interfaces.h"
#include "foundation/library/dictionary_class_ptr.h"

#include "CodeGenUtil.h"


namespace schema {    
    // dictionary access kyes
    extern const LPCSTR_t _PROPERTY_KEY_NAME;
    extern const LPCSTR_t _PROPERTY_KEY_PROPERTY_MODEL_ATTRIBUTES;
    extern const LPCSTR_t _PROPERTY_KEY_PROPERTIES;
    extern const LPCSTR_t _PROPERTY_KEY_ATTRIBUTES;
    // property type info models
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_CUSTOM_CONTAINER_NAME_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_CUSTOM_PRIVATE_INTERFACE_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_RUNTIME_CLASS_OPTIONS_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_OPTION_FLAGS_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_CUSTOM_CONTAINER_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_PARENT_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_WEAK_REFERENCE_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_BINDING_SOURCE_PATH_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_AUTO_GENERATE_KEY;
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_RUNTIME_ACTIVATED_KEY;

    // property type info view models
    extern const LPCSTR_t _PROPERTY_ATTRIBUTE_IS_SOURCE_INHERITED_KEY;


enum ModelTypeInfoType
{
    ModelTypeInfoType_None = 0,
    ModelTypeInfoType_ObservableObject,
    ModelTypeInfoType_Command,
    ModelTypeInfoType_ObservableCollection,
    ModelTypeInfoType_ObservableList,
    ModelTypeInfoType_AsyncOperation,

    ModelTypeInfoType_ViewModel,

    ModelTypeInfoType_Enumerable = 7,
    ModelTypeInfoType_Object = 8,
    ModelTypeInfoType_Dictionary = 9,
    ModelTypeInfoType_ObjectDispatch = 10,
	ModelTypeInfoType_Enum = 0xff,

};

typedef enum ModelTypeInfoType ModelTypeInfoType;

enum  LiteralType
{
    decimal,
    octal,
    hexadecimal
};
typedef enum LiteralType LiteralType;

// forward classes
class CModelSchema;

//------------------------------------------------------------------------------
// Class:   CTypeInfoBase
//
// Purpose: Base Type Info Class
//
//------------------------------------------------------------------------------
template <class T>
class CTypeInfoBase
{
protected:
    CTypeInfoBase(
        const wchar_t* name,
        const T&     parent):
        _name(name),
        _parent(parent)
    {
    }
public:
    const std::wstring& getName() const
    {
        return _name;
    }

    const std::wstring getFullName() const
    {
        return _parent.getNamespace() + L"." + _name;
    }

    const T& getParent() const
    {
        return _parent;
    }

private:
    const T&            _parent;
    std::wstring        _name;
};
//------------------------------------------------------------------------------
// Class:   CIdTypeInfoBase
//
// Purpose: Type Info base with extra 'Id' property
//
//------------------------------------------------------------------------------
template <class T>
class CIdTypeInfoBase : public CTypeInfoBase<T>
{
public:
	UINT32 getId() const
	{
		return _id;
	}
	void setId(UINT32 id)
	{
		_id = id;
	}
protected:
    CIdTypeInfoBase(
        UINT32 id,
        const wchar_t* name,
        const T&     parent):
        CTypeInfoBase<T>(name,parent),
        _id(id)
    {
    }
private:
    UINT32 _id;
};
//------------------------------------------------------------------------------
// Class:   CIIDTypeBase
//
// Purpose: Class with IID Type suuport
//
//------------------------------------------------------------------------------
class CIIDTypeBase
{
public:
	REFIID getType() const
	{
		return _iidType;
	}
protected:
    CIIDTypeBase(REFIID iidType):
		_iidType(iidType)
    {
    }
private:
    IID _iidType;
};

//------------------------------------------------------------------------------
// Class:   CTypeInfo
//
// Purpose: Model Type Info Class
//
//------------------------------------------------------------------------------
class CTypeInfo : 
	public CIdTypeInfoBase<CModelSchema>,
	public CIIDTypeBase
{
public:
    CTypeInfo(
        UINT32 id,
        const wchar_t* name,
        REFIID iidType,
        ModelTypeInfoType modelInfoType,
        const CTypeInfo *baseType,
        const CModelSchema& parent) :
    CIdTypeInfoBase<CModelSchema>(id,name,parent),
        _modelInfoType(modelInfoType),
        _baseType(baseType),
	CIIDTypeBase(iidType)
    {
    }

    virtual ~CTypeInfo()
    {
    }
    ModelTypeInfoType getModelType() const
    {
        return _modelInfoType;
    }
    const CTypeInfo *getBaseType() const
    {
        return _baseType;
    }
       
    void setBaseType(const CTypeInfo *pModelTypeInfo)
    {
        _baseType = pModelTypeInfo;
    }

    template <class T>
    void getDescendents(std::vector<const T*>& descendents) const;

    foundation::library::DictionaryClassPtr& getAttributes()
    {
        return _attributes;
    }

    const foundation::library::DictionaryClassPtr& getAttributes() const
    {
        return _attributes;
    }
    template<class T>
    void setAttribute(LPCSTR_t key, _In_ T value)
    {
        _attributes.Set(key, value);
    }

    template<class T>
    T getAttribute(LPCSTR_t key, _In_ T defaultValue) const
    {
        return _attributes.Get(key, defaultValue);
    }

    template<class T>
    T getValueWithParentCheck(LPCSTR_t key, T defaultVal) const;

    void setAttributes(foundation::library::DictionaryClassPtr& attributes)
    {
        _attributes = attributes;
    }

private:
    ModelTypeInfoType       _modelInfoType;
    const CTypeInfo    *_baseType;
    foundation::library::DictionaryClassPtr _attributes;
};
//------------------------------------------------------------------------------
// Class:   CEnumTypeInfo
//
// Purpose: Enum Type Info Class
//
//------------------------------------------------------------------------------
typedef std::vector< std::tuple<std::wstring, UINT32, LiteralType> > _EnumInfoVectorType;

class CEnumTypeInfo :
    public CTypeInfo
{
public:
    CEnumTypeInfo(
        UINT32 id,
        const wchar_t* enumTypeName,
        REFIID iidType,
        bool isFlags,
        const CEnumTypeInfo *baseType,
        const CModelSchema&     modelSchema) :
        CTypeInfo(id, enumTypeName, iidType, ModelTypeInfoType_Enum, baseType, modelSchema),
        _isFlags(isFlags)
    {
    }

    ~CEnumTypeInfo()
    {
    }

    bool isFlags() const { return _isFlags; }

    const _EnumInfoVectorType& getEnums() const
    {
        return _enumItems;
    }

    void addEnumItem(
        const wchar_t *enumItemName, 
        UINT32 enumItemValue,
        LiteralType literalType)
    {
        _enumItems.push_back(std::make_tuple(enumItemName, enumItemValue, literalType));
    }
private:
    bool                    _isFlags;
    _EnumInfoVectorType     _enumItems;
};
//------------------------------------------------------------------------------
// Class:   CPropertyTypeInfo
//
// Purpose: The simple property type info
//
//------------------------------------------------------------------------------
class CPropertyTypeInfo
{
public:
    CPropertyTypeInfo(
        foundation::PropertyType propertyType,
        const CTypeInfo *pModelType):
        _modelTypeInfo(pModelType),
        _propertyType(propertyType)
    {
    }

    CPropertyTypeInfo(foundation::PropertyType propertyType):
        _modelTypeInfo(nullptr),
        _propertyType(propertyType)
    {
        assert(propertyType != foundation::PropertyType::PropertyType_Inspectable);
    }

    CPropertyTypeInfo(const CTypeInfo *pModelType):
        _modelTypeInfo(pModelType),
        _propertyType(foundation::PropertyType::PropertyType_Inspectable)
    {
        assert(pModelType != nullptr);
    }

    foundation::PropertyType getPropertyType() const
    {
        return _propertyType;
    }

    const CTypeInfo *getModelType() const
    {
        return _modelTypeInfo;
    }
private:
    foundation::PropertyType      _propertyType;
    const CTypeInfo                        *_modelTypeInfo;
};


//------------------------------------------------------------------------------
// Class:   CMethodParamInfo
//
// Purpose: Method Parameter Info
//
//------------------------------------------------------------------------------
class CMethodParamInfo : public CPropertyTypeInfo
{
public:
    CMethodParamInfo(
        const wchar_t* parameterName,
        foundation::PropertyType propertyType,
        const CTypeInfo *pModelType) :
        CPropertyTypeInfo(propertyType, pModelType),
        _parameterName(parameterName)
    {
    }

    const std::wstring& getParameterName() const
    {
        return _parameterName;
    }
private:
    std::wstring _parameterName;
};

class CObjectDispatchInfo;

//------------------------------------------------------------------------------
// Class:   CPropertyInfo
//
// Purpose: Property Type info for a Model Type
//
//------------------------------------------------------------------------------
class CPropertyInfo :
    public CIdTypeInfoBase<CObjectDispatchInfo>,
    public CPropertyTypeInfo
{
public:
    CPropertyInfo(
        UINT32 id,
        const wchar_t* propertyName,
        foundation::PropertyType propertyType,
        const CTypeInfo *pModelType,
        UINT32 flags,
        const CObjectDispatchInfo&     parent) :
        CIdTypeInfoBase<CObjectDispatchInfo>(id, propertyName, parent),
        CPropertyTypeInfo(propertyType, pModelType),
        _flags(flags)
    {
    }

    UINT32 getFlags() const
    {
        return _flags;
    }

    foundation::library::DictionaryClassPtr& getAttributes()
    {
        return _attributes;
    }

    const foundation::library::DictionaryClassPtr& getAttributes() const
    {
        return _attributes;
    }

    template<class T>
    void setAttribute(LPCSTR_t key, _In_ T value)
    {
        _attributes.Set(key, value);
    }

    void setAttributes(foundation::library::DictionaryClassPtr& attributes)
    {
        _attributes = attributes;
    }

    // next properties are used for codegen tools
    UINT32 getRuntimeClassOptions() const;
    void setRuntimeClassOptions(UINT32 classOptions);

    UINT32 getOptionFlags() const;
    void setOptionFlags(UINT32 flagOptions);

    const std::wstring getBindingSourcePath() const;
    void setBindingSourcePath(const wchar_t *bindingSourcePath);

    bool isAutoGenerate() const;
    void setIsAutoGenerate(bool isAutoGenerate);

    bool isParent() const;
    void setIsParent(bool isParent);

    bool isCustomContainer() const;
    void setIsCustomContainer(bool isCustomContainer);

    bool isRuntimeActivated() const;
    void setIsRuntimeActivated(bool isRuntimeActivated);

private:

    template<class T>
    T getValueWithParentCheck(LPCSTR_t key, T defaultVal) const;

    UINT32 _flags;
    foundation::library::DictionaryClassPtr _attributes;
};
//------------------------------------------------------------------------------
// Class:   CMethodInfo
//
// Purpose: Method Info Class
//
//------------------------------------------------------------------------------

typedef std::vector<const CMethodParamInfo *> _MethodParamIteratorType;

class CMethodInfo : public CIdTypeInfoBase<CObjectDispatchInfo>
{
public:
    CMethodInfo(
        UINT32 id,
        const wchar_t* methodName,
        const CPropertyTypeInfo *pResultType,
        bool isAsync,
        const CObjectDispatchInfo&     parent) :
        CIdTypeInfoBase<CObjectDispatchInfo>(id, methodName, parent),
        _resultType(pResultType),
        _isAsync(isAsync)

    {
    }

    ~CMethodInfo()
    {
        delete _resultType;
    }

    const CPropertyTypeInfo* getResultType() const
    {
        return _resultType;
    }

    const _MethodParamIteratorType& getParameters() const
    {
        return _parameters;
    }
    bool getIsAsync() const { return _isAsync; }

    void addParameter(const CMethodParamInfo *pMethodParameter)
    {
        assert(pMethodParameter);
        _parameters.push_back(pMethodParameter);
    }
private:
    AutoDeleteVector<const CMethodParamInfo *>    _parameters;
    const CPropertyTypeInfo* _resultType;
    bool                    _isAsync;

};

//------------------------------------------------------------------------------
// Class:   CObjectDispatchInfo
//
// Purpose: The Property Data Info Class
//
//------------------------------------------------------------------------------
typedef std::vector<const CPropertyInfo *> _PropertyIteratorType;
typedef std::vector<const CMethodInfo *> _MethodIteratorType;

class CObjectDispatchInfo : public CTypeInfo
{
public:
	CObjectDispatchInfo(
		UINT32 id,
		const wchar_t* name,
		REFIID iidType,
        ModelTypeInfoType typeInfoType,
		const CObjectDispatchInfo *baseType,
		const CModelSchema& parent) :
		CTypeInfo(id, name, iidType, typeInfoType, baseType, parent)
	{
	}

    ~CObjectDispatchInfo()
    {}

    void addProperty(const CPropertyInfo *pPropertyInfo)
    {
        assert(pPropertyInfo);
        _properties.push_back(pPropertyInfo);
    }

    void addMethod(const CMethodInfo *pMethodInfo)
    {
        assert(pMethodInfo);
        _methods.push_back(pMethodInfo);
    }

    const _PropertyIteratorType& getProperties() const
    {
        return _properties;
    }

    const _MethodIteratorType& getMethods() const
    {
        return _methods;
    }

private:
    AutoDeleteVector<const CPropertyInfo *>    _properties;
    AutoDeleteVector<const CMethodInfo *>      _methods;

};

//------------------------------------------------------------------------------
// Class:   CEventInfo
//
// Purpose: Event Info Class
//
//------------------------------------------------------------------------------
class CObservableObjectInfo;
class CEventInfo : public CIdTypeInfoBase<CObservableObjectInfo>
{
public:
    CEventInfo(
        UINT32 id,
        const wchar_t* eventName,
        const CPropertyTypeInfo *pEventArgsType,
        const CObservableObjectInfo&     parent):
        CIdTypeInfoBase<CObservableObjectInfo>(id,eventName,parent),
        _eventArgsType(pEventArgsType)
    {
    }

    ~CEventInfo()
    {
        delete _eventArgsType;
    }

    const CPropertyTypeInfo* getEventArgsType() const
    {
        return _eventArgsType;
    }
private:
    const CPropertyTypeInfo*_eventArgsType;
};

//------------------------------------------------------------------------------
// Class:   CObservableObjectInfo
//
// Purpose: The Property Model Info Class
//
//------------------------------------------------------------------------------
typedef std::vector<const CEventInfo *> _EventIteratorType;

class CObservableObjectInfo : public CObjectDispatchInfo
{
public:
    CObservableObjectInfo(
        UINT32 id,
        const wchar_t* name,
        REFIID iidType,
        const CObservableObjectInfo *baseType,
        const CModelSchema& parent,
        ModelTypeInfoType typeInfoType = ModelTypeInfoType_ObservableObject) :
        CObjectDispatchInfo(id,name,iidType,typeInfoType,baseType,parent)
    {
    }

    void addEvent(const CEventInfo *pEventInfo)
    {
        assert(pEventInfo);
        _events.push_back(pEventInfo);
    }

    const _EventIteratorType& getEvents() const
    {
        return _events;
    }

    const std::wstring getCustomInterfaceName() const
    {
        return foundation::to_wstring(this->getAttributes().Get<foundation::string_t>(_PROPERTY_ATTRIBUTE_CUSTOM_CONTAINER_NAME_KEY, U("")).c_str());
    }

    void setCustomInterfaceName(const wchar_t *customInterfaceName)
    {
        this->getAttributes().Set(_PROPERTY_ATTRIBUTE_CUSTOM_CONTAINER_NAME_KEY, foundation::to_platform_string(customInterfaceName).c_str());
    }

    bool isCustomPrivateInterface() const
    {
        return this->getAttributes().Get<bool>(_PROPERTY_ATTRIBUTE_IS_CUSTOM_PRIVATE_INTERFACE_KEY, true);
    }

    void setIsCustomPrivateInterface(bool isCustomPrivateInterface)
    {
        this->getAttributes().Set(_PROPERTY_ATTRIBUTE_IS_CUSTOM_PRIVATE_INTERFACE_KEY, isCustomPrivateInterface);
    }

    void setIsCustomContainer(bool isCustomContainer)
    {
        this->getAttributes().Set(_PROPERTY_ATTRIBUTE_IS_CUSTOM_CONTAINER_KEY, isCustomContainer);
    }

    bool getIsCustomContainer()
    {
        return this->getAttributes().Get<bool>(_PROPERTY_ATTRIBUTE_IS_CUSTOM_CONTAINER_KEY, false);
    }

    UINT32 getRuntimeClassOptions() const
    {
        return this->getAttributes().Get<UINT32>(_PROPERTY_ATTRIBUTE_RUNTIME_CLASS_OPTIONS_KEY, 0);
    }

    void setRuntimeClassOptions(UINT32 classOptions)
    {
        this->getAttributes().Set(_PROPERTY_ATTRIBUTE_RUNTIME_CLASS_OPTIONS_KEY, classOptions);
    }


private:
    AutoDeleteVector<const CEventInfo *>       _events;
    std::wstring                               _customInterfaceName;
    bool                                       _isCustomPrivateInterface;
};
//------------------------------------------------------------------------------
// Class:   CCommandTypeInfo
//
// Purpose: The Command Model Info Class
//
//------------------------------------------------------------------------------
class CCommandTypeInfo : public CTypeInfo
{
public:
    CCommandTypeInfo(
        UINT32 id,
        const wchar_t* name,
        REFIID iidType,
        const CModelSchema& parent,
        const CPropertyTypeInfo *pParameterTypeInfo,
        const CPropertyTypeInfo *pResultTypeInfo,
        bool isAsync,
        bool isUnknwonState
        ):
    CTypeInfo(id,name,iidType,ModelTypeInfoType_Command,nullptr,parent),
        _parameterTypeInfo(pParameterTypeInfo),
        _resultTypeInfo(pResultTypeInfo),
        _isAsync(isAsync),
        _isUnknwonState(isUnknwonState)
    {
    }

    ~CCommandTypeInfo()
    {
        delete _parameterTypeInfo;
        delete _resultTypeInfo;
    }

    const CPropertyTypeInfo* getParameterTypeInfo() const
    {
        return _parameterTypeInfo;
    }
    const CPropertyTypeInfo* getResultTypeInfo() const
    {
        return _resultTypeInfo;
    }
    bool getIsAsync() const { return _isAsync; }
    bool getIsUnknownState() const { return _isUnknwonState; }
private:
    const CPropertyTypeInfo* _parameterTypeInfo;
    const CPropertyTypeInfo* _resultTypeInfo;
    bool                     _isAsync;
    bool                     _isUnknwonState;
};
//------------------------------------------------------------------------------
// Class:   CEnumerableTypeInfo
//
// Purpose: The Collection Model Info Class
//
//------------------------------------------------------------------------------

class CEnumerableTypeInfo : public CTypeInfo
{
public:
    CEnumerableTypeInfo(
        UINT32 id,
        const wchar_t* name,
        REFIID iidType,
        const CModelSchema& parent,
        const CPropertyTypeInfo *pItemTypeInfo,
        ModelTypeInfoType modelInfoType = ModelTypeInfoType_ObservableCollection
        ):
    CTypeInfo(id,name,iidType,modelInfoType,nullptr,parent),
        _itemTypeInfo(pItemTypeInfo)
    {
    }
    ~CEnumerableTypeInfo()
    {
        delete _itemTypeInfo;
    }

    const CPropertyTypeInfo* getItemTypeInfo() const
    {
        return _itemTypeInfo;
    }
private:
    const CPropertyTypeInfo* _itemTypeInfo;
};

//------------------------------------------------------------------------------
// Class:   CAsyncOperationTypeInfo
//
// Purpose: The AsyncOperation Model Info Class
//
//------------------------------------------------------------------------------

class CAsyncOperationTypeInfo : public CTypeInfo
{
public:
    CAsyncOperationTypeInfo(
        UINT32 id,
        const wchar_t* name,
        REFIID iidType,
        const CModelSchema& parent,
        const CPropertyTypeInfo *pResultTypeInfo
        ):
    CTypeInfo(id,name,iidType,ModelTypeInfoType_AsyncOperation,nullptr,parent),
        _resultTypeInfo(pResultTypeInfo)
    {
    }

    ~CAsyncOperationTypeInfo()
    {
        delete _resultTypeInfo;
    }
    const CPropertyTypeInfo* getResultTypeInfo() const
    {
        return _resultTypeInfo;
    }
private:
    const CPropertyTypeInfo* _resultTypeInfo;
};
//------------------------------------------------------------------------------
// Class:   CPropertyViewModelInfo
//
// Purpose: The Property Model Info Class
//
//------------------------------------------------------------------------------
class CPropertyViewModelInfo : public CObservableObjectInfo
{
public:
    CPropertyViewModelInfo(
        UINT32 id,
        const wchar_t* name,
        REFIID iidType,
        const CObservableObjectInfo *baseType,
        const CObservableObjectInfo *sourceType,
        bool isInherited,
        const CModelSchema& parent):
        CObservableObjectInfo(id,name,iidType,baseType,parent,ModelTypeInfoType_ViewModel),
            _sourceType(sourceType),
            _isInherited(isInherited)
    {
    }

    bool getIsInherited() const { return _isInherited; }
    const CObservableObjectInfo *getSourceType() const { return _sourceType; }
    void setSourceType(const CObservableObjectInfo *pModelTypeInfo)
    {
        _sourceType = pModelTypeInfo;
    }
private:
    const CObservableObjectInfo *_sourceType;
    bool _isInherited;
};
//------------------------------------------------------------------------------
// Class:   CModelSchema
//
// Purpose: The Model Schema Class
//
//------------------------------------------------------------------------------
// forward Vector iterators
typedef std::vector<const CTypeInfo *>     _TypeInfoIteratorType;
typedef std::vector<const CEnumTypeInfo *>      _EnumTypeInfoIteratorType;
typedef std::vector<const CModelSchema *>       _ModelSchemaIteratorType;

class CModelSchema
{
public:
    CModelSchema(const wchar_t *namespace_):
        _namespace(namespace_),
        _isFactorySchema(false)
    {
    }

    virtual ~CModelSchema() {}

    bool isFactorySchema() const { return _isFactorySchema;}

    const std::wstring& getNamespace() const
    {
        return _namespace;
    }

    const _EnumTypeInfoIteratorType& getEnumTypes() const
    {
        return _enumTypes;
    }

    const _TypeInfoIteratorType& getModelTypes() const
    {
        return _modelTypes;
    }

    const _ModelSchemaIteratorType& getReferenceSchemas() const
    {
        return _referenceSchemas;
    }

    foundation::library::DictionaryClassPtr& getAttributes()
    {
        return _attributes;
    }

    const foundation::library::DictionaryClassPtr& getAttributes() const
    {
        return _attributes;
    }

    template<class T>
    T getAttribute(LPCSTR_t key, _In_ T defaultValue) const
    {
        return _attributes.Get(key, defaultValue);
    }

    void setAttributes(foundation::library::DictionaryClassPtr& attributes)
    {
        _attributes = attributes;
    }

    const CEnumTypeInfo *resolveEnumType(const wchar_t *typeEnumName) const;
    const CTypeInfo *resolveModelType(const wchar_t *typeModelName) const;

    void addReferenceSchema(const CModelSchema *pModelSchema)
    {
        assert(pModelSchema);
        _referenceSchemas.push_back(pModelSchema);
    }

    void addModelType(const CTypeInfo *pModelTypeInfo)
    {
        assert(pModelTypeInfo);
        _modelTypes.push_back(pModelTypeInfo);
    }

    void addEnumType(const CEnumTypeInfo *pEnumTypeInfo)
    {
        assert(pEnumTypeInfo);
        _enumTypes.push_back(pEnumTypeInfo);
    }

    const CEnumTypeInfo *resolveEnumTypeLocal(const wchar_t *typeEnumName) const;
    const CTypeInfo *resolveModelTypeLocal(const wchar_t *typeModelName) const;

    static const CModelSchema& getFactorySchema();

private:
    bool                                        _isFactorySchema;
    std::wstring                                _namespace;
    AutoDeleteVector<const CModelSchema *>      _referenceSchemas;
    AutoDeleteVector<const CTypeInfo *>         _modelTypes;
    AutoDeleteVector<const CEnumTypeInfo *>     _enumTypes;
    foundation::library::DictionaryClassPtr                  _attributes;
};

template <class T>
inline void CTypeInfo::getDescendents(std::vector<const T*>& descendents) const
{
    for(_TypeInfoIteratorType::const_iterator iter =
        this->getParent().getModelTypes().begin();
        iter !=
        this->getParent().getModelTypes().end();
        ++iter)
    {
        if((*iter)->getBaseType() == this)
        {
            descendents.push_back((const T*)(*iter));
        }
    }
}

template<class T>
T CPropertyInfo::getValueWithParentCheck(LPCSTR_t key, T defaultVal) const
{
    return _attributes.Get(key, getParent().getValueWithParentCheck(key, defaultVal));
}

template<class T>
T CTypeInfo::getValueWithParentCheck(LPCSTR_t key, T defaultVal) const
{
    return _attributes.Get(key, getParent().getAttribute(key, defaultVal));
}

}

#endif
