/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectStatics.cpp
****/
#include "pch.h"
#include "ObjectStatics.h"

#include <pmod/library/library_util.h>
#include <foundation/dictionary_ptr.h>
#include <foundation/pv_util.h>
#include <foundation/library/library_util.h>
#include <foundation/library/logger_macros.h>
#include <foundation/library/util_stl.h>
#include <foundation/library/object_factory_util.h>

#include <foundation/library/com_ptr_singleton.h>
#include "ObjectFactory.h"

#include <map>

using namespace pmod;
using namespace pmod::library;

// {411EB729-14A3-4405-B839-464C56DE85B8}
const GUID pmod::library::IID_IObjectStatics =
{ 0x411eb729, 0x14a3, 0x4405,{ 0xb8, 0x39, 0x46, 0x4c, 0x56, 0xde, 0x85, 0xb8 } };

typedef std::map<UINT64, foundation::ComPtr<foundation::IObject> >                      _ObjectMapResolverType;
typedef std::map<foundation::ComPtr<foundation::IObject>, foundation::ComPtr<foundation::IDictionary>, foundation::library::ObjectLessThan>    _ObjectMapType;

// forwards
static HRESULT IInspectableToObject(
    _In_ foundation::IInspectable *pInspectable,
    _In_ IObjectSerializerCallback *pObjectSerializerCallback,
    _COM_Outptr_ foundation::IInspectable **ppObject);
static HRESULT ObjectToInspectable(
    _In_ foundation::IInspectable *pObject,
    _In_ IObjectDeserializerCallback *pObjectDeserializerCallback,
    _Out_ foundation::IInspectable **ppInspectable);

EXTERN_C FOUNDATION_API CObjectFactory *STDMETHODCALLTYPE GetObjectFactoryInstance();

//------------------------------------------------------------------------------
// Class : CDefaultObjectSerializer
//------------------------------------------------------------------------------
class CDefaultObjectSerializer :
    public foundation::ctl::ComBase,
    public IObjectSerializerCallback,
    public IDefaultObjectSerializerCallback
{
public:
    static HRESULT CreateInstance(bool serializeTypeInfo,IObjectSerializerCallback **ppObjectSerializerCallback)
    {
        CDefaultObjectSerializer *pObjectSerializer = nullptr;
        foundation::ctl::ComObject<CDefaultObjectSerializer>::CreateInstance(&pObjectSerializer);
        pObjectSerializer->_serializeTypeInfo = serializeTypeInfo;
        *ppObjectSerializerCallback = static_cast<IObjectSerializerCallback *>(pObjectSerializer);
        return S_OK;
    }
 
protected:
    CDefaultObjectSerializer():
        _serializeTypeInfo(true)
    {
    }
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == IObjectSerializerCallback::GetIID())
        {
            *ppInterface = static_cast<IObjectSerializerCallback *>(this);
        }
        else if (iid == IDefaultObjectSerializerCallback::GetIID())
        {
            *ppInterface = static_cast<IDefaultObjectSerializerCallback *>(this);
        }
        else
        {
            return foundation::ctl::ComBase::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }

    STDMETHOD(Invoke)(_In_ foundation::IInspectable *pObject, _COM_Outptr_ foundation::IDictionary **ppDictionary) override
    {
        return SerializeObjectInternal(pObject,_serializeTypeInfo,ppDictionary);
    }
private:
    STDMETHOD(SerializeObject)(_In_ foundation::IObject *pObject, _In_ bool serializeTypeInfo, _COM_Outptr_ foundation::IDictionary **ppDictionary)
    {
        return SerializeObjectInternal(pObject, serializeTypeInfo, ppDictionary);
    }
