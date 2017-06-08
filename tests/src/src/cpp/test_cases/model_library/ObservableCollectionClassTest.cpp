/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionClassTest.cpp
****/
#include "pch.h"

#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"
#include "TestModelFastInternal.g.h"
#include <foundation/library/type_info_factory_util.h>
#include <foundation/value_ptr.h>
#include <pmod/library/factory_util.h>
#include <pmod/library/observable_collection_stl.h>

#include "../shared/ObservableCollectionTest.h"

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::library;


class CObservableCollectionClassAdapter :
    public ITestObservableCollectionClass
{
public:
    CObservableCollectionClassAdapter(IObservableCollectionClass *pObservableCollectionClass):
        _pObservableCollectionClass(pObservableCollectionClass)
    {}

    HRESULT AppendItem(foundation::IInspectable *pValue) override
    {
        return _pObservableCollectionClass->AppendItemInternal(pValue);
    }

    HRESULT RemoveItem(UINT32 index) override
    {
        return _pObservableCollectionClass->RemoveItems(index, 1);
    }
    HRESULT Clear() override
    {
        return _pObservableCollectionClass->RemoveAllInternal();
    }

private:
    IObservableCollectionClass *_pObservableCollectionClass;
};


class CObservableCollectionClassTest : public CUnitTestBase
{
protected:
    void Test()
    {
        ComPtr<IObservableCollection_Int32> observable_collection_int32_ptr;
        UT_VERIFY_HR(CreateObservableCollection(
            IID_IObservableCollection_Int32,
            ObservableCollectionOptions::IsReadOnly, 
            observable_collection_int32_ptr.GetAddressOf()));
        ComPtr<IObservableCollectionClass> observable_collection_class_ptr;
        UT_VERIFY_HR(QueryInterface(observable_collection_int32_ptr, observable_collection_class_ptr.GetAddressOf()));

        CObservableCollectionClassAdapter classAdapter(observable_collection_class_ptr);
        CObservableCollectionTest test;
        test.TestInt32(classAdapter, observable_collection_int32_ptr);
    }

    void TestStl()
    {
        ComPtr<_Observable_vector_stl<INT32>> ptr;
        create_observable_collection_stl(ptr.GetAddressOf(), IID_IObservableCollection_Int32);

        ComPtr<IObservableCollection> ptr2;
        UT_VERIFY_HR(::QueryInterface(ptr->CastToInspectable(), ptr2.GetAddressOf()));

        CObservableCollectionClassAdapter classAdapter(ptr->GetClassInterface());
        CObservableCollectionTest test;
        test.TestInt32(classAdapter, 
            CastTo<IObservableCollection_Int32>(ptr->CastToInspectable()));

        TestInt32ObservableVector(ptr->GetInterface(), *ptr.Get());

        ComPtr<_Observable_vector_stl<HStringPtr>> stringVectorPtr;
        create_observable_collection_stl(stringVectorPtr.GetAddressOf(), IID_IObservableCollection_String);

        ComPtr<_Observable_set_stl<INT32>> setPtr;
        create_observable_collection_stl(setPtr.GetAddressOf(), IID_IObservableCollection_Int32);

        typedef ComPtr<TestModel::ITypeProperties> TypePropertiesPtr;

        ComPtr<_Observable_vector_stl<TypePropertiesPtr>> typePropertiesVector;
        create_observable_collection_stl(typePropertiesVector.GetAddressOf(), TestModel::ObservableCollection_ITypeProperties::IIDType);
        
        typedef TestModel::_FastTypePropertiesBase CTypePropertiesClass;
        typedef ComPtr<CTypePropertiesClass> TypePropertiesClassPtr;

        ComPtr<_Observable_vector_stl<TypePropertiesClassPtr,_UnboxComClassPtrAdapter<TestModel::ITypeProperties>>> typePropertiesClassVector;
        create_observable_collection_stl(typePropertiesClassVector.GetAddressOf(), TestModel::ObservableCollection_ITypeProperties::IIDType);

        ComPtr<_Observable_vector_stl<CTypePropertiesClass *, _UnboxComBaseAdapter<TestModel::ITypeProperties>>> typePropertiesRawPtrClassVector;
        create_observable_collection_stl(typePropertiesRawPtrClassVector.GetAddressOf(), TestModel::ObservableCollection_ITypeProperties::IIDType);

        ComPtr<_Observable_vector_stl<CTypePropertiesClass *, _NoUnboxAdapter>> typePropertiesRawPtrNoUnboxClassVector;
        create_observable_collection_stl(typePropertiesRawPtrNoUnboxClassVector.GetAddressOf(), TestModel::ObservableCollection_ITypeProperties::IIDType);
    }

public:
    BeginTestMethodMap(CObservableCollectionClassTest)
        TestMethod(Test)
        TestMethod(TestStl)
    EndTestMethodMap()

};
ImplementTestClass(CObservableCollectionClassTest);


