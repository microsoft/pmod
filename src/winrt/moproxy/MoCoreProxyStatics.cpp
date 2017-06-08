/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoCoreProxyStatics.cpp
****/

#include "pch.h"

#include "MoCoreProxyStatics.h"
#include "MoCoreProxyFactory.h"

STDMETHODIMP MoCoreProxyStatics::ToProxy(
        IInspectable *pInspectable,
        IInspectable **result)
{
    return CMoCoreProxyFactory::ToProxy(pInspectable, result);
}

STDMETHODIMP MoCoreProxyStatics::FromProxy(
    IInspectable *pInspectable,
    IInspectable **ppResult)
{
    return CMoCoreProxyFactory::FromProxy(pInspectable, ppResult);
}
