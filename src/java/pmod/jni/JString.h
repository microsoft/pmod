/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JString.h
****/

#pragma once

#include <jni.h>
#include <wchar.h>
#include "JObject.h"

namespace Java
{
    class JString : public JObject
    {
    public:
        JString(jstring jobj, bool removeLocalRef = false);
        JString(const char* content);
        JString(const wchar_t* content);
        JString(const wchar_t* content, size_t cchLength);

        ~JString();
    public:
        operator jstring () const {return (jstring)m_jobj;};
    public:
        // TODO: GetUTFString is not thread-safe.
        const char* GetUTFString() const;
        int GetUTFLength() const;

        const wchar_t* GetStringChars() const;
        int GetLength() const;

        bool CopyTo(wchar_t *pszDest, size_t* pcchBufferSize) const;

    private:
        void Init(const wchar_t* content, size_t cchLength);

        const char* m_pString;
        const wchar_t* m_pWString;
    };
}

