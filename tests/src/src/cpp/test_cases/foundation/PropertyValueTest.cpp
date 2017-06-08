/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValueTest.cpp
****/
#include "pch.h"

#include "UnitTestBase.h"

#include <foundation/pv_util.h>
#include <foundation/array_wrapper.h>
#include <foundation/library/datetime_util.h>
//#include <foundation/library/logger_util.h>
#include <foundation/value_ptr.h>

#include <algorithm>

using namespace pmod;
using namespace foundation;

struct _ICustom : public foundation::IInspectable
{
};

class CCustomClass : public foundation::ctl::ComInspectableBase,
    public _ICustom
{

};

class CFakeInspectable : public foundation::ctl::ComInspectableBase
{
public:
    ULONG GetRefCountInternal()
    {
        return this->GetRefCount();
    }
protected:
    void FinalRelease() override
    {
        foundation::ctl::ComInspectableBase::FinalRelease();
    }

    _pal_AtomicIntegerType AddRefImpl() override
    {
        return foundation::ctl::ComInspectableBase::AddRefImpl();
    }
	
    _pal_AtomicIntegerType ReleaseImpl() override
    {
        return foundation::ctl::ComInspectableBase::ReleaseImpl();
    }

};


class CPropertyValueTest : public CUnitTestBase
{
protected:

void InspectableArrayTest()
{
    foundation::ComPtr<CFakeInspectable> spItem1;
    foundation::ctl::ComInspectableObject<CFakeInspectable>::CreateInstance(spItem1.GetAddressOf());
    foundation::ComPtr<CFakeInspectable> spItem2;
    foundation::ctl::ComInspectableObject<CFakeInspectable>::CreateInstance(spItem2.GetAddressOf());

    foundation::IInspectable * items[] = { spItem1.Get(), spItem2.Get() };

    foundation::ComPtr<foundation::IInspectable> spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateInspectableArrayValue(2, items, spPropertyValue.GetAddressOf()));

#if !defined(_WINDOWS_RUNTIME)
    UT_VERIFY_BOOL(spItem1->GetRefCountInternal() == 2);
    UT_VERIFY_BOOL(spItem2->GetRefCountInternal() == 2);
#endif

    foundation::InspectableArrayWrapper inspectableArray;
    UT_VERIFY_HR(pv_util::GetInspectableArray(
        spPropertyValue, 
        inspectableArray.GetSizeAddressOf(), 
        inspectableArray.GetBufferAddressOf()));

    UT_VERIFY_BOOL(inspectableArray.GetSize() == 2);
    UT_VERIFY_BOOL(inspectableArray[0] == spItem1.Get());
    UT_VERIFY_BOOL(inspectableArray[1] == spItem2.Get());

#if !defined(_WINDOWS_RUNTIME)
    UT_VERIFY_BOOL(spItem1->GetRefCountInternal() == 3);
    UT_VERIFY_BOOL(spItem2->GetRefCountInternal() == 3);
#endif

    foundation::InspectableArrayWrapper inspectableArray2;
    inspectableArray2 = inspectableArray;

    inspectableArray.Clear();
    inspectableArray2.Clear();

#if !defined(_WINDOWS_RUNTIME)
    UT_VERIFY_BOOL(spItem1->GetRefCountInternal() == 2);
    UT_VERIFY_BOOL(spItem2->GetRefCountInternal() == 2);
#endif

    spPropertyValue.Release();

#if !defined(_WINDOWS_RUNTIME)
    UT_VERIFY_BOOL(spItem1->GetRefCountInternal() == 1);
    UT_VERIFY_BOOL(spItem2->GetRefCountInternal() == 1);
#endif
}