public:

    static HRESULT GetObjectTypeInfoType(
        _In_ foundation::IObjectTypeInfo *pObjectTypeInfo,
        UINT32 &modelType)
    {
        foundation_assert(pObjectTypeInfo);
        foundation::ComPtr<ICommandInfo> spCommandModelTypeInfo;
        foundation::ComPtr < foundation::IEnumerableTypeInfo > spCollectionModelTypeInfo;
        foundation::ComPtr<IObservableObjectInfo> spPropertModelTypeInfo;
        foundation::ComPtr<foundation::IObjectDispatchInfo> spPropertDataTypeInfo;
        foundation::ComPtr<foundation::IAsyncOperationTypeInfo> spAsyncOperationTypeInfo;

        if (SUCCEEDED(foundation::QueryInterface(pObjectTypeInfo, spCommandModelTypeInfo.GetAddressOf())))
        {
            modelType = TypeInfo_CommandModel;
        }
        else if (SUCCEEDED(foundation::QueryInterface(pObjectTypeInfo, spCollectionModelTypeInfo.GetAddressOf())))
        {
            modelType = TypeInfo_Collection;
        }
        else if (SUCCEEDED(foundation::QueryInterface(pObjectTypeInfo, spAsyncOperationTypeInfo.GetAddressOf())))
        {
            modelType = foundation::TypeInfo_AsyncOperation;
        }
        else if (SUCCEEDED(foundation::QueryInterface(pObjectTypeInfo, spPropertModelTypeInfo.GetAddressOf())))
        {
            modelType = TypeInfo_PropertyModel;
        }
        else if (SUCCEEDED(foundation::QueryInterface(pObjectTypeInfo, spPropertDataTypeInfo.GetAddressOf())))
        {
            modelType = foundation::TypeInfo_PropertyData;
        }
        else
        {
            IFR_ASSERT(E_UNEXPECTED);
        }
        return S_OK;
    }

    static HRESULT GetObjectTypeInfoType(
        _In_ foundation::IObject *pObject,
        UINT32 &modelType)
    {
        foundation_assert(pObject);
        foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
        IFR_ASSERT(pObject->GetTypeInfo(spTypeInfo.GetAddressOf()));
        return GetObjectTypeInfoType(spTypeInfo, modelType);
    }

    static HRESULT SerializeObjectInternal(
        _In_ foundation::IInspectable *pObject, 
        _In_ bool serializeTypeInfo, 
        _COM_Outptr_ foundation::IDictionary **ppDictionary)
    {
        foundation_assert(pObject);
        foundation_assert(ppDictionary);

        // try an Object
        foundation::ComPtr<foundation::IObject> spAsObject;
        if (SUCCEEDED(foundation::QueryInterface(pObject, spAsObject.GetAddressOf())))
        {
            return SerializeObjectInternal(spAsObject, serializeTypeInfo, ppDictionary);
        }
        // try a Dictionary
        foundation::ComPtr<foundation::IDictionary> spDictionary;
        if(SUCCEEDED(foundation::QueryInterface(pObject,spDictionary.GetAddressOf())))
        {
            *ppDictionary = spDictionary.Detach();
            return S_OK;
        }

        // TODO: handle non serialized values
        *ppDictionary = nullptr;
        return S_FALSE;
        //return E_NOTIMPL;
    }

    static HRESULT SerializeObjectInternal(
        _In_ foundation::IObject *pObject, 
        _In_ bool serializeTypeInfo, 
        _COM_Outptr_ foundation::IDictionary **ppDictionary)
    {
        foundation_assert(pObject);
        foundation_assert(ppDictionary);

        foundation::DictionaryPtr   serialize_ptr;
        IFR_ASSERT(foundation::library::CreateDictionaryClass(serialize_ptr.GetAddressOf()));

        // serialize id
        UINT64 id = GetUniversalId(pObject);

        IFR_ASSERT(serialize_ptr.SetValue(_KEY_ID, id));
        // mark object as model
        IFR_ASSERT(serialize_ptr.SetValue(_KEY_IS_OBJECT,true));

        if(serializeTypeInfo)
        {
            // serialize modelType and iidType
            foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
            IFR_ASSERT(pObject->GetTypeInfo(spTypeInfo.GetAddressOf()));

            UINT32 modelType;
            IFR_ASSERT(GetObjectTypeInfoType(spTypeInfo, modelType));

            if (modelType == TypeInfo_PropertyModel)
            {
                foundation::ComPtr<IObservableObjectInfo> spPropertModelTypeInfo;
                IFR_ASSERT(foundation::QueryInterface(spTypeInfo, spPropertModelTypeInfo.GetAddressOf()));
                bool bSerializeTypeInfo = false;
                foundation::library::IFoundationClassBase *pObjectClass = nullptr;
                if (SUCCEEDED(foundation::QueryWeakReference(pObject, &pObjectClass)))
                {
                    UINT16 options;
                    pObjectClass->GetFoundationOptions(&options);
                    bSerializeTypeInfo = (options & foundation::library::FoundationClassOptions_IsLocalObjectTypeInfo) != 0;
                }
                if (bSerializeTypeInfo)
                {
                    foundation::DictionaryPtr   type_info_ptr;
                    IFR_ASSERT(foundation::library::CreateDictionaryClass(type_info_ptr.GetAddressOf()));

                    foundation::DictionaryPtr   properties_type_info_ptr;
                    IFR_ASSERT(foundation::library::CreateDictionaryClass(properties_type_info_ptr.GetAddressOf()));

                    foundation::ArrayRefCountWrapper<foundation::IPropertyInfo> properties;
                    IFR_ASSERT(spPropertModelTypeInfo->GetProperties(
                        static_cast<UINT32>((int)foundation::InstanceFlags::All | (int)foundation::InstanceFlags::NoReserved),
                        properties.GetSizeAddressOf(),
                        properties.GetBufferAddressOf()));

                    for (UINT32 index = 0; index < properties.GetSize(); ++index)
                    {
                        UINT32 propertyId;
                        foundation::HStringPtr name;
                        foundation::PropertyType propertyType;
                        UINT32 flags;
                        GUID iidType;

                        properties[index]->GetId(&propertyId);
                        properties[index]->GetName(name.GetAddressOf());
                        properties[index]->GetPropertyType(&propertyType);
                        properties[index]->GetFlags(&flags);

                        foundation::ComPtr<foundation::IObjectTypeInfo> spPropertyTypeInfo;
                        properties[index]->GetTypeInfo(spPropertyTypeInfo.GetAddressOf());
                        if (spPropertyTypeInfo != nullptr)
                        {
                            spPropertyTypeInfo->GetType(&iidType);
                        }
                        else
                        {
                            iidType = foundation_GUID_NULL;
                        }

                        foundation::DictionaryPtr   property_type_info_ptr;
                        IFR_ASSERT(foundation::library::CreateDictionaryClass(property_type_info_ptr.GetAddressOf()));
                        property_type_info_ptr.SetValue(_KEY_ID, propertyId);
                        property_type_info_ptr.SetValue(_KEY_PROPERTY_TYPE, (UINT32)propertyType);
                        property_type_info_ptr.SetValue(_KEY_PROPERTY_INFO_FLAGS, flags);
                        property_type_info_ptr.SetValue(_KEY_TYPE_ID, iidType);

                        properties_type_info_ptr.SetValue(name.GetRawBuffer(), property_type_info_ptr);
                    }
                    type_info_ptr.SetValue(_KEY_PROPERTIES, properties_type_info_ptr);
                    serialize_ptr.SetValue(_KEY_TYPE_INFO, type_info_ptr);
                }
            }

            IID iidType;
            IFR_ASSERT(spTypeInfo->GetType(&iidType));

            if (serialize_ptr.HasKey(_KEY_TYPE_INFO))
            {
                foundation::DictionaryPtr   type_info_ptr;
                IFR_ASSERT(serialize_ptr.GetValuePtr(_KEY_TYPE_INFO, type_info_ptr));

                UINT32 typeId;
                foundation::HStringPtr name;
                spTypeInfo->GetTypeId(&typeId);
                spTypeInfo->GetName(name.GetAddressOf());

                type_info_ptr.SetValue(_KEY_TYPE_UNIQUED_ID, typeId);
                type_info_ptr.SetValue(_KEY_NAME, name);
            }

            IFR_ASSERT(serialize_ptr.SetValue(_KEY_OBJECT_TYPE,modelType));

            foundation::HStringPtr iidStr;
            IFR_ASSERT(foundation::pv_util::ToString(iidType, iidStr.GetAddressOf()));
            IFR_ASSERT(serialize_ptr.SetValue(_KEY_TYPE_ID, iidStr));
        }

        *ppDictionary = serialize_ptr.Detach();
        return S_OK;
    }
private:
    bool _serializeTypeInfo;
};

//------------------------------------------------------------------------------
// Class : CDefaultObjectDeserializer
//------------------------------------------------------------------------------
class CDefaultObjectDeserializer :
    public foundation::ctl::Implements<IObjectDeserializerCallback,&IID_IObjectDeserializerCallback>
{
protected:
    STDMETHOD(Invoke)(
        _In_ foundation::IDictionary *pDictionary,
        _COM_Outptr_ foundation::IInspectable **ppObject) override
    {
        foundation_assert(pDictionary);

        bool bFound;
        // resolve unified unique id
        pDictionary->HasKey(
            foundation::HStringRef(_KEY_IS_OBJECT),
            &bFound);
        if(bFound)
        {
            return DeserializeObjectInternal(
                pDictionary,
                _spModelResolverCallback,
                reinterpret_cast<foundation::IObject **>(ppObject));
        }
        // default is to pass the object untouched
        *ppObject = pDictionary;
        (*ppObject)->AddRef();
        // indicate we did not made any conversion
        return S_FALSE;
    }
public:
    static HRESULT DeserializeObjectInternal(
        _In_ foundation::IDictionary *pDictionary,
        _In_ IObjectResolverCallback *pObjectResolverCallback,
        _COM_Outptr_ foundation::IObject **ppObject)
    {
        foundation_assert(pDictionary);
        foundation_assert(pObjectResolverCallback);

        foundation::InspectablePtr spValue;
        // resolve unified unique id
        IFR_ASSERT(pDictionary->Lookup(
            foundation::HStringRef(_KEY_ID),
            spValue.ReleaseAndGetAddressOf()));
        UINT64 id;
        IFR_ASSERT(foundation::pv_util::GetUInt64Value(spValue, &id));

        // resolve iid type
        IID iidType = foundation_GUID_NULL;

        // Note that this next Lookup could fail if no iid type was serialized back
        if(SUCCEEDED(pDictionary->Lookup(
            foundation::HStringRef(_KEY_TYPE_ID),
            spValue.ReleaseAndGetAddressOf())))
        {
            foundation::HStringPtr iidTypeStr;
            IFR_ASSERT(foundation::pv_util::GetStringValue(spValue, iidTypeStr.GetAddressOf()));
            IFR(foundation::pv_util::ToGuid(iidTypeStr, iidType));
        }

        // use model resolver callback
        IFR_ASSERT(pObjectResolverCallback->Invoke(id, iidType, ppObject));
        return S_OK;
    }

public:
    foundation::ComPtr<IObjectResolverCallback> _spModelResolverCallback;
};
//------------------------------------------------------------------------------
// Class : CModelReferenceResolverCallback
//------------------------------------------------------------------------------
class CModelReferenceResolverCallback :
    public foundation::ctl::Implements<IObjectResolverCallback, &IID_IObjectResolverCallback>
{
public:
    _ObjectMapResolverType & GetObjectReferences() 
    {
        return _objectReferences;
    }
protected:
    STDMETHOD(Invoke)(UINT64 id, GUID iidModelType, foundation::IObject **ppObject) override
    {
        UNREFERENCED_PARAMETER(iidModelType);
        _ObjectMapResolverType::iterator iter = _objectReferences.find(id);
        if (iter != _objectReferences.end())
        {
            *ppObject = (*iter).second;
            (*ppObject)->AddRef();
            return S_OK;
        }
        foundation_assert(false);
        return E_FAIL;
    }
private:
    _ObjectMapResolverType _objectReferences;
};

