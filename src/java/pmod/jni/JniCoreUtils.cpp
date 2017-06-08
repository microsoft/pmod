/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JniCoreUtils.cpp
****/
#include "JniCoreUtils.h"

#include "foundation/pal.h"
#include "foundation/string_t.h"
#include "foundation/type_info_statics_util.h"

#include <stdlib.h>
#include <string>


#define INTERFACE_FLD "_interface"

using namespace foundation;
using namespace pmod;

static void swapUInt16(UINT8 *p)
{
    UINT16* p_dest = reinterpret_cast< UINT16* >(p);
    UINT16 const* const p_src = reinterpret_cast< UINT16 const* >(p);
    *p_dest = (*p_src >> 8) | (*p_src << 8);
}

static void swapUInt32(UINT8 *p)
{
    UINT32* p_dest = reinterpret_cast< UINT32* >(p);
    UINT32 const* const p_src = reinterpret_cast< UINT32 const* >(p);
    *p_dest = (*p_src >> 24) | ((*p_src & 0x00ff0000) >> 8) | ((*p_src & 0x0000ff00) << 8) | (*p_src << 24);
}

jobject createInspectableObject(JNIEnv *env, jclass jClass, foundation::IUnknown *pInterface)
{
    foundation_assert(pInterface);
    foundation_assert(jClass);

    // Note: we perform an AddRef since we are assuming the underlying java object will perform
    // a Release when disposed or finalized by the GC
    pInterface->AddRef();

    // call the constructor that takes long value
    jmethodID ctor =
        env->GetMethodID(jClass, "<init>", "(J)V");
    jobject jobject = env->NewObject(jClass, ctor, (jlong)pInterface);
    foundation_assert(jobject);
    return jobject;
}

jobject createInspectableObject(JNIEnv *env, const char* className, foundation::IUnknown *pInterface)
{
    foundation_assert(pInterface);

    jclass jClass = env->FindClass(className);
    foundation_assert(jClass);
    if (!jClass)
    {
        return nullptr;
    }
    return createInspectableObject(env, jClass, pInterface);
}

jobject createPropertyValue(JNIEnv *env, foundation::IPropertyValue *pPropertyValue)
{
    foundation_assert(pPropertyValue);

    jclass jClass = env->FindClass("com/microsoft/pmod/PropertyValue");
    foundation_assert(jClass);
    if (!jClass)
    {
        return nullptr;
    }
    pPropertyValue->AddRef();

    // call the constructor that takes long value
    jmethodID methodId =
        env->GetStaticMethodID(jClass, "createPropertyValue", "(J)Lcom/microsoft/pmod/PropertyValue;");
    jobject jobject = env->CallStaticObjectMethod(jClass, methodId, (jlong)pPropertyValue);
    foundation_assert(jobject);
    return jobject;
}

jobject createPropertyValue(JNIEnv *env, foundation::IUnknown *pInterface)
{
    if (pInterface == nullptr)
    {
        return (jobject)nullptr;
    }
    foundation::IPropertyValue *pPropertyValue;
    foundation::QueryWeakReference(pInterface, IID_IPropertyValue, &pPropertyValue);
    foundation_assert(pPropertyValue != nullptr);
    return createPropertyValue(env, pPropertyValue);
}

void ThrowComFail(JNIEnv *env, HRESULT hr,const char* desc)
{
    jclass failClass = env->FindClass("com/microsoft/pmod/ComException");
    // call the constructor that takes hr and message
    jmethodID failCons = 
        env->GetMethodID(failClass, "<init>", "(ILjava/lang/String;)V");
    if (!desc) desc = "Java/COM Error";
    jstring js = env->NewStringUTF(desc);
    jthrowable fail = (jthrowable)env->NewObject(failClass, failCons,(jint) hr, js);
    env->Throw(fail);
}