void StringArrayTest()
{
    foundation::HStringRef hString1(U("item1"));
    foundation::HStringRef hString2(U("item2"));

    HSTRING items[] = { (HSTRING)hString1, (HSTRING)hString2};

    foundation::ComPtr<foundation::IInspectable> spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateStringArrayValue(2, items, spPropertyValue.GetAddressOf()));

    foundation::HStringArrayWrapper hStringArray;
    UT_VERIFY_HR(pv_util::GetStringArray(
        spPropertyValue, 
        hStringArray.GetSizeAddressOf(), 
        hStringArray.GetBufferAddressOf()));

    UT_VERIFY_BOOL(hStringArray.GetSize() == 2);

    UT_VERIFY_BOOL(_pal_strcmp(HStringPtr(hStringArray[0]).GetRawBuffer(), U("item1")) == 0);
    UT_VERIFY_BOOL(_pal_strcmp(HStringPtr(hStringArray[1]).GetRawBuffer(), U("item2")) == 0);

    foundation::HStringArrayWrapper hStringArray2;
    hStringArray2 = hStringArray;

    HStringArrayWrapper::const_iterator iter = std::find_if(
        hStringArray.begin(), hStringArray.end(), 
        [](HSTRING item){
        return _pal_strcmp(_pal_GetStringRawBuffer(item, nullptr), U("item2")) == 0;
    });
    UT_VERIFY_BOOL(iter != hStringArray.end());

    HStringArrayWrapper::const_iterator iter2 = std::find_if(
        hStringArray.begin(), hStringArray.end(), 
        [](HSTRING item){
        return _pal_strcmp(_pal_GetStringRawBuffer(item, nullptr), U("item???")) == 0;
    });
    UT_VERIFY_BOOL(iter2 == hStringArray.end());

    hStringArray.Clear();
    hStringArray2.Clear();
}

void UInt8ArrayTest()
{
    UINT8 items[] = { 0x10, 0x11, 0x12 };
        
    foundation::ComPtr<foundation::IInspectable> spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateUInt8ArrayValue(3, items, spPropertyValue.GetAddressOf()));
        
    foundation::UInt8ArrayWrapper uint8Array;
    UT_VERIFY_HR(pv_util::GetUInt8Array(
        spPropertyValue, 
        uint8Array.GetSizeAddressOf(), 
        uint8Array.GetBufferAddressOf()));
        
    UT_VERIFY_BOOL(uint8Array.GetSize() == 3);
    int index = 0;
    for(foundation::UInt8ArrayWrapper::const_iterator iter =uint8Array.begin();
        iter != uint8Array.end();
        ++iter)
    {
        UT_VERIFY_BOOL((*iter) == (index + 0x10));
        ++index;
    }
}
    
void Int32ArrayTest()
{
    INT32 items[] = { 10, 20};

    foundation::ComPtr<foundation::IInspectable> spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateInt32ArrayValue(2, items, spPropertyValue.GetAddressOf()));

    foundation::Int32ArrayWrapper int32Array;
    UT_VERIFY_HR(pv_util::GetInt32Array(
        spPropertyValue, 
        int32Array.GetSizeAddressOf(), 
        int32Array.GetBufferAddressOf()));

    UT_VERIFY_BOOL(int32Array.GetSize() == 2);

    UT_VERIFY_BOOL(int32Array[0] == 10);
    UT_VERIFY_BOOL(int32Array[1] == 20);
}

void EnumArrayTest()
{
    enum MyEnum
    {
        My1, My2, My3
    };

    MyEnum items[] = { My1, My2, My1, My2 };

    foundation::InspectablePtr spPropertyValue = pv_util::CreateValue(4, items);

    foundation::ArrayWrapper<MyEnum> enumArray;
    UT_VERIFY_HR(pv_util::GetValue(
        spPropertyValue,
        enumArray.GetSizeAddressOf(),
        enumArray.GetBufferAddressOf()));

    UT_VERIFY_BOOL(enumArray.GetSize() == 4);

    UT_VERIFY_BOOL(enumArray[0] == My1);
    UT_VERIFY_BOOL(enumArray[1] == My2);
    UT_VERIFY_BOOL(enumArray[2] == My1);
    UT_VERIFY_BOOL(enumArray[3] == My2);
}
    
