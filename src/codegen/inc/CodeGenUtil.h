/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CodeGenUtil.h
****/
#ifndef _CODE_GEN_UTIL_DEFINED
#define _CODE_GEN_UTIL_DEFINED

#include <vector>
#include <map>
#include <string>

template <class T>
class AutoDeleteVector : public std::vector<T>
{
public:
    ~AutoDeleteVector()
    {
        clearAndDelete();
    }

    void clearAndDelete()
    {
        for(typename AutoDeleteVector::const_iterator iter = this->begin();
            iter != this->end();
            ++iter)
        {
            delete (*iter);
        }
        this->clear();
    }
};

// throw an Error using a format string
void _throwError(const wchar_t * _Format,...);

// get Namespace and type from full type definition
void GetNamespaceAndType(const wchar_t *type,std::wstring& _namespace,std::wstring& typeName);

bool isAbsolutePath(const wchar_t *filepath);

#endif