foundation::IInspectable *extractInterface(JNIEnv *env, jobject arg)
{
    jclass argClass = env->GetObjectClass(arg);
    jfieldID ajf = env->GetFieldID( argClass, INTERFACE_FLD, "J");
    jlong anum = env->GetLongField(arg, ajf);
    foundation::IInspectable *pInterface = (foundation::IInspectable *)anum;
    return pInterface;
}

foundation::IInspectable *extractInterfaceExpected(JNIEnv *env, jobject arg)
{
    foundation::IInspectable *pInterface = extractInterface(env,arg);
    if(pInterface == nullptr)
    {
        ThrowComFail(env,E_POINTER,"No Interface");
    }
    return pInterface;
}

void initInterface(JNIEnv *env, jobject _this,foundation::IInspectable *pInterface)
{
    jclass clazz = env->GetObjectClass(_this);
    jfieldID jf = env->GetFieldID( clazz, INTERFACE_FLD, "J");
    env->SetLongField(_this, jf, (UINT64)pInterface);
}

static void toHexBytes(std::string& os,UINT8 *&pIIdType,unsigned int nbytes)
{
    const char * hex = "0123456789abcdef";

    while(nbytes--)
    {
        os += hex[(*pIIdType>>4)&0xF];
        os += hex[(*pIIdType)&0xF];
        ++pIIdType;
    }
}

jstring toString(JNIEnv *env,GUID iidType)
{
    UINT8 *pIIdType = (UINT8 *)&iidType;

    std::string os;

    // first 4 bytes
    swapUInt32(pIIdType);
    toHexBytes(os,pIIdType,4);
    os += "-";
    // next 2 bytes
    swapUInt16(pIIdType);
    toHexBytes(os,pIIdType,2);
    os += "-";
    // next 2 bytes
    swapUInt16(pIIdType);
    toHexBytes(os,pIIdType,2);
    os += "-";
    // next 2 bytes
    toHexBytes(os,pIIdType,2);
    os += "-";
    // rest 6 bytes
    toHexBytes(os,pIIdType,6);

    return env->NewStringUTF(os.c_str());
}

static void fromHexBytes(const char *&pHexStr,UINT8 *&pIIdType,unsigned int nbytes)
{
    while(nbytes--)
    {
        char buff[3];
        buff[0] = *pHexStr++;
        buff[1] = *pHexStr++;
        buff[2] = 0;

        unsigned long hexVal = strtoul(buff,nullptr,16);

        *pIIdType = (UINT8)hexVal;
        ++pIIdType;
    }
}

static void fromHexBytesToShort(const char *&pHexStr,UINT8 *&pIIdType)
{
    UINT8 *pShort = pIIdType;
    fromHexBytes(pHexStr,pIIdType,2);
    swapUInt16(pShort);
}

static void fromHexBytesToLong(const char *&pHexStr,UINT8 *&pIIdType)
{
    UINT8 *pLong = pIIdType;
    fromHexBytes(pHexStr,pIIdType,4);
    swapUInt32(pLong);
}

IID toIIDType(JNIEnv *env,jstring jstr)
{
    const char *pUtfChars = env->GetStringUTFChars(jstr, 0);

    IID iidType;
    UINT8 *pIIdType = (UINT8 *)&iidType;

    const char *pHexStr = pUtfChars;

    // first 4 bytes
    fromHexBytesToLong(pHexStr,pIIdType);
    ++pHexStr; // skip '-'
    // next 2 bytes
    fromHexBytesToShort(pHexStr,pIIdType);
    ++pHexStr; // skip '-'
    // next 2 bytes
    fromHexBytesToShort(pHexStr,pIIdType);
    ++pHexStr; // skip '-'
    // next 2 bytes
    fromHexBytes(pHexStr,pIIdType,2);
    ++pHexStr; // skip '-'
    // rest 6 bytes
    fromHexBytes(pHexStr,pIIdType,6);

    env->ReleaseStringUTFChars(jstr,pUtfChars);
    return iidType;
}

jstring toString(JNIEnv *env,HSTRING hstr)
{
#ifdef __UTF16_STRINGS
    return env->NewStringUTF(foundation::to_utf8_string(hstr).c_str());
#else
    return env->NewStringUTF(foundation::get_raw_buffer(hstr));
#endif
}

