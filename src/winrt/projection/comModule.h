/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:comModule.h
****/
#pragma once

#include "activation.h"

namespace ctlwinrt
{
    typedef IActivationFactory* (*FACTORYCREATIONPFN)();

    struct activation_factory_entry
    {
        WCHAR *m_szClassID;
        IActivationFactory *m_pFactory;
        FACTORYCREATIONPFN m_pfnCreator;

        IActivationFactory* GetFactory()
        {
            if (m_pFactory == nullptr)
            {
                m_pFactory = m_pfnCreator();
            }

            return m_pFactory;
        }

        void DestroyFactory()
        {
            if (m_pFactory)
            {
                m_pFactory->Release();
                m_pFactory = nullptr;
            }
        }
    };

    extern activation_factory_entry __creationMap[];

    class module
    {
    public:
        IActivationFactory* GetActivationFactory(HSTRING hClassID);
        void ClearFactoryCache();
    };

    #define COMMA ,
#define ACTIVATION_FACTORY_MAP_BEGIN()      ctlwinrt::activation_factory_entry ctlwinrt::__creationMap[] = {
    #define ACTIVATION_FACTORY_MAP_END()      {nullptr, nullptr, nullptr}};
    #define ACTIVATION_FACTORY_ENTRY(s, f)    {s, nullptr, f},

    extern module __module;
    extern ULONG g_nObjects;

    void IncrementObjectCount();
    void DecrementObjectCount();
}
