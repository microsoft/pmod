/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValueImpl.cpp
****/
#include "PropertyValueImpl.h"

#include "JniCoreUtils.h"
#include <foundation/pv_util.h>
#include <foundation/hstring_wrapper.h>

#include <vector>

using namespace foundation;
static void initPropertyValue(JNIEnv *env, jobject _this,foundation::IInspectable *pInspectable)
{
    IPropertyValue *pPropertyValue = nullptr;
    foundation::QueryWeakReference(pInspectable,IID_IPropertyValue,&pPropertyValue);
    initInterface(env, _this,pPropertyValue);
}

template <class T,class jType>
T toPropertyValue(jType jValue)
{
    return (T)jValue;
}
template <>
bool toPropertyValue(jboolean jValue)
{
    return jValue ? true : false;
}

template <class T, class jArrayType, class jItemType>
static HRESULT CreatePropertyValueArray(
    JNIEnv *env,
    jobject _this,
    jArrayType array,
    jItemType *(JNIEnv::*pGetArrayElements)(jArrayType, jboolean *),
    void (JNIEnv::*pReleaseArrayElements)(jArrayType, jItemType *, jint)
    )
{
    jsize size = env->GetArrayLength(array);

    jItemType *pArrayItems = (env->*pGetArrayElements)(array, nullptr);
    if (pArrayItems != nullptr) {
        T *pBuffer = (T *)_pal_MemAlloc((UINT32)(size * sizeof(T)));

        for (UINT32 index = 0; index < (UINT32)size; ++index)
        {
            pBuffer[index] = toPropertyValue<T>(pArrayItems[index]);
        }
        foundation::InspectablePtr valuePtr = pv_util::CreateValue((UINT32)size, pBuffer);
        (env->*pReleaseArrayElements)(array, pArrayItems, JNI_ABORT);
        _pal_MemFree(pBuffer);

        initPropertyValue(env, _this, valuePtr.Detach());
    }
    return S_OK;
}