jbyteArray toStringByteArray(JNIEnv *env, HSTRING hstr)
{
    const char *string;
#ifdef __UTF16_STRINGS
    std::string utf8_str = foundation::to_utf8_string(hstr);
    string = utf8_str.c_str();
#else
    string  = foundation::get_raw_buffer(hstr);
#endif
    jbyteArray retVal = NULL;
    jbyte *buf = NULL;
    jsize len = (jsize)strlen(string);
    //The java code that converts these strings expects that an empty string
    //will only contain a null char and that a valid string will NOT contain
    //a null character.  Adding the null char to the raw string make the
    //java String class add two bas chars at the end of the converted
    //String.  The java string class expects the raw byte array to NOT
    //contain a null term.
    if (len == 0) {
        buf = new jbyte[1];
        buf[0] = '\0';
        len = 1;
    }
    else {
        buf = new jbyte[len];
        memcpy(buf, string, len);
    }
    retVal = env->NewByteArray(len);
    env->SetByteArrayRegion(retVal, 0, len, buf);
    delete[] buf;
    return retVal;
}

HSTRING toHString(JNIEnv *env,jstring jstr)
{
    const char *pUtf8Str = env->GetStringUTFChars(jstr, 0);
    size_t size = strlen(pUtf8Str);

    HSTRING hstr;
#ifdef __UTF16_STRINGS
    _pal_CreateString(foundation::to_pal_wstring(pUtf8Str).get(),(UINT32)size, &hstr);
#else
    _pal_CreateString(pUtf8Str,(UINT32)size, &hstr);
#endif

    env->ReleaseStringUTFChars(jstr,pUtf8Str);
    return hstr;
}

foundation::HStringPtr toHStringPtr(JNIEnv *env, jstring jstr)
{
    foundation::HStringPtr hstr_ptr;
    *(&hstr_ptr) = toHString(env, jstr);
    return hstr_ptr;
}

static bool isEmpty(IPropertyTypeInfo *pPropertyTypeInfo)
{
    PropertyType propertyType;
    pPropertyTypeInfo->GetPropertyType(&propertyType);
    return propertyType == PropertyType_Empty;
}

static jobject returnCommandModel(JNIEnv *env, ICommand *pCommandModel)
{
    ComPtr<ICommandInfo> spCommandModelInfo;
    foundation::GetObjectTypeInfo(pCommandModel, spCommandModelInfo.GetAddressOf());

    ComPtr<IPropertyTypeInfo> spParamTypeInfo;
    spCommandModelInfo->GetParameterTypeInfo(spParamTypeInfo.GetAddressOf());
    ComPtr<IPropertyTypeInfo> spResultTypeInfo;
    spCommandModelInfo->GetResultTypeInfo(spResultTypeInfo.GetAddressOf());

    if (isEmpty(spParamTypeInfo) && isEmpty(spResultTypeInfo))
    {
        return createInspectableObject(env, "com/microsoft/pmod/SimpleCommandImpl", pCommandModel);
    }
    else if (isEmpty(spParamTypeInfo) && !isEmpty(spResultTypeInfo))
    {
        return createInspectableObject(env, "com/microsoft/pmod/CommandResultImpl", pCommandModel);
    }
    else if (!isEmpty(spParamTypeInfo) && isEmpty(spResultTypeInfo))
    {
        return createInspectableObject(env, "com/microsoft/pmod/CommandParameterImpl", pCommandModel);
    }
    return createInspectableObject(env, "com/microsoft/pmod/CommandImpl", pCommandModel);
}

