/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CreateFoundationHelper.h
****/
#pragma once

template<class FoundationClassImpl, class TParameter, class T>
HRESULT  _CreateFoundationInstance(
    _In_ const TParameter *pParameters,
    _In_opt_ T *pOuter,
    _COM_Outptr_ T **ppNewInstance)
{
    HRESULT hr;
    if (pOuter)
    {
        foundation::ctl::AggregableComInspectableObject<FoundationClassImpl> *pInstance;
        IFHR_ASSERT(foundation::ctl::AggregableComInspectableObject<FoundationClassImpl>::CreateInstance(static_cast<foundation::IInspectable *>(pOuter), &pInstance));
        // Note we will set ppNewInstance right now since the aggregation could require
        // the returned Inner Ptr during a QI or other AddRef/Release
        // This was found when logger listeners perform QI during '_Initialize' messages 
        *ppNewInstance = reinterpret_cast<T*>(static_cast<foundation::ctl::INonDelegatingUnknown *>(pInstance));

        pInstance->_BeginInitialize();
        hr = pInstance->_Initialize(pParameters);
        pInstance->_EndInitialize();
        if (FAILED(hr))
        {
            pInstance->Release();
            IFHR_ASSERT(hr);
        }
    }
    else
    {
        foundation::ctl::ComInspectableObject<FoundationClassImpl> *pInstance;

        IFHR_ASSERT(foundation::ctl::ComInspectableObject<FoundationClassImpl>::CreateInstance(&pInstance));

        pInstance->_BeginInitialize();
        hr = pInstance->_Initialize(pParameters);
        pInstance->_EndInitialize();
        if (FAILED(hr))
        {
            pInstance->Release();
            IFHR_ASSERT(hr);
        }
        *ppNewInstance = reinterpret_cast<T*>(foundation::ctl::inspectable::CastToInspectable(pInstance));
    }
    return S_OK;
}


template<class FoundationClassImpl,class TParameter>
HRESULT  CreateFoundationInstance(
    _In_ const TParameter *pParameters,
    _In_opt_ foundation::IInspectable *pOuter,
    _COM_Outptr_ foundation::IInspectable **ppInspectable)
{
    return _CreateFoundationInstance<FoundationClassImpl>(
        pParameters, pOuter, ppInspectable);
}
