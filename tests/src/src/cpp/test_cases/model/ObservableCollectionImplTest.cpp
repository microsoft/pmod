/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionImplTest.cpp
****/
#include "pch.h"

#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"
#include "TestModelImplInternal.g.h"

#include <pmod/library/observable_collection_impl.h>
#include <foundation/library/type_info_factory_util.h>
#include <foundation/library/enumerable_impl.h>
#include <foundation/value_ptr.h>

#include "../shared/ObservableCollectionTest.h"

using namespace foundation;
using namespace foundation::library;
using namespace pmod;
using namespace pmod::library;

template <class T>
class CObservableVectorImplAdapter :
    public ITestObservableCollectionClass
{
public:
    CObservableVectorImplAdapter(T*pT):
        _pT(pT)
    {}

    HRESULT AppendItem(foundation::IInspectable *pValue) override
    {
        ValuePtr value(pValue);
        _pT->push_back(value.GetValue<typename T::value_type>());
        return S_OK;
    }

    HRESULT RemoveItem(UINT32 index)
    {
        _pT->erase(_pT->begin() + index);
        return S_OK;
    }
    HRESULT Clear() override
    {
        _pT->clear();
        return S_OK;
    }

private:
    T *_pT;
};

class CObservableCollectionImplTest : public CUnitTestBase
{
protected:

    class CTypeProperties : public TestModel::_TypePropertiesImplBase<>
    {
    protected:
        CTypeProperties():
            _intValue(0)
        {}
    public:
        HRESULT GetIntProperty(INT32* pValue) override
        {
            *pValue = _intValue;
            return S_OK;
        }
        HRESULT SetIntProperty(INT32 value) override
        {
            _intValue = value;
            return S_OK;
        }
    private:
        INT32 _intValue;
    };
    void ObservableVectors()
    {
        ComPtr<ObservableVectorImplInt32> vec_int32;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(vec_int32.GetAddressOf()));

        ComPtr<IObservableCollection_Int32> observable_collection_int32_ptr;
        UT_VERIFY_HR(QueryInterface(vec_int32->CastToInspectable(), observable_collection_int32_ptr.GetAddressOf()));
        CObservableCollectionTest test;
        CObservableVectorImplAdapter<ObservableVectorImplInt32> classTest(vec_int32);
        test.TestInt32(classTest, observable_collection_int32_ptr);
		TestInt32ObservableVector(vec_int32.Get(),*vec_int32.Get());

    }

    void TypePropertiesVectors()
    {
        typedef ComPtr<TestModel::ITypeProperties> ITypePropertiesPtr;

        typedef _ObservableVectorImpl<ITypePropertiesPtr, TestModel::ObservableCollection_ITypeProperties::IIDType> ObservableVectorTypeProperties;

        ComPtr<ObservableVectorTypeProperties> vector;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(vector.GetAddressOf()));

        ComPtr<TestModel::ICollection_ITypeProperties> type_properties_collection_ptr;
        UT_VERIFY_HR(QueryInterface(vector->CastToInspectable(), type_properties_collection_ptr.GetAddressOf()));

        ComPtr<CTypeProperties> typePropertiesPtr;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(typePropertiesPtr.GetAddressOf()));

        vector->push_back(typePropertiesPtr.Get());
        ValuePtr value;
        UT_VERIFY_HR(vector->GetItem(0, value.GetAddressOf()));
    }

    void EnumerableTest()
    {
        ComPtr<EnumerableVectorImpl> enumInspectablePtr;
        UT_VERIFY_HR(CreateEnumerableVectorImpl(TestModel::Enumerable_ITypeProperties::IIDType, enumInspectablePtr.GetAddressOf()));

        ComPtr<CTypeProperties> typeProperties1Ptr;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(typeProperties1Ptr.GetAddressOf()));
        typeProperties1Ptr->SetIntProperty(10);
        ComPtr<CTypeProperties> typeProperties2Ptr;
        UT_VERIFY_HR(ctl::inspectable::CreateInstance(typeProperties2Ptr.GetAddressOf()));
        typeProperties2Ptr->SetIntProperty(20);

        enumInspectablePtr->GetItems().push_back(typeProperties1Ptr->CastToInspectable());
        enumInspectablePtr->GetItems().push_back(typeProperties2Ptr->CastToInspectable());

        ComPtr<TestModel::IEnumerable_ITypeProperties> enumTypePropertiesPtr;
        UT_VERIFY_HR(::QueryInterface(enumInspectablePtr->CastToInspectable(), enumTypePropertiesPtr.GetAddressOf()));
        
        ComPtr<foundation::IIterator_impl<TestModel::ITypeProperties *>> typePropertiesIteratorPtr;
        UT_VERIFY_HR(enumTypePropertiesPtr->First(typePropertiesIteratorPtr.GetAddressOf()));

        INT32 value = 10;
        bool hasCurrent = true;
        while (hasCurrent)
        {
            ComPtr<TestModel::ITypeProperties> typePropertiesPtr;
            UT_VERIFY_HR(typePropertiesIteratorPtr->GetCurrent(typePropertiesPtr.GetAddressOf()));

            INT32 valueTest;
            typePropertiesPtr->GetIntProperty(&valueTest);

            UT_VERIFY_BOOL(valueTest == value);
            value += 10;
            typePropertiesIteratorPtr->MoveNext(&hasCurrent);
        }
    }

public:
	BeginTestMethodMap(CObservableCollectionImplTest)
        TestMethod(ObservableVectors)
        TestMethod(TypePropertiesVectors)
        TestMethod(EnumerableTest)
        EndTestMethodMap()

};
ImplementTestClass(CObservableCollectionImplTest);


