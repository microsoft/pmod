/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypePropertiesTest.cpp
****/
#include "TestModelFastInternal.g.h"
#include <pmod/library/object_factory.h>
#include <pal/pal_thread.h>

using namespace foundation;
using namespace foundation::library;
using namespace pmod::library;

typedef TestModel::_FastTypePropertiesBase CTypeProperties;
typedef ObjectFactory<CTypeProperties, TestModel::TypeProperties::IIDType, ObservableObjectOptions::None, true> CTypeProperties_Factory;


typedef TestModel::_FastTypePropertiesTestBase CTypePropertiesTest_BaseType;

class CTypePropertiesTest :
    public CTypePropertiesTest_BaseType
{
public:
    HRESULT _InitializeModel(IDispatcher *pWorkingDisptcher)
    {
        // Set value of TypeProperties property
        CTypeProperties_Factory::CreateInstanceAs(this->_typePropertiesValue.GetAddressOf());

        // define working dispatcher
        this->SetLocalWorkingDispatcher(pWorkingDisptcher);
        return S_OK;
    }
protected:
    HRESULT ChangePropertyAsyncInternal(UINT32 propertyId, foundation::IInspectable * propertyValue, const foundation::AsyncOperationClassPtr& async_operation_class_ptr) override
    {
        this->_typePropertiesValue->SetProperty(propertyId, propertyValue);
        return async_operation_class_ptr.SetCompleted(S_OK);
    }

private:
};

typedef ObjectFactory<CTypePropertiesTest, TestModel::TypePropertiesTest::IIDType, ObservableObjectOptions::None, true> CTypePropertiesTest_Factory;
HRESULT CreateTypePropertiesTest(IDispatcher *pWorkingDisptcher, TestModel::ITypePropertiesTest **pp)
{
    return CTypePropertiesTest_Factory::CreateInstanceAs(pp, pWorkingDisptcher);
}