template <class T, class jArrayType, class jItemType>
static jArrayType ReturnValueArray(
    JNIEnv *env,
    jobject _this,
    jArrayType(JNIEnv::*pNewArray)(jsize),
    void (JNIEnv::*pSetArrayRegion)(jArrayType, jsize, jsize, const jItemType *)
    )
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env, _this);
    if (pPropertyValue != nullptr)
    {
        UINT32 size = 0;
        T *pPropertyValueArray = nullptr;
        HRESULT hr = pv_util::GetValue(pPropertyValue, &size, &pPropertyValueArray);
        if (FAILED(hr))
        {
            ThrowComFail(env, hr, "IPropertyValue::GetArray");
        }
        else
        {
            jArrayType array = (env->*pNewArray)((jsize)size);
            jItemType *pjItemBuffer = (jItemType *)_pal_MemAlloc((UINT32)(size * sizeof(jItemType)));
            for (UINT32 index = 0; index < size; ++index)
            {
                pjItemBuffer[index] = (jItemType)pPropertyValueArray[index];
            }

            (env->*pSetArrayRegion)(array, 0, (jsize)size, pjItemBuffer);

            _pal_MemFree(pjItemBuffer);
            _pal_MemFree(pPropertyValueArray);
            return array;
        }
    }
    return (jArrayType)0;
}

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyValue_getTypeInternal
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        PropertyType propertyType;
        pPropertyValue->get_Type(&propertyType);
        return (jint)propertyType;
    }
    return (jint)PropertyType_Empty;
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createEmpty
    (JNIEnv *env, jobject _this)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateEmptyValue(&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateEmptyValue");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createByte
    (JNIEnv *env, jobject _this, jbyte value)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateUInt8Value(value,&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateUInt8Value");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createChar
    (JNIEnv *env, jobject _this, jchar value)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateChar16Value(value,&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateChar16Value");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createBoolean
    (JNIEnv *env, jobject _this, jboolean value)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateBooleanValue(value ? true:false,&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateBooleanValue");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createShort
    (JNIEnv *env, jobject _this, jshort value)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateInt16Value(value,&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateInt16Value");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createString
    (JNIEnv *env, jobject _this, jstring jstr)
{
    foundation::IInspectable *pInspectable = nullptr;
    foundation::HStringPtr hstr = toHStringPtr(env, jstr);

    HRESULT hr = foundation::pv_util::CreateStringValue(hstr, &pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateStringValue");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createInt
    (JNIEnv *env, jobject _this, jint intValue)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateInt32Value(intValue,&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateStringValue");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createLong
    (JNIEnv *env, jobject _this, jlong longValue)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateInt64Value(longValue,&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateInt64Value");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createSingle
    (JNIEnv *env, jobject _this, jfloat value)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateSingleValue(value,&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateSingleValue");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createDouble
    (JNIEnv *env, jobject _this, jdouble value)
{
    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateDoubleValue(value,&pInspectable);
    if(FAILED(hr))
    {
        ThrowComFail(env,hr,"IPropertyValue::CreateDoubleValue");
    }
    else
    {
        initPropertyValue(env,_this,pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createDateTime
(JNIEnv *env, jobject _this, jobject value)
{
    jclass jClass = env->FindClass("com/microsoft/pmod/PropertyValue");
    foundation_assert(jClass);
    if (!jClass)
    {
        return;
    }
    jmethodID methodId =
        env->GetStaticMethodID(jClass, "toCalendar", "(Ljava/util/Date;)Ljava/util/Calendar;");
    jobject jobject = env->CallStaticObjectMethod(jClass, methodId, value);

    jClass = env->FindClass("java/util/Calendar");
    methodId = env->GetMethodID(jClass, "get", "(I)I");

    foundation::SystemTime sysTime;

    sysTime.wYear = (USHORT)env->CallShortMethod(jobject, methodId, 1);
    sysTime.wMonth = (USHORT)env->CallShortMethod(jobject, methodId, 2) + 1;
    sysTime.wDay = (USHORT)env->CallShortMethod(jobject, methodId, 5);
    sysTime.wHour = (USHORT)env->CallShortMethod(jobject, methodId, 11);
    sysTime.wMinute = (USHORT)env->CallShortMethod(jobject, methodId, 12);
    sysTime.wSecond = (USHORT)env->CallShortMethod(jobject, methodId, 13);
    sysTime.wMilliseconds = (USHORT)env->CallShortMethod(jobject, methodId, 14);

    foundation::DateTime dt;
    _pal_SystemTimeToDateTime(sysTime, &dt);

    foundation::IInspectable *pInspectable = nullptr;
    HRESULT hr = foundation::pv_util::CreateDateTimeValue(dt, &pInspectable);
    if (FAILED(hr))
    {
        ThrowComFail(env, hr, "IPropertyValue::CreateDateTimeValue");
    }
    else
    {
        initPropertyValue(env, _this, pInspectable);
    }
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createByteArray
    (JNIEnv *env, jobject _this, jbyteArray array)
{
    CreatePropertyValueArray<BYTE>(env, _this, array, &JNIEnv::GetByteArrayElements, &JNIEnv::ReleaseByteArrayElements);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createCharArray
(JNIEnv *env, jobject _this, jcharArray array)
{
    CreatePropertyValueArray<WCHAR>(env, _this, array, &JNIEnv::GetCharArrayElements, &JNIEnv::ReleaseCharArrayElements);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createBooleanArray
(JNIEnv *env, jobject _this, jbooleanArray array)
{
    CreatePropertyValueArray<bool>(env, _this, array, &JNIEnv::GetBooleanArrayElements, &JNIEnv::ReleaseBooleanArrayElements);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createShortArray
(JNIEnv *env, jobject _this, jshortArray array)
{
    CreatePropertyValueArray<INT16>(env, _this, array, &JNIEnv::GetShortArrayElements, &JNIEnv::ReleaseShortArrayElements);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createIntArray
(JNIEnv *env, jobject _this, jintArray array)
{
    CreatePropertyValueArray<INT32>(env, _this, array, &JNIEnv::GetIntArrayElements, &JNIEnv::ReleaseIntArrayElements);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createLongArray
(JNIEnv *env, jobject _this, jlongArray array)
{
    CreatePropertyValueArray<INT64>(env, _this, array, &JNIEnv::GetLongArrayElements, &JNIEnv::ReleaseLongArrayElements);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createSingleArray
(JNIEnv *env, jobject _this, jfloatArray array)
{
    CreatePropertyValueArray<FLOAT>(env, _this, array, &JNIEnv::GetFloatArrayElements, &JNIEnv::ReleaseFloatArrayElements);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createDoubleArray
(JNIEnv *env, jobject _this, jdoubleArray array)
{
    CreatePropertyValueArray<DOUBLE>(env, _this, array, &JNIEnv::GetDoubleArrayElements, &JNIEnv::ReleaseDoubleArrayElements);
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createStringArray
(JNIEnv *env, jobject _this, jobjectArray array)
{
    jsize size = env->GetArrayLength(array);
    std::vector<HStringPtr> values;
    values.resize((size_t)size);
    for (jsize index = 0; index < size; ++index)
    {
        jstring jstr = (jstring)env->GetObjectArrayElement(array,index);
        values[index] = toHStringPtr(env, jstr);
    }
    foundation::InspectablePtr valuePtr = pv_util::CreateValue((UINT32)size, (HSTRING *)&values[0]);
    initPropertyValue(env, _this, valuePtr.Detach());
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_PropertyValue_createObjectArray
(JNIEnv *env, jobject _this, jobjectArray array)
{
    jsize size = env->GetArrayLength(array);
    std::vector<foundation::IInspectable *> values;
    values.resize((size_t)size);
    for (jsize index = 0; index < size; ++index)
    {
        jobject jobj = env->GetObjectArrayElement(array, index);
        values[index] = extractInterfaceIf(env, jobj);
    }
    foundation::InspectablePtr valuePtr = pv_util::CreateValue(
        (UINT32)size, 
        &values[0]);
    initPropertyValue(env, _this, valuePtr.Detach());
}

JNIEXPORT jboolean JNICALL Java_com_microsoft_pmod_PropertyValue_getBoolean
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        boolean boolValue;
        HRESULT hr = pPropertyValue->GetBoolean(&boolValue);
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetBoolean");
        }
        else
        {
            return (jboolean)boolValue;
        }
    }
    return (jboolean)false;
}

JNIEXPORT jbyteArray JNICALL Java_com_microsoft_pmod_PropertyValue_getStringInternal
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        foundation::HStringPtr hstring;
        HRESULT hr = pPropertyValue->GetString(hstring.GetAddressOf());
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetString");
        }
        else
        {
            return toStringByteArray(env, hstring);
        }
    }
    return (jbyteArray)nullptr;
}

JNIEXPORT jbyte JNICALL Java_com_microsoft_pmod_PropertyValue_getByte
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        BYTE byteValue;
        HRESULT hr = pPropertyValue->GetUInt8(&byteValue);
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetUInt8");
        }
        else
        {
            return (jbyte)byteValue;
        }
    }
    return (jbyte)0;
}

JNIEXPORT jint JNICALL Java_com_microsoft_pmod_PropertyValue_getInt
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        INT32 intValue;
        HRESULT hr = pPropertyValue->GetInt32(&intValue);
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetInt32");
        }
        else
        {
            return (jint)intValue;
        }
    }
    return (jint)0;
}

JNIEXPORT jchar JNICALL Java_com_microsoft_pmod_PropertyValue_getChar
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        WCHAR charValue;
        HRESULT hr = pPropertyValue->GetChar16(&charValue);
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetChar16");
        }
        else
        {
            return (jchar)charValue;
        }
    }
    return (jchar)0;
}

JNIEXPORT jshort JNICALL Java_com_microsoft_pmod_PropertyValue_getShort
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        INT16 shortValue;
        HRESULT hr = pPropertyValue->GetInt16(&shortValue);
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetInt32");
        }
        else
        {
            return (jshort)shortValue;
        }
    }
    return (jshort)0;
}

JNIEXPORT jlong JNICALL Java_com_microsoft_pmod_PropertyValue_getLong
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        INT64 longValue;
        HRESULT hr = pPropertyValue->GetInt64(&longValue);
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetInt64");
        }
        else
        {
            return (jlong)longValue;
        }
    }
    return (jlong)0;
}

JNIEXPORT jfloat JNICALL Java_com_microsoft_pmod_PropertyValue_getSingle
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        FLOAT floatValue;
        HRESULT hr = pPropertyValue->GetSingle(&floatValue);
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetSingle");
        }
        else
        {
            return (jfloat)floatValue;
        }
    }
    return (jfloat)0;
}

