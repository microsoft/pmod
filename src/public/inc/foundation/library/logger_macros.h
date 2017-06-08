/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger_macros.h
****/
#pragma once

#include "logger_util.h"

#ifdef __UTF16_STRINGS
#define XCPW(x) L##x
#else
#define XCPW(x) x
#endif

// The following macros which enable the definition of __WFILE__ are based on
// those described in MSDN library under __FILE__.
#if !defined(__WFILE__)
    #define __t2__(x)    XCPW(x)
    #define __t3__(x,y)  __t1__(#y)
    #define __WFILE__    __t2__(__FILE__)
#endif

// undef existing legacy macros if they were defined
#ifdef IFR
#undef IFR
#endif
#ifdef IFREx
#undef IFREx
#endif
#ifdef IFHR
#undef IFHR
#endif
#ifdef IFRF
#undef IFRF
#endif
#ifdef IFCL
#undef IFCL
#endif
#ifdef IFCOOM
#undef IFCOOM
#endif
#ifdef IFCPTR
#undef IFCPTR
#endif
#ifdef IFCPTREx
#undef IFCPTREx
#endif
#ifdef IFCEXPECT
#undef IFCEXPECT
#endif
#ifdef IFCEXPECTEx
#undef IFCEXPECTEx
#endif

#define IFR(x) {int hr_IFR = (x); if(FAILED(hr_IFR)) { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,hr_IFR,__WFILE__,__LINE__,XCPW(#x)); return hr_IFR; } }
#define IFR_OK(x) {int hr_IFR = (x); IFR(hr_IFR); if(hr_IFR == S_OK) { return hr_IFR; } }
#define IFREx(x, returnValue) {int hr_IFR = (x); if(FAILED(hr_IFR)) { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,hr_IFR,__WFILE__,__LINE__,XCPW(#x)); return returnValue;}}
#define IFHR(x) { hr = (x); if(FAILED(hr)) { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,hr,__WFILE__,__LINE__,XCPW(#x)); return hr; } }
#define IFHR_OK(x) { hr = (x); IFHR(x); if(hr == S_OK) { return hr; } }
#define IFRF(x) {int hr_IFR = (x); if(FAILED(hr_IFR)) { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,hr_IFR,__WFILE__,__LINE__,XCPW(#x));return false; } }
#define IFCL(x) { hr = (x); if(FAILED(hr)) { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,hr,__WFILE__,__LINE__,XCPW(#x)); goto Cleanup; } }

#define IFCOOM(x)     if ((x) == nullptr)  { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::NullPtr,E_OUTOFMEMORY,__WFILE__,__LINE__,XCPW(#x)); return E_OUTOFMEMORY; }
#define IFCPTR(x)     if ((x) == nullptr)  { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::NullPtr,E_POINTER,__WFILE__,__LINE__,XCPW(#x)); return E_POINTER; }
#define IFCPTREx(x, returnValue)     if ((x) == nullptr)  { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::NullPtr,returnValue,__WFILE__,__LINE__,XCPW(#x));return returnValue; }
#define IFCEXPECT(x)  if (!(x))         { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::Expected,E_UNEXPECTED,__WFILE__,__LINE__,XCPW(#x)); return E_UNEXPECTED; }
#define IFCEXPECTEx(x, returnValue)  if (!(x))         { foundation::logger_util::LogTrace(foundation::LoggingLevel::Error,foundation::Error_Category::Expected,returnValue,__WFILE__,__LINE__,XCPW(#x)); return returnValue; }

#define LOG_TRACE(level,category,x,hr) foundation::logger_util::LogTrace(level,category,hr,__WFILE__,__LINE__,XCPW(#x));
#define LOG_FAILURE(x,hr) LOG_TRACE(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,x,hr);
#define LOG_WARNING(x,hr) LOG_TRACE(foundation::LoggingLevel::Warning,foundation::Error_Category::Failure,x,hr);

// parameter based macros
#define LOG_TRACE_PARAM(level,category,x,hr,parameter) \
        foundation::logger_util::LogTrace(level,\
        category,\
        hr,\
        __WFILE__,\
        __LINE__,\
        XCPW(#x),\
        foundation::pv_util::CreateValue(parameter).Get());

#define LOG_FAILURE_PARAM(x,hr,parameter) LOG_TRACE(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,x,hr,parameter);
#define LOG_WARNING_PARAM(x,hr,parameter) LOG_TRACE(foundation::LoggingLevel::Warning,foundation::Error_Category::Failure,x,hr,parameter);

#define IFR_PARAM(x,parameter) {\
    int hr_IFR = (x);\
    if(FAILED(hr_IFR)) {\
        LOG_TRACE_PARAM(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,x,hr_IFR,parameter) \
        return hr_IFR; }\
  }

#define IFR_PARAM_VALUE(x,parameter) {\
    int hr_IFR = (x);\
    if(FAILED(hr_IFR)) {\
        LOG_TRACE_PARAM(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,x,hr_IFR,foundation::pv_util::CreateValue(parameter)) \
        return hr_IFR; }\
    }

#define IFHR_PARAM(x,parameter) {\
    hr = (x);\
    if(FAILED(hr)) {\
        LOG_TRACE_PARAM(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,x,hr,parameter) \
        return hr; }\
    }

#define IFR_PARAM_CALLBACK(x,callback) {\
    int hr_IFR = (x);\
    if(FAILED(hr_IFR)) {\
        LOG_TRACE_PARAM(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,x,hr_IFR,callback()) \
        return hr_IFR; }\
    }

#define IFHR_PARAM_CALLBACK(x,callback) {\
    hr = (x);\
    if(FAILED(hr)) {\
        LOG_TRACE_PARAM(foundation::LoggingLevel::Error,foundation::Error_Category::Failure,x,hr,callback()) \
        return hr; }\
        }
