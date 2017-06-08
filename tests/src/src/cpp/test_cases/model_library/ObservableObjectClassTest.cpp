/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectClassTest.cpp
****/
#include "pch.h"

#include <pmod/library/mtl.h>
#include <pmod/event_connection.h>

using namespace foundation;
using namespace pmod;
using namespace pmod::library;

class CObservableObjectClassTest : public CUnitTestBase
{
protected:

    void TestDelegate()
    {
        class CMyClass : public _ObservableObjectClass
        {
        protected:
            CMyClass():
                _countHandlers(0)
            {}

            STDMETHOD(OnAddEventHandler)(_In_ IPropertyChangedEventHandler *pEventHandler) override
            {
                ++_countHandlers;
                return S_OK;
            }

            STDMETHOD(OnRemoveEventHandler)(_In_ IPropertyChangedEventHandler *pEventHandler) override
            {
                --_countHandlers;
                return S_OK;
            }
        public:
            int _countHandlers;
        };

        CMyClass *pMyClass;
        UT_VERIFY_HR(CreateObjectClass(foundation_GUID_NULL, pmod::library::ObservableObjectOptions::None, &pMyClass));

        auto callback =
            [&](foundation::IUnknown *pSender, pmod::IPropertyChangedEventArgs *pEventArgs) -> HRESULT {
            return S_OK;
        };
        PropertyChangedConnection event_connection(callback);
        event_connection.Attach(pMyClass->GetInterface());

        UT_VERIFY_BOOL(pMyClass->_countHandlers == 1);
        event_connection.Detach();
        UT_VERIFY_BOOL(pMyClass->_countHandlers == 0);

        ctl::inspectable::ReleaseInterface(pMyClass);
    }

public:
	BeginTestMethodMap(CObservableObjectClassTest)
        TestMethod(TestDelegate)
	EndTestMethodMap()

};
ImplementTestClass(CObservableObjectClassTest);