JNIEXPORT jdouble JNICALL Java_com_microsoft_pmod_PropertyValue_getDouble
    (JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env,_this);
    if(pPropertyValue != nullptr)
    {
        DOUBLE doubleValue;
        HRESULT hr = pPropertyValue->GetDouble(&doubleValue);
        if(FAILED(hr))
        {
            ThrowComFail(env,hr,"IPropertyValue::GetDouble");
        }
        else
        {
            return (jdouble)doubleValue;
        }
    }
    return (jdouble)0;
}

JNIEXPORT jobject JNICALL Java_com_microsoft_pmod_PropertyValue_getDateTime
(JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env, _this);
    if (pPropertyValue != nullptr)
    {
        foundation::DateTime dt;
        HRESULT hr = pPropertyValue->GetDateTime(&dt);
        if (FAILED(hr))
        {
            ThrowComFail(env, hr, "IPropertyValue::GetDateTime");
        }
        else
        {
            foundation::SystemTime sysTime;
            _pal_DateTimeToSystemTime(dt, &sysTime);

            jclass jClass = env->FindClass("com/microsoft/pmod/PropertyValue");
            foundation_assert(jClass);
            jmethodID methodId =
                env->GetStaticMethodID(jClass, "toDate", "(IIIIIII)Ljava/util/Date;");
            jobject jobject = env->CallStaticObjectMethod(
                jClass, 
                methodId, 
                sysTime.wYear,
                sysTime.wMonth - 1, 
                sysTime.wDay,
                sysTime.wHour,
                sysTime.wMinute,
                sysTime.wSecond,
                sysTime.wMilliseconds
                );
            return jobject;
        }
    }
    return (jobject)0;
}