void UInt64ArrayTest()
{
    UINT64 uint64Val[2];
    uint64Val[0] = 123;
    uint64Val[1] = 134;
    
    foundation::ComPtr<foundation::IInspectable> spPropertyValueArray;
    
    pv_util::CreateUInt64ArrayValue(2, uint64Val, spPropertyValueArray.GetAddressOf());
    
    foundation::UInt64ArrayWrapper uint64Array;
    UT_VERIFY_HR(pv_util::GetUInt64Array(
        spPropertyValueArray, 
        uint64Array.GetSizeAddressOf(), 
        uint64Array.GetBufferAddressOf()));
    
    UT_VERIFY_BOOL(uint64Array.GetSize() == 2);
    
    UT_VERIFY_BOOL(uint64Array[0] == 123);
    UT_VERIFY_BOOL(uint64Array[1] == 134);
}

void Int64ArrayTest()
{
    INT64 int64Val[2];
    int64Val[0] = 123;
    int64Val[1] = 134;
        
    foundation::ComPtr<foundation::IInspectable> spPropertyValueArray;
        
    pv_util::CreateInt64ArrayValue(2, int64Val, spPropertyValueArray.GetAddressOf());
        
    foundation::Int64ArrayWrapper int64Array;
    UT_VERIFY_HR(pv_util::GetInt64Array(
        spPropertyValueArray, 
        int64Array.GetSizeAddressOf(), 
        int64Array.GetBufferAddressOf()));
        
    UT_VERIFY_BOOL(int64Array.GetSize() == 2);
        
    UT_VERIFY_BOOL(int64Array[0] == 123);
    UT_VERIFY_BOOL(int64Array[1] == 134);
}
 
 void BooleanArrayTest()
 {
     boolean booleanVal[2];
     booleanVal[0] = false;
     booleanVal[1] = true;
     
     foundation::ComPtr<foundation::IInspectable> spPropertyValueArray;
     
     pv_util::CreateBooleanArrayValue(2, booleanVal, spPropertyValueArray.GetAddressOf());
     
     foundation::BooleanArrayWrapper booleanArray;
     UT_VERIFY_HR(pv_util::GetBooleanArray(
        spPropertyValueArray, 
        booleanArray.GetSizeAddressOf(), 
        booleanArray.GetBufferAddressOf()));
     
     UT_VERIFY_BOOL(booleanArray.GetSize() == 2);
     
     UT_VERIFY_BOOL(!booleanArray[0]);
     UT_VERIFY_BOOL(booleanArray[1]);
}
   
void DoubleArrayTest()
{
   DOUBLE doubleVal[2];
   doubleVal[0] = 0.2;
   doubleVal[1] = 0.4;
   
   foundation::ComPtr<foundation::IInspectable> spPropertyValueArray;
   
   pv_util::CreateDoubleArrayValue(2, doubleVal, spPropertyValueArray.GetAddressOf());
   
   foundation::DoubleArrayWrapper doubleArray;
   UT_VERIFY_HR(pv_util::GetDoubleArray(
        spPropertyValueArray, 
        doubleArray.GetSizeAddressOf(), 
        doubleArray.GetBufferAddressOf()));
   
   UT_VERIFY_BOOL(doubleArray.GetSize() == 2);
   
   UT_VERIFY_BOOL(doubleArray[0] == 0.2);
   UT_VERIFY_BOOL(doubleArray[1] == 0.4);
}
      
void SingleArrayTest()
{
   FLOAT floatVal[2];
   floatVal[0] = 1.0;
   floatVal[1] = (FLOAT)1.1;
   
   foundation::ComPtr<foundation::IInspectable> spPropertyValueArray;
   
   pv_util::CreateSingleArrayValue(2, floatVal, spPropertyValueArray.GetAddressOf());
   
   foundation::SingleArrayWrapper singleArray;
   UT_VERIFY_HR(pv_util::GetSingleArray(
        spPropertyValueArray, 
        singleArray.GetSizeAddressOf(), 
        singleArray.GetBufferAddressOf()));
   
   UT_VERIFY_BOOL(singleArray.GetSize() == 2);
   
   UT_VERIFY_BOOL(singleArray[0] == (FLOAT)1.0);
   FLOAT val = singleArray[1];
   UT_VERIFY_BOOL(val == (FLOAT)1.1);
}

