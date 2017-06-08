/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectStatics.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <pmod/library/model_library.h>
#include <pmod/library/object_statics_keys.h>
#include <foundation/com_ptr.h>

class CObjectStatics :
    public foundation::ctl::ImplementsInspectable<pmod::library::IObjectStatics,&pmod::library::IID_IObjectStatics>
{
 public:
    static pmod::library::IObjectStatics *GetInstance();
    static HRESULT GetOrCreateInstance(pmod::library::IObjectStatics **ppObjectStatics);

    static HRESULT ActivateDefaultObject(
        REFIID iidType,
        _In_ foundation::IInspectable *pContextParameter,
        _COM_Outptr_ foundation::IObject **ppObject);
protected:
    ~CObjectStatics();

    // Verify if two items are Equal
    STDMETHOD(IsObjectEquals)(foundation::IObject *pObject1, foundation::IObject *pObject2, bool *pResult);

    // To/From for pInspectable
    STDMETHOD(ToObject)(
        _In_ foundation::IInspectable *pInspectable,
        _In_ pmod::library::IObjectSerializerCallback *pObjectSerializerCallback,
        _COM_Outptr_ foundation::IInspectable **ppObject);
    STDMETHOD(FromObject)(
        _In_ foundation::IInspectable *pObject,
        _In_ pmod::library::IObjectDeserializerCallback *pObjectDeserializerCallback,
        _COM_Outptr_ foundation::IInspectable **ppInspectable);

    // convert a hierarchy to/from to Object
    STDMETHOD(HierarchyToObject)(
        _In_ foundation::IInspectable *pInspectable,
        _In_ pmod::library::IObjectSerializerCallback *pObjectSerializerCallback,
        _In_opt_ pmod::library::ISerializeReferenceCallback *pSerializeReferenceCallback,
        _COM_Outptr_ foundation::IInspectable **ppObject);
    STDMETHOD(HierarchyFromObject)(
        _In_ foundation::IInspectable *pObject,
        _In_ pmod::library::IObjectDeserializerCallback *pObjectDeserializerCallback,
        _COM_Outptr_ foundation::IInspectable **ppInspectable);

    // deserialize a 'raw' Object using a Deserializer callback
    STDMETHOD(ObjectDeserializeFrom)(
            _Inout_ foundation::IInspectable **ppInspectable,
        _In_ pmod::library::IObjectDeserializerCallback *pObjectDeserializerCallback);

    // Inspect a IObjectNode and add content to a dictionary
    STDMETHOD(ObjectToDictionary)(
        _In_ foundation::IObject *pObject,
        _In_ pmod::library::IObjectSerializerCallback *pObjectSerializerCallback,
        _In_ foundation::IDictionary *pObjectDictionary);

    // Factory method to create default serializer/deserializer callbacks
    STDMETHOD(CreateDefaultObjectSerializer)(
        _In_ pmod::library::IObjectSerializerCallback **ppObjectSerializer);

    STDMETHOD(CreateDefaultObjectDeserializer)(
        _In_ pmod::library::IObjectResolverCallback *ppModelResolverCallback,
        _In_ pmod::library::IObjectDeserializerCallback **ppObjectDeserializer);

    STDMETHOD(CreateFactoryObjectResolverCallback)(
        _In_ pmod::library::IObjectResolverCallback **ppModelResolverCallback);
protected:

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::library::IObjectStatics::GetIID())
        {
            *ppInterface = static_cast<pmod::library::IObjectStatics *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

};