JNIEXPORT jbyteArray JNICALL Java_com_microsoft_pmod_PropertyValue_getByteArray
(JNIEnv *env, jobject _this)
{
    return ReturnValueArray<BYTE>(env, _this, &JNIEnv::NewByteArray, &JNIEnv::SetByteArrayRegion);
}

JNIEXPORT jcharArray JNICALL Java_com_microsoft_pmod_PropertyValue_getCharArray
(JNIEnv *env, jobject _this)
{
    return ReturnValueArray<WCHAR>(env, _this, &JNIEnv::NewCharArray, &JNIEnv::SetCharArrayRegion);
}

JNIEXPORT jbooleanArray JNICALL Java_com_microsoft_pmod_PropertyValue_getBooleanArray
(JNIEnv *env, jobject _this)
{
    return ReturnValueArray<bool>(env, _this, &JNIEnv::NewBooleanArray, &JNIEnv::SetBooleanArrayRegion);
}

JNIEXPORT jshortArray JNICALL Java_com_microsoft_pmod_PropertyValue_getShortArray
(JNIEnv *env, jobject _this)
{
    return ReturnValueArray<INT16>(env, _this, &JNIEnv::NewShortArray, &JNIEnv::SetShortArrayRegion);
}

JNIEXPORT jintArray JNICALL Java_com_microsoft_pmod_PropertyValue_getIntArray
(JNIEnv *env, jobject _this)
{
    return ReturnValueArray<INT32>(env, _this, &JNIEnv::NewIntArray, &JNIEnv::SetIntArrayRegion);
}

JNIEXPORT jlongArray JNICALL Java_com_microsoft_pmod_PropertyValue_getLongArray
(JNIEnv *env, jobject _this)
{
    return ReturnValueArray<INT64>(env, _this, &JNIEnv::NewLongArray, &JNIEnv::SetLongArrayRegion);
}

JNIEXPORT jfloatArray JNICALL Java_com_microsoft_pmod_PropertyValue_getFloatArray
(JNIEnv *env, jobject _this)
{
    return ReturnValueArray<FLOAT>(env, _this, &JNIEnv::NewFloatArray, &JNIEnv::SetFloatArrayRegion);
}

JNIEXPORT jdoubleArray JNICALL Java_com_microsoft_pmod_PropertyValue_getDoubleArray
(JNIEnv *env, jobject _this)
{
    return ReturnValueArray<DOUBLE>(env, _this, &JNIEnv::NewDoubleArray, &JNIEnv::SetDoubleArrayRegion);
}

JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_PropertyValue_getStringArrayInternal
(JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env, _this);
    if (pPropertyValue != nullptr)
    {
        HStringArrayWrapper arrayWrapper;
        HRESULT hr = pv_util::GetValue(
            pPropertyValue, 
            arrayWrapper.GetSizeAddressOf(),
            arrayWrapper.GetBufferAddressOf());
        if (FAILED(hr))
        {
            ThrowComFail(env, hr, "IPropertyValue::GetArray");
        }
        jclass objectClass = env->FindClass("java/lang/Object");
        jobjectArray jObjectArray = env->NewObjectArray((jsize)arrayWrapper.GetSize(), objectClass, 0);
        for (UINT32 index = 0; index < arrayWrapper.GetSize(); ++index)
        {
            env->SetObjectArrayElement(jObjectArray, (jsize)index, toStringByteArray(env, arrayWrapper[index]));
        }
        return jObjectArray;
    }
    return (jobjectArray)0;
}

JNIEXPORT jobjectArray JNICALL Java_com_microsoft_pmod_PropertyValue_getObjectArrayInternal
(JNIEnv *env, jobject _this)
{
    IPropertyValue *pPropertyValue = extractInterfaceExpectedAndCast<IPropertyValue>(env, _this);
    if (pPropertyValue != nullptr)
    {
        InspectableArrayWrapper arrayWrapper;
        HRESULT hr = pv_util::GetValue(
            pPropertyValue,
            arrayWrapper.GetSizeAddressOf(),
            arrayWrapper.GetBufferAddressOf());
        if (FAILED(hr))
        {
            ThrowComFail(env, hr, "IPropertyValue::GetArray");
        }
        return returnValues(env, arrayWrapper.GetSize(), (foundation::IInspectable **)arrayWrapper.GetBuffer());
    }
    return (jobjectArray)0;
}