//------------------------------------------------------------------------------
// Class : CFactoryModelResolverCallback
//------------------------------------------------------------------------------
class CFactoryModelResolverCallback :
    public foundation::ctl::Implements<IObjectResolverCallback, &IID_IObjectResolverCallback>
{
protected:
    STDMETHOD(Invoke)(UINT64 id, GUID iidType, foundation::IObject **ppObject) override
    {
        foundation_assert(ppObject);
        UNREFERENCED_PARAMETER(id);
        return GetObjectFactoryInstance()->ActivateObject(iidType, nullptr, ppObject);
    }
};
//------------------------------------------------------------------------------
// CopyDictionary
// Helper to copy from a source Dictionary into a target
//------------------------------------------------------------------------------
static HRESULT CopyDictionary(
    _In_ foundation::IDictionary *pSource,
    _In_ IObjectSerializerCallback *pObjectSerializerCallback,
    _COM_Outptr_ foundation::IDictionary **ppDictionary)
{
    foundation_assert(pSource);
    foundation_assert(ppDictionary);
    HRESULT hr;

    foundation::ArrayRefCountWrapper<foundation::IDictionaryPair> properties;
    IFHR_ASSERT(pSource->GetView(properties.GetSizeAddressOf(), properties.GetBufferAddressOf()));

    foundation::ComPtr<foundation::IDictionary>                  spObjectDictionary;
    IFHR_ASSERT(foundation::library::CreateDictionaryClass(spObjectDictionary.GetAddressOf()));
    for (UINT32 index = 0; index < properties.GetSize(); ++index)
    {
        foundation::HStringPtr key;
        IFHR_ASSERT(properties[index]->GetKey(key.GetAddressOf()));

        foundation::InspectablePtr spValue;
        IFHR_ASSERT(properties[index]->GetValue(spValue.GetAddressOf()));

        foundation::InspectablePtr spObject;
        IFR_ASSERT(IInspectableToObject(spValue, pObjectSerializerCallback, spObject.GetAddressOf()));
        bool replaced;
        IFHR_ASSERT(spObjectDictionary->Insert(key, spObject, &replaced));
    }
    *ppDictionary = spObjectDictionary.Detach();
    return S_OK;
}

