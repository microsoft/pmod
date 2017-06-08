/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:UnitTestBase.cpp
****/
#include "UnitTestBase.h"
#include <stdio.h>
#include <wchar.h>
#include <foundation/string_t.h>

#if defined(ANDROID)
#include <android/log.h>

#define _log_debug_message(frmt,...)   __android_log_print(ANDROID_LOG_DEBUG,"unit_test",frmt,__VA_ARGS__)
#define _log_error_message(frmt,...)   __android_log_print(ANDROID_LOG_ERROR,"unit_test",frmt,__VA_ARGS__)
#else
#define _log_debug_message(frmt,...)   printf(frmt,__VA_ARGS__)
#define _log_error_message(frmt,...)   fprintf(stderr,frmt,__VA_ARGS__)
#endif

static std::vector<_RunSetupCallback> &GetRunSetupCallbacks()
{
    static std::vector<_RunSetupCallback> container;
    return container;
}

std::vector<TestClassDeclaration *>& TestClassDeclaration::GetRegisteredClassDeclaration()
{
    static std::vector<TestClassDeclaration *> registeredDeclarations;

    return registeredDeclarations;
}

void TestAssert::Fail(const char  *message, const char *file, int line)
{
    _log_error_message("%s File:%s Line:%d", message, file,line);
}

void TestClassDeclaration::AddRunSetup(_RunSetupCallback pCallback)
{
    GetRunSetupCallbacks().push_back(pCallback);
}

bool TestClassDeclaration::RunAll(int argc, const CHAR_t* argv[])
{
    std::set<std::string> testSet;
    if (argc > 1)
    {
        for (int arg = 1; arg < argc; ++arg)
        {
            testSet.insert(foundation::to_utf8_string(argv[arg]));
        }
    }
    return RunAll(testSet.size() ? &testSet : nullptr);
}


bool TestClassDeclaration::RunAll(const std::set<std::string> *pTestSet)
{
    for (std::vector<_RunSetupCallback>::const_iterator iter = GetRunSetupCallbacks().begin();
        iter != GetRunSetupCallbacks().end();
            ++iter)
    {
        (*iter)(pTestSet);
    }

    int totalClasses = 0;
    int totalMethods = 0;
    int totalFailures = 0;

    for (std::vector<TestClassDeclaration *>::const_iterator iter = GetRegisteredClassDeclaration().begin();
        iter != GetRegisteredClassDeclaration().end();
        ++iter)
    {
        if(pTestSet && pTestSet->find((*iter)->_testClassname) == pTestSet->end())
        {
            continue;
        }
        CUnitTestBase *pUnitTestBase = (*iter)->_unitTestClassFactoryMethod();
        const UnitTestMetaDataItem *pUnitTestMetaDataItems = (*iter)->_unitTestMetaDataItems;

        ++totalClasses;
        totalMethods += CUnitTestBase::Test(
            pUnitTestBase, 
            (*iter)->_testClassname, 
            pUnitTestMetaDataItems, 
            totalFailures);
        // delete the create unit test class
        delete pUnitTestBase;

    }
    _log_debug_message("#################### Summary Classes:%d Tests:%d Failures:%d ####################\n", 
        totalClasses, 
        totalMethods,
        totalFailures);
    return totalFailures == 0;
}

int CUnitTestBase::Test(
        CUnitTestBase *pUnitTestBase,
        const char *pClassName,
        const UnitTestMetaDataItem *pUnitTestMetaDataItems,
        int& failures)
{
    _log_debug_message("#########################Unit Test Class -> %s ########################\n", pClassName);
    fflush(stdout);
    pUnitTestBase->TestClassSetup();

    int totalmethods = 0;

    while(pUnitTestMetaDataItems->methodName)
    {
        _log_debug_message("Method -> %s\n", pUnitTestMetaDataItems->methodName);
        pUnitTestBase->Setup();
        try
        {
            (pUnitTestBase->*pUnitTestMetaDataItems->function)();
        }
        catch (...)
        {
            _log_error_message("Failure -> %s\n", pUnitTestMetaDataItems->methodName);
            ++failures;
        }
        pUnitTestBase->Teardown();
        ++pUnitTestMetaDataItems;
        ++totalmethods;
    }
    pUnitTestBase->TestClassTeardown();
    return totalmethods;
}
