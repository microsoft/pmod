/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoJsonUtils.cpp
****/

#include "pch.h"

#include "MoJsonUtils.h"
#include <foundation/library/json_statics_util.h>

STDMETHODIMP MoJsonUtilsStatics::get_StreamProvider(
    IStreamProvider **value)
{
    return foundation::QueryInterface(
        foundation::json_util::GetStreamProvider(),
        value);
}

STDMETHODIMP MoJsonUtilsStatics::ToJson( 
        IInspectable *pInspectable,
        HSTRING *result)
{
    return foundation::json_util::GetJsonStatics()->ToJson(pInspectable, result);
}
                        
STDMETHODIMP MoJsonUtilsStatics::FromJson( 
        HSTRING json,
        IInspectable **ppResult)
{
    return foundation::json_util::GetJsonStatics()->FromJson(json, ppResult);
}