//------------------------------------------------------------------------------
// InspectableToDictionary
// Helper from Inspectable -> IDictionary
//------------------------------------------------------------------------------
static HRESULT InspectableToDictionary(
    _In_ foundation::IInspectable *pObject,
    _In_ IObjectSerializerCallback *pObjectSerializerCallback,
    _COM_Outptr_ foundation::IDictionary **ppDictionary)
{
    foundation_assert(pObject);
    foundation_assert(ppDictionary);

    HRESULT hr;

    foundation::ComPtr<foundation::IDictionary>   spDictionary;
    if(!foundation::IsTypeOf<foundation::IObject>(pObject) &&
        SUCCEEDED(foundation::QueryInterface(pObject,spDictionary.GetAddressOf())))
    {
        return CopyDictionary(
            spDictionary, 
            pObjectSerializerCallback,
            ppDictionary);
    }
    else if(pObjectSerializerCallback)
    {
        IFHR_ASSERT(pObjectSerializerCallback->Invoke(pObject,ppDictionary));
    }
    else
    {
        IFHR_ASSERT(CDefaultObjectSerializer::SerializeObjectInternal(pObject,true,ppDictionary));
    }
    return hr;
}
//------------------------------------------------------------------------------
// DictionaryToInspectable
// Helper to convert from IDictionary -> foundation::IInspectable
//------------------------------------------------------------------------------
static HRESULT DictionaryToInspectable(
    _In_ foundation::IDictionary *pDictionary,
    _In_ IObjectDeserializerCallback *pObjectDeserializerCallback,
    _COM_Outptr_ foundation::IInspectable **ppInspectable)
{
    foundation_assert(pDictionary);
    foundation_assert(ppInspectable);

    if(pObjectDeserializerCallback)
    {
        HRESULT hr = pObjectDeserializerCallback->Invoke(pDictionary,ppInspectable);
        IFHR_ASSERT(hr);
        if(hr == S_FALSE)
        {
            RELEASE_INTERFACE(*ppInspectable);
            foundation_assert(*ppInspectable == nullptr);

            // Deserializer could not resolve or Object was not supported
            foundation::ArrayRefCountWrapper<foundation::IDictionaryPair> properties;
            IFHR_ASSERT(pDictionary->GetView(properties.GetSizeAddressOf(),properties.GetBufferAddressOf()));

            foundation::ComPtr<foundation::IDictionary>                  spInspectableDictionary;
            IFHR_ASSERT(foundation::library::CreateDictionaryClass(spInspectableDictionary.GetAddressOf()));
            for(UINT32 index = 0 ;index < properties.GetSize();++index)
            {
                foundation::HStringPtr key;
                IFHR_ASSERT(properties[index]->GetKey(key.GetAddressOf()));

                foundation::InspectablePtr spObject;
                IFHR_ASSERT(properties[index]->GetValue(spObject.GetAddressOf()));
                foundation::InspectablePtr spValue;
                IFR_ASSERT(ObjectToInspectable(spObject,pObjectDeserializerCallback,spValue.GetAddressOf()));
                bool replaced;
                IFHR_ASSERT(spInspectableDictionary->Insert(key,spValue,&replaced));
            }
            *ppInspectable = spInspectableDictionary.Detach();
        }
    }
    else
    {
        // return the 'raw' Foundation Dictionary
        *ppInspectable = pDictionary;
        (*ppInspectable)->AddRef();
    }
    return S_OK;
}
//------------------------------------------------------------------------------
// InspectableArrayToObjectArray
// Helper to convert from InspectableArrayWrapper -> Object Array
//------------------------------------------------------------------------------
static HRESULT InspectableArrayToObjectArray(
    _In_ UINT32 size,
    _In_ foundation::IInspectable **ppInspectables,
    _In_ IObjectSerializerCallback *pObjectSerializerCallback,
    _COM_Outptr_ foundation::IInspectable **ppObjectArray)
{
    HRESULT hr;
    _Inspectable_ptr_vector_type objArray;
    objArray.resize(size);
    for(UINT32 i = 0;i < size;++i)
    {
        hr = IInspectableToObject(ppInspectables[i],pObjectSerializerCallback,objArray[i].GetAddressOf());
        IFHR_ASSERT(hr);
    }
    return foundation::pv_util::CreateInspectableArrayValue(
        size,
        size ? objArray.front().GetAddressOfPtr():nullptr,
        ppObjectArray);
}
//------------------------------------------------------------------------------
// ObjectArrayToInspectableArray
// Helper to convert from InspectableArrayWrapper -> Object Array
//------------------------------------------------------------------------------
static HRESULT ObjectArrayToInspectableArray(
    _In_ UINT32 size,
    _In_ foundation::IInspectable **ppObjects,
    _In_ IObjectDeserializerCallback *pObjectDeserializerCallback,
    _COM_Outptr_ foundation::IInspectable **ppInspectableArray)
{
    HRESULT hr;
    _Inspectable_ptr_vector_type inspectableArray;
    inspectableArray.resize(size);
    for(UINT32 i = 0;i < size;++i)
    {
        hr = ObjectToInspectable(ppObjects[i],pObjectDeserializerCallback,inspectableArray[i].GetAddressOf());
        IFHR_ASSERT(hr);
    }
    return foundation::pv_util::CreateInspectableArrayValue(
        size,
        size ? inspectableArray.front().GetAddressOfPtr():nullptr,
        ppInspectableArray);
}
//------------------------------------------------------------------------------
// IInspectableToObject
// Helper to convert from foundation::IInspectable -> Object
//------------------------------------------------------------------------------
static HRESULT IInspectableToObject(
    _In_ foundation::IInspectable *pInspectable,
    _In_ IObjectSerializerCallback *pObjectSerializerCallback,
    _COM_Outptr_ foundation::IInspectable **ppObject)
{
    if(pInspectable == nullptr)
    {
        *ppObject = nullptr;
        return S_OK;
    }
    HRESULT hr;
    foundation::PropertyType propertyType;
    IFHR_ASSERT(foundation::pv_util::GetType(pInspectable, &propertyType));
    if (propertyType == foundation::PropertyType_InspectableArray)
    {
        foundation::InspectableArrayWrapper arrayInspectableWrapper;
        IFHR_ASSERT(foundation::pv_util::GetInspectableArray(
            pInspectable,
            arrayInspectableWrapper.GetSizeAddressOf(),
            arrayInspectableWrapper.GetBufferAddressOf()));
        return InspectableArrayToObjectArray(
            arrayInspectableWrapper.GetSize(),
            (foundation::IInspectable **)arrayInspectableWrapper.GetBuffer(),
            pObjectSerializerCallback,
            ppObject);
    }
    else if (propertyType == foundation::PropertyType_Inspectable)
    {
        // it's not an IPropertyValue but an foundation::IInspectable
        return InspectableToDictionary(pInspectable,
            pObjectSerializerCallback,
            (foundation::IDictionary **)ppObject);
    }
    else
    {
        // a property value type => return the same object
        *ppObject = pInspectable;
        (*ppObject)->AddRef();
        return S_OK;
    }
}

