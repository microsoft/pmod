/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JniCoreUtils.h
****/
#ifndef _JNI_CORE_UTILS_DEFINED
#define _JNI_CORE_UTILS_DEFINED

#include <jni.h>

// propertymodel public headers
#include <pmod/interfaces.h>
#include <foundation/com_ptr.h>
#include <foundation/hstring_wrapper.h>

void ThrowComFail(JNIEnv *env, HRESULT hr, const char* desc);

inline bool ThrowIfComFail(JNIEnv *env, HRESULT hr, const char* desc)
{
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, desc);
        return true;
    }
    
    return false;
}

// Create an Interface based  class object
jobject createInspectableObject(
    JNIEnv *env, 
    jclass jClass, 
    foundation::IUnknown *pInterface);
jobject createInspectableObject(
    JNIEnv *env,
    const char* className,
    foundation::IUnknown *pInterface);

foundation::IInspectable *extractInterface(JNIEnv *env, jobject arg);
inline foundation::IInspectable *extractInterfaceIf(JNIEnv *env, jobject arg)
{
    return arg == nullptr ? nullptr : extractInterface(env, arg);
}

foundation::IInspectable *extractInterfaceExpected(JNIEnv *env, jobject arg);
void initInterface(JNIEnv *env, jobject _this, foundation::IInspectable *pInterface);

// convert GUID <-> jstring
jstring toString(JNIEnv *env, GUID iidType);
IID toIIDType(JNIEnv *env, jstring jstr);

// convert HSTRING <-> jstring
jstring toString(JNIEnv *env, HSTRING hstr);
jbyteArray toStringByteArray(JNIEnv *env, HSTRING hstr);
HSTRING toHString(JNIEnv *env, jstring jstr);
foundation::HStringPtr toHStringPtr(JNIEnv *env, jstring jstr);


jobject createPropertyValue(JNIEnv *env, foundation::IPropertyValue *pPropertyValue);
jobject createPropertyValue(JNIEnv *env, foundation::IUnknown *pInterface);


jobject returnValue(JNIEnv *env, foundation::IInspectable *pInterface);
jobject returnObjectTypeInfo(JNIEnv *env, foundation::IObjectTypeInfo *pModelTypeInfo);
jobjectArray returnValues(JNIEnv *env, UINT32 size, foundation::IInspectable **ppValues);

template <class T>
T * extractInterfaceExpectedAndCast(JNIEnv *env, jobject arg)
{
    return static_cast<T *>(extractInterfaceExpected(env,arg));
}

template <class T>
T * extractInterfaceExpectedAndQI(JNIEnv *env, jobject arg)
{
    foundation::IInspectable *pInspectable = extractInterfaceExpected(env, arg);
   if(pInspectable)
   {
       T *pT = nullptr;
       HRESULT hr = foundation::QueryWeakReference(pInspectable, &pT);
       if(FAILED(hr))
       {
           ThrowComFail(env, hr, "extractInterfaceExpectedAndQI");
       }
       return pT;
   }
   return nullptr;
}

#endif
