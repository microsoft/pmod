/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestRunner.cpp
****/
#include <stdio.h>

#include "UnitTestBase.h"
#include <foundation/library/logger_util.h>
#include <foundation/library/debug_library.h>
#include <fstream>

using namespace foundation;

#if defined(_WINDOWS)
#include <tchar.h>
#if defined(DEBUG)
EXTERN_C FOUNDATION_API HRESULT STDAPICALLTYPE SetWindowsRuntimeHStringEnabled(bool value);

int _AllocHook(int allocType, void *userData, size_t size, int
	blockType, long requestNumber, const unsigned char *filename, int
	lineNumber)
{
	//if (requestNumber == ???)
	//{
	//	printf("here\n");
	//}
	return TRUE;
}

#endif


int _tmain(int argc, const _TCHAR* argv[])

#else
int main(int argc, const char * argv[])
#endif
{
#if defined(_WINDOWS_RUNTIME)
    HRESULT hr = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif

#if defined(_WINDOWS)
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	// Enable next line if you want to track the allocation blocks in _WINDOWS
	//_CrtSetAllocHook(&_AllocHook);

#endif

#if defined(DEBUG)
    // track ref count on all proxies
    foundation::debug_util::SetOptionsFlag(0x2000);

#if defined(_WINDOWS) && !defined(_WINDOWS_RUNTIME)
    // Note: when running in DEBUG mode on windows we want to track HSTRING references
    // even when Windows Runtime is available
    // You should comment next line to allow VS to display HSTRING in the debugger window in 
    // case a piece of code is needed to be investigated 
    // SetWindowsRuntimeHStringEnabled(false);
#endif

#endif
#if !PMOD_LIB_LIBRARY_STATIC_LIB
    if (logger_util::IsAvailable())
#endif
    {
        logger_util::SetCategoryLevel(LoggingLevel::Debug);
    }

    bool succeed = TestClassDeclaration::RunAll(argc,argv);

    foundation_assert(succeed);
#if defined(DEBUG)
    if (foundation::debug_util::CountTrackedObjects())
    {
        foundation::debug_util::DumpTrackedObjects();
        foundation_assert(false);
    }
#endif

	return 0;
}

