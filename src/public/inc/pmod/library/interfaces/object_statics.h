/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_statics.h
****/
#pragma once

#include <foundation/interfaces/dictionary.h>
#include <foundation/interfaces/object.h>
#include "../pmod_lib_api.h"

namespace pmod {

    namespace library {
        ////////////////////////////////// Model Serializer/Deserializer ///////////////////////////////////
        // resolve an Object 
        BEGIN_DECLARE_INTERFACE(IObjectResolverCallback, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(UINT64 id, GUID iidModelType, foundation::IObject **ppObject) = 0;
        END_DECLARE_INTERFACE()

        // Serialize an Object into a Core Dictionary
        BEGIN_DECLARE_INTERFACE(IObjectSerializerCallback, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(
                _In_ foundation::IInspectable *pObject,
                _COM_Outptr_ foundation::IDictionary **ppCoreDictionary) = 0;
        END_DECLARE_INTERFACE()

        // Deserialize an Object from a Dictionary
        BEGIN_DECLARE_INTERFACE(IObjectDeserializerCallback, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(
                _In_ foundation::IDictionary *pDictionary,
                _COM_Outptr_ foundation::IInspectable **ppModel) = 0;
        END_DECLARE_INTERFACE()

        // Serialize reference model callback
        BEGIN_DECLARE_INTERFACE(ISerializeReferenceCallback, foundation::IUnknown, PMOD_LIB_API)
            STDMETHOD(Invoke)(
                _In_ foundation::IDictionary *pDictionary,
                _In_ foundation::IObject *pObject,
                UINT32 currentLevel,
                bool *bAddToDictionary,
                bool *bContinue) = 0;
        END_DECLARE_INTERFACE()

            /////////////////////////////////// Object Statics //////////////////////////////////////////////
        BEGIN_DECLARE_INTERFACE(IObjectStatics, foundation::IUnknown, PMOD_LIB_API)
            // Verify if two items are Equal
            STDMETHOD(IsObjectEquals)(foundation::IObject *pObject1, foundation::IObject *pObject2, bool *pResult) = 0;

            // To/From <-> Object for pInspectable
            STDMETHOD(ToObject)(
                _In_ foundation::IInspectable *pInspectable,
                _In_opt_ IObjectSerializerCallback *pObjectSerializerCallback,
                _COM_Outptr_ foundation::IInspectable **ppObject) = 0;
            STDMETHOD(FromObject)(
                _In_ foundation::IInspectable *pObject,
                _In_opt_ IObjectDeserializerCallback *pObjectDeserializerCallback,
                _COM_Outptr_ foundation::IInspectable **ppInspectable) = 0;

            // deep hierarchy to/from <-> Object
            STDMETHOD(HierarchyToObject)(
                _In_ foundation::IInspectable *pInspectable,
                _In_opt_ IObjectSerializerCallback *pObjectSerializerCallback,
                _In_opt_ ISerializeReferenceCallback *pSerializeReferenceCallback,
                _COM_Outptr_ foundation::IInspectable **ppObject) = 0;
            STDMETHOD(HierarchyFromObject)(
                _In_ foundation::IInspectable *pObject,
                _In_opt_ IObjectDeserializerCallback *pObjectDeserializerCallback,
                _COM_Outptr_ foundation::IInspectable **ppInspectable) = 0;


            // deserialize a 'raw' Object using a Deserializer callback
            STDMETHOD(ObjectDeserializeFrom)(
                _Inout_ foundation::IInspectable **ppInspectable,
                _In_ IObjectDeserializerCallback *pObjectDeserializerCallback) = 0;

            // Inspect a IObjectNode and add content to a dictionary
            STDMETHOD(ObjectToDictionary)(
                _In_ foundation::IObject *pObject,
                _In_ IObjectSerializerCallback *pObjectSerializerCallback,
                _In_ foundation::IDictionary *pObjectDictionary) = 0;

            // Factory method to create default serializer/deserializer callbacks
            STDMETHOD(CreateDefaultObjectSerializer)(
                _In_ IObjectSerializerCallback **ppModelSerializer) = 0;

            STDMETHOD(CreateDefaultObjectDeserializer)(
                _In_ IObjectResolverCallback *ppModelResolverCallback,
                _In_ IObjectDeserializerCallback **ppModelDeserializer) = 0;

            STDMETHOD(CreateFactoryObjectResolverCallback)(
                _In_ IObjectResolverCallback **ppModelResolverCallback) = 0;

        END_DECLARE_INTERFACE()
    }

}


