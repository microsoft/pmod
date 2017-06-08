/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JString.cpp
****/
#include "JString.h"
#include "JVMEnv.h"
#include "JniUtility.h"
#include <wchar.h>

using namespace Java;

JString::JString(jstring jobj, bool removeLocalRef) : JObject((jobject)jobj, removeLocalRef)
{
    m_pString = nullptr;
    m_pWString = nullptr;
}

JString::JString(const char* content)
{
    m_jobj = nullptr;
    JNIEnv* env = GetEnv();
    //TODO:Need to replace with NewString with Unicode encoding instead of NewStringUTF:3239900
    AttachLocalObj(env, (jobject)env->NewStringUTF(content));
    m_pString = nullptr;
    m_pWString = nullptr;
}

JString::JString(const wchar_t* content)
{
    Init(content, wcslen(content));
}

JString::JString(const wchar_t* content, size_t cchLength)
{
    Init(content, cchLength);
}

JString::~JString()
{
    if (m_pString && m_jobj != nullptr)
    {
        GetEnv()->ReleaseStringUTFChars((jstring)m_jobj, m_pString);
        m_pString = nullptr;
    }

    if (m_pWString && m_jobj != nullptr)
    {
        GetEnv()->ReleaseStringChars((jstring)m_jobj, (const jchar*)m_pWString);
        m_pWString = nullptr;
    }
}

const char* JString::GetUTFString() const
{
    if (m_pString == nullptr)
    {
        ((JString*)this)->m_pString = GetEnv()->GetStringUTFChars((jstring)m_jobj, nullptr);
    }

    return m_pString;
}

const wchar_t* JString::GetStringChars() const
{
    if (m_pWString == nullptr)
    {
        ((JString*)this)->m_pWString = (const wchar_t*)GetEnv()->GetStringChars((jstring)m_jobj, nullptr);
    }

    return m_pWString;
}

//+-----------------------------------------------------------------
// Routine Description:
//     Copy the wstring content to a buffer.
//
// Return value:
//     false if buffer is too small;
//     true for otherwise.
//
// Arguments:
//     pszDest (out):
//         Pointer of the buffer to contain the setting value. When it is nullptr,
//         will only set the needed buffer size.
//
//     pcchBufferSize (In and out):
//         When calling the method, it contains the buffer length;
//         after calling, it contains the actual length of value string (including)
//         the terminating 0.
//------------------------------------------------------------------
bool JString::CopyTo(wchar_t *pszDest, size_t* pcchBufferSize) const
{
    bool rc = true;
    size_t size = (size_t)GetLength();
    if (pszDest == nullptr)
    {
        goto Exit;
    }

    if (*pcchBufferSize > size)
    {
        wmemcpy(pszDest, GetStringChars(), size);
        *(pszDest + size) = L'\0';
    }
    else
    {
        rc = false;
    }

Exit:
    *pcchBufferSize = size + 1;
    return rc;
}

int JString::GetUTFLength() const
{
    return (int)GetEnv()->GetStringUTFLength((jstring)m_jobj);
}

int JString::GetLength() const
{
    return (int)GetEnv()->GetStringLength((jstring)m_jobj);
}

void JString::Init(const wchar_t* content, size_t cchLength)
{
    // This implementation will only work for 16bits wchar.
    m_jobj = nullptr;
    JNIEnv* env = GetEnv();
    AttachLocalObj(env, (jobject) env->NewString((const jchar *)content,(jsize)cchLength));
    m_pString = nullptr;
    m_pWString = nullptr;
}