void CharArrayTest()
{
   WCHAR charVal[2];
   charVal[0] = 'A';
   charVal[1] = 'D';
   
   foundation::ComPtr<foundation::IInspectable> spPropertyValueArray;
   
   pv_util::CreateChar16ArrayValue(2, charVal, spPropertyValueArray.GetAddressOf());
   
   foundation::Char16ArrayWrapper charArray;
   UT_VERIFY_HR(pv_util::GetChar16Array(
        spPropertyValueArray, 
        charArray.GetSizeAddressOf(), 
        charArray.GetBufferAddressOf()));
   
   UT_VERIFY_BOOL(charArray.GetSize() == 2);
   
   UT_VERIFY_BOOL(charArray[0] == 'A');
   UT_VERIFY_BOOL(charArray[1] == 'D');
}
   
void UInt8Test()
{
    foundation::ComPtr<foundation::IInspectable> spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateUInt8Value(10, spPropertyValue.GetAddressOf()));

    INT16 int16Val;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &int16Val));
    UT_VERIFY_BOOL(int16Val == 10);

    UINT16 uint16Val;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &uint16Val));
    UT_VERIFY_BOOL(uint16Val == 10);
    
    FLOAT floatVal;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &floatVal));
    UT_VERIFY_BOOL(floatVal == 10.0);
}

void CastArrayTest()
{
    std::vector<foundation::InspectablePtr> values;
    values.push_back(pv_util::CreateValue((INT32)100));
    values.push_back(pv_util::CreateValue((INT32)200));

    foundation::InspectablePtr spValue;
    pv_util::CreateInspectableArrayValue(2, values.front().GetAddressOfPtr(), spValue.GetAddressOf());

    Int32ArrayWrapper int32_array;
    UT_VERIFY_HR(pv_util::GetInt32Array(
        spValue, 
        int32_array.GetSizeAddressOf(), 
        int32_array.GetBufferAddressOf()));
    UT_VERIFY_BOOL(int32_array.GetSize() == 2);
    UT_VERIFY_BOOL(int32_array[0] == 100);
    UT_VERIFY_BOOL(int32_array[1] == 200);


}

void SingleTest()
{
    foundation::ComPtr<foundation::IInspectable> spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateSingleValue(10, spPropertyValue.GetAddressOf()));

    UINT8 uint8Val;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &uint8Val));
    UT_VERIFY_BOOL(uint8Val == 10);

    INT16 int16Val;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &int16Val));
    UT_VERIFY_BOOL(int16Val == 10);

    UT_VERIFY_HR(pv_util::CreateSingleValue(10.5, spPropertyValue.ReleaseAndGetAddressOf()));

    HRESULT hr = pv_util::GetValue(spPropertyValue, &uint8Val);
    UT_VERIFY_BOOL(FAILED(hr));

    DOUBLE doubleVal;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &doubleVal));
    UT_VERIFY_BOOL(doubleVal == 10.5);
}