static jobject returnAsyncOperation(JNIEnv *env, foundation::IAsyncOperation *pAsyncOperation)
{
    ComPtr<IAsyncOperationTypeInfo> spAsyncOperationTypeInfo;
    foundation::GetObjectTypeInfo(pAsyncOperation, spAsyncOperationTypeInfo.GetAddressOf());
    ComPtr<IPropertyTypeInfo> spResultTypeInfo;
    spAsyncOperationTypeInfo->GetResultTypeInfo(spResultTypeInfo.GetAddressOf());

    if (isEmpty(spResultTypeInfo))
    {
        return createInspectableObject(env, "com/microsoft/pmod/BaseAsyncOperationImpl", pAsyncOperation);
    }
    return createInspectableObject(env, "com/microsoft/pmod/AsyncOperationImpl", pAsyncOperation);
}

jobject returnValue(JNIEnv *env,foundation::IInspectable *pInterface)
{
    if(pInterface == nullptr)
    {
        return (jobject)nullptr;
    }

    IPropertyValue  *pPropertyValue;
    if(SUCCEEDED(foundation::QueryWeakReference(pInterface,IID_IPropertyValue,&pPropertyValue)))
    {
        return createPropertyValue(env,pPropertyValue);
    }

    IObservableList *pListModel;
    if(SUCCEEDED(foundation::QueryWeakReference(pInterface,&pListModel)))
    {
        return createInspectableObject(env,"com/microsoft/pmod/ObservableListImpl",pListModel);
    }

    IObservableCollection *pObservableCollection;
    if(SUCCEEDED(foundation::QueryWeakReference(pInterface,&pObservableCollection)))
    {
        return createInspectableObject(env,"com/microsoft/pmod/ObservableCollectionImpl",pObservableCollection);
    }

    ICommand *pCommandModel;
    if(SUCCEEDED(foundation::QueryWeakReference(pInterface,&pCommandModel)))
    {
        return returnCommandModel(env, pCommandModel);
    }

    foundation::IAsyncOperation *pAsyncOperation;
    if (SUCCEEDED(foundation::QueryWeakReference(pInterface, &pAsyncOperation)))
    {
        return returnAsyncOperation(env, pAsyncOperation);
    }

    IObservableObject *pObservableObject;
    if(SUCCEEDED(foundation::QueryWeakReference(pInterface,&pObservableObject)))
    {
        return createInspectableObject(env,"com/microsoft/pmod/ObservableObjectImpl",pObservableObject);
    }

    IDictionary *pDictionary;
    if (SUCCEEDED(foundation::QueryWeakReference(pInterface, &pDictionary)))
    {
        return createInspectableObject(env, "com/microsoft/pmod/Dictionary", pDictionary);
    }

    // generic IObject object
    IObject *pObject;
    if (SUCCEEDED(foundation::QueryWeakReference(pInterface, &pObject)))
    {
        return createInspectableObject(env, "com/microsoft/pmod/FoundationObjectImpl", pObject);
    }

    ThrowComFail(env, E_NOINTERFACE,"Unsupported Interface");
    return (jobject)nullptr;
}

jobject returnObjectTypeInfo(JNIEnv *env, foundation::IObjectTypeInfo *pTypeInfo)
{
    foundation_assert(pTypeInfo);

    IObservableObjectInfo *pPropertyModelInfo;
    if (SUCCEEDED(foundation::QueryWeakReference(pTypeInfo, &pPropertyModelInfo)))
    {
        return createInspectableObject(env,"com/microsoft/pmod/ObservableObjectInfo",pPropertyModelInfo);
    }
    // fallback
    return createInspectableObject(env, "com/microsoft/pmod/ObjectTypeInfo", pTypeInfo);
}

jobjectArray returnValues(JNIEnv *env,UINT32 size,foundation::IInspectable **ppValues)
{
    // create an array of Inspectables's
    jclass objectClass = env->FindClass("java/lang/Object");
    jobjectArray jObjectArray = env->NewObjectArray(size, objectClass, 0);
    // fill them in
    for(UINT32 index = 0;index < size;++index)
    {
        foundation::IInspectable *pValue = ppValues[index];

        jobject javaObject = returnValue(env,pValue);
        // put in object array
        env->SetObjectArrayElement(jObjectArray, index, javaObject);
    }
    
    return jObjectArray;
}