//------------------------------------------------------------------------------
// ObjectToInspectable
// Helper to convert from Object -> foundation::IInspectable
//------------------------------------------------------------------------------
static HRESULT ObjectToInspectable(
    _In_ foundation::IInspectable *pObject,
    _In_ IObjectDeserializerCallback *pObjectDeserializerCallback,
    _Out_ foundation::IInspectable **ppInspectable)
{
    foundation_assert(ppInspectable);

    if(pObject == nullptr)
    {
        *ppInspectable = nullptr;
        return S_OK;
    }
    HRESULT hr;
    foundation::PropertyType propertyType;
    IFHR_ASSERT(foundation::pv_util::GetType(pObject, &propertyType));
    if (propertyType == foundation::PropertyType_InspectableArray)
    {
        foundation::InspectableArrayWrapper arrayObjectWrapper;
        IFHR_ASSERT(foundation::pv_util::GetInspectableArray(
            pObject,
            arrayObjectWrapper.GetSizeAddressOf(),
            arrayObjectWrapper.GetBufferAddressOf()));
        return ObjectArrayToInspectableArray(
            arrayObjectWrapper.GetSize(),
            (foundation::IInspectable **)arrayObjectWrapper.GetBuffer(),
            pObjectDeserializerCallback,
            ppInspectable);
    }
    else if (propertyType == foundation::PropertyType_Inspectable)
    {
        // we expect an IDictionary
        foundation::ComPtr<foundation::IDictionary>                  spDictionary;
        IFHR_ASSERT(foundation::QueryInterface(pObject,spDictionary.GetAddressOf()));

        return DictionaryToInspectable(spDictionary,
                                    pObjectDeserializerCallback,
                                    ppInspectable);
    }
    else
    {
        // a property value type => return the same object
        *ppInspectable = pObject;
        (*ppInspectable)->AddRef();
        return S_OK;
    }
}
//------------------------------------------------------------------------------
// ObjectToDictionary
// Helper to serialize an IObject -> IDictionary
//------------------------------------------------------------------------------
static HRESULT ObjectToDictionary(
    _In_ foundation::IObject *pObject,
    _In_ IObjectSerializerCallback *pObjectSerializerCallback,
    _In_ foundation::IDictionary *pObjectDictionary,
    _In_ bool bAddToDictionary)
{
    foundation_assert(pObject);
    foundation_assert(pObjectSerializerCallback);
    foundation_assert(pObjectDictionary);

    HRESULT hr;
    bool replaced;

    foundation::ComPtr<foundation::IObjectDispatch> spAsPropertyData;
    foundation::ComPtr<IObservableCollection> spAsCollectionModel;
    foundation::ComPtr<ICommand> spAsCommandModel;
    foundation::ComPtr<foundation::IAsyncOperation> spAsAsyncOperation;

    if (SUCCEEDED(foundation::QueryInterface(pObject, spAsCollectionModel.GetAddressOf())))
    {
        // get items
        foundation::InspectableArrayWrapper items;
        IFHR_ASSERT(spAsCollectionModel->GetItems(items.GetSizeAddressOf(),items.GetBufferAddressOf()));

        foundation::InspectablePtr objectItemsArray;
        IFHR_ASSERT(InspectableArrayToObjectArray(
            items.GetSize(),
            (foundation::IInspectable **)items.GetBuffer(),
            pObjectSerializerCallback,
            objectItemsArray.GetAddressOf()));
        if(bAddToDictionary)
        {
            IFHR_ASSERT(pObjectDictionary->Insert(
                foundation::HStringRef(_KEY_ITEMS),
                objectItemsArray,&replaced));
        }
    }
    else if (SUCCEEDED(foundation::QueryInterface(pObject, spAsCommandModel.GetAddressOf())))
    {
        UINT32 state;
        IFHR_ASSERT(spAsCommandModel->GetState(&state));
        IFHR_ASSERT(pObjectDictionary->Insert(
            foundation::HStringRef(_KEY_COMMAND_STATE),
            foundation::pv_util::CreateValue(state),
            &replaced));
    }
    else if (SUCCEEDED(foundation::QueryInterface(pObject, spAsAsyncOperation.GetAddressOf())))
    {
        foundation::ResultStatus status;
        IFHR_ASSERT(spAsAsyncOperation->GetStatus(&status));
        HRESULT error;
        IFHR_ASSERT(spAsAsyncOperation->GetError(&error));


        bool replaced;
        IFHR_ASSERT(pObjectDictionary->Insert(
            foundation::HStringPtr(_KEY_ASYNC_OPERATION_STATUS),
            foundation::pv_util::CreateValue((UINT32)status),
            &replaced));
        IFHR_ASSERT(pObjectDictionary->Insert(
            foundation::HStringPtr(_KEY_ASYNC_OPERATION_ERROR),
            foundation::pv_util::CreateValue((UINT32)error),
            &replaced));
        
        if (status == foundation::ResultStatus::Completed)
        {
            foundation::InspectablePtr spResult;
            IFR_ASSERT(spAsAsyncOperation->GetResult(foundation::_InfiniteTimeout,spResult.GetAddressOf()));

            foundation::InspectablePtr spResultObject;
            IFHR_ASSERT(IInspectableToObject(spResult, pObjectSerializerCallback, spResultObject.GetAddressOf()));

            if (bAddToDictionary)
            {
                IFHR_ASSERT(pObjectDictionary->Insert(
                    foundation::HStringPtr(_KEY_ASYNC_OPERATION_RESULT),
                    spResultObject,
                    &replaced));
            }
        }
    }
    else if (SUCCEEDED(foundation::QueryInterface(pObject, spAsPropertyData.GetAddressOf())))
    {
        // get all the public properties
        foundation::ArrayRefCountWrapper<foundation::IPropertyIdPair> arrayProperties;
        IFHR_ASSERT(spAsPropertyData->GetProperties(
            foundation::pv_util::CreateValue(static_cast<UINT32>(foundation::InstanceFlags::All)),
            arrayProperties.GetSizeAddressOf(),
            arrayProperties.GetBufferAddressOf()));

        foundation::ComPtr<foundation::IObjectDispatchInfo> spPropertyDataInfo;
        IFHR_ASSERT(foundation::GetObjectTypeInfo(spAsPropertyData, spPropertyDataInfo.GetAddressOf()));
            
        foundation::ComPtr<foundation::IDictionary>                  spProperties;
        if(bAddToDictionary)
        {
            IFHR_ASSERT(foundation::library::CreateDictionaryClass(spProperties.GetAddressOf()));
        }

        for(UINT32 index = 0;index < arrayProperties.GetSize();++index)
        {
            UINT32 propertyId;
            arrayProperties[index]->GetKey(&propertyId);

            foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
            IFHR_ASSERT(spPropertyDataInfo->GetPropertyById(propertyId,spPropertyInfo.GetAddressOf()));
            foundation::HStringPtr key;
            IFHR_ASSERT(spPropertyInfo->GetName(key.GetAddressOf()));

            foundation::InspectablePtr spValue;
            arrayProperties[index]->GetValue(spValue.GetAddressOf());

            foundation::InspectablePtr spObject;
            IFHR_ASSERT(IInspectableToObject(spValue,pObjectSerializerCallback,spObject.GetAddressOf()));

            if(bAddToDictionary)
            {
                IFHR_ASSERT(spProperties->Insert(
                    key,
                    spObject,
                    &replaced));
            }
        }
        if(bAddToDictionary)
        {
            IFHR_ASSERT(pObjectDictionary->Insert(
                foundation::HStringRef(_KEY_PROPERTIES),
                spProperties,&replaced));
        }
    }
    else
    {
        foundation_assert(false);
        return E_UNEXPECTED;
    }
    return S_OK;
}
//------------------------------------------------------------------------------
// ObjectFromDictionary
// Helper to traverse an IObject content from an IDictionary
//------------------------------------------------------------------------------
static HRESULT ObjectFromDictionary(
    _In_ foundation::IObject        *pObject,
    _In_ foundation::IDictionary    *pObjectDictionary,
    _In_ IObjectDeserializerCallback *pObjectDeserializerCallback)
{
    foundation_assert(pObject);
    foundation_assert(pObjectDictionary);
    foundation_assert(pObjectDeserializerCallback);
    foundation::ComPtr<foundation::IObjectDispatch> spAsPropertyData;
    foundation::ComPtr<IObservableList> spAsListModel;
    foundation::ComPtr<ICommand> spAsCommandModel;

    foundation::InspectablePtr spValue;
    HRESULT hr;

    if (SUCCEEDED(foundation::QueryInterface(pObject, spAsListModel.GetAddressOf())))
    {
        IFHR_ASSERT(pObjectDictionary->Lookup(foundation::HStringRef(_KEY_ITEMS),spValue.GetAddressOf()));
        foundation::InspectableArrayWrapper itemsArrayWrapper;
        IFHR_ASSERT(foundation::pv_util::GetInspectableArray(spValue,
            itemsArrayWrapper.GetSizeAddressOf(),
            itemsArrayWrapper.GetBufferAddressOf()));

        for(UINT32 index = 0;index < itemsArrayWrapper.GetSize(); ++index)
        {
            foundation::InspectablePtr spItem;
            IFR_ASSERT(ObjectToInspectable(itemsArrayWrapper[index],pObjectDeserializerCallback,spItem.GetAddressOf()));
            IFR_ASSERT(spAsListModel->AppendItem(spItem));
        }
    }
    else if (SUCCEEDED(foundation::QueryInterface(pObject, spAsCommandModel.GetAddressOf())))
    {
        // TODO: deserialize current command state ?
    }
    else if (SUCCEEDED(foundation::QueryInterface(pObject, spAsPropertyData.GetAddressOf())))
    {
        IFHR_ASSERT(pObjectDictionary->Lookup(foundation::HStringRef(_KEY_PROPERTIES),spValue.GetAddressOf()));

        foundation::ComPtr<foundation::IObjectDispatchInfo> spPropertyDataInfo;
        IFR_ASSERT(foundation::GetObjectTypeInfo(spAsPropertyData, spPropertyDataInfo.GetAddressOf()));

        foundation::ComPtr<foundation::IDictionary> spProperties;
        IFHR_ASSERT(foundation::QueryInterface(spValue,spProperties.GetAddressOf()));

        foundation::ArrayRefCountWrapper<foundation::IDictionaryPair> arrayProperties;
        IFHR_ASSERT(spProperties->GetView(arrayProperties.GetSizeAddressOf(),arrayProperties.GetBufferAddressOf()));
        for(UINT32 index = 0;index < arrayProperties.GetSize(); ++index)
        {
            foundation::HStringPtr key;
            arrayProperties[index]->GetKey(key.GetAddressOf());

            foundation::ComPtr<foundation::IPropertyInfo> spPropertyInfo;
            IFHR_ASSERT(spPropertyDataInfo->GetPropertyByName(key,spPropertyInfo.GetAddressOf()));

            UINT32 flags;
            IFHR_ASSERT(spPropertyInfo->GetFlags(&flags));
            if( (flags & foundation::PropertyFlagType::CanWrite) == 0)
            {
                continue;
            }
            UINT32 propertyId;
            IFHR_ASSERT(spPropertyInfo->GetId(&propertyId));

            foundation::InspectablePtr spObject;
            arrayProperties[index]->GetValue(spObject.GetAddressOf());

            foundation::InspectablePtr spValue;
            IFR_ASSERT(ObjectToInspectable(spObject,pObjectDeserializerCallback,spValue.GetAddressOf()));
            IFR_ASSERT(spAsPropertyData->SetProperty(propertyId,spValue));
        }
    }
    return S_OK;
}
//------------------------------------------------------------------------------
// Class : CModelHierarchySerializer
//------------------------------------------------------------------------------
class CModelHierarchySerializer :
    public foundation::ctl::ComBase,
    public IObjectSerializerCallback
{
public:
    static HRESULT CreateInstance(
        IObjectSerializerCallback *pSerializerCallback,
        ISerializeReferenceCallback *pSerializeReferenceCallback,
        CModelHierarchySerializer **ppHierarchyJsonSerilaizer)
    {
        foundation_assert(pSerializerCallback);

        foundation::ctl::ComObject<CModelHierarchySerializer>::CreateInstance(ppHierarchyJsonSerilaizer);
        (*ppHierarchyJsonSerilaizer)->Initialize(pSerializerCallback,pSerializeReferenceCallback);
        return S_OK;
    }

    HRESULT AddReferenceModels(foundation::IDictionary *pParentObject)
    {
        foundation_assert(pParentObject);

        _Inspectable_ptr_vector_type objArray;
        objArray.resize(_referenceObjects.size());

        UINT32 index = 0;
        for (_ObjectMapType::iterator iter = _referenceObjects.begin();
            iter != _referenceObjects.end();
            ++iter)
        {
            objArray[index++] = (*iter).second;
        }
        foundation::InspectablePtr spReferenceModelsArray;
        IFR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
            (UINT32)objArray.size(),
            objArray.size() ? objArray.front().GetAddressOfPtr():nullptr,
            spReferenceModelsArray.GetAddressOf()));

        bool bReplaced;
        IFR_ASSERT(pParentObject->Insert(
             foundation::HStringRef(_KEY_REFERENCE_MODELS),
             spReferenceModelsArray,
             &bReplaced));
        return S_OK;
    }