void ChangeTypeTest()
{
    foundation::InspectablePtr spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateUInt8Value(1, spPropertyValue.GetAddressOf()));

    UT_VERIFY_HR(pv_util::ChangeType(foundation::PropertyType_Boolean, spPropertyValue.GetAddressOfPtr()));
    UT_VERIFY_HR(pv_util::VerifyType(spPropertyValue, foundation::PropertyType_Boolean));
    // convert Boolean <-> String
    UT_VERIFY_HR(pv_util::ChangeType(foundation::PropertyType_String, spPropertyValue.GetAddressOfPtr()));
    UT_VERIFY_HR(pv_util::ChangeType(foundation::PropertyType_Boolean, spPropertyValue.GetAddressOfPtr()));
    bool boolValue;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &boolValue));
    UT_VERIFY_BOOL(boolValue);

    spPropertyValue = pv_util::CreateValue(U("123"));
    UINT32 ivalue;
    HRESULT hr = pv_util::GetValue(spPropertyValue, &ivalue);
    UT_VERIFY_BOOL(FAILED(hr));

    // convert DateTime <-> String
    DateTime now = pv_util::Now();
    UT_VERIFY_HR(pv_util::CreateDateTimeValue(
        now, 
        spPropertyValue.ReleaseAndGetAddressOf()));

    UT_VERIFY_HR(pv_util::ChangeType(
        PropertyType_String, spPropertyValue.GetAddressOfPtr()));

    UT_VERIFY_HR(pv_util::ChangeType(
        PropertyType_DateTime, spPropertyValue.GetAddressOfPtr()));

    DateTime now2;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &now2));
    UT_VERIFY_BOOL(now.UniversalTime == now2.UniversalTime);

    // a partial DateTime without time
    spPropertyValue = pv_util::CreateValue(U("2015-11-25"));
    UT_VERIFY_HR(pv_util::ChangeType(
        PropertyType_DateTime, spPropertyValue.GetAddressOfPtr()));
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &now));
    foundation::SystemTime sysTime;
    UT_VERIFY_HR(_pal_DateTimeToSystemTime(now, &sysTime));
    UT_VERIFY_BOOL(sysTime.wYear == 2015);
    UT_VERIFY_BOOL(sysTime.wMonth == 11);
    UT_VERIFY_BOOL(sysTime.wDay == 25);

    UINT64 values[] = { 100, 200 };
    UT_VERIFY_HR(pv_util::CreateUInt64ArrayValue(2, values, spPropertyValue.ReleaseAndGetAddressOf()));
    UT_VERIFY_HR(pv_util::ChangeType(foundation::PropertyType_UInt32Array, spPropertyValue.GetAddressOfPtr()));

    UInt32ArrayWrapper uint32_array;
    UT_VERIFY_HR(pv_util::GetValueArray(spPropertyValue, 
        uint32_array.GetSizeAddressOf(), 
        uint32_array.GetBufferAddressOf()));

    UT_VERIFY_BOOL(uint32_array[0] == 100);
    UT_VERIFY_BOOL(uint32_array[1] == 200);

    UT_VERIFY_HR(pv_util::ChangeType(foundation::PropertyType_StringArray, spPropertyValue.GetAddressOfPtr()));

    HStringArrayWrapper hstring_array;
    UT_VERIFY_HR(pv_util::GetValueArray(spPropertyValue, 
        hstring_array.GetSizeAddressOf(),
        hstring_array.GetBufferAddressOf()));
    UT_VERIFY_BOOL(hstring_array.GetSize() == 2);

    UT_VERIFY_BOOL(_pal_strcmp(HStringPtr(hstring_array[0]).GetRawBuffer(), U("100")) == 0);
    UT_VERIFY_BOOL(_pal_strcmp(HStringPtr(hstring_array[1]).GetRawBuffer(), U("200")) == 0);

    // double to float cast
    UT_VERIFY_HR(pv_util::CreateDoubleValue(
        10.8,
        spPropertyValue.ReleaseAndGetAddressOf()));
    UT_VERIFY_HR(pv_util::ChangeType(
        PropertyType_Single, spPropertyValue.GetAddressOfPtr()));
    FLOAT floatValue;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &floatValue));
   // UT_VERIFY_BOOL(floatValue == 10.8);

    UT_VERIFY_HR(pv_util::CreateInt32Value(
        150,
        spPropertyValue.ReleaseAndGetAddressOf()));
    UT_VERIFY_HR(pv_util::ChangeType(
        PropertyType_Single, spPropertyValue.GetAddressOfPtr()));
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &floatValue));
    UT_VERIFY_BOOL(floatValue == 150.0);
}

