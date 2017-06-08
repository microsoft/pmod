/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionTest.cpp
****/
#include "pch.h"
#include "ObservableCollectionTest.h"
#include <foundation/value_ptr.h>
#include <pmod/event_connection.h>

using namespace foundation;
using namespace pmod;

template <class TInterface>
void VerifyCount(TInterface *pObservableCollection,UINT32 verifyCount)
{
    UINT32 count;
    UT_VERIFY_HR(pObservableCollection->GetCount(&count));
    UT_VERIFY_BOOL(count == verifyCount);
}

template <class TInterface, class Value>
void VerifyValueAt(TInterface *pObservableCollection, UINT32 index, const Value& verifyValue)
{
    Value value;
    UT_VERIFY_HR(pObservableCollection->GetItem(index,&value));
    UT_VERIFY_BOOL(value == verifyValue);
}

template <class Value>
void AppendItem(ITestObservableCollectionClass& classImpl,const Value& value)
{
    UT_VERIFY_HR(classImpl.AppendItem(ValuePtr(value)));
}

CObservableCollectionTest::CObservableCollectionTest()
{
    _notifycollectionchanged_connection.SetSink(this, &CObservableCollectionTest::OnNotifyCollectionChanged);
}

void CObservableCollectionTest::Attach(foundation::IUnknown *pUnk)
{
    ComPtr<IObservableCollection> ptr;
    UT_VERIFY_HR(QueryInterface(pUnk, ptr.GetAddressOf()));
    _notifycollectionchanged_connection.Attach(ptr);
}

void CObservableCollectionTest::Detach()
{
    _notifycollectionchanged_connection.Detach();
}

void CObservableCollectionTest::TestInt32(
    ITestObservableCollectionClass& classImpl,
    IObservableCollection_Int32 *pObservableCollection_Int32)
{
    NotifyCollectionChangedConnection connection;

    VerifyCount(pObservableCollection_Int32, 0);
    AppendItem(classImpl, 100);
    VerifyCount(pObservableCollection_Int32, 1);
    VerifyValueAt(pObservableCollection_Int32, 0, 100);

    Attach(pObservableCollection_Int32);
    AppendItem(classImpl, 200);
    UT_VERIFY_BOOL(_lastEventArgsPtr != nullptr);

    NotifyCollectionModelChangedAction action;
    UT_VERIFY_HR(_lastEventArgsPtr->GetAction(&action));
    UT_VERIFY_BOOL(action == NotifyCollectionModelChangedAction::ItemAdded);

    ComPtr<IEnumerable> enumerablePtr;
    UT_VERIFY_HR(::QueryInterface(pObservableCollection_Int32, enumerablePtr.GetAddressOf()));

    Detach();

    ComPtr<IObservableCollection> observableCollectionPtr;
    UT_VERIFY_HR(QueryInterface(pObservableCollection_Int32, observableCollectionPtr.GetAddressOf()));
    TestObservableObject(classImpl, observableCollectionPtr);
}

void CObservableCollectionTest::TestObservableObject(
    ITestObservableCollectionClass& classImpl,
    pmod::IObservableCollection *pObservableCollection)
{
    ComPtr<IObservableObject> observableObjectPtr;
    UT_VERIFY_HR(QueryInterface(pObservableCollection, observableObjectPtr.GetAddressOf()));

    foundation::ComPtr<pmod::IPropertyChangedEventArgs> eventArgsPtr;

    pmod::PropertyChangedConnection connection;

    auto propertyChangedCallBack =
        [&](foundation::IUnknown *pSender, pmod::IPropertyChangedEventArgs *pEventArgs) -> HRESULT {
        eventArgsPtr = pEventArgs;
        return S_OK;
    };
    connection.SetSink(propertyChangedCallBack);

    ValuePtr value;
    UT_VERIFY_HR(observableObjectPtr->GetProperty(ObservableCollection::Property_Count, value.GetAddressOf()));
    UT_VERIFY_BOOL(value.GetValue<UINT32>() != 0);
    connection.Attach(observableObjectPtr);

    classImpl.Clear();

    UT_VERIFY_BOOL(eventArgsPtr != nullptr);
    UT_VERIFY_HR(observableObjectPtr->GetProperty(ObservableCollection::Property_Count, value.ReleaseAndGetAddressOf()));
    UT_VERIFY_BOOL(value.GetValue<UINT32>() == 0);

    connection.Detach();

}

