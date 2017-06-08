/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MethodSupportImpl.cpp
****/
#include "TestModelFastInternal.g.h"
#include <pmod/library/object_factory.h>
#include <pal/pal_thread.h>

using namespace foundation;
using namespace foundation::library;
using namespace pmod::library;

typedef TestModel::_FastMethodSupportBase CMethodSupport_BaseType;

class CMethodSupport :
    public CMethodSupport_BaseType
{
public:
    HRESULT _InitializeModel(IDispatcher *pWorkingDisptcher)
    {
        this->SetLocalWorkingDispatcher(pWorkingDisptcher);
        return S_OK;
    }
private:
    HRESULT VoidAsyncInternal(const foundation::AsyncOperationClassPtr& async_operation_class_ptr) override
    {
        _pal_Delay(100);
        async_operation_class_ptr.SetCompleted(S_OK);
        return S_OK;
    }
};

typedef ObjectFactory<CMethodSupport, TestModel::MethodSupport::IIDType, ObservableObjectOptions::None, true> CMethodSupport_Factory;
HRESULT CreateMethodSupport(IDispatcher *pWorkingDisptcher, TestModel::IMethodSupport **pp)
{
    return CMethodSupport_Factory::CreateInstanceAs(pp, pWorkingDisptcher);
}