void DatetimeTest()
{
    foundation::SystemTime t;
    t.wSecond = 30;
    t.wMinute = 15;
    t.wHour = 2;
    t.wDay = 05;
    t.wMonth = 3;
    t.wYear = 1994;
    t.wMilliseconds = 0;

    foundation::InspectablePtr spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateSystemTimeValue(t, spPropertyValue.GetAddressOf()));

    foundation::SystemTime t2;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &t2));


    UT_VERIFY_BOOL(t2.wYear == t.wYear);
    UT_VERIFY_BOOL(t2.wMonth == t.wMonth);
    UT_VERIFY_BOOL(t2.wDay == t.wDay);

    foundation::DateTime dt;
    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &dt));

    foundation::DateTime dt2;
    _pal_DateTimeAdd(dt, 86400 * 1000, &dt2);

    UT_VERIFY_HR(pv_util::GetValue(spPropertyValue, &t2));
    _pal_DateTimeToSystemTime(dt2, &t2);

    UT_VERIFY_BOOL(t2.wDay == (t.wDay + 1));

    // create a 
    time_t timer;
    time(&timer);

    foundation::InspectablePtr dtValue1_ptr = datetime_util::CreateValue(&timer);
    UT_VERIFY_HR(pv_util::GetValue(dtValue1_ptr, &dt));

    foundation::SystemTime sys_time;
    _pal_DateTimeToSystemTime(dt, &sys_time);

    time_t timer_2;
    UT_VERIFY_HR(datetime_util::to_time_t(sys_time, &timer_2));

    UT_VERIFY_BOOL(timer == timer_2);
}

void DatetimeUtilTest()
{
    time_t  tm;
    foundation::SystemTime t;
    _pal_SystemTimeNow(&t);

    pal::library::TraceMessage(U("_pal_SystemTimeNow: wDay:%d wMonth:%d wYear:%d wDay:%d wHour:%d wMinute:%d wSecond:%d\n"),
        t.wDay, 
        t.wMonth, 
        t.wYear, 
        t.wHour, 
        t.wMinute, 
        t.wSecond);

    t.wSecond = 15;
    t.wMinute = 10;
    t.wHour = 12;
    t.wDay = 9;
    t.wMonth = 9;
    t.wYear = 1999;
    t.wMilliseconds = 0;

    datetime_util::to_time_t(t, &tm);

    foundation::SystemTime t2;
    datetime_util::from_time_t(&tm, &t2);

    pal::library::TraceMessage(U("datetime_util::from_time_t: wDay:%d wMonth:%d wYear:%d wDay:%d wHour:%d wMinute:%d wSecond:%d\n"),
        t.wDay,
        t.wMonth,
        t.wYear,
        t.wHour,
        t.wMinute,
        t.wSecond);

    UT_VERIFY_BOOL(t2.wDay == t.wDay);
    UT_VERIFY_BOOL(t2.wMonth == t.wMonth);
    UT_VERIFY_BOOL(t2.wYear == t.wYear);
    UT_VERIFY_BOOL(t2.wHour == t.wHour);
    UT_VERIFY_BOOL(t2.wMinute == t.wMinute);
    UT_VERIFY_BOOL(t2.wSecond == t.wSecond);
}

void CompareTest()
{
    foundation::DateTime dt_now = pv_util::Now();
    foundation::InspectablePtr dtValue_ptr = pv_util::CreateValue(dt_now);
    foundation::InspectablePtr intValue_ptr = pv_util::CreateValue(dt_now.UniversalTime);

    UT_VERIFY_BOOL(pv_util::CompareInspectableValues(dtValue_ptr, intValue_ptr) == 0);
    intValue_ptr = pv_util::CreateValue(dt_now.UniversalTime + 1);
    UT_VERIFY_BOOL(pv_util::CompareInspectableValues(dtValue_ptr, intValue_ptr) < 0);
    UT_VERIFY_BOOL(pv_util::CompareInspectableValues(intValue_ptr, dtValue_ptr) > 0);
}

void SerializeTest()
{
    foundation::ComPtr<foundation::IInspectable> spPropertyValue;
    UT_VERIFY_HR(pv_util::CreateStringValue(U("property value"), spPropertyValue.GetAddressOf()));

    foundation::UInt8ArrayWrapper buffer;
    UT_VERIFY_HR(pv_util::Serialize(
        pv_util:: ToPropertyValuePtr(spPropertyValue), 
        buffer.GetSizeAddressOf(), 
        buffer.GetBufferAddressOf()));

    foundation::PropertyValuePtr spPropertyValue2;
    UT_VERIFY_HR(pv_util::Deserialize(
        buffer.GetSize(), 
        (UINT8 *)buffer.GetBuffer(), 
        spPropertyValue2.GetAddressOf()));

    UT_VERIFY_BOOL(pv_util::CompareInspectableValues(spPropertyValue, spPropertyValue2) == 0);
}