protected:
    CModelHierarchySerializer():
        _currentLevel(0)
    {
    }

    HRESULT Initialize(IObjectSerializerCallback *pSerializerCallback,ISerializeReferenceCallback *pSerializeReferenceCallback)
    {
        this->_spSerializerCallback = pSerializerCallback;
        this->_spSerializeReferenceCallback = pSerializeReferenceCallback;
        return S_OK;
    }

    // Interface IObjectSerializerCallback
    STDMETHOD(Invoke)(
            _In_ foundation::IInspectable *pInspectable,
            _COM_Outptr_ foundation::IDictionary **ppDictionary) override
    {
        HRESULT hr;
        // if an IObject inspect the hierarchy
        foundation::ComPtr<foundation::IObject> spAsObject;
        if (SUCCEEDED(foundation::QueryInterface(pInspectable, spAsObject.GetAddressOf())))
        {
            // default serialization for an Object without type info
            IFHR_ASSERT(CDefaultObjectSerializer::SerializeObjectInternal(
                    spAsObject,
                    false, 
                    ppDictionary));

            ++_currentLevel;
            // continue with deep serialization
            HRESULT hr = SerializeReferenceObject(spAsObject);
            --_currentLevel;
            IFHR(hr);
        }
        else
        {
            // delegate to object serialization
            IFHR_ASSERT(_spSerializerCallback->Invoke(pInspectable, ppDictionary));
        }
        return hr;
    }

private:
    bool IsInObjectReferences(_In_ foundation::IObject *pObject)
    {
        return (_referenceObjects.find(pObject) != _referenceObjects.end());
    }

    HRESULT SerializeReferenceObject(_In_ foundation::IObject *pObject)
    {
        foundation_assert(pObject);
        if (IsInObjectReferences(pObject))
        {
            // indicate Object was already serialized
            return S_FALSE;
        }

        // next two options will define how to continue with the 
        // object model inspection
        bool bAddToDictionary = true;
        bool bContinue = true;
        bool bAddReferenceModel = false;

        HRESULT hr;

        UINT32 modelType;
        IFHR_ASSERT(CDefaultObjectSerializer::GetObjectTypeInfoType(pObject,modelType));

        foundation::ComPtr<foundation::IDictionary>   spObjectDictionary;
        if (modelType == foundation::TypeInfo_PropertyData)
        {
            // we serialize the IObjectDispatch entity 
            IFHR_ASSERT(CDefaultObjectSerializer::SerializeObjectInternal(
                pObject,
                true,
                spObjectDictionary.GetAddressOf()));
        }
        else
        {
            // serialize using the callback
            IFHR_ASSERT(_spSerializerCallback->Invoke(pObject, spObjectDictionary.GetAddressOf()));
            bAddReferenceModel = (hr == S_OK); // != S_FALSE in case it was serialized before

            if (_spSerializeReferenceCallback != nullptr)
            {
                IFHR_ASSERT(_spSerializeReferenceCallback->Invoke(
                    spObjectDictionary,
                    pObject,
                    this->_currentLevel,
                    &bAddToDictionary,
                    &bContinue));
            }
        }

        if(bAddReferenceModel || bContinue)
        {
            // cache this object to avoid future serialization
            _referenceObjects[pObject] = spObjectDictionary;
        }

        if(bContinue)
        {
            return ObjectToDictionary(pObject, this, spObjectDictionary, bAddToDictionary);
        }
        return S_OK;
    }
private:
    foundation::ComPtr<IObjectSerializerCallback>     _spSerializerCallback;
    foundation::ComPtr<ISerializeReferenceCallback>   _spSerializeReferenceCallback;
    
    _ObjectMapType                                _referenceObjects;
    UINT32                                        _currentLevel;
};

