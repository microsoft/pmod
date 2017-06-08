/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionTest.h
****/
#pragma once

#include <pmod/interfaces/observable_collection.h>
#include <pmod/event_connection.h>
#include <foundation/com_ptr.h>

class ITestObservableCollectionClass
{
public:
    virtual HRESULT AppendItem(foundation::IInspectable *pValue) = 0;
    virtual HRESULT RemoveItem(UINT32 index) = 0;
    virtual HRESULT Clear() = 0;
};

class CObservableCollectionTest
{
public:
    CObservableCollectionTest();
    void TestInt32(
        ITestObservableCollectionClass& classImpl,
        pmod::IObservableCollection_Int32 *pObservableCollection_Int32);
private:
    void Attach(foundation::IUnknown *pUnk);
    void Detach();
    void TestObservableObject(
        ITestObservableCollectionClass& classImpl,
        pmod::IObservableCollection *pObservableCollection);

    HRESULT OnNotifyCollectionChanged(
        foundation::IUnknown *pSender,
        pmod::INotifyCollectionChangedEventArgs *pEventArgs)
    {
        this->_lastEventArgsPtr = pEventArgs;
        return S_OK;
    }

private:
    foundation::ComPtr<pmod::INotifyCollectionChangedEventArgs> _lastEventArgsPtr;

    pmod::NotifyCollectionChangedConnection _notifycollectionchanged_connection;
};

struct _NotifyCollectionHelper
{
	_NotifyCollectionHelper()
	{
		_connection.SetSink([&](foundation::IUnknown *pSender, pmod::INotifyCollectionChangedEventArgs *pEventArgs) -> HRESULT {
			pEventArgs->GetAction(&_action);
			_oldItems.Clear();
			_newItems.Clear();
			pEventArgs->GetOldStartingIndex(&_oldIndex);
			pEventArgs->GetNewStartingIndex(&_newIndex);

			pEventArgs->GetOldItems(_oldItems.GetSizeAddressOf(), _oldItems.GetBufferAddressOf());
			pEventArgs->GetNewItems(_newItems.GetSizeAddressOf(), _newItems.GetBufferAddressOf());
			return S_OK;
		});
	}
	pmod::NotifyCollectionChangedConnection _connection;

	foundation::NotifyCollectionModelChangedAction _action;
	UINT32 _oldIndex;
	UINT32 _newIndex;
	foundation::InspectableArrayWrapper _oldItems;
	foundation::InspectableArrayWrapper _newItems;
};

template <class vector_t>
void TestInt32ObservableVector(
	pmod::IObservableCollection *pObservableCollection,
	vector_t& vec)
{
	_NotifyCollectionHelper connectionHelper;
	vec.push_back(100);
	UINT32 count;
	pObservableCollection->GetCount(&count);
	UT_VERIFY_BOOL(count == 1);
	std::vector<int> v{ 10,20, 30 };
	vec = v;
	pObservableCollection->GetCount(&count);
	UT_VERIFY_BOOL(count == 3);

	connectionHelper._connection.Attach(pObservableCollection);
	vec.clear();
	UT_VERIFY_BOOL(connectionHelper._action == foundation::NotifyCollectionModelChangedAction::Reset);
	vec.push_back(50);
	UT_VERIFY_BOOL(connectionHelper._action == foundation::NotifyCollectionModelChangedAction::ItemAdded);
	UT_VERIFY_BOOL(connectionHelper._oldIndex == foundation::NINDEX);
	UT_VERIFY_BOOL(connectionHelper._newIndex == 0);
	vec = v;
	UT_VERIFY_BOOL(connectionHelper._action == foundation::NotifyCollectionModelChangedAction::Reset);
}