void CreateValueTest()
{
    foundation::InspectablePtr value;

    value = foundation::CreateValue(true);
    bool bVal;
    UT_VERIFY_HR(foundation::GetValue(value, bVal));
    UT_VERIFY_BOOL(bVal);

    value = foundation::CreateValue(100);
    UINT32 intVal;
    UT_VERIFY_HR(foundation::GetValue(value, intVal));
    UT_VERIFY_BOOL(intVal == 100);

    value = foundation::CreateValue((INT64)200);
    value = foundation::CreateValue(U("hello"));
    foundation::HStringPtr str(U("hello"));
    value = foundation::CreateValue(str);

    // UTF-8
    value = foundation::CreateValue("hello");
    std::string utf8_str;
    UT_VERIFY_HR(foundation::GetValue(value, utf8_str));
    UT_VERIFY_BOOL(utf8_str == "hello");

    UInt8ArrayWrapper uint8_array;
    value = foundation::CreateValue(uint8_array);

    std::vector<int> vInt;
    vInt.push_back(100);
    vInt.push_back(200);

    value = foundation::CreateValue(vInt);
    vInt.clear();
    UT_VERIFY_HR(foundation::GetValue(value, vInt));
    UT_VERIFY_BOOL(vInt.size() == 2);
    UT_VERIFY_BOOL(vInt[0] == 100);
    UT_VERIFY_BOOL(vInt[1] == 200);

    std::vector<foundation::HStringPtr> vHstringPtr;
    vHstringPtr.push_back(foundation::HStringPtr(U("hello")));
    value = foundation::CreateValue(vHstringPtr);
    vHstringPtr.clear();
    foundation::GetValue(value, vHstringPtr);
    UT_VERIFY_BOOL(vHstringPtr.size() == 1);
    UT_VERIFY_BOOL(string_t(vHstringPtr[0].GetRawBuffer()) == U("hello"));

    struct ICustom : foundation::IInspectable
    {
        static const IID& GetIID() { return foundation_GUID_NULL; }
    };
    foundation::ComPtr<ICustom> custom_ptr;
    value = foundation::CreateValue(custom_ptr);

    value = foundation::CreateValue((ICustom *)nullptr);

    std::vector<foundation::ComPtr<ICustom>> vCustom;
    vCustom.push_back(nullptr);
    value = foundation::CreateValue(vCustom);
    vCustom.clear();
    foundation::GetValue(value, vCustom);

    std::set<int> vIntSet;
    vIntSet.insert(100);
    value = foundation::CreateValue(vIntSet);

    std::set<foundation::ComPtr<ICustom>> vCustomSet;
    value = foundation::CreateValue(vCustomSet);

    // test enum array values
    enum EnumType { Value1, Value2 };

    EnumType enumValue = EnumType::Value2;
    ArrayWrapper<EnumType> enumValues;
    enumValues.CopyFrom(1, &enumValue);

    value = foundation::CreateValue(enumValues);
    enumValues.Clear();
    foundation::GetValue(value, enumValues);
    UT_VERIFY_BOOL(enumValues.GetSize() == 1);
    UT_VERIFY_BOOL(enumValues[0] == EnumType::Value2);

    // bool arrays
    std::vector<bool> vBools;
    vBools.push_back(true);
    vBools.push_back(false);
    value = foundation::CreateValue(vBools);
    foundation::GetValue(value, vBools);
    UT_VERIFY_BOOL(vBools.size() == 2);
    UT_VERIFY_BOOL(vBools[0] == true);
    UT_VERIFY_BOOL(vBools[1] == false);
}