//------------------------------------------------------------------------------
// ObjectDeserializeFrom
// Helper to deserialize an object in a raw format
//------------------------------------------------------------------------------
static HRESULT ObjectDeserializeFrom(
    _Inout_ foundation::IInspectable **ppInspectable,
    _In_ IObjectDeserializerCallback *pModelDeserializer)
{
    foundation_assert(ppInspectable);
    foundation_assert(*ppInspectable);
    HRESULT hr = S_FALSE;

    foundation::ComPtr<foundation::IDictionary> spAsCoreDictionary;
    if(SUCCEEDED(foundation::QueryInterface(*ppInspectable,spAsCoreDictionary.GetAddressOf())))
    {
        // navigate the properties
        foundation::ArrayRefCountWrapper<foundation::IDictionaryPair> arrayProperties;
        IFHR_ASSERT(spAsCoreDictionary->GetView(arrayProperties.GetSizeAddressOf(),arrayProperties.GetBufferAddressOf()));
        for(UINT32 index = 0;index < arrayProperties.GetSize();++index)
        {
            foundation::HStringPtr key;
            arrayProperties[index]->GetKey(key.GetAddressOf());
            foundation::InspectablePtr spValue;
            arrayProperties[index]->GetValue(spValue.GetAddressOf());
            if(spValue != nullptr)
            {
                IFHR_ASSERT(ObjectDeserializeFrom(spValue.GetAddressOfPtr(),pModelDeserializer));
                if(hr == S_OK)
                {
                    bool replaced;
                    IFHR_ASSERT(spAsCoreDictionary->Insert(key,spValue,&replaced));
                }
            }
        }
        // attempt to convert
        (*ppInspectable)->Release();
        *ppInspectable = nullptr;
        IFHR_ASSERT(pModelDeserializer->Invoke(spAsCoreDictionary,ppInspectable));
    }
    else
    {
        foundation::PropertyType propertyType;
        IFR_ASSERT(foundation::pv_util::GetType(*ppInspectable, &propertyType));
        if (propertyType == foundation::PropertyType_InspectableArray)
        {
            _Inspectable_ptr_vector_type objectsDeserialized;

            foundation::InspectableArrayWrapper arrayInspectableWrapper;
            IFHR_ASSERT(foundation::pv_util::GetInspectableArray(
                *ppInspectable,
                arrayInspectableWrapper.GetSizeAddressOf(),
                arrayInspectableWrapper.GetBufferAddressOf()));
            for(UINT32 index = 0;index < arrayInspectableWrapper.GetSize();++index)
            {
                foundation::InspectablePtr spValue = arrayInspectableWrapper[index];
                if(spValue != nullptr) 
                {
                    IFHR_ASSERT(ObjectDeserializeFrom(spValue.GetAddressOfPtr(),pModelDeserializer));
                }
                objectsDeserialized.push_back(spValue);
            }
            // create a new array with deserialized values
            (*ppInspectable)->Release();
            *ppInspectable = nullptr;
            IFHR_ASSERT(foundation::pv_util::CreateInspectableArrayValue(
                    (UINT32)objectsDeserialized.size(),
                    objectsDeserialized.size() ? objectsDeserialized.front().GetAddressOfPtr():nullptr,
                    ppInspectable));
        }
    }
    return hr;
}


IObjectStatics *CObjectStatics::GetInstance()
{
    return foundation::library::_GetFactoryInstance<CObjectStatics, IObjectStatics>();
}

HRESULT CObjectStatics::GetOrCreateInstance(IObjectStatics **ppObjectStatics)
{
    IFCPTR(ppObjectStatics);
    *ppObjectStatics = GetInstance();
    (*ppObjectStatics)->AddRef();
    return S_OK;
}

CObjectStatics::~CObjectStatics()
{
}

STDMETHODIMP CObjectStatics::ToObject(
        _In_ foundation::IInspectable *pInspectable,
        _In_opt_ IObjectSerializerCallback *pObjectSerializerCallback,
        _COM_Outptr_ foundation::IInspectable **ppObject)
{
    return IInspectableToObject(pInspectable,pObjectSerializerCallback,ppObject);
}

STDMETHODIMP CObjectStatics::FromObject(
        _In_ foundation::IInspectable *pObject,
        _In_opt_ IObjectDeserializerCallback *pObjectDeserializerCallback,
        _COM_Outptr_ foundation::IInspectable **ppInspectable)
{
    return ObjectToInspectable(pObject,pObjectDeserializerCallback,ppInspectable);
}


STDMETHODIMP CObjectStatics::ObjectDeserializeFrom(
             _Inout_ foundation::IInspectable **ppInspectable,
            _In_ IObjectDeserializerCallback *pObjectDeserializerCallback)
{
    foundation_assert(pObjectDeserializerCallback);
    IFCPTR(pObjectDeserializerCallback);
    return ::ObjectDeserializeFrom(ppInspectable,pObjectDeserializerCallback);
}

STDMETHODIMP CObjectStatics::CreateDefaultObjectSerializer(
        _In_ IObjectSerializerCallback **ppObjectSerializer)
{
    return CDefaultObjectSerializer::CreateInstance(true,ppObjectSerializer);
}

STDMETHODIMP CObjectStatics::CreateDefaultObjectDeserializer(
        _In_ IObjectResolverCallback *pModelresolverCallback,
        _In_ IObjectDeserializerCallback **ppObjectDeserializer)
{
    foundation_assert(pModelresolverCallback);
    IFCPTR(pModelresolverCallback);
    CDefaultObjectDeserializer *pObjectDeserializer = nullptr;
    foundation::ctl::ComObject<CDefaultObjectDeserializer>::CreateInstance(&pObjectDeserializer);
    pObjectDeserializer->_spModelResolverCallback = pModelresolverCallback;
    *ppObjectDeserializer = static_cast<IObjectDeserializerCallback *>(pObjectDeserializer);
    return S_OK;
}

STDMETHODIMP CObjectStatics::CreateFactoryObjectResolverCallback(
        _In_ IObjectResolverCallback **ppModelResolverCallback)
{
    foundation_assert(ppModelResolverCallback);
    IFCPTR(ppModelResolverCallback);
    CFactoryModelResolverCallback *pFactoryModelResolverCallback = nullptr;
    foundation::ctl::ComObject<CFactoryModelResolverCallback>::CreateInstance(&pFactoryModelResolverCallback);
    *ppModelResolverCallback = static_cast<IObjectResolverCallback *>(pFactoryModelResolverCallback);
    return S_OK;
}

STDMETHODIMP CObjectStatics::HierarchyToObject(
            _In_ foundation::IInspectable *pInspectable,
            _In_ IObjectSerializerCallback *pObjectSerializerCallback,
            _In_opt_ ISerializeReferenceCallback *pSerializeReferenceCallback,
            _COM_Outptr_ foundation::IInspectable **ppObject)
{
    foundation_assert(pInspectable);
    IFCPTR(pInspectable);
    foundation_assert(pObjectSerializerCallback);
    IFCPTR(pObjectSerializerCallback);
    foundation_assert(ppObject);
    IFCPTR(ppObject);

    CModelHierarchySerializer *pModelHierarchySerializer = nullptr;
    CModelHierarchySerializer::CreateInstance(pObjectSerializerCallback,pSerializeReferenceCallback,&pModelHierarchySerializer);

    foundation::ComPtr<foundation::IUnknown> spUnk;
    spUnk.Attach(static_cast<foundation::ctl::ComBase *>(pModelHierarchySerializer));

    foundation::InspectablePtr spObject;

    HRESULT hr;
    IFHR_ASSERT(IInspectableToObject(pInspectable,pModelHierarchySerializer,spObject.GetAddressOf()));
    
    // TODO: for non serialzable object we will make an exception for now
    if (spObject == nullptr && hr == S_FALSE)
    {
        *ppObject = nullptr;
        return S_FALSE;
    }

    foundation_assert(spObject != nullptr);
    foundation::ComPtr<foundation::IDictionary> spObjectAsDictionary;
    if(SUCCEEDED(foundation::QueryInterface(spObject,spObjectAsDictionary.GetAddressOf())))
    {
        IFHR_ASSERT(pModelHierarchySerializer->AddReferenceModels(spObjectAsDictionary));
    }
    *ppObject = spObject.Detach();
    return S_OK;
}

