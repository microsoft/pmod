/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:exception.h
****/
#pragma once

#include <exception>
#include <foundation/pal.h>

namespace foundation
{

class com_exception : public std::exception
{
private:
    HRESULT _result;

public:
    /// <summary>
    ///     Constructs a <c>core_exception</c> object.
    /// </summary>
    /// <param name="hr">
    ///     A HRESULT error.
    /// </param>
    /**/
    explicit com_exception(_In_  HRESULT hr) throw()
        : _result(hr)
    {
    }

    /// <summary>
    ///     Constructs a <c>core_exception</c> object.
    /// </summary>
    /**/
    com_exception() throw()
        : exception()
    {
    }

    ~com_exception() throw () {}

    HRESULT result()
    {
        return _result;
    }
};

#define THROW_IFR(x) {int hr_IFR = (x); if(FAILED(hr_IFR)) { throw foundation::com_exception(hr_IFR); } }
#define THROW_IFHR(x) { hr = (x); if(FAILED(hr)) { throw foundation::com_exception(hr); } }


}

