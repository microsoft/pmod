/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:UnitTestBase.h
****/
#ifndef _UNIT_TEST_BASE_DEFINED
#define _UNIT_TEST_BASE_DEFINED

#define UT_VERIFY_HR(op)         { HRESULT ut_throw_HR = (op); if(FAILED(ut_throw_HR)) { TestAssert::Fail(( #op),__FILE__,__LINE__); throw ut_throw_HR; } }
#define UT_VERIFY_BOOL(op)       { bool ut_throw_BOOL = !!(op); if(!ut_throw_BOOL) { TestAssert::Fail(( #op),__FILE__,__LINE__); throw ut_throw_BOOL; } }

#include <pal/pal_core.h>

#include <stdlib.h>

#if !defined(CUnitTestBase)

#include <vector>
#include <set>
#include <string>

class CUnitTestBase;

class TestAssert
{
public:
    static void Fail(const char *message,const char *file,int line);
};

class CUnitTestBase;

typedef void (CUnitTestBase::*UnitTestFunction)(void);
typedef CUnitTestBase *(*UnitTestClassFactoryMethod)(void);

typedef struct tagUnitTestMetaDataItem
{
    const char *     methodName;
    UnitTestFunction function;
}  UnitTestMetaDataItem;

#define BeginTestMethodMap(classname)               \
    typedef classname ActiveTestClassType;              \
    static const char *CppUnitTestBase_GetClassName() throw() \
    { \
        return #classname; \
    } \
    static UnitTestMetaDataItem *GetUnitTestMetaDataItemsInternal()        \
    {                                                              \
        static UnitTestMetaDataItem unitTestMetaDataItems[] =  \
        {   \

#define TestMethod(methodname) \
    { #methodname,static_cast<UnitTestFunction>(&ActiveTestClassType::methodname)},\


#define EndTestMethodMap()                               \
            { 0,0 }                                        \
        };                                                 \
        return unitTestMetaDataItems;                      \
    }                                                      \
   public: \
    static UnitTestMetaDataItem *GetUnitTestMetaDataItems()        \
    {  \
        return GetUnitTestMetaDataItemsInternal();  \
    }   \
    static CUnitTestBase *CppUnitTest_FactoryMethod() \
    {  \
            return new ActiveTestClassType(); \
    }  \

#define GetTestClassDeclarationNameFromClassName(classname) addTestClass_##classname

// Adds the specified test class to the suite of unit test for the library
#define ImplementTestClass(classname) \
    TestClassDeclaration GetTestClassDeclarationNameFromClassName(classname)( #classname, classname::CppUnitTest_FactoryMethod,classname::GetUnitTestMetaDataItems());  \

typedef void(*_RunSetupCallback)(const std::set<std::string> *pTestSet);

class TestClassDeclaration
{
public:
    TestClassDeclaration(
        const char* classname,
        UnitTestClassFactoryMethod unitTestClassFactoryMethod,
        const UnitTestMetaDataItem *unitTestMetaDataItems):
        _testClassname(classname),
        _unitTestClassFactoryMethod(unitTestClassFactoryMethod),
        _unitTestMetaDataItems(unitTestMetaDataItems)
    {
        GetRegisteredClassDeclaration().push_back(this);
    }

    static bool RunAll(int argc, const CHAR_t* argv[]);
    static bool RunAll(const std::set<std::string> *pTestSet = nullptr);
    static void AddRunSetup(_RunSetupCallback pCallback);

private:
    const char *_testClassname;
    UnitTestClassFactoryMethod _unitTestClassFactoryMethod;
    const UnitTestMetaDataItem *_unitTestMetaDataItems;

    static std::vector<TestClassDeclaration *>& GetRegisteredClassDeclaration();
};

class CUnitTestBase
{
public:
    template <class T>
    static void Test()
    {
        T *pUnitTestBase = T::CppUnitTest_FactoryMethod();
        try
        {
            Test(
                pUnitTestBase,
                T::CppUnitTestBase_GetClassName(),
                T::GetUnitTestMetaDataItems());
        }
        catch(...)
        {
            // TODO: handle HR and BOOL exceptions
        }
        delete pUnitTestBase;
    }

private:
    friend class TestClassDeclaration;
    static int Test(
        CUnitTestBase *pUnitTestBase,
        const char *pClassName,
        const UnitTestMetaDataItem *pUnitTestMetaDataItems,
        int& failures);
protected:
	static void Fail(const char *message);
public:
    virtual ~CUnitTestBase() {}
    virtual void Setup() {};
    virtual void Teardown() {};
    virtual void TestClassSetup() {};
    virtual void TestClassTeardown() {};

};


#endif // defined(CUnitTestBase)

#endif //_UNIT_TEST_BASE_DEFINED