STDMETHODIMP CObjectStatics::HierarchyFromObject(
    _In_ foundation::IInspectable *pObject,
    _In_ IObjectDeserializerCallback *pObjectDeserializerCallback,
    _COM_Outptr_ foundation::IInspectable **ppInspectable)
{
    foundation_assert(pObject);
    IFCPTR(pObject);
    foundation_assert(pObjectDeserializerCallback);
    IFCPTR(pObjectDeserializerCallback);

    foundation::ComPtr<foundation::IDictionary> spObjectAsDictionary;
    if(SUCCEEDED(foundation::QueryInterface(pObject,spObjectAsDictionary.GetAddressOf())))
    {
        foundation::ComPtr<CModelReferenceResolverCallback, foundation::ctl::_ComBasePtrImpl> spModelReferenceResolverCallback;
        foundation::ctl::ComObject<CModelReferenceResolverCallback>::CreateInstance(spModelReferenceResolverCallback.GetAddressOf());

        std::vector<foundation::IObject *> referenceModels;

        foundation::InspectablePtr spValue;
        IFR_ASSERT(spObjectAsDictionary->Lookup(
             foundation::HStringRef(_KEY_REFERENCE_MODELS),
             spValue.GetAddressOf()));

        foundation::InspectableArrayWrapper arrayModelsWrapper;
        IFR_ASSERT(foundation::pv_util::GetInspectableArray(spValue,
            arrayModelsWrapper.GetSizeAddressOf(),
            arrayModelsWrapper.GetBufferAddressOf()));
        // deserialize models without any internal content
        for(UINT32 index = 0;index < arrayModelsWrapper.GetSize();++index)
        {
            foundation::ComPtr<foundation::IDictionary> spModelInfoMap;
            IFR_ASSERT(foundation::QueryInterface(arrayModelsWrapper[index],spModelInfoMap.GetAddressOf()));
            foundation::ComPtr<foundation::IObject> spReferenceModel;
            IFR_ASSERT(pObjectDeserializerCallback->Invoke(spModelInfoMap,(foundation::IInspectable **)spReferenceModel.GetAddressOf()));

            foundation_assert(spReferenceModel != nullptr);
            UINT64 uniqueId;
            foundation::InspectablePtr spUniqueId;
            IFR_ASSERT(spModelInfoMap->Lookup(foundation::HStringRef(_KEY_ID),spUniqueId.GetAddressOf()));
            IFR_ASSERT(foundation::pv_util::GetValue(spUniqueId, &uniqueId));

            spModelReferenceResolverCallback->GetObjectReferences()[uniqueId] = spReferenceModel;
            referenceModels.push_back(spReferenceModel);
        }

        // create a Model deserializer that will resolve using our map references models
        foundation::ComPtr<IObjectDeserializerCallback>    spObjectDeserializerCallback;
        IFR_ASSERT(CreateDefaultObjectDeserializer(
            spModelReferenceResolverCallback,
            spObjectDeserializerCallback.GetAddressOf()));

        for(UINT32 index = 0;index < arrayModelsWrapper.GetSize();++index)
        {
            foundation::ComPtr<foundation::IDictionary> spModelInfoMap;
            IFR_ASSERT(foundation::QueryInterface(arrayModelsWrapper[index],spModelInfoMap.GetAddressOf()));

            IFR_ASSERT(::ObjectFromDictionary(referenceModels[index],spModelInfoMap,spObjectDeserializerCallback));
        }
        return ObjectToInspectable(pObject,spObjectDeserializerCallback,ppInspectable);
    }
    *ppInspectable = pObject;
    (*ppInspectable)->AddRef();
    return S_OK;
}

STDMETHODIMP CObjectStatics::ObjectToDictionary(
        _In_ foundation::IObject *pObject,
        _In_ IObjectSerializerCallback *pObjectSerializerCallback,
        _In_ foundation::IDictionary *pObjectDictionary)
{
    IFCPTR_ASSERT(pObject);
    IFCPTR_ASSERT(pObjectSerializerCallback);
    IFCPTR_ASSERT(pObjectDictionary);
    return ::ObjectToDictionary(pObject, pObjectSerializerCallback, pObjectDictionary, true);
}

HRESULT CObjectStatics::ActivateDefaultObject(
    REFIID iidType,
    _In_ foundation::IInspectable *pContextParameter,
    _COM_Outptr_ foundation::IObject **ppObject)
{
    foundation::ComPtr<foundation::IObjectTypeInfo> spTypeInfo;
    IFR(foundation::GetTypeInfo(iidType, spTypeInfo.GetAddressOf()));

    UINT32 modelOptions = 0;
    if (pContextParameter)
    {
        foundation::PropertyType propertyType;
        foundation::pv_util::GetType(pContextParameter, &propertyType);
        if (propertyType == foundation::PropertyType_UInt32 || propertyType == foundation::PropertyType_Int32)
        {
            foundation::pv_util::GetValue(pContextParameter, &modelOptions);
        }
    }

    if (foundation::IsTypeOf<foundation::IEnumerableTypeInfo>(spTypeInfo))
    {
        IFR_ASSERT(CreateObservableCollection(
            iidType,
            (ObservableCollectionOptions)modelOptions,
            ppObject));
    }
    else if (foundation::IsTypeOf<ICommandInfo>(spTypeInfo))
    {
        IFR_ASSERT(CreateCommand(
            iidType,
            (CommandOptions)modelOptions,
            ppObject));
    }
    else if (foundation::IsTypeOf<IObservableObjectInfo>(spTypeInfo))
    {
        IFR_ASSERT(CreateObservableObject(
            IsEqualIID(iidType, IID_IObservableObject) ? foundation_GUID_NULL : iidType,
            (ObservableObjectOptions)((int)ObservableObjectOptions::InitializeProperties | modelOptions),
            ppObject));
    }
    else if (foundation::IsTypeOf<foundation::IObjectDispatchInfo>(spTypeInfo))
    {
        IFR_ASSERT(foundation::library::CreateObjectDispatch(
            iidType,
            (foundation::ObjectDispatchOptions)((int)foundation::ObjectDispatchOptions::None | modelOptions),
            ppObject));
    }
    else
    {
        foundation_assert(false);
        return E_NOTIMPL;
    }
    return S_OK;
}

STDMETHODIMP CObjectStatics::IsObjectEquals(foundation::IObject *pObject1, foundation::IObject *pObject2, bool *pResult)
{
    return IsObjectEqualsInternal(pObject1, pObject2, pResult);
}

class _RegisterActivationFactory : public foundation::library::_RegisterActivationFactory
{
public:
    _RegisterActivationFactory():
        foundation::library::_RegisterActivationFactory(
            U("pmod.ObjectStatics"),
            reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObjectStatics::GetInstance)
            )
    {
        GetObjectFactoryInstance()->SetActivateDefaultObjectCallback(&CObjectStatics::ActivateDefaultObject);
    }
};

static _RegisterActivationFactory _serviceRegister;
