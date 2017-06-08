/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyDataTest.h
****/
#pragma once

#include <foundation/interfaces/object_dispatch.h>
#include <foundation/library/interfaces/dispatcher.h>
#include <memory>


class ITestPropertyDataFactory
{
public:
    virtual HRESULT Create(
        const IID& iidType,
        foundation::IObjectDispatch **ppPropertyData) const = 0;
};

class CPropertyDataTest
{
public:
    void Test(const ITestPropertyDataFactory& factory);
};