void ValuePtrTest()
{
    ValuePtr value(100);
    INT32 intVal;
    UT_VERIFY_HR(value.GetValue(intVal));
    UT_VERIFY_BOOL(intVal == 100);
    UT_VERIFY_BOOL(value.GetValue<INT32>() == 100);
    UT_VERIFY_BOOL(value == 100);

    value = U("hello");
    foundation::HStringPtr str;
    UT_VERIFY_HR(value.GetValue(str));
    UT_VERIFY_BOOL(string_t(str.GetRawBuffer()) == U("hello"));

    value = "hello";
    std::string utf8_str;
    UT_VERIFY_HR(value.GetValue(utf8_str));
    UT_VERIFY_BOOL(utf8_str == "hello");

    GUID iid = foundation_GUID_NULL;
    value = iid;
    UT_VERIFY_HR(value.ChangeType(PropertyType_String));
    string_t str_t;
    UT_VERIFY_HR(value.GetValue(str_t));
    UT_VERIFY_BOOL(str_t == U("00000000-0000-0000-0000-000000000000"));

    value = foundation::CreateArrayWithValues(100, U("hello"));
    std::vector<ValuePtr> values;
    UT_VERIFY_HR(value.GetValue(values));
    UT_VERIFY_BOOL(values.size() == 2);
    UT_VERIFY_BOOL(values[0].Compare(ValuePtr(100)) == 0);
    UT_VERIFY_BOOL(values[1].Compare(ValuePtr(U("hello"))) == 0);

}

void CopyToTest()
{
    std::vector<INT32> vInts;
    vInts.push_back(100);
    vInts.push_back(100);

    Int32ArrayWrapper int_array;
    CopyTo(vInts, int_array.GetSizeAddressOf(), int_array.GetBufferAddressOf());
    UT_VERIFY_BOOL(int_array.GetSize() == 2);

    std::vector<foundation::ComPtr<_ICustom>> vCustomPtrs;
    vCustomPtrs.resize(2);
    foundation::ctl::ComInspectableObject<CCustomClass>::CreateInstance(vCustomPtrs[0].GetAddressOf());

    ArrayRefCountWrapper<_ICustom> custom_array;
    CopyTo(vCustomPtrs, custom_array.GetSizeAddressOf(), custom_array.GetBufferAddressOf());
    UT_VERIFY_BOOL(custom_array.GetSize() == 2);

    std::vector<HStringPtr> vStrings;
    vStrings.push_back(HStringPtr(U("one")));
    vStrings.push_back(HStringPtr(U("two")));

    HStringArrayWrapper str_array;
    CopyTo(vStrings, str_array.GetSizeAddressOf(), str_array.GetBufferAddressOf());
    UT_VERIFY_BOOL(str_array.GetSize() == 2);

    std::vector<bool> vBools;
    vBools.push_back(true);
    vBools.push_back(false);

    BoolArrayWrapper bool_array;
    CopyTo(vBools, bool_array.GetSizeAddressOf(), bool_array.GetBufferAddressOf());
    UT_VERIFY_BOOL(bool_array.GetSize() == 2);
}

public:
    BeginTestMethodMap(CPropertyValueTest)
        TestMethod(InspectableArrayTest)
        TestMethod(UInt8ArrayTest)
        TestMethod(EnumArrayTest)
        TestMethod(Int32ArrayTest)
        TestMethod(Int64ArrayTest)
        TestMethod(UInt64ArrayTest)
        TestMethod(StringArrayTest)
        TestMethod(SingleArrayTest)
        TestMethod(CharArrayTest)
        TestMethod(DoubleArrayTest)
        TestMethod(BooleanArrayTest)
        TestMethod(CastArrayTest)
        TestMethod(UInt8Test)
        TestMethod(SingleTest)
        TestMethod(ChangeTypeTest)
        TestMethod(DatetimeTest)
        TestMethod(DatetimeUtilTest)
        TestMethod(CompareTest)
        TestMethod(SerializeTest)
        TestMethod(CreateValueTest)
        TestMethod(ValuePtrTest)
        TestMethod(CopyToTest)
    EndTestMethodMap()
private:
};

ImplementTestClass(CPropertyValueTest);

