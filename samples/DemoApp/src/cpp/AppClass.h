/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AppClass.h
****/

#pragma once

#include "DemoAppFastInternal.g.h"
#include <pmod/library/library_util.h>

class CAppClass :
    public DemoApp::_FastAppDemoBase
{
public:
	HRESULT _InitializeModel();
	HRESULT _InitializeModel(foundation::IInspectable *pInspectable);
private:
    // Commands override
    HRESULT ExecuteAddItem(HSTRING parameter) override;
    HRESULT ExecuteAddNextItem() override;
    HRESULT CanExecuteAddItem(HSTRING parameter, bool* pValue) override;
	HRESULT ExecuteRemoveItem(DemoApp::IItem * parameter) override;
	HRESULT CanExecuteRemoveItem(DemoApp::IItem * parameter,bool* pValue) override;
	HRESULT ExecuteClearItems() override;

    // Methods override
    HRESULT CreateItemAsyncInternal(HSTRING name, const foundation::AsyncOperationClassPtr& async_operation_class_ptr) override;

private:
    // declare a type safe collection for public API (using the ICollection_IItem interface)
    typedef foundation::_ComClassPtr<
        DemoApp::ICollection_IItem,
        pmod::library::_ObservableCollectionClassHostWrapper,
        pmod::_RefCountedObservableCollectionPtr<DemoApp::ICollection_IItem>> _IItemCollectionClass;

    _IItemCollectionClass _itemsClassPtr;
};
